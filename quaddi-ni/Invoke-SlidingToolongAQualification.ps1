# SPDX-License-Identifier: 0BSD
# SLIDINGTOOLONGA: native exact exponent witness with independent BigInteger,
# optional exact Oracle JDK 27.0.1.1 parity. No Python or toolchain downloads.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [string]$Exponent='0',
 [string]$OracleJdkHome=''
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'SLIDINGTOOLONGA|HOLD|native_Windows_MSVC_required'}
if($Exponent -cnotmatch '^[0-9]+$'){throw 'SLIDINGTOOLONGA|HOLD|exponent_not_nonnegative_integer'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$ev=[IO.Path]::GetFullPath($EvidenceDirectory)
if($ev -ceq $root -or $ev.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'SLIDINGTOOLONGA|HOLD|evidence_inside_checkout'
}
[IO.Directory]::CreateDirectory($ev)|Out-Null
$cpp=Join-Path $root 'quaddi-ni/SlidingToolongA.cpp'
$javaSrc=Join-Path $root 'quaddi-ni/SlidingToolongA.java'
$lines=Join-Path $root 'quaddi-ni/default-lines.tsv'
$render=Join-Path $root 'quaddi-ni/Invoke-SlidingToolongARender.ps1'
$misc=Join-Path $root 'quaddi-ni/default-misc.tsv'
$source=@($cpp,$javaSrc,$lines,$misc,$render,$PSCommandPath)
foreach($path in $source){if(-not [IO.File]::Exists($path)){throw ('SLIDINGTOOLONGA|HOLD|source_missing|'+$path)}}
$before=@($source|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'SLIDINGTOOLONGA|HOLD|vswhere_missing'}
$install=@(& $vswhere -latest -products '*' -requires 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($install.Count -ne 1 -or -not $install[0]){throw 'SLIDINGTOOLONGA|HOLD|MSVC_missing'}
& (Join-Path $install[0] 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=(Get-Command cl.exe -ErrorAction Stop).Source
$exe=Join-Path $ev 'slidingtoolonga-native.exe'
if([IO.File]::Exists($exe)){throw 'SLIDINGTOOLONGA|HOLD|native_binary_exists'}
& $cl /nologo /std:c++20 /O2 /W4 /permissive- /EHsc /fp:precise ('/Fe:'+$exe) $cpp
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($exe)){throw 'SLIDINGTOOLONGA|HOLD|CXX20_compile_failed'}
function ExactPower([Numerics.BigInteger]$m,[Numerics.BigInteger]$p){
 $out=[Numerics.BigInteger]::One
 $base=[Numerics.BigInteger]::Abs($m)
 $two=[Numerics.BigInteger]::new(2)
 while($p -gt [Numerics.BigInteger]::Zero){
  if(-not $p.IsEven){$out*=$base}
  $p=[Numerics.BigInteger]::Divide($p,$two)
  if($p -gt [Numerics.BigInteger]::Zero){$base*=$base}
 }
 return $out
}
$c=[Globalization.CultureInfo]::InvariantCulture
$sep=[char]9
function Evaluate([string]$p,[string]$tag){
 $path=Join-Path $ev ($tag+'.tsv')
 if([IO.File]::Exists($path)){throw 'SLIDINGTOOLONGA|HOLD|witness_exists'}
 & $exe $lines $p > $path
 if($LASTEXITCODE -ne 0){throw ('SLIDINGTOOLONGA|HOLD|native_run_failed|'+$p)}
 $rows=[IO.File]::ReadAllLines($path,[Text.Encoding]::UTF8)
 $sources=@([IO.File]::ReadAllLines($lines)|Where-Object{$_ -and -not $_.StartsWith('#')})
 $header=(@('META','SLIDINGTOOLONGA-001',('POWER='+$p),'X=0','EXACT_DECIMAL') -join $sep)
 $tail=(@('END',$sources.Count,'RELEASE=HOLD') -join $sep)
 if($rows.Count -ne $sources.Count+2 -or $rows[0] -cne $header -or $rows[-1] -cne $tail){
  throw ('SLIDINGTOOLONGA|HOLD|wrong_count_or_meta|'+$p)
 }
 $exponent=[Numerics.BigInteger]::Parse($p,$c)
 for($i=0;$i -lt $sources.Count;$i++){
  $input=$sources[$i].Split('|');$row=$rows[$i+1].Split($sep)
  if($input.Count -ne 3 -or $row.Count -ne 9 -or
     $row[0] -cne 'SOURCE' -or $row[1] -cne [string]($i+1) -or
     $row[2] -cne $input[0] -or $row[3] -cne $input[1] -or $row[4] -cne $input[2]){
   throw ('SLIDINGTOOLONGA|HOLD|independent_identity_mismatch|'+$i)
  }
  $intercept=$input[2];$negative=$intercept.StartsWith('-')
  $clean=$intercept.TrimStart([char[]]@('-','+'))
  $dot=$clean.IndexOf('.')
  $scale=if($dot -lt 0){0}else{$clean.Length-$dot-1}
  $magnitude=[Numerics.BigInteger]::Parse($clean.Replace('.',''),$c)
  $value=ExactPower $magnitude $exponent
  $sign=if($exponent.IsZero){1}elseif($magnitude.IsZero){0}elseif($negative -and -not $exponent.IsEven){-1}else{1}
  $scaleText=if($magnitude.IsZero -and -not $exponent.IsZero){'0'}else{([Numerics.BigInteger]$scale*$exponent).ToString($c)}
  $digits=$value.ToString($c)
  if($row[5] -cne [string]$sign -or $row[6] -cne $scaleText -or
     $row[7] -cne [string]$digits.Length -or $row[8] -cne $digits){
   throw ('SLIDINGTOOLONGA|HOLD|independent_BigInteger_disagrees|'+$p+'|'+$input[0])
  }
 }
 Write-Host ('SLIDINGTOOLONGA|CXX20_BIGINT=PASS|POWER='+$p+'|SOURCES='+$sources.Count+'|EXACT=1')
 return $path
}
$zero=Evaluate '0' 'power0'
$null=Evaluate '646' 'power646'
$null=Evaluate '647' 'power647'
$canonical=([Numerics.BigInteger]::Parse($Exponent,$c)).ToString($c)
if($canonical -eq '0'){$chosen=$zero}
elseif($canonical -eq '646'){$chosen=Join-Path $ev 'power646.tsv'}
elseif($canonical -eq '647'){$chosen=Join-Path $ev 'power647.tsv'}
else{$chosen=Evaluate $canonical 'selected-power'}
$bad=Join-Path $ev 'bad-input.tsv'
[IO.File]::WriteAllText($bad,'Invalid|2|not-a-number'+[Environment]::NewLine,[Text.Encoding]::ASCII)
& $exe $bad 647 1>$null 2>$null
if($LASTEXITCODE -eq 0){throw 'SLIDINGTOOLONGA|HOLD|bad_decimal_accepted'}
if(-not $OracleJdkHome){$OracleJdkHome=$env:SWE_ORACLE_JDK27_HOME}
if($OracleJdkHome){
 $jdk=[IO.Path]::GetFullPath($OracleJdkHome)
 $manifest=Join-Path $jdk 'release';$javac=Join-Path $jdk 'bin/javac.exe';$java=Join-Path $jdk 'bin/java.exe'
 if(-not [IO.File]::Exists($manifest) -or -not [IO.File]::Exists($javac) -or -not [IO.File]::Exists($java)){
  throw 'SLIDINGTOOLONGA|HOLD|Oracle_missing'
 }
 $metadata=[IO.File]::ReadAllText($manifest)
 if($metadata -cnotmatch '(?m)^JAVA_VERSION="27\.0\.1\.1"\r?$' -or
    $metadata -cnotmatch '(?m)^IMPLEMENTOR="Oracle Corporation"\r?$'){
  throw 'SLIDINGTOOLONGA|HOLD|Oracle_release_mismatch'
 }
 $classes=Join-Path $ev 'java-classes'
 if([IO.Directory]::Exists($classes)){throw 'SLIDINGTOOLONGA|HOLD|existing_Java_classes'}
 [IO.Directory]::CreateDirectory($classes)|Out-Null
 & $javac --release 27 -d $classes $javaSrc
 if($LASTEXITCODE -ne 0){throw 'SLIDINGTOOLONGA|HOLD|Oracle_compile_failed'}
 foreach($p in @('0','646','647',$canonical)|Select-Object -Unique){
  $out=Join-Path $ev ('java-p'+$p+'.tsv')
  & $java -Dfile.encoding=UTF-8 -cp $classes SlidingToolongA $lines $p > $out
  if($LASTEXITCODE -ne 0){throw ('SLIDINGTOOLONGA|HOLD|Oracle_run_failed|'+$p)}
  $native=if($p -eq '0'){$zero}elseif($p -eq '646'){Join-Path $ev 'power646.tsv'}elseif($p -eq '647'){Join-Path $ev 'power647.tsv'}else{$chosen}
  $left=[IO.File]::ReadAllLines($native);$right=[IO.File]::ReadAllLines($out)
  if($left.Count -ne $right.Count){throw 'SLIDINGTOOLONGA|HOLD|Oracle_count_mismatch'}
  for($i=0;$i -lt $left.Count;$i++){
   if($left[$i] -cne $right[$i]){throw ('SLIDINGTOOLONGA|HOLD|Oracle_CPP_mismatch|'+$p+'|'+$i)}
  }
  Write-Host ('SLIDINGTOOLONGA|ORACLE_CPP=PASS|POWER='+$p)
 }
}else{Write-Host 'SLIDINGTOOLONGA|ORACLE_CPP=HOLD|ORACLE_NOT_PROVISIONED'}
$out=Join-Path $ev 'slidingtoolonga-origin.xhtml'
& pwsh -NoLogo -NoProfile -NonInteractive -File $render -WitnessFile $chosen -Destination $out
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($out)){throw 'SLIDINGTOOLONGA|HOLD|XHTML_failed'}
$power647Report=Join-Path $ev 'slidingtoolonga-power647.xhtml'
& pwsh -NoLogo -NoProfile -NonInteractive -File $render -WitnessFile (Join-Path $ev 'power647.tsv') -Destination $power647Report
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($power647Report)){throw 'SLIDINGTOOLONGA|HOLD|XHTML647_failed'}
if($OracleJdkHome){
 $javaWitness=Join-Path $ev 'java-p647.tsv'
 $javaReport=Join-Path $ev 'slidingtoolonga-oracle-power647.xhtml'
 if(-not [IO.File]::Exists($javaWitness)){throw 'SLIDINGTOOLONGA|HOLD|oracle_witness_missing'}
 & pwsh -NoLogo -NoProfile -NonInteractive -File $render -WitnessFile $javaWitness -Destination $javaReport
 if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($javaReport)){throw 'SLIDINGTOOLONGA|HOLD|oracle_XHTML_failed'}
 $a=(Get-FileHash -LiteralPath $power647Report -Algorithm SHA256).Hash
 $b=(Get-FileHash -LiteralPath $javaReport -Algorithm SHA256).Hash
 if($a -cne $b){throw 'SLIDINGTOOLONGA|HOLD|Oracle_CXX20_XHTML_not_identical'}
 Write-Host ('SLIDINGTOOLONGA|ORACLE_CPP_XHTML=PASS|POWER=647|SHA256='+$a.ToLowerInvariant())
}
Write-Host 'SLIDINGTOOLONGA|EXACT_646_647=PASS|XHTML647=PASS|RELEASE=HOLD'
$after=@($source|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
for($i=0;$i -lt $source.Count;$i++){if($before[$i] -cne $after[$i]){throw 'SLIDINGTOOLONGA|HOLD|source_mutated'}}
Write-Host ('SLIDINGTOOLONGA|NATIVE=PASS|XHTML=PASS|SELECTED_POWER='+$canonical+'|SOURCE_SHA256='+$before[0].ToLowerInvariant()+'|RELEASE=HOLD')
