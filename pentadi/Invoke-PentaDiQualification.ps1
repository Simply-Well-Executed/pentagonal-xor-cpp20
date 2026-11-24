# SPDX-License-Identifier: 0BSD
# PENTADI-001: fifth lane. Invoked only AFTER successful QuadDi job.
# Tested C11 => guarded 9-line TSV => optional licensed DCC32 Pascal runner.
# Always generate REAL MSVC /FA ASM from the clean C11 source, with no
# executable published. Delphi disassembly gets a DIFFERENT provenance label.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [string]$DelphiDcc32Path='',
 [string]$DelphiDcc32Sha256='',
 [switch]$RequireDelphiCompiler
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'PENTADI|HOLD|Windows_native_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$ev=[IO.Path]::GetFullPath($EvidenceDirectory)
if($ev -ceq $root -or
   $ev.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'PENTADI|HOLD|evidence_inside_checkout'
}
if([IO.Directory]::Exists($ev) -and @(Get-ChildItem -LiteralPath $ev -Force).Count -gt 0){
 throw 'PENTADI|HOLD|evidence_already_present'
}
[IO.Directory]::CreateDirectory($ev)|Out-Null
$csrc=Join-Path $root 'pentadi/PentaDiC11BranchRecords.c'
$unit=Join-Path $root 'pentadi/BranchPinToTsht.pas'
$program=Join-Path $root 'pentadi/Delphi007PinPipe.dpr'
$reference=Join-Path $root 'pentadi/PentaDiRel8_reference.asm'
$sources=@($csrc,$unit,$program,$reference,
 (Join-Path $root 'shared/bitflying_c_abi_001.h'),$PSCommandPath)
