#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
# FIFONFU supervised ready batches, EXPLORATORY. No network operations.
[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$ManifestPath,
    [Parameter(Mandatory)][string]$WorkerPath,
    [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{64}$')][string]$WorkerSha256,
    [Parameter(Mandatory)][string]$RunDirectory,
    [ValidateRange(1,64)][int]$Workers = 4,
    [ValidateRange(1,10)][int]$MaxAttempts = 3,
    [ValidateRange(1,3600)][int]$HeartbeatTtlSeconds = 10,
    [ValidateRange(1,86400)][int]$MaxAttemptSeconds = 300,
    [ValidateRange(50,2000)][int]$PollMilliseconds = 200
)
$ErrorActionPreference = 'Stop'
if ($MaxAttemptSeconds -lt $HeartbeatTtlSeconds -or
    $PollMilliseconds -ge ($HeartbeatTtlSeconds * 1000)) {
    throw 'Invalid supervision deadlines'
}
function Read-Object([string]$Path, [long]$Limit) {
    $item = Get-Item -LiteralPath $Path
    if ($item.PSIsContainer -or $item.Length -gt $Limit) { throw 'Invalid JSON size/type' }
    Get-Content -LiteralPath $Path -Raw -Encoding utf8 | ConvertFrom-Json -AsHashtable -Depth 12
}
function Get-Digest([string]$Path) {
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function Get-Utc { [DateTimeOffset]::UtcNow.ToString('o') }
$manifest = Read-Object $ManifestPath 1048576
if ($manifest.protocol -cne 'NINZA-FIFONFU-READY-v1' -or
    $manifest.retry_safe -isnot [bool] -or -not $manifest.retry_safe -or
    $manifest.job_ids -isnot [array]) { throw 'Explicit retry-safe ready manifest required' }
$ids = @($manifest.job_ids)
if ($ids.Count -lt 1 -or $ids.Count -gt 4096) { throw 'Invalid ready-set size' }
$seen = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach ($id in $ids) {
    if ($id -isnot [string] -or $id -cnotmatch '^[a-z0-9][a-z0-9._-]{0,63}$' -or
        -not $seen.Add($id)) { throw 'Invalid or duplicate job ID' }
}
$worker = (Get-Item -LiteralPath $WorkerPath).FullName
if ([IO.Path]::GetExtension($worker) -ine '.ps1' -or
    (Get-Digest $worker) -ne $WorkerSha256.ToLowerInvariant()) { throw 'Worker pin mismatch' }
$root = [IO.Path]::GetFullPath($RunDirectory)
if ([IO.Directory]::Exists($root) -or [IO.File]::Exists($root)) { throw 'Run directory already exists' }
[void][IO.Directory]::CreateDirectory($root)
$workerCopy = Join-Path $root 'pinned-worker.ps1'
[IO.File]::Copy($worker,$workerCopy,$false)
if ((Get-Digest $workerCopy) -ne $WorkerSha256.ToLowerInvariant()) { throw 'Copied worker pin mismatch' }
$pwsh = Join-Path $PSHOME $(if ($IsWindows) { 'pwsh.exe' } else { 'pwsh' })
$ledgerPath = Join-Path $root 'JSONLedger.json'
$queue = [Collections.Generic.Queue[object]]::new()
$active = @{}
$jobs = @(
    foreach ($id in $ids) {
        $j = [pscustomobject][ordered]@{job_id=$id; state='READY'; attempts=@(); accepted_output=$null}
        $queue.Enqueue($j)
        $j
    }
)
$ledger = [ordered]@{
    protocol='NINZA-FIFONFU-SUPERVISOR-v1'; run_id=[guid]::NewGuid().ToString('N')
    state='RUNNING'; started_utc=(Get-Utc); updated_utc=(Get-Utc)
    worker_sha256=$WorkerSha256.ToLowerInvariant(); ready_order=$ids
    retry_order='TAIL_OF_READY_QUEUE_V1'; success_exit=42
    workers=$Workers; max_attempts=$MaxAttempts; heartbeat_ttl_seconds=$HeartbeatTtlSeconds
    max_attempt_seconds=$MaxAttemptSeconds; jobs=$jobs
}
function Save-Ledger {
    $ledger.updated_utc = Get-Utc
    $tmp = Join-Path $root ('.ledger-' + [guid]::NewGuid().ToString('N') + '.tmp')
    try {
        $bytes = [Text.UTF8Encoding]::new($false).GetBytes(($ledger | ConvertTo-Json -Depth 12))
        $stream = [IO.File]::Open($tmp,[IO.FileMode]::CreateNew,[IO.FileAccess]::Write,[IO.FileShare]::None)
        try { $stream.Write($bytes,0,$bytes.Length); $stream.Flush($true) } finally { $stream.Dispose() }
        [IO.File]::Move($tmp,$ledgerPath,$true)
    } finally {
        if ([IO.File]::Exists($tmp)) { [IO.File]::Delete($tmp) }
    }
}
function Stop-Child([Diagnostics.Process]$Process) {
    try {
        if (-not $Process.HasExited) { $Process.Kill($true) }
        return $Process.WaitForExit(5000)
    } catch {
        try { return $Process.HasExited } catch { return $false }
    }
}
function Retry-Or-Hold($Job,$Attempt,[string]$Reason) {
    $Attempt.state='FAILED'; $Attempt.reason=$Reason; $Attempt.ended_utc=Get-Utc
    if ($Job.attempts.Count -lt $MaxAttempts) {
        $Job.state='RETRY_READY'; $queue.Enqueue($Job)
    } else { $Job.state='HOLD' }
}
Save-Ledger
Write-Host "Parent ledger: $ledgerPath"
try {
    while ($queue.Count -gt 0 -or $active.Count -gt 0) {
        while ($queue.Count -gt 0 -and $active.Count -lt $Workers) {
            $job=$queue.Dequeue()
            $number=$job.attempts.Count+1
            $dir=Join-Path $root ("job-{0}-attempt-{1:D2}" -f $job.job_id,$number)
            [void][IO.Directory]::CreateDirectory($dir)
            $heartbeat=Join-Path $dir 'heartbeat.json'
            $receipt=Join-Path $dir 'receipt.json'
            $output=Join-Path $dir 'output.bin'
            $attempt=[pscustomobject][ordered]@{
                attempt=$number; state='STARTING'; pid=$null; start_utc=(Get-Utc)
                heartbeat_seq=-1; heartbeat_utc=$null; exit_code=$null
                reason=$null; ended_utc=$null; directory=$dir
            }
            $job.attempts += $attempt
            $job.state='RUNNING'
            Save-Ledger
            if ((Get-Digest $workerCopy) -ne $ledger.worker_sha256) {
                $job.state='HOLD'; $attempt.state='HOLD'; $attempt.reason='WORKER_PIN_CHANGED'
                throw 'Pinned worker changed during supervision'
            }
            $psi=[Diagnostics.ProcessStartInfo]::new()
            $psi.FileName=$pwsh; $psi.UseShellExecute=$false; $psi.WorkingDirectory=$dir
            foreach ($arg in @('-NoLogo','-NoProfile','-NonInteractive','-ExecutionPolicy','Bypass',
                '-File',$workerCopy,'-JobId',$job.job_id,'-Attempt',[string]$number,
                '-HeartbeatPath',$heartbeat,'-ReceiptPath',$receipt,'-OutputPath',$output)) {
                $psi.ArgumentList.Add($arg)
            }
            $process=$null
            try {
                $process=[Diagnostics.Process]::Start($psi)
                if ($null -eq $process) { throw 'Process start returned null' }
            } catch {
                Retry-Or-Hold $job $attempt 'LAUNCH_FAILED'
                Save-Ledger
                continue
            }
            $clock=[Diagnostics.Stopwatch]::StartNew()
            $active[$job.job_id]=@{job=$job; attempt=$attempt; process=$process; clock=$clock
                last_ms=[long]0; heartbeat=$heartbeat; receipt=$receipt; output=$output}
            $attempt.pid=$process.Id; $attempt.state='RUNNING'
            Save-Ledger
        }
        foreach ($key in @($active.Keys)) {
            $a=$active[$key]; $p=$a.process; $j=$a.job; $t=$a.attempt
            $elapsed=$a.clock.ElapsedMilliseconds
            if ($p.HasExited) {
                $t.exit_code=$p.ExitCode
                $valid=$false; $hash=$null
                if ($t.exit_code -eq 42) {
                    try {
                        $r=Read-Object $a.receipt 65536
                        if ($r.protocol -ceq 'NINZA-FIFONFU-RECEIPT-v1' -and
                            $r.job_id -ceq $j.job_id -and $r.attempt -eq $t.attempt -and
                            $r.status -ceq 'COMPLETE' -and
                            $r.output_sha256 -is [string] -and $r.output_sha256 -cmatch '^[0-9a-f]{64}$' -and
                            [IO.File]::Exists($a.output)) {
                            $hash=Get-Digest $a.output
                            $valid=($hash -ceq $r.output_sha256)
                        }
                    } catch { $valid=$false }
                }
                if ($valid) {
                    $t.state='COMPLETE'; $t.ended_utc=Get-Utc
                    $j.state='COMPLETE'; $j.accepted_output=@{path=$a.output; sha256=$hash; attempt=$t.attempt}
                } else {
                    $reason=if ($t.exit_code -eq 42) { 'INVALID_RECEIPT' } else { 'NON_42_EXIT' }
                    Retry-Or-Hold $j $t $reason
                }
                $p.Dispose(); $active.Remove($key)
                Save-Ledger
                continue
            }
            if ([IO.File]::Exists($a.heartbeat)) {
                try {
                    $h=Read-Object $a.heartbeat 65536
                    if ($h.protocol -ceq 'NINZA-FIFONFU-HEARTBEAT-v1' -and
                        $h.job_id -ceq $j.job_id -and $h.attempt -eq $t.attempt -and
                        ($h.seq -is [long] -or $h.seq -is [int]) -and
                        $h.seq -ge 0 -and $h.seq -gt $t.heartbeat_seq) {
                        $t.heartbeat_seq=$h.seq; $t.heartbeat_utc=Get-Utc
                        $a.last_ms=$elapsed
                        Save-Ledger
                    }
                } catch {
                    # Malformed or concurrently replaced heartbeat never extends TTL.
                }
            }
            $reason=$null
            if ($elapsed -ge ($MaxAttemptSeconds * 1000L)) { $reason='ATTEMPT_DEADLINE' }
            elseif (($elapsed-$a.last_ms) -ge ($HeartbeatTtlSeconds * 1000L)) { $reason='HEARTBEAT_TTL' }
            if ($reason) {
                if (-not (Stop-Child $p)) {
                    $j.state='HOLD'; $t.state='HOLD'; $t.reason='STOP_UNCONFIRMED'
                    throw 'Cannot confirm child stopped; no respawn'
                }
                $t.exit_code=$p.ExitCode
                Retry-Or-Hold $j $t $reason
                $p.Dispose(); $active.Remove($key)
                Save-Ledger
            }
        }
        if ($active.Count -gt 0) { Start-Sleep -Milliseconds $PollMilliseconds }
    }
    if (@($jobs | Where-Object state -ne 'COMPLETE').Count -eq 0) { $ledger.state='COMPLETE' }
    else { $ledger.state='HOLD' }
} catch {
    $ledger.state='HOLD'
    $ledger.parent_error=$_.Exception.Message
} finally {
    foreach ($a in @($active.Values)) {
        $stopped=Stop-Child $a.process
        $a.job.state='HOLD'; $a.attempt.state='HOLD'
        $a.attempt.reason=if ($stopped) { 'PARENT_STOP' } else { 'STOP_UNCONFIRMED' }
        $a.attempt.ended_utc=Get-Utc
        $a.process.Dispose()
    }
    if ($ledger.state -ne 'COMPLETE') {
        foreach ($j in $jobs) {
            if ($j.state -ne 'COMPLETE') { $j.state='HOLD' }
        }
    }
    Save-Ledger
}
if ($ledger.state -eq 'COMPLETE') { exit 42 }
exit 1
