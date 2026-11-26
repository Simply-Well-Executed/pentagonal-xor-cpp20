# SPDX-License-Identifier: 0BSD
# DIAERY three-flavor qualification: native C++20 <-> XHTML Strict fixture
# Oracle Java 27.0.1.1 <-> XHTML and Oracle Java <-> C++20 when provisioned.
# No Oracle binaries are downloaded. Java is NOT silently substituted.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [string]$OracleJdkHome='',
 [switch]$RequireOracle
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'TRIFLAVOR|HOLD|native_Windows_MSVC_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
if($evidence.StartsWith($root+[IO.Path]::DirectorySeparatorChar,
 [StringComparison]::OrdinalIgnoreCase)){
 throw 'TRIFLAVOR|HOLD|evidence_inside_checkout'
}
[IO.Directory]::CreateDirectory($evidence)|Out-Null
$document=Join-Path $root 'diaery/tri_flavors.xhtml'
$cpp=Join-Path $root 'diaery/tri_flavors.cpp'
$javaSrc=Join-Path $root 'diaery/DiaeryTriFlavors.java'
$native=Join-Path $evidence 'diaery-triflavors-native.exe'
$classes=Join-Path $evidence 'diaery-triflavors-java-classes'
foreach($p in @($document,$cpp,$javaSrc)){
 if(-not [IO.File]::Exists($p)){throw ('TRIFLAVOR|HOLD|missing_source|'+$p)}
}
if([IO.File]::Exists($native)){throw 'TRIFLAVOR|HOLD|native_output_already_exists'}
$sourceFiles=@($document,$cpp,$javaSrc)
$before=@($sourceFiles | ForEach-Object {(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
# Independent decimal/rational witness. No approximate floating point.
$j=[Numerics.BigInteger]::Parse('2864498081537971875')
$oldTotal=[Numerics.BigInteger]::Parse('1833278772184302')
$e=[Numerics.BigInteger]::Parse('2718281828459045235')
$js=[Numerics.BigInteger]::Pow([Numerics.BigInteger]::new(10),17)
$es=[Numerics.BigInteger]::Pow([Numerics.BigInteger]::new(10),18)
if(($oldTotal*100000/64) -ne $j){throw 'TRIFLAVOR|HOLD|jig_source_drift'}
$jr=($j%$js*64)%$js;$er=($e%$es*64)%$es
$jgap=[Numerics.BigInteger]::Min($jr,$js-$jr)*10
$egap=[Numerics.BigInteger]::Min($er,$es-$er)
if($jgap -ne [Numerics.BigInteger]::Parse('278772184302000000') -or
   $egap -ne [Numerics.BigInteger]::Parse('29962978621104960')){
 throw 'TRIFLAVOR|HOLD|loop_oracle_disagrees'
}
$expected='TRIFLAVOR|PASS|JIG_X1E17=2864498081537971875|RADIUS2_MIN=2500|JIG_GAP_X1E18=278772184302000000|REFERENCE_GAP_X1E18=29962978621104960|SWEEP=4096|NORMALIZED_CLOSED=1|RELEASE=HOLD'
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'TRIFLAVOR|HOLD|vswhere_missing'}
$install=@(& $vswhere -latest -products '*' -requires `
 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($install.Count -ne 1 -or -not $install[0]){throw 'TRIFLAVOR|HOLD|MSVC_not_installed'}
& (Join-Path $install[0] 'Common7/Tools/Launch-VsDevShell.ps1') `
 -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=(Get-Command cl.exe -ErrorAction Stop).Source
& $cl /nologo /std:c++20 /O2 /W4 /permissive- /EHsc `
 "/Fe:$native" $cpp kernel32.lib
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($native)){
 throw 'TRIFLAVOR|HOLD|CXX20_compile_failed'
}
$observed=@(& $native $document)
if($LASTEXITCODE -ne 0 -or $observed.Count -ne 1 -or
 $observed[0] -cne $expected){throw 'TRIFLAVOR|HOLD|CPP_XHTML_mismatch'}
$docText=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($document))
$mutations=@(
 @('radius2_min=2500','radius2_min=2499'),
 @('normalized_closed=1','normalized_closed=0'),
 @('jig_x1e17=2864498081537971875','jig_x1e17=2864498081537971874')
)
$badPaths=[Collections.Generic.List[string]]::new()
foreach($m in $mutations){
 if(-not $docText.Contains($m[0])){throw 'TRIFLAVOR|HOLD|missing_mutation_canary'}
 $bad=Join-Path $evidence ('triflavors-negative-'+$badPaths.Count+'.xhtml')
 if([IO.File]::Exists($bad)){throw 'TRIFLAVOR|HOLD|negative_fixture_exists'}
 [IO.File]::WriteAllBytes($bad,[Text.Encoding]::ASCII.GetBytes(
     $docText.Replace($m[0],$m[1])))
 $badPaths.Add($bad)
 & $native $bad | Out-Null
 if($LASTEXITCODE -eq 0){throw 'TRIFLAVOR|HOLD|CPP_accepted_corrupt_XHTML'}
}
Write-Host 'TRIFLAVOR|CPP_XHTML=PASS|CIRCLE_MONOTONE=PASS|LOOP_EXACT=PASS'
if([string]::IsNullOrWhiteSpace($OracleJdkHome)){
 if($RequireOracle){throw 'TRIFLAVOR|HOLD|Oracle_JDK27_not_provisioned'}
 Write-Host 'TRIFLAVOR|JAVA_XHTML=HOLD|JAVA_CPP=HOLD|ORACLE_NOT_PROVISIONED'
}else{
 $jdk=[IO.Path]::GetFullPath($OracleJdkHome)
 $release=Join-Path $jdk 'release'
 $javac=Join-Path $jdk 'bin/javac.exe'
 $java=Join-Path $jdk 'bin/java.exe'
 if(-not [IO.File]::Exists($release)-or
    -not [IO.File]::Exists($javac)-or
    -not [IO.File]::Exists($java)){
  throw 'TRIFLAVOR|HOLD|missing_Oracle_27_binaries'
 }
 $metadata=[IO.File]::ReadAllText($release)
 if($metadata -cnotmatch '(?m)^JAVA_VERSION="27\.0\.1\.1"\r?$' -or
    $metadata -cnotmatch '(?m)^IMPLEMENTOR="Oracle Corporation"\r?$'){
  throw 'TRIFLAVOR|HOLD|wrong_JDK_version_or_vendor'
 }
 if([IO.Directory]::Exists($classes)){throw 'TRIFLAVOR|HOLD|classes_already_exist'}
 [IO.Directory]::CreateDirectory($classes)|Out-Null
 & $javac --release 27 -d $classes $javaSrc
 if($LASTEXITCODE -ne 0){throw 'TRIFLAVOR|HOLD|Oracle_javac_failed'}
 $jout=@(& $java -Dfile.encoding=UTF-8 -cp $classes DiaeryTriFlavors $document)
 if($LASTEXITCODE -ne 0 -or $jout.Count -ne 1 -or
    $jout[0] -cne $expected -or $jout[0] -cne $observed[0]){
  throw 'TRIFLAVOR|HOLD|Oracle_Java_vs_XHTML_vs_CPP_disagree'
 }
 foreach($bad in $badPaths){
  & $java -Dfile.encoding=UTF-8 -cp $classes DiaeryTriFlavors $bad 2>$null | Out-Null
  if($LASTEXITCODE -eq 0){throw 'TRIFLAVOR|HOLD|Java_accepted_corrupt_XHTML'}
 }
 Write-Host 'TRIFLAVOR|JAVA_XHTML=PASS|JAVA_CPP=PASS|ORACLE_JDK=27.0.1.1'
}
$after=@($sourceFiles | ForEach-Object {(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
for($i=0;$i -lt $before.Count;$i++){
 if($before[$i] -cne $after[$i]){throw 'TRIFLAVOR|HOLD|source_mutation'}
}
Write-Host ('TRIFLAVOR|SOURCE_SHA256='+$before[0].ToLowerInvariant())
Write-Host 'TRIFLAVOR|NATIVE_QUALIFICATION=PASS|RELEASE=HOLD'
