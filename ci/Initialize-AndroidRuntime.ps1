# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Host PowerShell 7 runtime qualifier for the pinned Android source-study image.
# The image is deliberately SDK-free and may have no PowerShell binary inside.
# Only approved, read-only-network Docker commands enter the container.
[CmdletBinding()]
param(
    [Parameter(Mandatory)][string[]] $DockerBaseArgs,
    [string] $EvidenceDir = 'evidence',
    [ValidatePattern('^[A-Za-z0-9][A-Za-z0-9_.-]*$')][string] $BuildDir = 'build-shared-container'
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence = [IO.Path]::GetFullPath($EvidenceDir)
$build = Join-Path $root $BuildDir
$record = [ordered]@{
    schema = 'SWE-Android-source-runtime-v2'
    created_utc = (Get-Date).ToUniversalTime().ToString('o')
    source_sha = $env:GITHUB_SHA
    validation_scope = 'SDK-free published image; native C/C++ CTest and optional Java17 standalone tests'
    ndk_compile = 'not-run: requires a separate qualified disposable NDK stage'
    apk_build = 'not-run: independently tested in disposable Android SDK validation job'
    ndk_present = $null
    stage_results = @()
    test_outcome = 'not-run'
    java_test_outcome = 'not-run: optional Java tools not checked'
    failure_code = 0
    failure_reason = $null
}
$stages = [System.Collections.Generic.List[object]]::new()
$failureCode = 0
$failureReason = $null
$lastCode = 0

function Stop-Qualification {
    param([int] $Code, [string] $Reason)
    $script:failureCode = $Code
    $script:failureReason = $Reason
    throw $Reason
}

function Invoke-QualifiedCommand {
    param(
        [Parameter(Mandatory)][string] $Name,
        [Parameter(Mandatory)][string] $ShellCommand,
        [Parameter(Mandatory)][int] $FailureCode,
        [switch] $Optional
    )
    $started = (Get-Date).ToUniversalTime().ToString('o')
    $log = Join-Path $evidence ("android-" + $Name + '.log')
    Write-Host "Android source runtime: $Name"
    $output = @(& docker @DockerBaseArgs $ShellCommand 2>&1 |
        Tee-Object -FilePath $log)
    $code = [int]$LASTEXITCODE
    $script:lastCode = $code
    $stages.Add([ordered]@{
        stage = $Name
        started_utc = $started
        finished_utc = (Get-Date).ToUniversalTime().ToString('o')
        raw_exit_code = $code
        failure_code = if ($code -eq 0) { 0 } else { $FailureCode }
        log = [IO.Path]::GetFileName($log)
        passed = ($code -eq 0)
        optional = [bool]$Optional
    })
    if ($code -ne 0 -and !$Optional) {
        Stop-Qualification $FailureCode "Required $Name check failed (native exit $code); see $([IO.Path]::GetFileName($log))."
    }
    return $output
}

try {
    [void][IO.Directory]::CreateDirectory($evidence)
    [void][IO.Directory]::CreateDirectory($build)
    [void][IO.Directory]::CreateDirectory((Join-Path $build 'java-source-study-classes'))

    $null = Invoke-QualifiedCommand 'cmake-path' 'command -v cmake' 21
    $null = Invoke-QualifiedCommand 'ctest-path' 'command -v ctest' 22
    $null = Invoke-QualifiedCommand 'cxx-path' 'command -v c++' 23
    $containerBuild = '/work/' + $BuildDir
    $containerEvidence = '/work/' + [IO.Path]::GetFileName($evidence)
    $null = Invoke-QualifiedCommand 'cmake-configure' ('cmake -S shared -B ' + $containerBuild + ' -DCMAKE_BUILD_TYPE=Release') 41
    $null = Invoke-QualifiedCommand 'cmake-build' ('cmake --build ' + $containerBuild + ' --config Release --parallel 2') 42
    $null = Invoke-QualifiedCommand 'cmake-ctest' ('ctest --test-dir ' + $containerBuild + ' -C Release --output-on-failure --output-junit ' + $containerEvidence + '/ctest-results.xml') 43
    $record.test_outcome = 'passed: native C/C++ acceptance under SDK-free image; not Android NDK'

    $ndkProbe = 'for p in "${ANDROID_NDK_HOME:-}" "${ANDROID_NDK_ROOT:-}"; do if [ -n "$p" ] && [ -f "$p/build/cmake/android.toolchain.cmake" ]; then printf "NDK_PRESENT\n"; exit 0; fi; done; printf "NDK_ABSENT_EXPECTED\n"'
    $probe = @(Invoke-QualifiedCommand 'ndk-optional-discovery' $ndkProbe 26)
    $record.ndk_present = ($probe -contains 'NDK_PRESENT')
    if (!$record.ndk_present) {
        Write-Host 'SWE-ANDROID-SOURCE-01: SDK/NDK intentionally omitted from public source image. No NDK cross-compile claimed.'
    }

    $null = Invoke-QualifiedCommand 'javac-optional' 'command -v javac' 24 -Optional
    $hasJavac = ($lastCode -eq 0)
    $null = Invoke-QualifiedCommand 'java-optional' 'command -v java' 24 -Optional
    $hasJava = ($lastCode -eq 0)
    if ($hasJavac -and $hasJava) {
        $null = Invoke-QualifiedCommand 'java17-version' 'v=$(javac -version 2>&1); printf "%s\n" "$v"; case "$v" in "javac 17"|"javac 17."*) exit 0 ;; *) exit 24 ;; esac' 24
        $pkg='android/app/src/main/java/com/simplywellexecuted/fastestwhisper10d'
        $testPkg='android/app/src/test/java/com/simplywellexecuted/fastestwhisper10d'
        $sources=@(
            "$pkg/Stability.java",
            "$pkg/Acoustic10D.java",
            "$testPkg/StandaloneTest.java"
        )
        foreach ($file in $sources) {
            if (!(Test-Path -LiteralPath (Join-Path $root $file) -PathType Leaf)) {
                Stop-Qualification 25 "Required Java source is missing: $file"
            }
        }
        $classDir = '/work/' + $BuildDir + '/java-source-study-classes'
        $sourceArgs = ($sources | ForEach-Object { '/work/' + $_ }) -join ' '
        $compile = 'javac -encoding UTF-8 --release 17 -d ' + $classDir + ' ' + $sourceArgs
        $null = Invoke-QualifiedCommand 'java17-compile' $compile 31
        $execute = 'java -cp ' + $classDir + ' com.simplywellexecuted.fastestwhisper10d.StandaloneTest'
        $null = Invoke-QualifiedCommand 'java17-acceptance' $execute 32
        $record.java_test_outcome = 'passed'
    } else {
        $record.java_test_outcome = 'not-run: published image lacks optional java/javac; disposable Android SDK stage covers APK validation'
        Write-Host 'SWE-ANDROID-SOURCE-05: Java/JDK not in published digest; Java tests not claimed here.'
    }
    Write-Host 'PASS: pinned SDK-free source-image native C/C++ acceptance checks; optional Java scope separately recorded.'

} catch {
    $record.test_outcome = 'failed'
    if ($failureCode -eq 0) {
        $failureCode = 90
        $failureReason = $_.Exception.Message
    }
    $message = ([string]$failureReason).Replace("`r", ' ').Replace("`n", ' ')
    Write-Host "::error title=SWE-Android-source-$failureCode::$message"
} finally {
    $record.failure_code = $failureCode
    $record.failure_reason = $failureReason
    $record.stage_results = @($stages.ToArray())
    try {
        $record | ConvertTo-Json -Depth 7 |
            Set-Content -LiteralPath (Join-Path $evidence 'android-source-runtime.json') -Encoding utf8
    } catch {
        Write-Host "::error title=SWE-evidence-write::Android source runtime record was not written: $($_.Exception.Message)"
        if ($failureCode -eq 0) { $failureCode = 91 }
    }
}
if ($failureCode -ne 0) {
    throw "Android source runtime rejected: failure code $failureCode; $failureReason"
}
