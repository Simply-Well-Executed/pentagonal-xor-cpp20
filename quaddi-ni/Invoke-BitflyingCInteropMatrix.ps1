# SPDX-License-Identifier: 0BSD
# BITFLYING-C11-CPP20-001 | same MSVC cl.exe, explicit language modes.
# Positive builds MUST run; negative builds MUST fail with diagnostics.
# Negative compiler checks are evidence of incompatibility, not a policy.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'BITFLYING|HOLD|Windows_MSVC_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$ev=[IO.Path]::GetFullPath($EvidenceDirectory)
if($ev -ceq $root -or
   $ev.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'BITFLYING|HOLD|evidence_inside_checkout'
}
[IO.Directory]::CreateDirectory($ev)|Out-Null
$sources=@(
 (Join-Path $root 'shared/bitflying_c_abi_001.h'),
 (Join-Path $root 'quaddi-ni/BitflyingCInteropTest.c'),
 (Join-Path $root 'quaddi-ni/BitflyingC11OnlyProbe.c'),
 (Join-Path $root 'quaddi-ni/BitflyingCpp20OnlyProbe.cpp'),
 $PSCommandPath
)
foreach($file in $sources){
 if(-not [IO.File]::Exists($file)){throw ('BITFLYING|HOLD|missing_source|'+$file)}
}
$hashBefore=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'BITFLYING|HOLD|vswhere_missing'}
$installs=@(& $vswhere -latest -products '*' -requires 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($installs.Count -ne 1 -or -not $installs[0]){throw 'BITFLYING|HOLD|missing_or_ambiguous_MSVC'}
& (Join-Path $installs[0] 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=(Get-Command cl.exe -ErrorAction Stop).Source
$shared=Join-Path $root 'quaddi-ni/BitflyingCInteropTest.c'
$cOnly=Join-Path $root 'quaddi-ni/BitflyingC11OnlyProbe.c'
$cppOnly=Join-Path $root 'quaddi-ni/BitflyingCpp20OnlyProbe.cpp'
function Test-Mode {
 param(
  [Parameter(Mandatory=$true)][string]$Label,
  [Parameter(Mandatory=$true)][string]$Source,
  [Parameter(Mandatory=$true)][ValidateSet('C11','CPP20')][string]$Mode,
  [Parameter(Mandatory=$true)][bool]$ExpectCompile,
  [Parameter(Mandatory=$true)][bool]$ExpectSharedOutput
 )
 $exe=Join-Path $ev ($Label+'.exe')
 $obj=Join-Path $ev ($Label+'.obj')
 $flags=@('/nologo','/O2','/W4')
 if($Mode -ceq 'C11'){
  $flags+=@('/TC','/std:c11')
 }else{
  $flags+=@('/TP','/std:c++20','/permissive-','/Zc:__cplusplus','/EHsc')
 }
 $flags+=@('/Fo:'+$obj,'/Fe:'+$exe,$Source)
 # Native compile failures are expected in two explicitly negative probes.
 $PSNativeCommandUseErrorActionPreference=$false
 $diagnostics=@(& $cl @flags 2>&1 | ForEach-Object{[string]$_})
 $exit=[int]$LASTEXITCODE
 [IO.File]::WriteAllLines((Join-Path $ev ($Label+'.compiler.txt')),$diagnostics)
 if($ExpectCompile){
  if($exit -ne 0 -or -not [IO.File]::Exists($exe)){
   foreach($diagnostic in $diagnostics){Write-Host ('BITFLYING_COMPILER_DIAGNOSTIC|'+$Label+'|'+$diagnostic)}
   throw ('BITFLYING|HOLD|compile_rejected|'+$Label+'|exit='+$exit)
  }
  $output=@(& $exe 2>&1 | ForEach-Object{[string]$_})
  $code=[int]$LASTEXITCODE
  [IO.File]::WriteAllLines((Join-Path $ev ($Label+'.runtime.txt')),$output)
  if($code -ne 0){throw ('BITFLYING|HOLD|runtime_failed|'+$Label)}
  if($ExpectSharedOutput){
   $expected='BITFLYING_C_ABI|SHARED_C11_CPP20=PASS|NBIT=PASS|NOBIT=PASS|NOP=PASS|PLUS_MINUS=PASS|ASSIGN_SIGNS=PASS|AND_EQUAL_COMPLEMENT=PASS|NO_SILENT_LOSS=PASS|RELEASE=HOLD'
   if($output.Count -ne 1 -or $output[0] -cne $expected){
    throw ('BITFLYING|HOLD|output_drift|'+$Label)
   }
  }
  Write-Host ('BITFLYING_COMPILER|'+$Label+'|MODE='+$Mode+'|VALID=PASS|RUN=PASS|RELEASE=HOLD')
 }else{
  if($exit -eq 0){throw ('BITFLYING|HOLD|unexpected_mode_acceptance|'+$Label)}
  $evidence=[string]::Join([Environment]::NewLine,$diagnostics)
  if($evidence -notmatch '(?i)(error C[0-9]{4}|fatal error|error:)'){
   throw ('BITFLYING|HOLD|missing_compiler_diagnostic|'+$Label)
  }
  Write-Host ('BITFLYING_COMPILER|'+$Label+'|MODE='+$Mode+'|NOT_VALID=PASS|EXPECTED_REJECTION=PASS|RELEASE=HOLD')
 }
}
Test-Mode -Label 'shared-c11' -Source $shared -Mode C11 -ExpectCompile $true -ExpectSharedOutput $true
Test-Mode -Label 'shared-cpp20' -Source $shared -Mode CPP20 -ExpectCompile $true -ExpectSharedOutput $true
Test-Mode -Label 'c11-generic-in-c11' -Source $cOnly -Mode C11 -ExpectCompile $true -ExpectSharedOutput $false
Test-Mode -Label 'c11-generic-in-cpp20' -Source $cOnly -Mode CPP20 -ExpectCompile $false -ExpectSharedOutput $false
Test-Mode -Label 'cpp20-consteval-in-cpp20' -Source $cppOnly -Mode CPP20 -ExpectCompile $true -ExpectSharedOutput $false
Test-Mode -Label 'cpp20-consteval-in-c11' -Source $cppOnly -Mode C11 -ExpectCompile $false -ExpectSharedOutput $false
$hashAfter=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
for($i=0;$i -lt $sources.Count;$i++){
 if($hashBefore[$i] -cne $hashAfter[$i]){throw 'BITFLYING|HOLD|source_mutation_detected'}
}
Write-Host 'BITFLYING_COMPILER|SAME_MSVC_CL=PASS|VALID=4|NOT_VALID=2|SOURCE_UNCHANGED=PASS|RELEASE=HOLD'
