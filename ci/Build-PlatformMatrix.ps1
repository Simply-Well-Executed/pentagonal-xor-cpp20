[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('windows-x86','windows-x64','windows-x86-64',
                 'windows-amd64','windows-arm64')]
    [string] $Profile,
    [Parameter(Mandatory = $true)]
    [version] $LatestStablePowerShellVersion,
    [Parameter(Mandatory = $true)]
    [string] $ReleaseEvidenceUri
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

if ([Environment]::OSVersion.Platform -ne [PlatformID]::Win32NT) {
    throw 'Windows matrix builder requires a native Windows host'
}
if ($PSVersionTable.PSVersion.Major -lt 7) {
    throw 'PowerShell 7 or newer is required'
}
$expectedReleaseUri = "https://github.com/PowerShell/PowerShell/releases/tag/v$LatestStablePowerShellVersion"
if ($ReleaseEvidenceUri -cne $expectedReleaseUri) {
    throw "Release URI must match independently reviewed version: $expectedReleaseUri"
}
$presentVersion = [version]$PSVersionTable.PSVersion
if ($presentVersion -ne $LatestStablePowerShellVersion) {
    throw "PowerShell $presentVersion differs from operator-declared latest $LatestStablePowerShellVersion"
}

$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$manifest = Join-Path $root 'ci/native-platform-matrix.yml'
if (-not (Test-Path -LiteralPath $manifest -PathType Leaf)) {
    throw 'Local YAML platform matrix is missing'
}
$manifestLine = '  ' + $Profile + ':'
if (-not (Select-String -LiteralPath $manifest -SimpleMatch -Pattern $manifestLine -Quiet)) {
    throw "Profile not declared in native-platform-matrix.yml: $Profile"
}
foreach ($tool in @('git', 'cmake')) {
    if ($null -eq (Get-Command -Name $tool -ErrorAction SilentlyContinue)) {
        throw "Required installed local executable is missing: $tool"
    }
}
$canonical = switch ($Profile) {
    'windows-x86' { 'windows-x86' }
    'windows-arm64' { 'windows-arm64' }
    default { 'windows-x64' }
}
$architecture = switch ($canonical) {
    'windows-x86' { 'Win32' }
    'windows-arm64' { 'ARM64' }
    default { 'x64' }
}
$build = Join-Path $root "build/iron-matrix/$canonical"
$configure = Join-Path $build 'configured'
$artifacts = Join-Path $build 'artifacts'
[void][IO.Directory]::CreateDirectory($build)
$record = [ordered]@{
    schema = 'SWE-IRON-PLATFORM-RESULT-v1'
    profile = $Profile
    canonical_profile = $canonical
    source_commit = 'UNKNOWN'
    host_os = 'Windows'
    host_cpu = [Environment]::GetEnvironmentVariable('PROCESSOR_ARCHITECTURE')
    stage = 'preflight'
    status = 'FAIL'
    scope = 'std-free-core-archive-only'
    runtime_tests = 'NOT_TESTED'
    full_repository_std_free = 'NOT_CERTIFIED'
    release_version_verified_online = $false
    operator_declared_latest_powershell = [string]$LatestStablePowerShellVersion
    downloads_executed_by_script = 0
}
try {
    & git -C $root diff --quiet --exit-code HEAD --
    if ($LASTEXITCODE -ne 0) {
        throw 'Tracked source has changed; review/commit before verification'
    }
    $shaLines = @(& git -C $root rev-parse --verify HEAD)
    if ($LASTEXITCODE -ne 0 -or $shaLines.Count -ne 1) {
        throw 'Cannot verify local Git HEAD'
    }
    $sha = ([string]$shaLines[0]).Trim()
    if ($sha -notmatch '^[0-9a-fA-F]{40}$') {
        throw 'Malformed Git commit SHA'
    }
    $record.source_commit = $sha

    $record.stage = 'toolchain'
    $vswhere = Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswhere -PathType Leaf)) {
        throw 'No local Visual Studio vswhere discovery tool'
    }
    $vsVersionLines = @(& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion)
    if ($LASTEXITCODE -ne 0 -or $vsVersionLines.Count -eq 0) {
        throw 'Native Visual Studio MSVC C++ toolchain missing'
    }
    $vsVersion = ([string]$vsVersionLines[0]).Trim()
    $generator = switch (([version]$vsVersion).Major) {
        17 { 'Visual Studio 17 2022' }
        18 { 'Visual Studio 18 2026' }
        default { throw "Unsupported installed Visual Studio $vsVersion" }
    }
    $record.cmake_generator = $generator
    $record.cmake_architecture = $architecture

    $record.stage = 'configure'
    $configureArgs = @(
        '-S', $root, '-B', $configure,
        '-G', $generator, '-A', $architecture,
        '-DCMAKE_BUILD_TYPE=Release',
        '-DSWE_STD_FREE_CORE_ONLY=ON',
        '-DBUILD_TESTING=OFF',
        '-DCMAKE_CXX_STANDARD=20',
        '-DCMAKE_CXX_STANDARD_REQUIRED=ON',
        '-DCMAKE_CXX_EXTENSIONS=OFF',
        '-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF',
        '-DFETCHCONTENT_FULLY_DISCONNECTED=ON'
    )
    & cmake @configureArgs
    if ($LASTEXITCODE -ne 0) { throw 'Native CMake configuration failed' }

    $record.stage = 'compile'
    & cmake --build $configure --config Release --target swe_std_free_portable_core --parallel
    if ($LASTEXITCODE -ne 0) { throw 'Native STD-free archive build failed' }

    $record.stage = 'install'
    & cmake --install $configure --config Release --prefix $artifacts
    if ($LASTEXITCODE -ne 0) { throw 'Native archive installation failed' }

    $record.stage = 'complete'
    $record.status = 'ARCHIVE_BUILT_NOT_RUNTIME_TESTED'
    Write-Host "IRON|MATRIX|ARCHIVE_BUILT_NOT_RUNTIME_TESTED|$Profile|$sha"
}
catch {
    $record.failure = [string]$_.Exception.Message
    throw
}
finally {
    $lines = @()
    foreach ($key in $record.Keys) {
        $value = [string]$record[$key]
        $value = $value.Replace("'", "''")
        $lines += ("{0}: '{1}'" -f $key, $value)
    }
    $lines | Set-Content -LiteralPath (Join-Path $build 'iron-build-result.yml') -Encoding utf8
}
