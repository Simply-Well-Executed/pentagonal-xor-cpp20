# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Collect a reproducible, read-only CI evidence snapshot on success or failure.
# Compiler executables are fingerprinted, never copied or relicensed.
# Run in PowerShell 7 on Windows, Linux and macOS.

[CmdletBinding()]
param(
    [string] $BuildDir = 'build',
    [string] $EvidenceDir = 'evidence',
    [string] $BuildOutcome = 'not-run',
    [string] $TestOutcome = 'not-run',
    [string] $TargetArch = 'native'
)

$ErrorActionPreference = 'Stop'
$evidence = [IO.Path]::GetFullPath($EvidenceDir)
$build = [IO.Path]::GetFullPath($BuildDir)
New-Item -ItemType Directory -Force -Path $evidence | Out-Null
$metadata = Join-Path $evidence 'metadata'
$buildFiles = Join-Path $evidence 'build-files'
New-Item -ItemType Directory -Force -Path $metadata, $buildFiles | Out-Null

$toolPaths = [System.Collections.Generic.List[object]]::new()
$seenToolPaths = [System.Collections.Generic.HashSet[string]]::new(
    [StringComparer]::OrdinalIgnoreCase)

function Add-ToolFingerprint {
    param([string] $Name, [string] $Path)
    if ([string]::IsNullOrWhiteSpace($Path)) { return }
    $resolved = [IO.Path]::GetFullPath($Path)
    if (!(Test-Path -LiteralPath $resolved -PathType Leaf)) { return }
    if (!$seenToolPaths.Add($resolved)) { return }
    $info = Get-Item -LiteralPath $resolved
    $toolPaths.Add([ordered]@{
        name = $Name
        path = $resolved
        size_bytes = $info.Length
        sha256 = (Get-FileHash -LiteralPath $resolved -Algorithm SHA256).Hash.ToLowerInvariant()
        binary_redistributed = $false
    })
}

foreach ($name in @('cmake', 'ctest', 'git', 'pwsh', 'c++', 'g++', 'clang++',
                    'clang-cl.exe', 'cl.exe', 'link.exe', 'ld', 'objcopy')) {
    $command = Get-Command $name -ErrorAction SilentlyContinue |
        Where-Object { $_.Source -and (Test-Path -LiteralPath $_.Source -PathType Leaf) } |
        Select-Object -First 1
    if ($command) { Add-ToolFingerprint -Name $name -Path $command.Source }
}

# Visual Studio often launches cl.exe through MSBuild, outside the pwsh PATH.
# The exact target architecture is supplied by the workflow matrix.
if ($IsWindows) {
    $vswhere = Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) (
        'Microsoft Visual Studio/Installer/vswhere.exe')
    if (Test-Path -LiteralPath $vswhere -PathType Leaf) {
        Add-ToolFingerprint -Name 'vswhere.exe' -Path $vswhere
        $vsRoot = & $vswhere -latest -products '*' -property installationPath
        if ($LASTEXITCODE -eq 0 -and $vsRoot) {
            $msvcRoot = Join-Path ([string]($vsRoot | Select-Object -First 1)) 'VC/Tools/MSVC'
            if (Test-Path -LiteralPath $msvcRoot) {
                $clCandidates = @(Get-ChildItem -LiteralPath $msvcRoot -Filter cl.exe -File -Recurse -ErrorAction SilentlyContinue |
                    Where-Object { $_.DirectoryName -match ('[\\/]' + [regex]::Escape($TargetArch) + '$') } |
                    Sort-Object FullName)
                foreach ($candidate in $clCandidates) {
                    Add-ToolFingerprint -Name 'MSVC cl.exe candidate' -Path $candidate.FullName
                }
            }
        }
    }
}

if ($IsMacOS) {
    $xcrun = Get-Command xcrun -ErrorAction SilentlyContinue
    if ($xcrun) {
        $clang = & xcrun --find clang++ 2>$null
        if ($LASTEXITCODE -eq 0 -and $clang) {
            Add-ToolFingerprint -Name 'Xcode clang++' -Path ([string]$clang)
        }
    }
}

