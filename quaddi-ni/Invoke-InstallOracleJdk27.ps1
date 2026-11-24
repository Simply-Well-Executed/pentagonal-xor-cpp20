# SPDX-License-Identifier: 0BSD
# Oracle JDK is licensed under Oracle NFTC, NOT this project's 0BSD.
# Archive SHA-256: Oracle JDK 27.0.1.1, published 2026-11-23.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$EvidenceDirectory,
      [string]$GithubEnvFile=$env:GITHUB_ENV)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows -or [Runtime.InteropServices.RuntimeInformation]::OSArchitecture -ne
   [Runtime.InteropServices.Architecture]::X64){throw 'ORACLE_JDK|HOLD|Windows_x64_required'}
$url='https://download.oracle.com/java/27/latest/jdk-27_windows-x64_bin.zip'
$expected='9a2972e8ade166718fdc47e903b10b88e046b9839efb0a5cacb994a4be3266f7'
$folder=[IO.Path]::GetFullPath($EvidenceDirectory)
if([IO.Directory]::Exists($folder) -and @(Get-ChildItem -LiteralPath $folder -Force).Count -gt 0){
 throw 'ORACLE_JDK|HOLD|evidence_not_empty'
}
[IO.Directory]::CreateDirectory($folder)|Out-Null
$archive=Join-Path $folder 'oracle-jdk-27.0.1.1.zip'
$target=Join-Path $folder 'unpacked'
Invoke-WebRequest -Uri $url -OutFile $archive -MaximumRedirection 3 -TimeoutSec 600
$sha=(Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
if($sha -cne $expected){throw ('ORACLE_JDK|HOLD|archive_SHA256_mismatch|actual='+$sha)}
Expand-Archive -LiteralPath $archive -DestinationPath $target
$roots=@(Get-ChildItem -LiteralPath $target -Directory)
if($roots.Count -ne 1){throw 'ORACLE_JDK|HOLD|unexpected_archive_root'}
$jdk=$roots[0].FullName
$release=Join-Path $jdk 'release'
$java=Join-Path $jdk 'bin/java.exe'
$javac=Join-Path $jdk 'bin/javac.exe'
foreach($file in @($release,$java,$javac)){
 if(-not [IO.File]::Exists($file)){throw ('ORACLE_JDK|HOLD|missing_file|'+$file)}
}
$manifest=[IO.File]::ReadAllText($release)
if($manifest -cnotmatch '(?m)^JAVA_VERSION="27\.0\.1\.1"\r?$' -or
   $manifest -cnotmatch '(?m)^IMPLEMENTOR="Oracle Corporation"\r?$'){
 throw 'ORACLE_JDK|HOLD|wrong_Oracle_manifest'
}
$javaVersion=@(& $java --version)
if($LASTEXITCODE -ne 0 -or -not ($javaVersion -match '27\.0\.1\.1')){
 throw 'ORACLE_JDK|HOLD|java_version_mismatch'
}
$compilerVersion=@(& $javac -version)
if($LASTEXITCODE -ne 0 -or -not ($compilerVersion -match '27\.0\.1\.1')){
 throw 'ORACLE_JDK|HOLD|javac_version_mismatch'
}
if(-not [string]::IsNullOrWhiteSpace($GithubEnvFile)){
 if(-not [IO.File]::Exists($GithubEnvFile)){throw 'ORACLE_JDK|HOLD|GITHUB_ENV_missing'}
 Add-Content -LiteralPath $GithubEnvFile -Value ('ORACLE_JDK_HOME='+$jdk) -Encoding utf8
}
Write-Host ('ORACLE_JDK|PASS|VERSION=27.0.1.1|VENDOR=Oracle Corporation|SHA256='+$sha)
Write-Host 'ORACLE_JDK|LICENSE=Oracle_NFTC|EVALUATION_OFFLINE_AFTER_PROVISIONING'
