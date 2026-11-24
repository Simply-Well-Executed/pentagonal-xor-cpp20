#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent $PSScriptRoot
$supervisor=Join-Path $repo 'tools/Invoke-NinzaFifonfu.ps1'
$worker=Join-Path $PSScriptRoot 'fixtures/NinzaFifonfuWorker.ps1'
$sha=(Get-FileHash -LiteralPath $worker -Algorithm SHA256).Hash
$pwsh=Join-Path $PSHOME $(if ($IsWindows) {'pwsh.exe'} else {'pwsh'})
$root=Join-Path ([IO.Path]::GetTempPath()) ('ninza-fifonfu-'+[guid]::NewGuid().ToString('N'))
[void][IO.Directory]::CreateDirectory($root)
function Require([bool]$Condition,[string]$Reason) { if (-not $Condition) { throw $Reason } }
try {
    $manifest=Join-Path $root 'manifest.json'
    $run=Join-Path $root 'recover'
    @{protocol='NINZA-FIFONFU-READY-v1';retry_safe=$true;job_ids=@('success','retry','stall','deadline')} |
        ConvertTo-Json | Set-Content -LiteralPath $manifest -Encoding utf8
    & $pwsh -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File $supervisor -ManifestPath $manifest -WorkerPath $worker -WorkerSha256 $sha -RunDirectory $run -Workers 4 -MaxAttempts 2 -HeartbeatTtlSeconds 5 -MaxAttemptSeconds 10 -PollMilliseconds 100
    Require ($LASTEXITCODE -eq 42) 'Recovered batch did not return 42'
    $l=Get-Content -LiteralPath (Join-Path $run 'JSONLedger.json') -Raw | ConvertFrom-Json
    Require ($l.state -ceq 'COMPLETE') 'Recovered ledger state'
    Require (($l.jobs | Where-Object job_id -eq 'success').attempts.Count -eq 1) 'Success retried'
    foreach ($pair in @(@('retry','NON_42_EXIT'),@('stall','HEARTBEAT_TTL'),@('deadline','ATTEMPT_DEADLINE'))) {
        $j=$l.jobs | Where-Object job_id -eq $pair[0]
        Require ($j.attempts.Count -eq 2 -and $j.attempts[0].reason -ceq $pair[1] -and $j.attempts[1].exit_code -eq 42) ('Recovery failure: '+$pair[0])
    }
    $run=Join-Path $root 'hold'
    @{protocol='NINZA-FIFONFU-READY-v1';retry_safe=$true;job_ids=@('permanent','missing-receipt')} |
        ConvertTo-Json | Set-Content -LiteralPath $manifest -Encoding utf8
    & $pwsh -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File $supervisor -ManifestPath $manifest -WorkerPath $worker -WorkerSha256 $sha -RunDirectory $run -Workers 2 -MaxAttempts 2 -HeartbeatTtlSeconds 5 -MaxAttemptSeconds 10 -PollMilliseconds 100
    Require ($LASTEXITCODE -ne 42) 'Failed batch returned success'
    $l=Get-Content -LiteralPath (Join-Path $run 'JSONLedger.json') -Raw | ConvertFrom-Json
    Require ($l.state -ceq 'HOLD') 'Failed ledger state'
    foreach ($j in $l.jobs) { Require ($j.state -ceq 'HOLD' -and $j.attempts.Count -eq 2) 'Retry bound' }
    Require (($l.jobs | Where-Object job_id -eq 'missing-receipt').attempts[0].reason -ceq 'INVALID_RECEIPT') 'Exit 42 alone accepted'
    Write-Host 'NINZA FIFONFU supervision fixtures PASS'
} finally {
    # Retain evidence rather than removing failed or completed run ledgers.
    Write-Host "Fixture evidence: $root"
}
