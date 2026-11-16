# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Startup qualification for the published SDK-free Android source-study image.
# This never downloads an SDK, creates signing keys, or pretends to run NDK builds.
[CmdletBinding()]
param(
    [string] $EvidenceDir = '/work/evidence',
    [string] $BuildDir = '/work/build-shared-container'
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence = [IO.Path]::GetFullPath($EvidenceDir)
$build = [IO.Path]::GetFullPath($BuildDir)
$record = [ordered]@{
    schema = 'SWE-Android-source-runtime-v1'
    created_utc = (Get-Date).ToUniversalTime().ToString('o')
    source_sha = $env:GITHUB_SHA
    scope = 'SDK-free source-study, Java 17 standalone acceptance tests'
    ndk_cross_compile = 'not-run: SDK-free public image'
    apk_build = 'not-run: delegated to disposable Android SDK validation job'
    tools = [ordered]@{}
    ndk = [ordered]@{ present = $false; path = $null; disposition = 'not required for source-study validation' }
    test_outcome = 'not-run'
    failure_code = 0
    failure_reason = $null
}
$failureCode = 0
$failureReason = $null

function Stop-Qualification {
    param([int] $Code, [string] $Reason)
    $script:failureCode = $Code
    $script:failureReason = $Reason
    throw $Reason
}
function Require-Tool {
    param([string] $Name, [int] $Code)
    $tool = Get-Command -Name $Name -CommandType Application -ErrorAction SilentlyContinue |
        Select-Object -First 1
    if (!$tool) { Stop-Qualification $Code "Required command '$Name' is absent from SDK-free source-study image." }
    $record.tools[$Name] = $tool.Source
}

try {
    [void][IO.Directory]::CreateDirectory($evidence)
    [void][IO.Directory]::CreateDirectory($build)
    Require-Tool -Name 'cmake' -Code 21
    Require-Tool -Name 'javac' -Code 22
    Require-Tool -Name 'java' -Code 23

    # Discovery only. The public stage intentionally has no Android SDK/NDK.
    # Never equate an absent NDK with a damaged or incorrect Docker pull.
    $candidates = @(
        $env:ANDROID_NDK_HOME, $env:ANDROID_NDK_ROOT,
        $(if ($env:ANDROID_HOME) { Join-Path $env:ANDROID_HOME 'ndk' })
    ) | Where-Object { ![string]::IsNullOrWhiteSpace($_) }
    foreach ($candidate in $candidates) {
        $toolchain = Join-Path $candidate 'build/cmake/android.toolchain.cmake'
        if (Test-Path -LiteralPath $toolchain -PathType Leaf) {
            $record.ndk.present = $true
            $record.ndk.path = [IO.Path]::GetFullPath($candidate)
            break
        }
    }
    if (!$record.ndk.present) {
        Write-Host 'SWE-ANDROID-SOURCE-01: Android NDK absent as expected in the published SDK-free source-study image.'
        Write-Host 'SWE-ANDROID-SOURCE-02: NDK cross-compilation must run in a separately qualified disposable NDK image.'
    }

    $javacVersion = ((& javac -version 2>&1) | Out-String).Trim()
    if ($LASTEXITCODE -ne 0 -or $javacVersion -notmatch '^javac 17(?:[.\s]|$)') {
        Stop-Qualification 24 "Java 17 compiler required; detected '$javacVersion'."
    }
    $record.tools.javac_version = $javacVersion

    $pkg = 'android/app/src/main/java/com/simplywellexecuted/fastestwhisper10d'
    $testPkg = 'android/app/src/test/java/com/simplywellexecuted/fastestwhisper10d'
    $inputs = @(
        (Join-Path $root "$pkg/Stability.java"),
        (Join-Path $root "$pkg/Acoustic10D.java"),
        (Join-Path $root "$testPkg/StandaloneTest.java")
    )
    foreach ($input in $inputs) {
        if (!(Test-Path -LiteralPath $input -PathType Leaf)) {
            Stop-Qualification 25 "Required standalone Java source is missing: $input"
        }
    }
    $classes = Join-Path $build 'java-source-study-classes'
    [void][IO.Directory]::CreateDirectory($classes)

    Write-Host 'SWE-ANDROID-SOURCE-03: Compiling Java 17 standalone mathematical acceptance tests.'
    & javac -encoding UTF-8 --release 17 -d $classes @inputs
    $native = $LASTEXITCODE
    if ($native -ne 0) {
        Stop-Qualification 31 "Java test compilation failed (javac exit=$native)."
    }

    Write-Host 'SWE-ANDROID-SOURCE-04: Running Java 17 standalone acceptance tests.'
    & java -cp $classes 'com.simplywellexecuted.fastestwhisper10d.StandaloneTest'
    $native = $LASTEXITCODE
    if ($native -ne 0) {
        Stop-Qualification 32 "Java acceptance tests failed (java exit=$native)."
    }
    $record.test_outcome = 'passed'
    Write-Host 'PASS: SDK-free Android source-study image qualified; Java 17 acceptance tests passed.'
} catch {
    $record.test_outcome = 'failed'
    if ($failureCode -eq 0) {
        $failureCode = 90
        $failureReason = $_.Exception.Message
    }
    $message = ([string]$failureReason).Replace("`r", ' ').Replace("`n", ' ')
    Write-Host "::error title=SWE-Android-source-preflight-$failureCode::$message"
} finally {
    $record.failure_code = $failureCode
    $record.failure_reason = $failureReason
    try {
        [void][IO.Directory]::CreateDirectory($evidence)
        $record | ConvertTo-Json -Depth 6 |
            Set-Content -LiteralPath (Join-Path $evidence 'android-source-runtime.json') -Encoding utf8
    } catch {
        Write-Host "::error title=SWE-evidence-write::Unable to retain Android source-runtime diagnostics: $($_.Exception.Message)"
        if ($failureCode -eq 0) { $failureCode = 91 }
    }
}
exit $failureCode
