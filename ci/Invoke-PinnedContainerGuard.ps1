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
    android_variant = if ($Flavor -eq 'android') { 'Release; compile-only' } else { 'not-applicable' }
    runtime_tests = if ($Flavor -eq 'android') { 'not-run: cross-compiled binaries need a device/emulator' } else { 'CTest on container' }
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

function Invoke-ContainerStage {
    param([string] $Name, [string] $Command)
    Write-Host "Container stage: $Name"
    & docker @baseArgs $Command *>&1 |
        Tee-Object -FilePath (Join-Path $evidence "$Name.log")
    $code = $LASTEXITCODE
    if ($code -ne 0) { throw "Container $Name failed with exit code $code" }
}

if ($Flavor -eq 'linux') {
    Invoke-ContainerStage -Name 'toolchain' -Command 'command -v cmake; command -v ctest; command -v c++; cmake --version; c++ --version'
    Invoke-ContainerStage -Name 'configure' -Command "cmake -S shared -B $BuildDir -DCMAKE_BUILD_TYPE=Release"
    Invoke-ContainerStage -Name 'build' -Command "cmake --build $BuildDir --config Release --parallel 2"
    Invoke-ContainerStage -Name 'test' -Command "ctest --test-dir $BuildDir -C Release --output-on-failure --output-junit /work/$EvidenceDir/ctest-results.xml"
} else {
    # Android image is a compilation environment, not proof of device execution.
    # Require a real NDK and never select a Debug build variant.
    Invoke-ContainerStage -Name 'android-toolchain' -Command 'command -v cmake; test -n "${ANDROID_NDK_HOME:-}"; test -f "$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake"; cmake --version'
    Invoke-ContainerStage -Name 'configure' -Command "test -n \"\${ANDROID_NDK_HOME:-}\"; cmake -S shared -B $BuildDir -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=\$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 -DANDROID_STL=c++_static"
    Invoke-ContainerStage -Name 'build' -Command "cmake --build $BuildDir --config Release --parallel 2"
    'Cross-compiled Android executables were not run on a device or emulator.' |
        Set-Content -LiteralPath (Join-Path $evidence 'android-runtime-test-limitation.txt') -Encoding utf8
}
Write-Host "Pinned $Flavor build completed: $reference ($Platform)"
