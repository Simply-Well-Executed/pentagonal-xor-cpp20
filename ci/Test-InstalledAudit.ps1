# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Consumer test: compile and execute the installed public header API.

[CmdletBinding()]
param(
    [string] $Stage = 'stage',
    [string] $BuildDir = 'build-consumer',
    [string] $VsArch = ''
)

$ErrorActionPreference = 'Stop'
$prefix = [IO.Path]::GetFullPath($Stage).Replace('\\', '/')
$required = @(
    'include/swe/deca_sigma.hpp',
    'include/swe/tri_pi.hpp',
    'include/swe/coreao.hpp',
    'include/swe/pentagonal_xor_std_free.h',
    'lib/cmake/SWEAuditHeaders/SWEAuditHeadersTargets.cmake'
)
foreach ($path in $required) {
    if (!(Test-Path -LiteralPath (Join-Path $prefix $path) -PathType Leaf)) {
        throw "Release stage is missing public API: $path"
    }
}

$args = @('-S', 'tests/consumer', '-B', $BuildDir,
          "-DSWE_AUDIT_PREFIX=$prefix")
if ($IsWindows -and ![string]::IsNullOrWhiteSpace($VsArch)) {
    $args += @('-A', $VsArch)
}
& cmake @args
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& cmake --build $BuildDir --config Release --parallel
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& ctest --test-dir $BuildDir -C Release --output-on-failure
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host 'PASS: shipped headers compile and run from an isolated consumer project.'
