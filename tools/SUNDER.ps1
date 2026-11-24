#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
# SUNDER: audited local admission for Ping. The 66 degree projection is metadata.
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
$PSNativeCommandUseErrorActionPreference=$false
function Digest([string]$Path) {
 (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function ByteDigest([byte[]]$Bytes) {
 [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($Bytes)).ToLowerInvariant()
}
function Write-New([string]$Path,[byte[]]$Bytes) {
 $s=[IO.File]::Open($Path,[IO.FileMode]::CreateNew,[IO.FileAccess]::Write,[IO.FileShare]::None)
 try {$s.Write($Bytes,0,$Bytes.Length);$s.Flush($true)} finally {$s.Dispose()}
}
$utf8=[Text.UTF8Encoding]::new($false,$true)
function Write-Json([string]$Path,$Value) {
 Write-New $Path ($utf8.GetBytes(($Value | ConvertTo-Json -Depth 12)))
}
function Read-Json([string]$Path,[long]$Limit) {
 $i=Get-Item -LiteralPath $Path
 if ($i.PSIsContainer -or $i.Length -gt $Limit) {throw 'Invalid JSON file'}
 Get-Content -LiteralPath $Path -Raw -Encoding utf8 | ConvertFrom-Json -AsHashtable -Depth 12
}
if ($MaxAttemptSeconds -lt $HeartbeatTtlSeconds -or $PollMilliseconds -ge 1000*[Math]::Min($IdleTtlSeconds,[Math]::Min($StartupTtlSeconds,$HeartbeatTtlSeconds))) {throw 'Invalid deadlines'}
$source=Get-Item -LiteralPath $ManifestPath
if ($source.PSIsContainer -or $source.Length -gt 1048576) {throw 'Invalid manifest size'}
$payload=[IO.File]::ReadAllBytes($source.FullName)
if ($payload.Length -gt 1048576) {throw 'Manifest exceeds limit'}
$manifestHash=ByteDigest $payload
$manifestText=$utf8.GetString($payload)
if ($manifestText.StartsWith([string][char]0xFEFF)) {$manifestText=$manifestText.Substring(1)}
$m=$manifestText | ConvertFrom-Json -AsHashtable -Depth 12
if ($m.protocol -cne 'NINZA-PIPI-READY-v1' -or $m.retry_safe -isnot [bool] -or -not $m.retry_safe -or
 $m.jobs -isnot [array] -or $m.jobs.Count -lt 1 -or $m.jobs.Count -gt 4096) {throw 'Explicit retry-safe PiPi manifest required'}
$expected=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
foreach ($j in $m.jobs) {
 if ($j -isnot [Collections.IDictionary] -or $j.job_id -isnot [string] -or
     $j.job_id -cnotmatch '^[a-z0-9][a-z0-9._-]{0,63}$' -or $expected.ContainsKey($j.job_id)) {throw 'Invalid job ID'}
 $lane=if ($j.Contains('lane')) {$j.lane} else {'LIFODFU'}
 if ($lane -isnot [string] -or $lane -cnotin @('FIFONFU','LIFODFU')) {throw 'Invalid lane'}
 $expected.Add($j.job_id,$lane)
}
$worker=(Get-Item -LiteralPath $WorkerPath).FullName
$pin=$WorkerSha256.ToLowerInvariant()
if ([IO.Path]::GetExtension($worker) -ine '.ps1' -or (Digest $worker) -cne $pin) {throw 'Worker pin mismatch'}
$root=[IO.Path]::GetFullPath($RunDirectory)
if ([IO.Directory]::Exists($root) -or [IO.File]::Exists($root)) {throw 'Run directory exists'}
[void][IO.Directory]::CreateDirectory($root)
$stage=Join-Path $root 'stage'
[void][IO.Directory]::CreateDirectory($stage)
$pins=[ordered]@{}
foreach ($name in @('Invoke-NinzaPiPi.ps1','NinzaPiPiHelper.ps1','SUNDER.ps1')) {
 $from=Join-Path $PSScriptRoot $name;$to=Join-Path $stage $name
 $pins[$name]=Digest $from
 [IO.File]::Copy($from,$to,$false)
 if ((Digest $to) -cne $pins[$name]) {throw 'Stage copy mismatch'}
}
$workerCopy=Join-Path $stage 'selected-worker.ps1'
[IO.File]::Copy($worker,$workerCopy,$false)
if ((Digest $workerCopy) -cne $pin) {throw 'Worker copy mismatch'}
$settings=[ordered]@{workers=$Workers;max_attempts=$MaxAttempts;idle_ttl_seconds=$IdleTtlSeconds
 heartbeat_ttl_seconds=$HeartbeatTtlSeconds;startup_ttl_seconds=$StartupTtlSeconds
 max_attempt_seconds=$MaxAttemptSeconds;poll_milliseconds=$PollMilliseconds}
$envelope=[ordered]@{
 protocol='NINZA-Ping-ENVELOPE-v1';run_id=[guid]::NewGuid().ToString('N')
 created_utc=[DateTimeOffset]::UtcNow.ToString('o')
 stages=@('WRAP','CHURN','SUNDER','RIP','INTEGRATE','REVIEW')
 projection=@{angle_degrees=66;direction='UP';kind='GEOMETRIC_PROJECTION'}
 payload=@{encoding='base64';bytes=$payload.Length;sha256=$manifestHash;data=[Convert]::ToBase64String($payload)}
 provenance=@{worker_sha256=$pin;stage_sha256=$pins;settings=$settings}
}
$wrap=Join-Path $root 'Ping-envelope.json'
Write-Json $wrap $envelope
$wrapHash=Digest $wrap
$result=[ordered]@{protocol='NINZA-Ping-RESULT-v1';run_id=$envelope.run_id
 state='HOLD';envelope_sha256=$wrapHash;manifest_sha256=$manifestHash
 supervisor_exit=$null;ledger_sha256=$null;accepted_jobs=0;review='NOT_RUN';reason=$null}
try {
 # CHURN: record the 66 degree projection; never evaluate it as code.
 # SUNDER: separate payload from immutable provenance.
 # RIP: extract verified manifest bytes without evaluating data.
 $read=Read-Json $wrap 2097152
 if ((Digest $wrap) -cne $wrapHash -or $read.protocol -cne $envelope.protocol -or
  $read.run_id -cne $envelope.run_id -or $read.projection.angle_degrees -ne 66 -or
  $read.projection.direction -cne 'UP' -or $read.projection.kind -cne 'GEOMETRIC_PROJECTION' -or
  $read.payload.encoding -cne 'base64') {throw 'Envelope mismatch'}
 $unwrapped=[Convert]::FromBase64String($read.payload.data)
 if ($unwrapped.Length -ne $payload.Length -or $read.payload.bytes -ne $payload.Length -or
  (ByteDigest $unwrapped) -cne $manifestHash -or $read.payload.sha256 -cne $manifestHash) {throw 'Payload mismatch'}
 $ready=Join-Path $stage 'ready.json'
 Write-New $ready $unwrapped
 foreach ($name in $pins.Keys) {
  if ((Digest (Join-Path $stage $name)) -cne $pins[$name]) {throw 'Stage source changed'}
 }
 if ((Digest $workerCopy) -cne $pin) {throw 'Worker source changed'}
 $nit=Join-Path $root 'NINZA'
 # INTEGRATE: invoke the approved local PiPi entry point, using named arguments.
 $pwsh=Join-Path $PSHOME $(if ($IsWindows) {'pwsh.exe'} else {'pwsh'})
 & $pwsh -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File (Join-Path $stage 'Invoke-NinzaPiPi.ps1') -ManifestPath $ready -WorkerPath $workerCopy -WorkerSha256 $pin -RunDirectory $nit -Workers $Workers -MaxAttempts $MaxAttempts -IdleTtlSeconds $IdleTtlSeconds -HeartbeatTtlSeconds $HeartbeatTtlSeconds -StartupTtlSeconds $StartupTtlSeconds -MaxAttemptSeconds $MaxAttemptSeconds -PollMilliseconds $PollMilliseconds
 $result.supervisor_exit=$LASTEXITCODE
 $ledger=Join-Path $nit 'JSONLedger.json'
 $result.ledger_sha256=Digest $ledger
 if ((Digest $wrap) -cne $wrapHash -or (Digest $ready) -cne $manifestHash -or (Digest $source.FullName) -cne $manifestHash -or (Digest $worker) -cne $pin -or (Digest $workerCopy) -cne $pin) {throw 'Source or envelope changed'}
 foreach ($name in $pins.Keys) {
  if ((Digest (Join-Path $stage $name)) -cne $pins[$name]) {throw 'Stage source changed after run'}
 }
 $l=Read-Json $ledger 100663296
 if ($l.protocol -cne 'NINZA-PIPI-SUPERVISOR-v1' -or $l.worker_sha256 -cne $pin -or
  $l.helper_sha256 -cne $pins['NinzaPiPiHelper.ps1'] -or $l.jobs.Count -ne $expected.Count) {throw 'Ledger identity mismatch'}
 $seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
 foreach ($j in $l.jobs) {
  if (-not $expected.ContainsKey($j.job_id) -or -not $seen.Add($j.job_id) -or $j.lane -cne $expected[$j.job_id]) {throw 'Ledger job mismatch'}
  if ($j.state -cne 'COMPLETE') {continue}
  $a=$j.attempts[-1];$out=$j.accepted_output
  if ($a.helper_id -cnotmatch '^helper-[0-9]{5,}$' -or $a.state -cne 'COMPLETE' -or $a.exit_code -ne 42 -or
   $a.attempt -ne $out.attempt) {throw 'Completion record mismatch'}
  $mail=Join-Path $nit $a.helper_id;$file=Join-Path $mail 'output.bin'
  if ([IO.Path]::GetFullPath($out.path) -cne [IO.Path]::GetFullPath($file)) {throw 'Output path mismatch'}
  $r=Read-Json (Join-Path $mail 'receipt.json') 65536
  if ($out.sha256 -cnotmatch '^[0-9a-f]{64}$' -or (Digest $file) -cne $out.sha256 -or
   $r.protocol -cne 'NINZA-FIFONFU-RECEIPT-v1' -or $r.job_id -cne $j.job_id -or
   $r.attempt -ne $a.attempt -or $r.status -cne 'COMPLETE' -or $r.output_sha256 -cne $out.sha256) {throw 'Receipt mismatch'}
  $result.accepted_jobs++
 }
 if ((Digest $ledger) -cne $result.ledger_sha256) {throw 'Ledger changed during review'}
 $result.review='LOCAL_CONSISTENCY'
 if ($result.supervisor_exit -eq 42 -and $l.state -ceq 'COMPLETE' -and $result.accepted_jobs -eq $expected.Count) {
  $result.state='COMPLETE'
 } else {$result.reason='SUPERVISOR_HOLD_OR_INCOMPLETE'}
} catch {$result.reason=$_.Exception.Message}
finally {Write-Json (Join-Path $root 'Ping-result.json') $result}
Write-Host "SUNDER: $($result.state) | $root"
if ($result.state -ceq 'COMPLETE') {exit 42}
exit 1
