# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Pinned Docker build witness for the shared C/C++ guard.
# Requires PowerShell 7 on a Linux GitHub-hosted runner and Docker.
# No Node/npm and no Android Debug variant is invoked.

[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidateSet('linux', 'android')][string] $Flavor,
    [Parameter(Mandatory)][string] $Image,
    [Parameter(Mandatory)][string] $Digest,
    [Parameter(Mandatory)][ValidateSet('linux/amd64', 'linux/arm64')][string] $Platform,
    [string] $BuildDir = 'build-shared-container',
    [string] $EvidenceDir = 'evidence'
)

$ErrorActionPreference = 'Stop'
if (!$IsLinux) { throw 'Pinned Docker execution requires a Linux host.' }
if ($Digest -cnotmatch '^sha256:[a-f0-9]{64}$') {
    throw 'A full 64-hex SHA-256 image digest is mandatory. A mutable latest tag is not a pin.'
}
$expectedImage = if ($Flavor -eq 'linux') {
    'paulcheek/opensuse-leap-hardened:latest'
} else {
    'paulcheek/android-source-study-v1:latest'
}
if ($Image -cne $expectedImage) { throw "Unexpected image: $Image" }
if ($Flavor -eq 'android' -and $Platform -ne 'linux/amd64') {
    throw 'Android study image is restricted to the explicitly configured amd64 audit/build target.'
}

$root = [IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot))
$evidence = Join-Path $root $EvidenceDir
New-Item -ItemType Directory -Force -Path $evidence | Out-Null
$reference = "$Image@$Digest"
$uid = ((& id -u) | Select-Object -First 1).ToString().Trim()
$gid = ((& id -g) | Select-Object -First 1).ToString().Trim()
if ($LASTEXITCODE -ne 0 -or !$uid -or !$gid) { throw 'Unable to identify non-root host user.' }
if ($uid -eq '0') { throw 'Container build host user must not be root.' }

$manifest = [ordered]@{
    schema = 'SWE-pinned-container-guard-v1'
    generated_utc = (Get-Date).ToUniversalTime().ToString('o')
    flavor = $Flavor
    requested_image = $Image
    immutable_reference = $reference
    platform = $Platform
    source_sha = $env:GITHUB_SHA
    no_node_build_commands = $true
    android_variant = if ($Flavor -eq 'android') { 'not-applicable: published SDK-free source image; native C/C++ only' } else { 'not-applicable' }
    runtime_tests = if ($Flavor -eq 'android') { 'Native C/C++ CTest and optional Java; NDK compile and APK execution not run' } else { 'CTest on container' }
    validation_scope = if ($Flavor -eq 'android') { 'SDK-free native CMake/CTest qualification; Java optional' } else { 'native CMake and CTest inside openSUSE' }
    third_party_license_review = 'required before redistribution; no toolchains or image layers are included in source license grant'
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $evidence 'container-manifest.json') -Encoding utf8

& docker pull --platform $Platform $reference *>&1 |
    Tee-Object -FilePath (Join-Path $evidence 'docker-pull.log')
if ($LASTEXITCODE -ne 0) { throw "Pinned Docker pull failed: $reference" }

& docker image inspect --format '{{json .RepoDigests}}' $reference *>&1 |
    Tee-Object -FilePath (Join-Path $evidence 'docker-repodigests.json')
if ($LASTEXITCODE -ne 0) { throw 'Image inspection failed.' }

$baseArgs = @(
    'run', '--rm', '--platform', $Platform,
    '--network', 'none', '--read-only',
    '--cap-drop', 'ALL', '--security-opt', 'no-new-privileges',
    '--tmpfs', '/tmp:rw,nosuid,nodev',
    '--user', "${uid}:${gid}",
    '--mount', "type=bind,src=$root,dst=/work",
    '--workdir', '/work', '--entrypoint', '/bin/sh',
    $reference, '-ec'
)

