#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
# PiPi finite ready-batch supervisor. Parent is the sole ledger writer.
[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$ManifestPath,
 [Parameter(Mandatory)][string]$WorkerPath,
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{64}$')][string]$WorkerSha256,
 [Parameter(Mandatory)][string]$RunDirectory,
 [ValidateRange(1,64)][int]$Workers=64,
 [ValidateRange(1,10)][int]$MaxAttempts=3,
 [ValidateRange(1,3600)][int]$IdleTtlSeconds=15,
 [ValidateRange(1,3600)][int]$HeartbeatTtlSeconds=10,
 [ValidateRange(1,3600)][int]$StartupTtlSeconds=90,
 [ValidateRange(1,86400)][int]$MaxAttemptSeconds=300,
 [ValidateRange(50,2000)][int]$PollMilliseconds=200
)
$ErrorActionPreference='Stop'
if ($MaxAttemptSeconds -lt $HeartbeatTtlSeconds -or $PollMilliseconds -ge 1000*[Math]::Min($IdleTtlSeconds,[Math]::Min($StartupTtlSeconds,$HeartbeatTtlSeconds))) { throw 'Invalid deadlines' }
function Read-Json([string]$Path,[long]$Limit=65536) {
 $item=Get-Item -LiteralPath $Path
 if ($item.PSIsContainer -or $item.Length -gt $Limit) { throw 'Invalid JSON file' }
 Get-Content -LiteralPath $Path -Raw -Encoding utf8 | ConvertFrom-Json -AsHashtable -Depth 12
}
function Digest([string]$Path) { (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
function Utc { [DateTimeOffset]::UtcNow.ToString('o') }
function Publish([string]$Path,$Value,[bool]$Replace=$false) {
 $tmp=$Path+'.'+[guid]::NewGuid().ToString('N')+'.tmp'
 try {
  $bytes=[Text.UTF8Encoding]::new($false).GetBytes(($Value | ConvertTo-Json -Depth 14))
  $s=[IO.File]::Open($tmp,[IO.FileMode]::CreateNew,[IO.FileAccess]::Write,[IO.FileShare]::None)
  try { $s.Write($bytes,0,$bytes.Length);$s.Flush($true) } finally { $s.Dispose() }
  [IO.File]::Move($tmp,$Path,$Replace)
 } finally { if ([IO.File]::Exists($tmp)) { [IO.File]::Delete($tmp) } }
}
$m=Read-Json $ManifestPath 1048576
if ($m.protocol -cne 'NINZA-PIPI-READY-v1' -or $m.retry_safe -isnot [bool] -or -not $m.retry_safe -or
 $m.jobs -isnot [array] -or $m.jobs.Count -lt 1 -or $m.jobs.Count -gt 4096) { throw 'Explicit finite retry-safe manifest required' }
$fifo=[Collections.Generic.Queue[object]]::new()
$lifo=[Collections.Generic.Stack[object]]::new()
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$jobs=@(foreach ($entry in $m.jobs) {
 if ($entry -isnot [Collections.IDictionary] -or $entry.job_id -isnot [string] -or
  $entry.job_id -cnotmatch '^[a-z0-9][a-z0-9._-]{0,63}$' -or -not $seen.Add($entry.job_id)) { throw 'Invalid/duplicate job ID' }
 $lane=if ($entry.Contains('lane')) {$entry.lane} else {'LIFODFU'}
 if ($lane -cnotin @('FIFONFU','LIFODFU')) { throw 'Unknown lane' }
 $j=[pscustomobject][ordered]@{job_id=$entry.job_id;lane=$lane;state='READY';attempts=@();accepted_output=$null}
 if ($lane -ceq 'FIFONFU') {$fifo.Enqueue($j)} else {$lifo.Push($j)}
 $j
})
$root=[IO.Path]::GetFullPath($RunDirectory)
if ([IO.Directory]::Exists($root) -or [IO.File]::Exists($root)) { throw 'Run directory already exists' }
$worker=(Get-Item -LiteralPath $WorkerPath).FullName
$pin=$WorkerSha256.ToLowerInvariant()
if ([IO.Path]::GetExtension($worker) -ine '.ps1' -or (Digest $worker) -cne $pin) { throw 'Worker pin mismatch' }
$helper=Join-Path $PSScriptRoot 'NinzaPiPiHelper.ps1';$helperPin=Digest $helper
[void][IO.Directory]::CreateDirectory($root)
$workerCopy=Join-Path $root 'worker.ps1';$helperCopy=Join-Path $root 'helper.ps1'
[IO.File]::Copy($worker,$workerCopy,$false);[IO.File]::Copy($helper,$helperCopy,$false)
if ((Digest $workerCopy) -cne $pin -or (Digest $helperCopy) -cne $helperPin) { throw 'Copy pin mismatch' }
$pwsh=Join-Path $PSHOME $(if ($IsWindows) {'pwsh.exe'} else {'pwsh'})
$ledgerPath=Join-Path $root 'JSONLedger.json'
$history=[Collections.Generic.List[object]]::new();$active=[ordered]@{}
$ledger=[ordered]@{
 protocol='NINZA-PIPI-SUPERVISOR-v1';run_id=[guid]::NewGuid().ToString('N')
 state='RUNNING';started_utc=(Utc);updated_utc=(Utc);worker_sha256=$pin;helper_sha256=$helperPin
 workers=$Workers;idle_ttl_seconds=$IdleTtlSeconds;heartbeat_ttl_seconds=$HeartbeatTtlSeconds
 startup_ttl_seconds=$StartupTtlSeconds;max_attempt_seconds=$MaxAttemptSeconds;max_attempts=$MaxAttempts
 arbitration='ALTERNATE_WHEN_BOTH_READY_V1';retry_policy='NEW_ADMISSION_SAME_LANE_V1'
 dispatch_count=0;spawn_count=0;jobs=$jobs;helpers=$history
}
function Save-Ledger { $ledger.updated_utc=Utc;Publish $ledgerPath $ledger $true }
function Stop-Child($p) {
 try { if (-not $p.HasExited) {$p.Kill($true)};return $p.WaitForExit(5000) }
 catch { try {return $p.HasExited} catch {return $false} }
}
function Spawn {
 if ((Digest $workerCopy) -cne $pin -or (Digest $helperCopy) -cne $helperPin) { throw 'Pinned source changed' }
 $ledger.spawn_count++
 $id='helper-{0:D5}' -f $ledger.spawn_count;$dir=Join-Path $root $id
 [void][IO.Directory]::CreateDirectory($dir)
 $note=[pscustomobject][ordered]@{helper_id=$id;pid=$null;state='SPAWNING';spawned_utc=(Utc);ended_utc=$null;reason=$null;exit_code=$null;mailbox=$dir}
 $history.Add($note);Save-Ledger
 $psi=[Diagnostics.ProcessStartInfo]::new();$psi.FileName=$pwsh;$psi.UseShellExecute=$false;$psi.WorkingDirectory=$dir
 foreach ($arg in @('-NoLogo','-NoProfile','-NonInteractive','-ExecutionPolicy','Bypass','-File',$helperCopy,
 '-Mailbox',$dir,'-WorkerPath',$workerCopy,'-WorkerSha256',$pin,'-HelperId',$id,'-IdleTtlSeconds',"$IdleTtlSeconds",'-PollMilliseconds',"$PollMilliseconds")) { $psi.ArgumentList.Add($arg) }
 $p=[Diagnostics.Process]::Start($psi)
 if ($null -eq $p) { throw 'Helper launch failed' }
 $active[$id]=@{process=$p;note=$note;clock=[Diagnostics.Stopwatch]::StartNew();job=$null;attempt=$null;last_ms=0L}
 $note.pid=$p.Id
}
function Fail-Attempt($a,[string]$Reason,[bool]$AllowRetry) {
 $j=$a.job;$t=$a.attempt;$t.state='FAILED';$t.reason=$Reason;$t.ended_utc=Utc
 if ($AllowRetry -and $j.attempts.Count -lt $MaxAttempts) {
  $j.state='RETRY_READY'
  if ($j.lane -ceq 'FIFONFU') {$fifo.Enqueue($j)} else {$lifo.Push($j)}
 } else {$j.state=if ($Reason -eq 'HELP_REQUIRED') {'HELP_REQUIRED'} else {'HOLD'}}
}
function Retire($id,$a,[string]$Reason) {
 $a.note.state='RETIRED';$a.note.reason=$Reason;$a.note.ended_utc=Utc;$a.note.exit_code=$a.process.ExitCode
 $a.process.Dispose();$active.Remove($id)
}
$nextLane='FIFONFU';$spawnLimit=2*$Workers+$jobs.Count*$MaxAttempts
Save-Ledger;Write-Host "Parent ledger: $ledgerPath"
try {
 for ($i=0;$i -lt $Workers;$i++) { Spawn }
 while ($active.Count -gt 0 -or $fifo.Count+$lifo.Count -gt 0) {
  $dirty=$false
  foreach ($id in @($active.Keys)) {
   $a=$active[$id];$p=$a.process;$dir=$a.note.mailbox;$elapsed=$a.clock.ElapsedMilliseconds
   if ($null -ne $a.job) {
    $j=$a.job;$t=$a.attempt;$ep=Join-Path $dir 'emergency.json'
    if ([IO.File]::Exists($ep)) {
     $detail='INVALID_EMERGENCY_RECORD'
     try {
      $e=Read-Json $ep
      if ($e.protocol -ceq 'NINZA-LIFODFU-EMERGENCY-v1' -and $e.job_id -ceq $j.job_id -and $e.attempt -eq $t.attempt -and
          $e.reason -is [string] -and $e.reason -cmatch '^[A-Z0-9_]{1,64}$') {$detail=$e.reason}
     } catch {}
     $t.emergency_reason=$detail
     if (-not (Stop-Child $p)) {throw 'Emergency stop unconfirmed; no respawn'}
     $t.exit_code=$p.ExitCode;Fail-Attempt $a 'HELP_REQUIRED' $false;Retire $id $a 'HELP_REQUIRED';$dirty=$true;continue
    }
    if ($p.HasExited) {
     $t.exit_code=$p.ExitCode;$valid=$false;$hash=$null
     if ($t.exit_code -eq 42) {
      try {
       $r=Read-Json (Join-Path $dir 'receipt.json');$out=Join-Path $dir 'output.bin'
       if ($r.protocol -ceq 'NINZA-FIFONFU-RECEIPT-v1' -and $r.job_id -ceq $j.job_id -and
        $r.attempt -eq $t.attempt -and $r.status -ceq 'COMPLETE' -and $r.output_sha256 -is [string] -and
        $r.output_sha256 -cmatch '^[0-9a-f]{64}$' -and [IO.File]::Exists($out)) {$hash=Digest $out;$valid=($hash -ceq $r.output_sha256)}
      } catch {}
     }
     if ($valid) {
      $t.state='COMPLETE';$t.ended_utc=Utc;$j.state='COMPLETE';$j.accepted_output=@{path=$out;sha256=$hash;attempt=$t.attempt}
      Retire $id $a 'JOB_COMPLETE'
     } else {
      $reason=if ($t.exit_code -eq 42) {'INVALID_RECEIPT'} else {'NON_42_EXIT'}
      Fail-Attempt $a $reason $true;Retire $id $a $reason
     }
     $dirty=$true;continue
    }
    $hp=Join-Path $dir 'heartbeat.json'
    if ([IO.File]::Exists($hp)) {
     try {
      $h=Read-Json $hp
      if ($h.protocol -ceq 'NINZA-FIFONFU-HEARTBEAT-v1' -and $h.job_id -ceq $j.job_id -and $h.attempt -eq $t.attempt -and
       ($h.seq -is [int] -or $h.seq -is [long]) -and $h.seq -ge 0 -and $h.seq -gt $t.heartbeat_seq) {
       $t.heartbeat_seq=$h.seq;$t.heartbeat_utc=Utc;$a.last_ms=$elapsed;$dirty=$true
      }
     } catch {}
    }
    $reason=$null
    if ($elapsed -ge $MaxAttemptSeconds*1000L) {$reason='ATTEMPT_DEADLINE'}
    elseif ($elapsed-$a.last_ms -ge $HeartbeatTtlSeconds*1000L) {$reason='HEARTBEAT_TTL'}
    if ($reason) {
     if (-not (Stop-Child $p)) {throw 'Child stop unconfirmed; no respawn'}
     $t.exit_code=$p.ExitCode;Fail-Attempt $a $reason $true;Retire $id $a $reason;$dirty=$true
    }
    continue
   }
   if ($p.HasExited) {
    $idle=$false
    try {
     $r=Read-Json (Join-Path $dir 'retired.json')
     $idle=($p.ExitCode -eq 0 -and $r.protocol -ceq 'NINZA-PIPI-IDLE-v1' -and $r.helper_id -ceq $id -and $r.reason -ceq 'IDLE_TTL')
    } catch {}
    if (-not $idle) {throw 'Unleased helper exited without idle receipt'}
    Retire $id $a 'IDLE_TTL';$dirty=$true;continue
   }
   if ($a.note.state -eq 'SPAWNING') {
    try {
     $r=Read-Json (Join-Path $dir 'ready.json')
     if ($r.protocol -ceq 'NINZA-PIPI-READY-v1' -and $r.helper_id -ceq $id) {$a.note.state='IDLE';$a.clock.Restart();$elapsed=0;$dirty=$true}
    } catch {}
    if ($a.note.state -eq 'SPAWNING' -and $elapsed -ge $StartupTtlSeconds*1000L) {throw 'Helper startup TTL expired'}
   }
   if ($a.note.state -eq 'IDLE' -and $fifo.Count+$lifo.Count -gt 0) {
    if ($fifo.Count -gt 0 -and ($lifo.Count -eq 0 -or $nextLane -eq 'FIFONFU')) {$j=$fifo.Dequeue();$nextLane='LIFODFU'}
    else {$j=$lifo.Pop();$nextLane='FIFONFU'}
    $ledger.dispatch_count++
    $t=[pscustomobject][ordered]@{attempt=($j.attempts.Count+1);helper_id=$id;dispatch_seq=$ledger.dispatch_count
     state='RUNNING';start_utc=(Utc);ended_utc=$null;heartbeat_seq=-1;heartbeat_utc=$null;exit_code=$null;reason=$null;emergency_reason=$null}
    $j.attempts+=$t;$j.state='RUNNING';$a.job=$j;$a.attempt=$t;$a.note.state='LEASED'
    Save-Ledger
    $a.clock.Restart();$a.last_ms=0L
    Publish (Join-Path $dir 'lease.json') @{protocol='NINZA-PIPI-LEASE-v1';helper_id=$id;job_id=$j.job_id;attempt=$t.attempt}
   } elseif ($a.note.state -eq 'IDLE' -and $elapsed -ge ($IdleTtlSeconds+5)*1000L) {
    if (-not (Stop-Child $p)) {throw 'Idle stop unconfirmed'}
    Retire $id $a 'IDLE_WATCHDOG';$dirty=$true
   }
  }
  if ($dirty) {Save-Ledger}
  $available=@($active.Values | Where-Object { $null -eq $_.job }).Count
  while ($fifo.Count+$lifo.Count -gt $available -and $active.Count -lt $Workers) {
   if ($ledger.spawn_count -ge $spawnLimit) {throw 'Spawn budget exhausted'}
   Spawn;$available++
  }
  if ($active.Count -gt 0) {Start-Sleep -Milliseconds $PollMilliseconds}
 }
 if (@($jobs | Where-Object state -ne 'COMPLETE').Count -eq 0) {$ledger.state='COMPLETE'} else {$ledger.state='HOLD'}
} catch {$ledger.state='HOLD';$ledger.parent_error=$_.Exception.Message}
finally {
 foreach ($id in @($active.Keys)) {
  $a=$active[$id];$stopped=Stop-Child $a.process
  $reason=if ($stopped) {'PARENT_STOP'} else {'STOP_UNCONFIRMED'}
  if ($null -ne $a.job) {Fail-Attempt $a $reason $false}
  $a.note.state='HOLD';$a.note.reason=$reason;$a.note.ended_utc=Utc
  if ($stopped) {$a.note.exit_code=$a.process.ExitCode}
  $a.process.Dispose()
 }
 foreach ($j in $jobs) {if ($j.state -cnotin @('COMPLETE','HELP_REQUIRED')) {$j.state='HOLD'}}
 Save-Ledger
}
if ($ledger.state -ceq 'COMPLETE') {exit 42}
exit 1
