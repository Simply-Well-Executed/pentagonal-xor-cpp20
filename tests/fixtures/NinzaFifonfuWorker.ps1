#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
# Synthetic worker only; never consumes audio or network data.
param([string]$JobId,[int]$Attempt,[string]$HeartbeatPath,[string]$ReceiptPath,[string]$OutputPath)
$ErrorActionPreference='Stop'
function Publish([string]$Path,$Object) {
    $tmp=$Path+'.tmp'
    [IO.File]::WriteAllText($tmp,($Object | ConvertTo-Json -Compress),[Text.UTF8Encoding]::new($false))
    [IO.File]::Move($tmp,$Path,$true)
}
function Beat([long]$Sequence) {
    Publish $HeartbeatPath @{protocol='NINZA-FIFONFU-HEARTBEAT-v1';job_id=$JobId;attempt=$Attempt;seq=$Sequence}
}
Beat 0
if ($JobId -eq 'retry' -and $Attempt -eq 1) { exit 7 }
if ($JobId -eq 'stall' -and $Attempt -eq 1) { Start-Sleep -Seconds 60; exit 7 }
if ($JobId -eq 'permanent') { exit 9 }
if ($JobId -eq 'missing-receipt') { exit 42 }
if ($JobId -eq 'deadline' -and $Attempt -eq 1) {
    for ($i=1; $i -le 100; $i++) { Start-Sleep -Milliseconds 200; Beat $i }
}
[IO.File]::WriteAllText($OutputPath,("fixture "+$JobId),[Text.UTF8Encoding]::new($false))
$sha=(Get-FileHash -LiteralPath $OutputPath -Algorithm SHA256).Hash.ToLowerInvariant()
Publish $ReceiptPath @{protocol='NINZA-FIFONFU-RECEIPT-v1';job_id=$JobId;attempt=$Attempt;status='COMPLETE';output_sha256=$sha}
exit 42
