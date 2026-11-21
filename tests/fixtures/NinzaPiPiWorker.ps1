#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
param([string]$JobId,[int]$Attempt,[string]$HeartbeatPath,[string]$ReceiptPath,[string]$OutputPath,[string]$EmergencyPath)
$ErrorActionPreference='Stop'
function Publish([string]$Path,$Value) {
 $tmp=$Path+'.tmp'
 [IO.File]::WriteAllText($tmp,($Value | ConvertTo-Json -Compress),[Text.UTF8Encoding]::new($false))
 [IO.File]::Move($tmp,$Path,$true)
}
Publish $HeartbeatPath @{protocol='NINZA-FIFONFU-HEARTBEAT-v1';job_id=$JobId;attempt=$Attempt;seq=0}
if ($JobId -eq 'help') {
 Publish $EmergencyPath @{protocol='NINZA-LIFODFU-EMERGENCY-v1';job_id=$JobId;attempt=$Attempt;reason='UNRESOLVED_DEPENDENCY'}
 exit 7
}
if ($JobId -eq 'retry' -and $Attempt -eq 1) {exit 7}
if ($JobId -eq 'stall' -and $Attempt -eq 1) {Start-Sleep -Seconds 60;exit 7}
[IO.File]::WriteAllText($OutputPath,$JobId,[Text.UTF8Encoding]::new($false))
$sha=(Get-FileHash -LiteralPath $OutputPath -Algorithm SHA256).Hash.ToLowerInvariant()
Publish $ReceiptPath @{protocol='NINZA-FIFONFU-RECEIPT-v1';job_id=$JobId;attempt=$Attempt;status='COMPLETE';output_sha256=$sha}
exit 42
