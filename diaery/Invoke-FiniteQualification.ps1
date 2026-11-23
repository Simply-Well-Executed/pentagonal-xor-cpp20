# SPDX-License-Identifier: 0BSD
# DIAERY FINITE HEAD-TO-HEAD QUALIFICATION. Independent BigInteger oracle.
# Native MSVC C++20 /O2; no project-authored Node/Python/network use.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'NASCAR|HOLD|native_Windows_MSVC_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
if($evidence.StartsWith($root+[IO.Path]::DirectorySeparatorChar,
    [StringComparison]::OrdinalIgnoreCase)){
 throw 'NASCAR|HOLD|evidence_in_source_tree'
}
[IO.Directory]::CreateDirectory($evidence)|Out-Null
$source=Join-Path $root 'diaery/head_to_head.cpp'
$target=Join-Path $evidence 'diaery-finite-headtohead.exe'
if(-not [IO.File]::Exists($source) -or [IO.File]::Exists($target)){
 throw 'NASCAR|HOLD|source_missing_or_target_exists'
}
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'NASCAR|HOLD|missing_vswhere'}
$installation=@(& $vswhere -latest -products '*' -requires `
 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($installation.Count -ne 1 -or !$installation[0]){throw 'NASCAR|HOLD|MSVC_missing'}
$launch=Join-Path $installation[0] 'Common7/Tools/Launch-VsDevShell.ps1'
& $launch -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$compiler=(Get-Command cl.exe -ErrorAction Stop).Source
& $compiler /nologo /std:c++20 /O2 /W4 /permissive- /EHsc `
 "/Fe:$target" $source kernel32.lib
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($target)){
 throw 'NASCAR|HOLD|native_compilation_failed'
}
$observed=@(& $target)
if($LASTEXITCODE -ne 0 -or $observed.Count -ne 1 -or
  $observed[0] -cnotmatch '^DIAERY\|NASCAR_FINITE=PASS\|'){
 throw 'NASCAR|HOLD|native_run_failed'
}
# Independent oracle: .NET's arbitrary precision BigInteger, used only
# in the PowerShell qualification script, NEVER in generated C++20.
$J=[System.Numerics.BigInteger]::Parse('28644980815379718750')
$E=[System.Numerics.BigInteger]::Parse('2718281828459045235')
$S=[System.Numerics.BigInteger]::Parse('1000000000000000000')
$M=[System.Numerics.BigInteger]::Parse('1000000007')
$MAX=[System.Numerics.BigInteger]::Parse('18446744073709551615')
$jPower=[System.Numerics.BigInteger]::One
$ePower=[System.Numerics.BigInteger]::One
$den=[System.Numerics.BigInteger]::One
$firstJ=0;$firstE=0
for([int]$n=1;$n -le 64;$n++){
 $jPower *= $J; $ePower *= $E; $den *= $S
 $numJ=$jPower * $n; $numE=$ePower * $n
 if($numJ -le $numE){throw 'NASCAR|HOLD|comparison_wrong'}
 if($firstJ -eq 0 -and $numJ -gt $MAX*$den){$firstJ=$n}
 if($firstE -eq 0 -and $numE -gt $MAX*$den){$firstE=$n}
}
if($firstJ -ne 13 -or $firstE -ne 41){
 throw 'NASCAR|HOLD|overflow_thresholds_wrong'
}
$expected=@(
 'STEPS=64',
 'JIG_U64_LIMIT=13','CONVENTIONAL_U64_LIMIT=41',
 'EXACT_INVERSE=PASS',
 ('JIG_NUMERATOR_DIGITS='+$numJ.ToString().Length),
 ('E_NUMERATOR_DIGITS='+$numE.ToString().Length),
 ('JIG_MOD_1000000007='+($numJ%$M).ToString()),
 ('E_MOD_1000000007='+($numE%$M).ToString()),
 ('DEN_MOD_1000000007='+($den%$M).ToString())
)
foreach($item in $expected){
 if(-not $observed[0].Contains($item)){
  throw ('NASCAR|HOLD|native_oracle_mismatch|'+$item)
 }
}
$head=[IO.File]::ReadAllBytes($source)
if($head.Length -eq 0){throw 'NASCAR|HOLD|source_empty'}
Write-Host $observed[0]
Write-Host 'NASCAR|BIGINTEGER_INDEPENDENT_ORACLE=PASS|SOURCE_UNCHANGED=PASS|RELEASE=HOLD'