# Compiler identity as reported by CMake, retained independently of the
# executable fingerprints. In a failed configure this file may not exist.
if (Test-Path -LiteralPath $build -PathType Container) {
    $compilerInfo = Get-ChildItem -LiteralPath $build -Filter CMakeCXXCompiler.cmake -File -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($compilerInfo) {
        Copy-Item -LiteralPath $compilerInfo.FullName -Destination (Join-Path $metadata 'CMakeCXXCompiler.cmake')
    }
    $cache = Join-Path $build 'CMakeCache.txt'
    if (Test-Path -LiteralPath $cache -PathType Leaf) {
        Copy-Item -LiteralPath $cache -Destination (Join-Path $metadata 'CMakeCache.txt')
    }
}

# Retain locally produced binaries, MSVC link/compile PDBs, object files,
# and native debug files, including partial outputs from failed compilation.
# Never copy the compiler executable from its installation directory.
if (Test-Path -LiteralPath $build -PathType Container) {
    $extensions = @('.pdb', '.obj', '.o', '.exe', '.dll', '.lib',
                    '.so', '.dylib', '.a', '.debug', '.ilk')
    $outputs = Get-ChildItem -LiteralPath $build -File -Recurse -ErrorAction SilentlyContinue |
        Where-Object {
            ($extensions -contains $_.Extension.ToLowerInvariant()) -or
            ($_.Name -in @('pentagonal_xor', 'swe_guard', 'deca_sigma_test',
                          'tri_pi_test', 'coreao_test', 'parity_reference_test',
                          'pentagonal_xor_std_free_test'))
        }
    foreach ($file in $outputs) {
        $relative = [IO.Path]::GetRelativePath($build, $file.FullName)
        $target = Join-Path $buildFiles $relative
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
        Copy-Item -LiteralPath $file.FullName -Destination $target -Force
    }
}

# Preserve the complete source snapshot, including 0BSD license,
# exact workflow definitions, tests and documentation, at the built commit.
$archive = Join-Path $evidence 'source-snapshot.zip'
$git = Get-Command git -ErrorAction SilentlyContinue
if ($git) {
    & git archive --format=zip --output="$archive" HEAD
    if ($LASTEXITCODE -ne 0) {
        Write-Warning 'Source archive failed; source commit is still recorded.'
    }
}

$versions = [ordered]@{}
foreach ($name in @('cmake', 'ctest', 'git', 'pwsh')) {
    try {
        $output = & $name --version 2>&1 | Select-Object -First 3
        $versions[$name] = (@($output) -join ' ').Trim()
    } catch {
        $versions[$name] = 'unavailable'
    }
}

$resolvedHead = 'unavailable'
try {
    $resolvedHead = ((& git rev-parse HEAD 2>$null) | Select-Object -First 1)
} catch { }

$manifest = [ordered]@{
    schema = 'SWE-CI-evidence-v1'
    generated_utc = (Get-Date).ToUniversalTime().ToString('o')
    repo = $env:GITHUB_REPOSITORY
    run_id = $env:GITHUB_RUN_ID
    run_attempt = $env:GITHUB_RUN_ATTEMPT
    workflow = $env:GITHUB_WORKFLOW
    requested_ref = $env:GITHUB_REF
    github_sha = $env:GITHUB_SHA
    checked_out_head = $resolvedHead
    runner_os = $env:RUNNER_OS
    runner_arch = $env:RUNNER_ARCH
    target_arch = $TargetArch
    build_outcome = $BuildOutcome
    test_outcome = $TestOutcome
    compiler_binaries_included = $false
    compiler_policy = 'hash/version/path only; third-party license governs redistribution'
    versions = $versions
    tools = @($toolPaths)
}
$manifest | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath (Join-Path $evidence 'manifest.json') -Encoding utf8

# Conventional sha256sum format, with paths relative to evidence/.
# The SHA256SUMS file is intentionally excluded from hashing itself.
$checksums = [System.Collections.Generic.List[string]]::new()
Get-ChildItem -LiteralPath $evidence -File -Recurse |
    Where-Object { $_.FullName -ne (Join-Path $evidence 'SHA256SUMS') } |
    Sort-Object FullName |
    ForEach-Object {
        $relative = [IO.Path]::GetRelativePath($evidence, $_.FullName).Replace('\', '/')
        $hash = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
        $checksums.Add("$hash  $relative")
    }
$checksums | Set-Content -LiteralPath (Join-Path $evidence 'SHA256SUMS') -Encoding utf8

Write-Host "Evidence captured: $($checksums.Count) SHA-256 entries; build=$BuildOutcome test=$TestOutcome"
Write-Host 'Compiler paths, versions and SHA256 are metadata only, not redistributed binaries.'