foreach($src in $sources){
 if(-not [IO.File]::Exists($src)){throw ('PENTADI|HOLD|missing_source|'+$src)}
}
$before=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
$pascalText=[IO.File]::ReadAllText($unit)
if(-not $pascalText.Contains('{$R+}',[StringComparison]::Ordinal) -or
   -not $pascalText.Contains('{$Q+}',[StringComparison]::Ordinal) -or
   -not $pascalText.Contains('{$I+}',[StringComparison]::Ordinal)){
 throw 'PENTADI|HOLD|range_overflow_io_checks_not_enabled'
}
$hashes=[regex]::Matches($pascalText,'Hash16:\$[0-9A-Fa-f]{4};')
if($hashes.Count -ne 16){throw 'PENTADI|HOLD|hash_partition_count'}
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'PENTADI|HOLD|MSVC_locator_missing'}
$installed=@(& $vswhere -latest -products '*' -requires 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($installed.Count -ne 1 -or -not $installed[0]){
 throw 'PENTADI|HOLD|MSVC_not_unique'
}
& (Join-Path $installed[0] 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=(Get-Command cl.exe -ErrorAction Stop).Source
$ml64=(Get-Command ml64.exe -ErrorAction Stop).Source
$dumpbin=(Get-Command dumpbin.exe -ErrorAction Stop).Source
$listing=Join-Path $ev 'PentaDi_C11_MSVC_O2.asm'
$cobj=Join-Path $ev 'PentaDi_C11.obj'
$masmObj=Join-Path $ev 'PentaDiRel8_reference.obj'
$transient=Join-Path $ev 'PentaDi_C11_TEMP_EXECUTE_ONLY.exe'
$tsv=Join-Path $ev 'PentaDi_C11_to_Delphi7.tsv'
$expected=[string[]]@(
 '1|7|10|LOOP7|SHORT|2|1000|1070',
 '2|7|10|LOOP7|LONG|5|1000|1070',
 '3|7|10|LOOP7|SHORT|2|1000|1070',
 '1|8|20|OVER8|SHORT|2|100|500',
 '1|9|21|PLUS127|SHORT|2|100|227',
 '1|10|22|MINUS128|SHORT|2|228|100',
 '1|11|23|PLUS128|SHORT|2|100|228',
 '1|12|24|MINUS129|SHORT|2|229|100',
 '1|13|25|NO_WRAP|SHORT|2|65530|3'
)
# /FA is an actual compiler-generated textual assembly listing,
# not a renamed EXE, OBJ, guessed IR, or unreviewed Delphi output.
& $cl /nologo /TC /std:c11 /O2 /W4 /c /FA ('/Fa'+$listing) ('/Fo'+$cobj) $csrc
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($listing) -or
   (Get-Item -LiteralPath $listing).Length -lt 200){
 throw 'PENTADI|HOLD|MSVC_C11_ASM_not_generated'
}
Write-Host 'PENTADI|C11_TO_ASM=PASS|COMPILER=MSVC|OPTIMIZATION=O2|PRODUCER=C11_NOT_DELPHI7|RELEASE=HOLD'
& $ml64 /nologo /c ('/Fo'+$masmObj) $reference
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($masmObj)){
 throw 'PENTADI|HOLD|MASM_reference_not_assembled'
}
Write-Host 'PENTADI|MASM_REL8_REFERENCE=PASS|HAND_AUTHORED=YES|OUTPUT_ASM_SOURCE_ONLY|RELEASE=HOLD'
# Execute C11 only for gate validation. Delete the transient executable.
try {
 & $cl /nologo /TC /std:c11 /O2 /W4 ('/Fe'+$transient) $csrc
 if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($transient)){
  throw 'PENTADI|HOLD|C11_runtime_build_failed'
 }
 $rows=[string[]]@(& $transient)
 if($LASTEXITCODE -ne 0 -or $rows.Count -ne $expected.Count){
  throw 'PENTADI|HOLD|C11_pipe_output_count'
 }
 for($i=0;$i -lt $expected.Count;$i++){
  if($rows[$i] -cne $expected[$i]){
   throw ('PENTADI|HOLD|C11_source_drift|index='+$i)
  }
 }
 [IO.File]::WriteAllLines($tsv,$rows,[Text.Encoding]::ASCII)
}finally{
 if([IO.File]::Exists($transient)){
  Remove-Item -LiteralPath $transient -Force
 }
}
Write-Host 'PENTADI|C11_RUNTIME=PASS|BRANCH_RECORDS=9|REL8_BOUNDARIES=PASS|TEMP_EXE_DELETED=PASS|RELEASE=HOLD'
# The full Borland/Embarcadero Delphi 7 toolchain is proprietary.
# Deliberately no WinWorld download, installer execution, serial, or licence
# impersonation. Trusted licensed DCC32 is opt-in by exact binary hash.
$delphiRan=$false
if($DelphiDcc32Path -or $DelphiDcc32Sha256){
 if(-not $DelphiDcc32Path -or -not $DelphiDcc32Sha256 -or
    $DelphiDcc32Sha256 -cnotmatch '^[a-fA-F0-9]{64}$'){
  throw 'PENTADI|HOLD|incomplete_DCC32_binary_pin'
 }
 $dcc=[IO.Path]::GetFullPath($DelphiDcc32Path)
 if(-not [IO.File]::Exists($dcc)){
  throw 'PENTADI|HOLD|pinned_DCC32_file_missing'
 }
 if($dcc.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
  throw 'PENTADI|HOLD|compiler_from_untrusted_checkout'
 }
 $actual=(Get-FileHash -LiteralPath $dcc -Algorithm SHA256).Hash
 if($actual -cne $DelphiDcc32Sha256.ToUpperInvariant()){
  throw 'PENTADI|HOLD|DCC32_SHA256_mismatch'
 }
 # Copy only plain source to a short trusted output path for legacy DCC32.
 $srcDir=Join-Path $ev 'delphi-source'
 [IO.Directory]::CreateDirectory($srcDir)|Out-Null
 Copy-Item -LiteralPath $unit -Destination (Join-Path $srcDir 'BranchPinToTsht.pas')
 Copy-Item -LiteralPath $program -Destination (Join-Path $srcDir 'Delphi007PinPipe.dpr')
 $outputExe=Join-Path $ev 'Delphi007PinPipe.exe'
 Push-Location $srcDir
 try {
  & $dcc -Q -B ('-E'+$ev) ('-N'+$ev) 'Delphi007PinPipe.dpr'
  if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($outputExe)){
   throw 'PENTADI|HOLD|Delphi7_compiler_rejected_Pascal'
  }
  $actualRows=[string[]]@(Get-Content -LiteralPath $tsv -Encoding ASCII | & $outputExe)
  if($LASTEXITCODE -ne 0 -or
     -not ($actualRows -contains 'PENTADI_DELPHI7|RECORDS=9|OSCILLATIONS=1|PINS=1|RANGE_REJECTED=4|DNF=PASS|REMAP=PASS|RELEASE=HOLD')){
   throw 'PENTADI|HOLD|Delphi7_branch_pin_execution_failed'
  }
  $delphiAsm=Join-Path $ev 'PentaDi_Delphi7_DUMPBIN_DISASSEMBLY.asm'
  $disassembly=[string[]]@(& $dumpbin /DISASM $outputExe 2>&1 | ForEach-Object{[string]$_})
  if($LASTEXITCODE -ne 0 -or $disassembly.Count -lt 10){
   throw 'PENTADI|HOLD|Delphi7_disassembly_not_generated'
  }
  $header=[string[]]@(
   '; PROPRIETARY_COMPILER_DERIVED_DISASSEMBLY_NOT_0BSD',
   '; Produced by dumpbin DISASM of verified Delphi7 executable',
   '; NOT assembler source emitted directly by dcc32; no EXE delivered'
  )
  [IO.File]::WriteAllLines($delphiAsm,[string[]]($header+$disassembly),[Text.Encoding]::ASCII)
  $delphiRan=$true
 }finally{
  Pop-Location
  if([IO.File]::Exists($outputExe)){
   Remove-Item -LiteralPath $outputExe -Force
  }
 }
 Write-Host 'PENTADI|DELPHI7=PASS|USER_PROVISIONED_DCC32=HASH_PINNED|PIPE=PASS|ASSEMBLY=DUMPBIN_DISASSEMBLY|EXE_DELETED=PASS|RELEASE=HOLD'
}else{
 Write-Host 'PENTADI|DELPHI7=HOLD_LICENSED_DCC32_UNAVAILABLE|NO_UNVERIFIED_DOWNLOAD|NOT_TESTED|RELEASE=HOLD'
 if($RequireDelphiCompiler){
  throw 'PENTADI|HOLD|Delphi7_required_but_unavailable'
 }
}
$after=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
for($i=0;$i -lt $sources.Count;$i++){
 if($before[$i] -cne $after[$i]){
  throw 'PENTADI|HOLD|source_modified'
 }
}
$asmHash=(Get-FileHash -LiteralPath $listing -Algorithm SHA256).Hash.ToLowerInvariant()
$inputHash=(Get-FileHash -LiteralPath $tsv -Algorithm SHA256).Hash.ToLowerInvariant()
$delphiStatus=if($delphiRan){'VERIFIED'}else{'HOLD'}
Write-Host ('PENTADI|FIVE_LANE_PREFLIGHT=PASS|C11_ASM_SHA256='+$asmHash+'|PIPE_SHA256='+$inputHash+'|DELPHI7='+$delphiStatus+'|NO_EXE_DELIVERABLE|RELEASE=HOLD')
