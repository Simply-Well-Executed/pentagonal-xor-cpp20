# SPDX-License-Identifier: 0BSD
# Native Windows C++20 verification: no hosted runner, image, container or API calls.
# Latest-stable version is asserted by the operator, not discovered over a network.
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][version] $LatestStableVersion,
    [Parameter(Mandatory = $true)][string] $ReleaseEvidenceUri
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidencePath = Join-Path $root '.iron-native-evidence.json'
$record = [ordered]@{
    schema = 'SWE-IRON-NATIVE-v1'
    started_utc = [DateTime]::UtcNow.ToString('o')
    completed_utc = $null
    source_commit = $null
    platform = [Environment]::OSVersion.VersionString
    host_powershell = [string]$PSVersionTable.PSVersion
    operator_declared_latest = [string]$LatestStableVersion
    operator_release_evidence = $ReleaseEvidenceUri
    release_version_verified_online = $false
    runner_image = 'NONE'
    github_actions_runner = 'NONE'
    stages_passed = @()
    status = 'BLOCKED'
    failure_stage = $null
    failure = $null
}
$stage = 'prerequisites'
try {
    if (-not $IsWindows) { throw 'Native verification requires an actual Windows host.' }
    $expectedUrl = "https://github.com/PowerShell/PowerShell/releases/tag/v$LatestStableVersion"
    if ($ReleaseEvidenceUri -cne $expectedUrl) {
        throw "Release evidence must identify the declared official version: $expectedUrl"
    }
    $actual = [version]$PSVersionTable.PSVersion
    if ($actual -ne $LatestStableVersion) {
        throw "PowerShell $actual differs from operator-declared latest stable $LatestStableVersion. No build attempted."
    }
    foreach ($tool in @('git', 'cmake', 'ctest')) {
        if ($null -eq (Get-Command -Name $tool -ErrorAction SilentlyContinue)) {
            throw "Required local tool is unavailable: $tool"
        }
    }
    $record.stages_passed += $stage

    $stage = 'source-integrity'
    $head = @(& git -C $root rev-parse --verify HEAD)
    if ($LASTEXITCODE -ne 0 -or $head.Count -ne 1) { throw 'Local source commit cannot be verified.' }
    $record.source_commit = ([string]$head[0]).Trim()
    & git -C $root diff --quiet --exit-code HEAD --
    if ($LASTEXITCODE -ne 0) { throw 'Tracked source has uncommitted changes; review before building.' }
    $record.stages_passed += $stage

    $stage = 'magic-comb'
    $comb = Join-Path $root 'ci/Invoke-MagicComb.ps1'
    if (-not (Test-Path -LiteralPath $comb -PathType Leaf)) { throw 'Magic Comb verifier is missing.' }
    & $comb -Mode SelfTest -RepositoryRoot $root -NestingDepth 9
    if (-not $?) { throw 'Magic Comb self-test failed.' }
    & $comb -Mode Verify -RepositoryRoot $root -NestingDepth 9
    if (-not $?) { throw 'Magic Comb source verification failed.' }
    $record.stages_passed += $stage

    $stage = 'toolchain'
    $vswhere = Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswhere -PathType Leaf)) { throw 'Local Visual Studio C++ toolchain discovery is unavailable.' }
    $global:LASTEXITCODE = 0
    $versions = @(& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion)
    $probeExit = [int]$global:LASTEXITCODE
    if ($probeExit -ne 0 -or $versions.Count -eq 0) { throw 'Local MSVC installation not detected.' }
    $installed = [string]$versions[0]
    if ([string]::IsNullOrWhiteSpace($installed)) { throw 'MSVC version is empty.' }
    $generator = switch (([version]$installed).Major) {
        18 { 'Visual Studio 18 2026' }
        17 { 'Visual Studio 17 2022' }
        default { throw "Unsupported local MSVC version $installed" }
    }
    $record.msbuild_toolchain = $installed
    $record.cmake_generator = $generator
    $record.stages_passed += $stage

    $stage = 'native-cxx20-o2'
    Push-Location -LiteralPath $root
    try {
        & cmake -S . -B build -G $generator -A x64 -DBUILD_TESTING=ON -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF '-DCMAKE_CXX_FLAGS_RELEASE=/O2 /DNDEBUG'
        if ($LASTEXITCODE -ne 0) { throw 'Local CMake configure failed.' }
        & cmake --build build --config Release --parallel
        if ($LASTEXITCODE -ne 0) { throw 'Local C++20 /O2 build failed.' }
        & ctest --test-dir build -C Release --output-on-failure
        if ($LASTEXITCODE -ne 0) { throw 'Local CTest verification failed.' }
    } finally {
        Pop-Location
    }
    $record.stages_passed += $stage
    $record.status = 'PASS'
    Write-Host "IRON|NATIVE|PASS|source=$($record.source_commit)|PowerShell=$actual"
} catch {
    $record.failure_stage = $stage
    $record.failure = [string]$_.Exception.Message
    throw
} finally {
    $record.completed_utc = [DateTime]::UtcNow.ToString('o')
    $record | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $evidencePath -Encoding utf8
    Write-Host "IRON|NATIVE|EVIDENCE|$evidencePath"
}
