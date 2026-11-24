# SPDX-License-Identifier: 0BSD
# No Node/Python/JDK downloads. Oracle JDK 27.0.1.1 is mandatory.
[CmdletBinding()]
param([string]$RepositoryRoot=(Split-Path $PSScriptRoot -Parent),
 [string]$EvidenceDirectory='', [string]$OracleJdkHome='',
 [string]$LinesFile='', [string]$SamplesFile='', [string]$MiscFile='',
 [ValidateRange(0,2147483647)][int]$Power=0)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'QUADDI|HOLD|Windows_MSVC_only'}
$culture=[Globalization.CultureInfo]::InvariantCulture
$root=[IO.Path]::GetFullPath($RepositoryRoot)
if(-not $EvidenceDirectory){$EvidenceDirectory=Join-Path $env:TEMP 'quaddi-ni-evidence'}
$ev=[IO.Path]::GetFullPath($EvidenceDirectory)
if($ev -ceq $root -or $ev.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'QUADDI|HOLD|evidence_inside_checkout'}
[IO.Directory]::CreateDirectory($ev)|Out-Null
if(-not $LinesFile){$LinesFile=Join-Path $root 'quaddi-ni/default-lines.tsv'}
if(-not $SamplesFile){$SamplesFile=Join-Path $root 'quaddi-ni/default-xs.tsv'}
if(-not $MiscFile){$MiscFile=Join-Path $root 'quaddi-ni/default-misc.tsv'}
$source=@((Join-Path $root 'quaddi-ni/QuadDiNI.cpp'),(Join-Path $root 'quaddi-ni/QuadDiNI.java'),(Join-Path $root 'quaddi-ni/Invoke-OracleNativeParity.ps1'),(Join-Path $root 'quaddi-ni/Invoke-Render.ps1'),$LinesFile,$SamplesFile,$MiscFile)
foreach($f in $source){if(-not [IO.File]::Exists($f)){throw ('QUADDI|HOLD|missing_source|'+$f)}}
$before=@($source|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
if(-not $OracleJdkHome){$OracleJdkHome=$env:SWE_ORACLE_JDK27_HOME}
if(-not $OracleJdkHome){throw 'QUADDI|HOLD|Oracle_not_provisioned'}
$jdk=[IO.Path]::GetFullPath($OracleJdkHome)
$manifest=Join-Path $jdk 'release'; $java=Join-Path $jdk 'bin/java.exe'; $javac=Join-Path $jdk 'bin/javac.exe'
if(-not [IO.File]::Exists($manifest) -or -not [IO.File]::Exists($java) -or -not [IO.File]::Exists($javac)){throw 'QUADDI|HOLD|Oracle_binaries_missing'}
$metadata=[IO.File]::ReadAllText($manifest)
if($metadata -cnotmatch '(?m)^JAVA_VERSION="27\.0\.1\.1"\r?$' -or $metadata -cnotmatch '(?m)^IMPLEMENTOR="Oracle Corporation"\r?$'){throw 'QUADDI|HOLD|wrong_Oracle_release'}
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'QUADDI|HOLD|vswhere_missing'}
$installed=@(& $vswhere -latest -products '*' -requires 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($installed.Count -ne 1 -or -not $installed[0]){throw 'QUADDI|HOLD|MSVC_missing'}
& (Join-Path $installed[0] 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=(Get-Command cl.exe -ErrorAction Stop).Source
$exe=Join-Path $ev 'QuadDiNI.exe'
if([IO.File]::Exists($exe)){throw 'QUADDI|HOLD|output_exists'}
& $cl /nologo /std:c++20 /O2 /W4 /permissive- /fp:precise /EHsc ('/Fe:'+$exe) (Join-Path $root 'quaddi-ni/QuadDiNI.cpp')
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($exe)){throw 'QUADDI|HOLD|CXX20_compile_failed'}
$classes=Join-Path $ev 'java-classes'
if([IO.Directory]::Exists($classes)){throw 'QUADDI|HOLD|class_output_exists'}
[IO.Directory]::CreateDirectory($classes)|Out-Null
& $javac --release 27 -d $classes (Join-Path $root 'quaddi-ni/QuadDiNI.java')
if($LASTEXITCODE -ne 0){throw 'QUADDI|HOLD|Oracle_compile_failed'}
function Verify([string]$left,[string]$right,[int]$power,[string]$lines,[string]$samples,[string]$misc){
 $a=[IO.File]::ReadAllLines($left);$b=[IO.File]::ReadAllLines($right)
 $lc=@([IO.File]::ReadAllLines($lines)|Where-Object{$_ -and -not $_.StartsWith('#')}).Count
 $xc=@([IO.File]::ReadAllLines($samples)|Where-Object{$_ -and -not $_.StartsWith('#')}).Count
 $mc=@([IO.File]::ReadAllLines($misc)|Where-Object{$_ -and -not $_.StartsWith('#')}).Count
 $pc=6+$lc+$mc
 $tab=[char]9
 if($a.Count -ne $b.Count -or $a.Count -ne (2+$mc+7*$lc*$xc)){throw 'QUADDI|HOLD|record_count_disagrees'}
 if($a[0] -cne (@('META','1','6',$lc,$mc,$pc,$power) -join $tab)){throw 'QUADDI|HOLD|bad_meta'}
 if($a[-1] -cne (@('END',$lc,$pc,$power,'RELEASE=HOLD') -join $tab)){throw 'QUADDI|HOLD|bad_tail'}
 for($i=0;$i -lt $a.Count;$i++){
  $af=$a[$i].Split($tab);$bf=$b[$i].Split($tab)
  if($af.Count -ne $bf.Count){throw ('QUADDI|HOLD|field_count|'+$i)}
  for($j=0;$j -lt $af.Count;$j++){
   if($af[0] -ceq 'ROW' -and $j -in @(6,7,8,9,10,11) -and $af[$j] -cne 'NA' -and $bf[$j] -cne 'NA'){
    $x=[double]::Parse($af[$j],$culture);$y=[double]::Parse($bf[$j],$culture)
    if(-not [double]::IsFinite($x) -or -not [double]::IsFinite($y)){throw 'QUADDI|HOLD|nonfinite_result'}
    $tol=1e-11*[Math]::Max(1,[Math]::Max([Math]::Abs($x),[Math]::Abs($y)))
    if([Math]::Abs($x-$y) -gt $tol){throw ('QUADDI|HOLD|numerical_parity|'+$i+'|'+$j)}
   }elseif($af[$j] -cne $bf[$j]){throw ('QUADDI|HOLD|source_or_identity_parity|'+$i+'|'+$j)}
  }
 }
 Write-Host ('QUADDI|PARITY=PASS|POWER='+$power+'|LINES='+$lc+'|PAGES='+$pc)
}
function Pair([int]$power,[string]$lines,[string]$samples,[string]$misc,[string]$tag){
 $left=Join-Path $ev ($tag+'-native.tsv');$right=Join-Path $ev ($tag+'-oracle.tsv')
 & $exe $lines $samples ([string]$power) $misc > $left
 if($LASTEXITCODE -ne 0){throw ('QUADDI|HOLD|native_exit|'+$tag)}
 & $java -Dfile.encoding=UTF-8 -cp $classes QuadDiNI $lines $samples ([string]$power) $misc > $right
 if($LASTEXITCODE -ne 0){throw ('QUADDI|HOLD|oracle_exit|'+$tag)}
 Verify $left $right $power $lines $samples $misc
}
for($p=0;$p -le 6;$p++){Pair $p $LinesFile $SamplesFile $MiscFile ('default-p'+$p)}
if($Power -gt 6){Pair $Power $LinesFile $SamplesFile $MiscFile ('selected-p'+$Power)}
$extra=Join-Path $ev '17-lines.tsv'
$examples=for($i=0;$i -lt 17;$i++){('Independent_'+$i+'|'+$i+'|-3')}
[IO.File]::WriteAllLines($extra,[string[]]$examples,[Text.Encoding]::ASCII)
Pair 6 $extra $SamplesFile $MiscFile 'custom-17'
$bad=Join-Path $ev 'bad-lines.tsv'
[IO.File]::WriteAllText($bad,('Bad|not-a-number|-3'+[Environment]::NewLine),[Text.Encoding]::ASCII)
& $exe $bad $SamplesFile 6 $MiscFile 1>$null 2>$null
if($LASTEXITCODE -eq 0){throw 'QUADDI|HOLD|native_accepted_bad_input'}
& $java -cp $classes QuadDiNI $bad $SamplesFile 6 $MiscFile 1>$null 2>$null
if($LASTEXITCODE -eq 0){throw 'QUADDI|HOLD|Oracle_accepted_bad_input'}
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'quaddi-ni/Invoke-Render.ps1') -WitnessFile (Join-Path $ev ('default-p'+$Power+'-native.tsv')) -Destination (Join-Path $ev 'quaddi-ni-report.xhtml')
if($LASTEXITCODE -ne 0){throw 'QUADDI|HOLD|XHTML_render_failed'}
$after=@($source|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
for($i=0;$i -lt $source.Count;$i++){if($before[$i] -cne $after[$i]){throw 'QUADDI|HOLD|source_mutation'}}
Write-Host ('QUADDI|CXX20_ORACLE=PASS|SHA256='+$before[0].ToLowerInvariant())
Write-Host 'QUADDI|RELEASE=HOLD|NO_AUTO_MERGE'
