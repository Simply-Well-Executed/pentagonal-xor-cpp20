# SPDX-License-Identifier: 0BSD
# DIAERY UNBOUND HEAD-TO-HEAD | independent BigInteger oracle.
# Dynamic heap-limb native C++20 and independent BigInteger agree at
# selected finite endpoints; no physical infinity or speedup is claimed.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'UNBOUND|HOLD|MSVC_Windows_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
if($evidence.StartsWith($root+[IO.Path]::DirectorySeparatorChar,
 [StringComparison]::OrdinalIgnoreCase)){
 throw 'UNBOUND|HOLD|evidence_inside_source'
}
[IO.Directory]::CreateDirectory($evidence)|Out-Null
$src=Join-Path $root 'diaery/unbound_head_to_head.cpp'
$exe=Join-Path $evidence 'diaery-unbound-native.exe'
if(-not [IO.File]::Exists($src)-or [IO.File]::Exists($exe)){
 throw 'UNBOUND|HOLD|missing_source_or_output_exists'
}
$before=(Get-FileHash -LiteralPath $src -Algorithm SHA256).Hash
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'UNBOUND|HOLD|missing_vswhere'}
$install=@(& $vswhere -latest -products '*' -requires `
 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($install.Count -ne 1 -or !$install[0]){throw 'UNBOUND|HOLD|missing_MSVC'}
& (Join-Path $install[0] 'Common7/Tools/Launch-VsDevShell.ps1') `
 -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=(Get-Command cl.exe -ErrorAction Stop).Source
& $cl /nologo /std:c++20 /O2 /W4 /permissive- /EHsc `
 "/Fe:$exe" $src kernel32.lib
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($exe)){
 throw 'UNBOUND|HOLD|compile_failed'
}
$J=[System.Numerics.BigInteger]::Parse('28644980815379718750')
$E=[System.Numerics.BigInteger]::Parse('2718281828459045235')
$S=[System.Numerics.BigInteger]::Parse('1000000000000000000')
$MAX=[System.Numerics.BigInteger]::Parse('18446744073709551615')
$M=[System.Numerics.BigInteger]::Parse('1000000007')
$jpower=[System.Numerics.BigInteger]::One
$epower=[System.Numerics.BigInteger]::One
$den=[System.Numerics.BigInteger]::One
$firstJ=0;$firstE=0
foreach($n in @(64,128,256)){
 $start=if($n -eq 64){1}elseif($n -eq 128){65}else{129}
 for([int]$i=$start;$i -le $n;$i++){
  $jpower*=$J;$epower*=$E;$den*=$S
  $jn=$jpower*$i;$en=$epower*$i
  if($firstJ -eq 0 -and $jn -gt $den*$MAX){$firstJ=$i}
  if($firstE -eq 0 -and $en -gt $den*$MAX){$firstE=$i}
 }
 $out=@(& $exe --steps $n)
 if($LASTEXITCODE -ne 0 -or $out.Count -ne 1){
  throw ('UNBOUND|HOLD|native_run_failed|'+$n)
 }
 $expected=@(
  'DIAERY|NASCAR_UNBOUND=PASS',
  ('STEPS='+$n),('JIG_U64_LIMIT='+$firstJ),
  ('CONVENTIONAL_U64_LIMIT='+$firstE),
  ('JIG_NUMERATOR_DIGITS='+$jn.ToString().Length),
  ('E_NUMERATOR_DIGITS='+$en.ToString().Length),
  ('JIG_MOD_1000000007='+($jn%$M).ToString()),
  ('E_MOD_1000000007='+($en%$M).ToString()),
  ('DEN_MOD_1000000007='+($den%$M).ToString()),
  'EXACT_RECIPROCAL=STRUCTURAL','RELEASE=HOLD'
 )
 foreach($w in $expected) {
  if(-not $out[0].Contains($w)){
   throw ('UNBOUND|HOLD|oracle_disagrees|'+$n+'|'+$w)
  }
 }
 Write-Host $out[0]
}
foreach($arg in @('0','-1','abc','4294967296')){
 $neg=@(& $exe --steps $arg)
 if($LASTEXITCODE -eq 0 -or -not ($neg -match 'NASCAR_UNBOUND=HOLD')){
  throw ('UNBOUND|HOLD|invalid_argument_accepted|'+$arg)
 }
}
$after=(Get-FileHash -LiteralPath $src -Algorithm SHA256).Hash
if($after -cne $before){throw 'UNBOUND|HOLD|source_mutated'}
Write-Host ('UNBOUND|SHA256='+$after.ToLowerInvariant())
Write-Host 'UNBOUND|QUALIFICATION=PASS|GROWING_LIMBS=PASS|INDEPENDENT_ORACLE=PASS|RELEASE=HOLD'
