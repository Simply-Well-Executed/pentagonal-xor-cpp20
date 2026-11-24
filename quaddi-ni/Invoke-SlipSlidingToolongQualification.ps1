# SPDX-License-Identifier: 0BSD
# ISO C++20 /O2 + exact Oracle Java 27 independent guarded-width checks.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [Parameter(Mandatory=$true)][string]$OracleJdkHome
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'SLIPSLIDINGTOOLONG|HOLD|Windows_MSVC_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$ev=[IO.Path]::GetFullPath($EvidenceDirectory)
if($ev -ceq $root -or $ev.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'SLIPSLIDINGTOOLONG|HOLD|evidence_inside_checkout'
}
[IO.Directory]::CreateDirectory($ev)|Out-Null
$sources=@(
 (Join-Path $root 'shared/slipslidingtoolong.hpp'),
 (Join-Path $root 'shared/toolong_q62_core.hpp'),
 (Join-Path $root 'quaddi-ni/SlipSlidingToolongTest.cpp'),
 (Join-Path $root 'quaddi-ni/SlipSlidingToolongOracle.java'),
 $PSCommandPath
)
foreach($f in $sources){if(-not [IO.File]::Exists($f)){throw ('SLIPSLIDINGTOOLONG|HOLD|source_missing|'+$f)}}
$before=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'SLIPSLIDINGTOOLONG|HOLD|vswhere_missing'}
$install=@(& $vswhere -latest -products '*' -requires 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($install.Count -ne 1 -or -not $install[0]){throw 'SLIPSLIDINGTOOLONG|HOLD|MSVC_missing'}
& (Join-Path $install[0] 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=(Get-Command cl.exe -ErrorAction Stop).Source
$native=Join-Path $ev 'slipslidingtoolong.exe'
if([IO.File]::Exists($native)){throw 'SLIPSLIDINGTOOLONG|HOLD|native_binary_exists'}
& $cl /nologo /std:c++20 /O2 /W4 /permissive- /EHsc /fp:precise ('/Fe:'+$native) (Join-Path $root 'quaddi-ni/SlipSlidingToolongTest.cpp')
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($native)){throw 'SLIPSLIDINGTOOLONG|HOLD|native_compile_failed'}
$nativeOutput=@(& $native)
if($LASTEXITCODE -ne 0){throw 'SLIPSLIDINGTOOLONG|HOLD|native_test_failed'}
$jdk=[IO.Path]::GetFullPath($OracleJdkHome)
$release=Join-Path $jdk 'release'
$java=Join-Path $jdk 'bin/java.exe'
$javac=Join-Path $jdk 'bin/javac.exe'
if(-not [IO.File]::Exists($release) -or -not [IO.File]::Exists($java) -or -not [IO.File]::Exists($javac)){
 throw 'SLIPSLIDINGTOOLONG|HOLD|Oracle_JDK_missing'
}
$manifest=[IO.File]::ReadAllText($release)
if($manifest -cnotmatch '(?m)^JAVA_VERSION="27\.0\.1\.1"\r?$' -or $manifest -cnotmatch '(?m)^IMPLEMENTOR="Oracle Corporation"\r?$'){
 throw 'SLIPSLIDINGTOOLONG|HOLD|Oracle_vendor_version_mismatch'
}
$classes=Join-Path $ev 'java-classes'
if([IO.Directory]::Exists($classes)){throw 'SLIPSLIDINGTOOLONG|HOLD|classes_exist'}
[IO.Directory]::CreateDirectory($classes)|Out-Null
& $javac --release 27 -d $classes (Join-Path $root 'quaddi-ni/SlipSlidingToolongOracle.java')
if($LASTEXITCODE -ne 0){throw 'SLIPSLIDINGTOOLONG|HOLD|Oracle_compile_failed'}
$javaOutput=@(& $java '-Dfile.encoding=UTF-8' -cp $classes SlipSlidingToolongOracle)
if($LASTEXITCODE -ne 0){throw 'SLIPSLIDINGTOOLONG|HOLD|Oracle_test_failed'}
$expected='SLIPSLIDINGTOOLONG|WIDTHS=10|PAIRS=100|GLINTING=45|GLIZZING=45|EQUAL=10|PASS'
$loss='SLIPSLIDINGTOOLONG|NARROWING_LOSS_REJECTED=45|Q62_128_TO_64=PASS|32BIT_LOSS_REJECTED=PASS'
if($nativeOutput.Count -lt 4 -or $javaOutput.Count -lt 4 -or $nativeOutput[0] -cne $expected -or $javaOutput[0] -cne $expected -or $nativeOutput[1] -cne $loss -or $javaOutput[1] -cne $loss){
 throw 'SLIPSLIDINGTOOLONG|HOLD|independent_width_or_Q62_disagreement'
}
foreach($line in $nativeOutput){Write-Host $line}
foreach($line in $javaOutput){Write-Host ('ORACLE_MODEL|'+$line)}
$after=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
for($i=0;$i -lt $sources.Count;$i++){
 if($before[$i] -cne $after[$i]){throw 'SLIPSLIDINGTOOLONG|HOLD|source_mutated'}
}
Write-Host ('GUARDMEMORYGUARD|CXX20_ORACLE=PASS|SOURCE_SHA256='+$before[0].ToLowerInvariant()+'|RELEASE=HOLD')
