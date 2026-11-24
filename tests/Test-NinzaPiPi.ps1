#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent $PSScriptRoot
$supervisor=Join-Path $repo 'tools/Invoke-NinzaPiPi.ps1'
$worker=Join-Path $PSScriptRoot 'fixtures/NinzaPiPiWorker.ps1'
$pin=(Get-FileHash -LiteralPath $worker -Algorithm SHA256).Hash
$pwsh=Join-Path $PSHOME $(if ($IsWindows) {'pwsh.exe'} else {'pwsh'})
$root=Join-Path ([IO.Path]::GetTempPath()) ('ninza-pipi-'+[guid]::NewGuid().ToString('N'))
[void][IO.Directory]::CreateDirectory($root)
function Require([bool]$Condition,[string]$Reason) {if (-not $Condition) {throw $Reason}}
function Run-Batch([string]$Name,$Entries,[int]$Workers,[int]$Expected) {
 $manifest=Join-Path $root ($Name+'.json');$run=Join-Path $root $Name
 @{protocol='NINZA-PIPI-READY-v1';retry_safe=$true;jobs=$Entries} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $manifest -Encoding utf8
 & $pwsh -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File $supervisor -ManifestPath $manifest -WorkerPath $worker -WorkerSha256 $pin -RunDirectory $run -Workers $Workers -MaxAttempts 2 -IdleTtlSeconds 8 -HeartbeatTtlSeconds 5 -MaxAttemptSeconds 12 -PollMilliseconds 100 | Out-Host
 Require ($LASTEXITCODE -eq $Expected) ('Wrong exit: '+$Name)
 Get-Content -LiteralPath (Join-Path $run 'JSONLedger.json') -Raw | ConvertFrom-Json
}
try {
 $l=Run-Batch 'order' @(@{job_id='f1';lane='FIFONFU'},@{job_id='l1'},@{job_id='f2';lane='FIFONFU'},@{job_id='l2'}) 2 42
 $ordered=@(foreach ($j in $l.jobs) { [pscustomobject]@{id=$j.job_id;seq=$j.attempts[0].dispatch_seq} }) | Sort-Object seq
 Require (($ordered.id -join ',') -ceq 'f1,l2,f2,l1') 'FIFO/LIFO alternating order'
 Require ($l.state -ceq 'COMPLETE') 'Order batch incomplete'
 $l=Run-Batch 'idle' @(@{job_id='only';lane='FIFONFU'}) 3 42
 Require ($l.spawn_count -eq 3) 'Warm pool not spawned'
 Require (@($l.helpers | Where-Object reason -eq 'IDLE_TTL').Count -eq 2) 'Idle helpers did not retire'
 $l=Run-Batch 'recover' @(@{job_id='retry';lane='FIFONFU'},@{job_id='stall'}) 2 42
 Require (($l.jobs | Where-Object job_id -eq 'retry').attempts[0].reason -ceq 'NON_42_EXIT') 'Non-42 retry'
 Require (($l.jobs | Where-Object job_id -eq 'stall').attempts[0].reason -ceq 'HEARTBEAT_TTL') 'Heartbeat retry'
 foreach ($j in $l.jobs) {Require ($j.attempts.Count -eq 2 -and $j.state -ceq 'COMPLETE') 'Recovery incomplete'}
 $l=Run-Batch 'emergency' @(@{job_id='help'},@{job_id='normal';lane='FIFONFU'}) 2 1
 $j=$l.jobs | Where-Object job_id -eq 'help'
 Require ($j.state -ceq 'HELP_REQUIRED' -and $j.attempts.Count -eq 1 -and $j.attempts[0].emergency_reason -ceq 'UNRESOLVED_DEPENDENCY') 'Emergency retried or ignored'
 Require (($l.jobs | Where-Object job_id -eq 'normal').state -ceq 'COMPLETE') 'Independent job failed'
 Write-Host 'NINZA PiPi fixtures PASS'
} finally {Write-Host "Retained evidence: $root"}
