# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Download Google SDK only inside disposable Docker validation.
[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$SdkHome,
    [switch]$AcceptLicenses
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

if (!$AcceptLicenses) {
    throw 'Explicit license acceptance is required in the disposable validation stage.'
}
$root = [IO.Path]::GetFullPath($SdkHome)
[void][IO.Directory]::CreateDirectory($root)
$url = 'https://dl.google.com/android/repository/commandlinetools-linux-15859902_latest.zip'
$expected = '4e4c464f145a7512b57d088ac6c278c03c9eea610886b35a5e0804e74eedf583'
$archive = Join-Path ([IO.Path]::GetTempPath()) 'android-pinned-tools.zip'
$unpack = Join-Path ([IO.Path]::GetTempPath()) ('android-sdk-' + [guid]::NewGuid().ToString('N'))
[void][IO.Directory]::CreateDirectory($unpack)
try {
    $client = [Net.Http.HttpClient]::new()
    try {
        $stream = $client.GetStreamAsync($url).GetAwaiter().GetResult()
        try {
            $writer = [IO.File]::Create($archive)
            try { $stream.CopyTo($writer) }
            finally { $writer.Dispose() }
        } finally { $stream.Dispose() }
    } finally { $client.Dispose() }

    $actual = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -cne $expected) {
        throw "SDK archive SHA256 mismatch: $actual"
    }
    Add-Type -AssemblyName System.IO.Compression
    [IO.Compression.ZipFile]::ExtractToDirectory($archive,$unpack)
    $source = Join-Path $unpack 'cmdline-tools'
    $destination = Join-Path $root 'cmdline-tools/latest'
    if (!(Test-Path -LiteralPath (Join-Path $source 'bin/sdkmanager') -PathType Leaf)) {
        throw 'Android command-line archive lacks sdkmanager.'
    }
    [void][IO.Directory]::CreateDirectory((Split-Path $destination -Parent))
    Move-Item -LiteralPath $source -Destination $destination

    $sdk = Join-Path $destination 'bin/sdkmanager'
    $env:ANDROID_HOME = $root
    $env:ANDROID_SDK_ROOT = $root

    # Consent is scoped to this disposable CI stage only. The public
    # Docker image never embeds or redistributes Google's Android SDK.
    (1..100 | ForEach-Object { 'y' }) | & $sdk "--sdk_root=$root" --licenses
    if ($LASTEXITCODE -ne 0) { throw 'Android SDK license step failed.' }
    & $sdk "--sdk_root=$root" 'platforms;android-35' 'build-tools;35.0.0'
    if ($LASTEXITCODE -ne 0) { throw 'Pinned Android SDK installation failed.' }

    foreach ($name in @('aapt2','d8','zipalign','apksigner')) {
        if (!(Test-Path -LiteralPath (Join-Path $root "build-tools/35.0.0/$name") -PathType Leaf)) {
            throw "Missing pinned Android SDK tool: $name"
        }
    }
    Write-Output "PASS ephemeral Android SDK archive SHA256 $actual"
} finally {
    Remove-Item -LiteralPath $archive -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $unpack -Recurse -Force -ErrorAction SilentlyContinue
}
