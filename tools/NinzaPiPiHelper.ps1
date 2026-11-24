#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
param([string]$Mailbox,[string]$WorkerPath,[string]$WorkerSha256,[string]$HelperId,
      [int]$IdleTtlSeconds=15,[int]$PollMilliseconds=200)
$ErrorActionPreference='Stop'
function Publish([string]$Path,$Value) {
    $tmp=$Path+'.tmp'
    [IO.File]::WriteAllText($tmp,($Value | ConvertTo-Json -Compress),[Text.UTF8Encoding]::new($false))
    [IO.File]::Move($tmp,$Path,$false)
}
function Digest([string]$Path) { (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
if ((Digest $WorkerPath) -cne $WorkerSha256) { exit 1 }
$leasePath=Join-Path $Mailbox 'lease.json'
$clock=[Diagnostics.Stopwatch]::StartNew()
Publish (Join-Path $Mailbox 'ready.json') @{protocol='NINZA-PIPI-READY-v1';helper_id=$HelperId}
while (-not [IO.File]::Exists($leasePath)) {
    if ($clock.ElapsedMilliseconds -ge $IdleTtlSeconds * 1000L) {
        Publish (Join-Path $Mailbox 'retired.json') @{protocol='NINZA-PIPI-IDLE-v1';helper_id=$HelperId;reason='IDLE_TTL'}
        exit 0
    }
    Start-Sleep -Milliseconds $PollMilliseconds
}
try {
    $lease=Get-Content -LiteralPath $leasePath -Raw -Encoding utf8 | ConvertFrom-Json -AsHashtable
    if ($lease.protocol -cne 'NINZA-PIPI-LEASE-v1' -or $lease.helper_id -cne $HelperId -or
        $lease.job_id -cnotmatch '^[a-z0-9][a-z0-9._-]{0,63}$' -or
        $lease.attempt -lt 1 -or $lease.attempt -gt 10 -or (Digest $WorkerPath) -cne $WorkerSha256) { exit 1 }
    $argsForWorker=@{JobId=$lease.job_id;Attempt=$lease.attempt
        HeartbeatPath=(Join-Path $Mailbox 'heartbeat.json');ReceiptPath=(Join-Path $Mailbox 'receipt.json')
        OutputPath=(Join-Path $Mailbox 'output.bin');EmergencyPath=(Join-Path $Mailbox 'emergency.json')}
    $global:LASTEXITCODE=0
    & $WorkerPath @argsForWorker
    exit $LASTEXITCODE
} catch { exit 1 }
