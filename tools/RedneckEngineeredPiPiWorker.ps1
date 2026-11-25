#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
# Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
# Real PiPi worker: run the pinned native C++20 renderer into a private
# per-attempt mailbox, produce a hashed ZIP image bundle, and exit 42.
# No network, no synthetic acknowledgement, no source mutation.
[CmdletBinding()]
param([string]$JobId,[int]$Attempt,[string]$HeartbeatPath,
      [string]$ReceiptPath,[string]$OutputPath,[string]$EmergencyPath)
$ErrorActionPreference='Stop'
function Publish([string]$Path,[object]$Value,[bool]$Replace=$false) {
  $tmp=$Path+'.tmp'
  [IO.File]::WriteAllText($tmp,($Value | ConvertTo-Json -Compress -Depth 6),
       [Text.UTF8Encoding]::new($false))
  [IO.File]::Move($tmp,$Path,$Replace)
}
function Help-Required([string]$Reason) {
  Publish $EmergencyPath @{
    protocol='NINZA-LIFODFU-EMERGENCY-v1'
    job_id=$JobId;attempt=$Attempt;reason=$Reason
  }
  exit 7
}
if($JobId -cnotin @('head','tail') -or $Attempt -lt 1 -or
   [string]::IsNullOrWhiteSpace($env:REDNECK_RENDER_EXE) -or
   $env:REDNECK_RENDER_SHA256 -cnotmatch '^[a-f0-9]{64}$'){
  Help-Required 'INVALID_RENDER_CONTRACT'
}
$renderer=$env:REDNECK_RENDER_EXE
if(-not [IO.File]::Exists($renderer) -or
   (Get-FileHash -LiteralPath $renderer -Algorithm SHA256).Hash.ToLowerInvariant() -cne
    $env:REDNECK_RENDER_SHA256){Help-Required 'RENDER_PIN_MISMATCH'}
$dir=Join-Path (Split-Path -Parent $OutputPath) ('images-'+$JobId)
if([IO.Directory]::Exists($dir) -or [IO.File]::Exists($dir)){
  Help-Required 'OUTPUT_ALREADY_EXISTS'
}
[void][IO.Directory]::CreateDirectory($dir)
Publish $HeartbeatPath @{
 protocol='NINZA-FIFONFU-HEARTBEAT-v1';job_id=$JobId;attempt=$Attempt;seq=0
}
& $renderer $dir
if($LASTEXITCODE -ne 0){Help-Required 'NATIVE_RENDER_FAILED'}
$names=@('bake','cook','cubemap_posx','cubemap_negx','cubemap_posy',
         'cubemap_negy','cubemap_posz','cubemap_negz')
$header=[Text.Encoding]::ASCII.GetBytes("P6"+[char]10+
        "# C5824 PENNYWORTH"+[char]10+"64 64"+[char]10+"255"+[char]10)
foreach($name in $names) {
  $path=Join-Path $dir ($name+'.ppm')
  if(-not [IO.File]::Exists($path)){Help-Required 'MISSING_FACE'}
  $bytes=[IO.File]::ReadAllBytes($path)
  if($bytes.Length -ne ($header.Length+64*64*3)){
    Help-Required 'INVALID_FACE_SIZE'
  }
  for($i=0;$i -lt $header.Length;++$i) {
    if($bytes[$i] -ne $header[$i]){Help-Required 'INVALID_FACE_HEADER'}
  }
}
# The ZIP bundle is only a delivery format, not a model proof.
[IO.Compression.ZipFile]::CreateFromDirectory(
    $dir,$OutputPath,[IO.Compression.CompressionLevel]::Optimal,$false)
if(-not [IO.File]::Exists($OutputPath)){Help-Required 'MISSING_OUTPUT'}
$sha=(Get-FileHash -LiteralPath $OutputPath -Algorithm SHA256).Hash.ToLowerInvariant()
Publish $HeartbeatPath @{
 protocol='NINZA-FIFONFU-HEARTBEAT-v1';job_id=$JobId;attempt=$Attempt;seq=1
}
Publish $ReceiptPath @{
 protocol='NINZA-FIFONFU-RECEIPT-v1';job_id=$JobId;attempt=$Attempt
 status='COMPLETE';output_sha256=$sha
}
exit 42
