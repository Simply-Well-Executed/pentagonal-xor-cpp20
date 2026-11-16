# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Gradle-free Android debug APK build using the pinned Android SDK and JDK 17.
# Bytecode: javac -> jar -> d8. Resources: aapt2. Packaging: ZIP -> zipalign
# -> apksigner. Tests: standalone Java main, no JUnit or Maven dependencies.
[CmdletBinding()]
param(
    [string] $SdkHome = $env:ANDROID_HOME,
    [string] $BuildToolsVersion = '35.0.0',
    [string] $PlatformApi = '35'
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$working = Join-Path $root 'android/app/build'
$testClasses = Join-Path $working 'test-classes'
$appClasses = Join-Path $working 'classes'
$dexDir = Join-Path $working 'dex'
$flatDir = Join-Path $working 'flat'
$packageDir = Join-Path $working 'outputs/apk/debug'
foreach ($path in @($testClasses, $appClasses, $dexDir, $flatDir, $packageDir)) {
    [void][IO.Directory]::CreateDirectory($path)
}

function Invoke-Checked {
    param([string]$Command, [string[]]$Arguments)
    & $Command @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw ("Native command failed ({0}): {1}" -f $LASTEXITCODE, $Command)
    }
}

$pkg = 'com/simplywellexecuted/fastestwhisper10d'
$javaMain = Join-Path $root "android/app/src/main/java/$pkg"
$javaTest = Join-Path $root "android/app/src/test/java/$pkg"
$stability = Join-Path $javaMain 'Stability.java'
$acoustic = Join-Path $javaMain 'Acoustic10D.java'
$activity = Join-Path $javaMain 'MainActivity.java'
$standalone = Join-Path $javaTest 'StandaloneTest.java'
$testName = 'com.simplywellexecuted.fastestwhisper10d.StandaloneTest'

Write-Host '1/8 Pure-Java mathematical tests (no Android SDK or Gradle dependency)'
Invoke-Checked 'javac' @('-encoding','UTF-8','--release','17','-d',
    $testClasses,$stability,$acoustic,$standalone)
Invoke-Checked 'java' @('-cp',$testClasses,$testName)

if ([string]::IsNullOrWhiteSpace($SdkHome)) {
    throw 'ANDROID_HOME is required to package an APK. Pure Java tests passed.'
}
$androidJar = Join-Path $SdkHome "platforms/android-$PlatformApi/android.jar"
$toolsDir = Join-Path $SdkHome "build-tools/$BuildToolsVersion"
$exe = if ($IsWindows) { '.exe' } else { '' }
$aapt2 = Join-Path $toolsDir "aapt2$exe"
$d8Command = if ($IsWindows) { 'd8.bat' } else { 'd8' }
$d8 = Join-Path $toolsDir $d8Command
$zipalign = Join-Path $toolsDir "zipalign$exe"
$signerCommand = if ($IsWindows) { 'apksigner.bat' } else { 'apksigner' }
$apksigner = Join-Path $toolsDir $signerCommand
foreach ($file in @($androidJar,$aapt2,$d8,$zipalign,$apksigner)) {
    if (!(Test-Path -LiteralPath $file -PathType Leaf)) {
        throw "Pinned Android SDK component not found: $file"
    }
}

Write-Host '2/8 Java 17 Android app compilation'
Invoke-Checked 'javac' @('-encoding','UTF-8','-source','17','-target','17',
    '-classpath',$androidJar,'-d',$appClasses,$stability,$acoustic,$activity)

Write-Host '3/8 Standalone AAPT2 resource compile/link'
$values = Join-Path $root 'android/app/src/main/res/values/strings.xml'
$manifest = Join-Path $root 'android/app/src/main/AndroidManifest.xml'
Invoke-Checked $aapt2 @('compile',$values,'-o',$flatDir)
$compiled = @(Get-ChildItem -LiteralPath $flatDir -Filter '*.flat' -File)
if ($compiled.Count -ne 1) { throw 'Expected exactly one compiled Android resource.' }
$unsigned = Join-Path $working 'unsigned.apk'
Invoke-Checked $aapt2 @('link','-o',$unsigned,'-I',$androidJar,
    '--manifest',$manifest,'--min-sdk-version','26','--target-sdk-version','35',
    '--version-code','2','--version-name','0.2.0-bleeding',
    '--auto-add-overlay','-R',$compiled[0].FullName)

Write-Host '4/8 D8 dex conversion (no Gradle plugin)'
$jarPath = Join-Path $working 'app-classes.jar'
Invoke-Checked 'jar' @('--create','--file',$jarPath,'-C',$appClasses,'.')
Invoke-Checked $d8 @('--min-api','26','--lib',$androidJar,'--output',$dexDir,$jarPath)
$dex = Join-Path $dexDir 'classes.dex'
if (!(Test-Path -LiteralPath $dex -PathType Leaf)) { throw 'D8 produced no classes.dex.' }

Write-Host '5/8 Assemble dex into binary-manifest APK'
Add-Type -AssemblyName 'System.IO.Compression'
$archive = [IO.Compression.ZipFile]::Open($unsigned,[IO.Compression.ZipArchiveMode]::Update)
try {
    if ($null -ne $archive.GetEntry('classes.dex')) {
        throw 'APK unexpectedly already includes classes.dex.'
    }
    $entry = $archive.CreateEntry('classes.dex',[IO.Compression.CompressionLevel]::Optimal)
    $input = [IO.File]::OpenRead($dex)
    $output = $entry.Open()
    try { $input.CopyTo($output) }
    finally { $input.Dispose(); $output.Dispose() }
} finally { $archive.Dispose() }

Write-Host '6/8 ZIP align before signing'
$aligned = Join-Path $working 'aligned.apk'
Invoke-Checked $zipalign @('-f','4',$unsigned,$aligned)

Write-Host '7/8 Sign with one-time DEBUG keystore only'
$keystore = Join-Path $working 'temporary-debug-key.p12'
$signed = Join-Path $packageDir 'app-debug.apk'
try {
    Invoke-Checked 'keytool' @('-genkeypair','-noprompt','-keystore',$keystore,
        '-storetype','PKCS12','-storepass','android','-keypass','android',
        '-alias','androiddebugkey','-keyalg','RSA','-keysize','2048',
        '-validity','365','-dname','CN=Experimental Debug,O=Simply Well Executed,C=US')
    Invoke-Checked $apksigner @('sign','--ks',$keystore,'--ks-pass','pass:android',
        '--key-pass','pass:android','--out',$signed,$aligned)
} finally {
    Remove-Item -LiteralPath $keystore -Force -ErrorAction SilentlyContinue
}

Write-Host '8/8 Verify APK signature, DEX and manifest presence'
Invoke-Checked $apksigner @('verify','--verbose',$signed)
$entries = @(& jar tf $signed)
if ($LASTEXITCODE -ne 0) { throw 'Unable to inspect resulting APK.' }
foreach ($entry in @('classes.dex','AndroidManifest.xml','resources.arsc')) {
    if ($entry -notin $entries) { throw "APK is missing required entry: $entry" }
}
$sha = (Get-FileHash -LiteralPath $signed -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('PASS: Gradle-free Android debug APK, SHA256 {0}' -f $sha)
Write-Host ('File: {0}' -f $signed)