$stageRecords = [System.Collections.Generic.List[object]]::new()
function Get-ExitDiagnosis {
    param([int] $ExitCode)
    switch ($ExitCode) {
        0 { return 'pass' }
        1 { return 'generic command failure: inspect individual check output' }
        2 { return 'incorrect invocation or tool-specific failure' }
        21 { return 'source-image requirement: CMake unavailable' }
        22 { return 'source-image requirement: CTest unavailable' }
        23 { return 'source-image requirement: C++ compiler unavailable' }
        24 { return 'source-image requirement: Java compiler version mismatch' }
        25 { return 'source-image requirement: missing Java source file' }
        31 { return 'Java source compilation failed' }
        32 { return 'Java acceptance tests failed' }
        41 { return 'Native CMake configure failed in pinned source image' }
        42 { return 'Native C/C++ compile failed in pinned source image' }
        43 { return 'Native CTest failed in pinned source image' }
        90 { return 'unclassified runtime exception; see android-source-runtime.json' }
        91 { return 'could not write source-runtime evidence file' }
        124 { return 'external timeout; inspect runner logs' }
        125 { return 'Docker engine, invocation, or daemon failure' }
        126 { return 'container command present but not executable' }
        127 { return 'container command not found' }
        130 { return 'terminated by interrupt (SIGINT on Unix)' }
        137 { return 'terminated by SIGKILL; inspect resource limits and OOM evidence' }
        143 { return 'terminated by SIGTERM; inspect cancellation and timeout' }
        default { return 'unclassified tool exit: inspect stage log and preserve raw numeric code' }
    }
}
function Invoke-ContainerStage {
    param([Parameter(Mandatory)][string] $Name, [Parameter(Mandatory)][string] $Command)
    Write-Host "Container stage: $Name"
    $started = (Get-Date).ToUniversalTime().ToString('o')
    & docker @baseArgs $Command *>&1 |
        Tee-Object -FilePath (Join-Path $evidence "$Name.log")
    $code = [int]$LASTEXITCODE
    $diagnosis = Get-ExitDiagnosis $code
    $stageRecords.Add([ordered]@{
        stage = $Name
        started_utc = $started
        finished_utc = (Get-Date).ToUniversalTime().ToString('o')
        exit_code = $code
        diagnosis = $diagnosis
        log = "$Name.log"
        passed = ($code -eq 0)
    })
    $stageRecords | ConvertTo-Json -Depth 5 |
        Set-Content -LiteralPath (Join-Path $evidence 'container-stage-results.json') -Encoding utf8
    if ($code -ne 0) {
        Write-Host "::error title=SWE container stage '$Name'::$diagnosis (exit=$code). See $Name.log and container-stage-results.json."
        throw "Container stage '$Name' failed (exit=$code; diagnosis=$diagnosis)"
    }
}

if ($Flavor -eq 'linux') {
    Invoke-ContainerStage -Name 'toolchain' -Command 'command -v cmake; command -v ctest; command -v c++; cmake --version; c++ --version'
    Invoke-ContainerStage -Name 'configure' -Command "cmake -S shared -B $BuildDir -DCMAKE_BUILD_TYPE=Release"
    Invoke-ContainerStage -Name 'build' -Command "cmake --build $BuildDir --config Release --parallel 2"
    Invoke-ContainerStage -Name 'test' -Command "ctest --test-dir $BuildDir -C Release --output-on-failure --output-junit /work/$EvidenceDir/ctest-results.xml"
} else {
    # The pinned public image is intentionally SDK-free. It has Java 17
    # and CMake, but does not contain an NDK. Validate exactly that scope.
    # An actual unsigned Release APK is independently built in the
    # disposable android-sdk stage by the docker-android workflow job.
    # Run the envscript in the HOST PowerShell environment. The immutable
    # SDK-free container needs only /bin/sh, Java and javac, not pwsh.
    $started = (Get-Date).ToUniversalTime().ToString('o')
    $runtimeError = $null
    try {
        & (Join-Path $PSScriptRoot 'Initialize-AndroidRuntime.ps1') -DockerBaseArgs $baseArgs -EvidenceDir $evidence -BuildDir $BuildDir *>&1 |
            Tee-Object -FilePath (Join-Path $evidence 'android-source-runtime.log')
    } catch {
        $runtimeError = $_.Exception.Message
    } finally {
        $runtimeStatus = 0
        $runtimeRecord = Join-Path $evidence 'android-source-runtime.json'
        if (Test-Path -LiteralPath $runtimeRecord -PathType Leaf) {
            try {
                $runtimeStatus = [int]((Get-Content -LiteralPath $runtimeRecord -Raw | ConvertFrom-Json).failure_code)
            } catch {
                if ($runtimeStatus -eq 0) { $runtimeStatus = 90 }
                if (!$runtimeError) { $runtimeError = 'Unable to read Android source runtime diagnostic JSON.' }
            }
        } elseif (!$runtimeError) {
            $runtimeStatus = 91
            $runtimeError = 'Missing source runtime diagnostic JSON.'
        }
        if ($runtimeError -and $runtimeStatus -eq 0) { $runtimeStatus = 90 }
        $stageRecords.Add([ordered]@{
            stage = 'android-source-runtime'
            started_utc = $started
            finished_utc = (Get-Date).ToUniversalTime().ToString('o')
            exit_code = $runtimeStatus
            diagnosis = if ($runtimeStatus -eq 0) { 'pass' } else { $runtimeError }
            log = 'android-source-runtime.log'
            passed = ($runtimeStatus -eq 0)
        })
        $stageRecords | ConvertTo-Json -Depth 5 |
            Set-Content -LiteralPath (Join-Path $evidence 'container-stage-results.json') -Encoding utf8
    }
    if ($runtimeStatus -ne 0) {
        throw "Pinned Android source-image qualification failed (code=$runtimeStatus): $runtimeError"
    }
}
Write-Host "Pinned $Flavor build completed: $reference ($Platform)"
# Explicitly own the final job status; optional source-image probes can exit 127.
$global:LASTEXITCODE = 0
exit 0
