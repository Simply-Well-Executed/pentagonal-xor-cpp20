# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Android Checkpoint 10: preserve compiler/test/APK SHA256 evidence on failures.
[CmdletBinding()]
param([string]$EvidenceDir = 'evidence-android')

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence = [IO.Path]::GetFullPath((Join-Path $root $EvidenceDir))
[void][IO.Directory]::CreateDirectory($evidence)

$apkDir = Join-Path $root 'android/app/build/outputs/apk/debug'
$apks = @(Get-ChildItem -LiteralPath $apkDir -Filter '*.apk' -File -ErrorAction SilentlyContinue)
# Record compiler and packaging toolchain byte fingerprints, not copies.
# No keystores or signing secrets are collected or uploaded.
$toolCandidates = [Collections.Generic.List[object]]::new()
foreach ($command in @('javac','java','jar','keytool')) {
    $cmd = Get-Command $command -ErrorAction SilentlyContinue |
        Where-Object { $_.Source -and (Test-Path -LiteralPath $_.Source -PathType Leaf) } |
        Select-Object -First 1
    if ($cmd) { $toolCandidates.Add([ordered]@{ name = $command; path = $cmd.Source }) }
}
if (![string]::IsNullOrWhiteSpace($env:ANDROID_HOME)) {
    $bin = Join-Path $env:ANDROID_HOME 'build-tools/35.0.0'
    foreach ($tool in @('aapt2','d8','zipalign','apksigner')) {
        $toolPath = Join-Path $bin $tool
        if (Test-Path -LiteralPath $toolPath -PathType Leaf) {
            $toolCandidates.Add([ordered]@{ name = $tool; path = $toolPath })
        }
    }
}
$toolHashes = @(
    foreach ($tool in $toolCandidates) {
        [ordered]@{
            name = $tool.name
            path = $tool.path
            sha256 = (Get-FileHash -LiteralPath $tool.path -Algorithm SHA256).Hash.ToLowerInvariant()
            redistributed = $false
        }
    }
)
$testLog = Join-Path $evidence 'build-and-test.log'
$standaloneTestObserved = (Test-Path -LiteralPath $testLog -PathType Leaf) -and
    [bool](Select-String -LiteralPath $testLog -SimpleMatch 'independent Java checks (Stability + raw audio)' -Quiet)

$manifest = [ordered]@{
    schema = 'Fastest-Whisper-10D-Android-evidence-v1'
    generated_utc = (Get-Date).ToUniversalTime().ToString('o')
    github_sha = $env:GITHUB_SHA
    github_run = $env:GITHUB_RUN_ID
    github_attempt = $env:GITHUB_RUN_ATTEMPT
    runner = $env:RUNNER_OS
    protocol = 'Stability-9L3PX5-U16LE-v1'
    offline_and_weight_free = $true
    test_runner = 'Java17-StandaloneTest-main-no-JUnit'
    tests_observed_passing = $standaloneTestObserved
    tools = $toolHashes
    debug_signing = $true
    release_signing = $false
    apk_built = ($apks.Count -gt 0)
    apk_sha256 = @(
        foreach ($apk in $apks) {
            [ordered]@{
                file = $apk.Name
                size = $apk.Length
                sha256 = (Get-FileHash -LiteralPath $apk.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
            }
        }
    )
}
$manifest | ConvertTo-Json -Depth 6 |
    Set-Content -LiteralPath (Join-Path $evidence 'manifest.json') -Encoding utf8

# The dependency-free Java test emits its exact results to build-and-test.log.
# There is deliberately no Gradle/JUnit XML directory to read.
$sumPath = Join-Path $evidence 'SHA256SUMS'
$lines = [Collections.Generic.List[string]]::new()
Get-ChildItem -LiteralPath $evidence -File -Recurse |
    Where-Object { $_.FullName -ne $sumPath } |
    Sort-Object FullName | ForEach-Object {
        $relative = [IO.Path]::GetRelativePath($evidence, $_.FullName).Replace('\', '/')
        $hash = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
        $lines.Add(('{0}  {1}' -f $hash, $relative))
    }
$lines | Set-Content -LiteralPath $sumPath -Encoding utf8
Write-Output ('Android evidence: {0} APK(s), {1} SHA256 members.' -f $apks.Count,$lines.Count)
if ($apks.Count -eq 0) {
    Write-Warning 'No APK produced. Preserve build failure logs without claiming success.'
}
