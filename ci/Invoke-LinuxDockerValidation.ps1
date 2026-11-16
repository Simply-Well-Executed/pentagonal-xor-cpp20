# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Disposable non-privileged Linux C++20/C11 validation stage.
[CmdletBinding()]
param()
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence = Join-Path $root 'evidence-linux'
[void][IO.Directory]::CreateDirectory($evidence)

function Invoke-Check([string]$Name, [string]$LogPath, [string[]]$Arguments) {
    & $Name @Arguments *>&1 | Tee-Object -FilePath $LogPath
    if ($LASTEXITCODE -ne 0) { throw "$Name rejected validation: $LASTEXITCODE" }
}

& (Join-Path $PSScriptRoot 'Test-ReleaseSafety.ps1')
Invoke-Check cmake (Join-Path $evidence 'configure-root.log') @(
    '-S','.','-B','build-root-linux','-DCMAKE_BUILD_TYPE=Release','-DBUILD_TESTING=ON')
Invoke-Check cmake (Join-Path $evidence 'build-root.log') @(
    '--build','build-root-linux','--parallel')
Invoke-Check ctest (Join-Path $evidence 'test-root.log') @(
    '--test-dir','build-root-linux','--output-on-failure','--output-junit',
    (Join-Path $evidence 'root-junit.xml'))

Invoke-Check cmake (Join-Path $evidence 'configure-shared.log') @(
    '-S','shared','-B','build-shared-linux','-DCMAKE_BUILD_TYPE=Release')
Invoke-Check cmake (Join-Path $evidence 'build-shared.log') @(
    '--build','build-shared-linux','--parallel')
Invoke-Check ctest (Join-Path $evidence 'test-shared.log') @(
    '--test-dir','build-shared-linux','--output-on-failure','--output-junit',
    (Join-Path $evidence 'shared-junit.xml'))

Invoke-Check cmake (Join-Path $evidence 'install.log') @(
    '--install','build-root-linux','--config','Release','--prefix','stage')
& (Join-Path $PSScriptRoot 'Test-InstalledAudit.ps1') -Stage stage -BuildDir build-consumer-linux *>&1 |
    Tee-Object -FilePath (Join-Path $evidence 'consumer.log')
if (!$?) { throw 'Installed C++20 consumer rejected staged headers.' }

$manifest = [ordered]@{
    schema = 'SWE-GAMP4-style-container-CPP-v1'
    source_contract = 'root C++20 + shared C11/C++20 + installed consumer'
    checks_complete = $true
    validation_stage_only = $true
    sdk_included = $false
    generated_utc = (Get-Date).ToUniversalTime().ToString('o')
}
$manifest | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $evidence 'manifest.json') -Encoding utf8
$rows = Get-ChildItem -LiteralPath $evidence -File | Sort-Object Name | ForEach-Object {
    '{0}  {1}' -f (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant(),$_.Name
}
$rows | Set-Content -LiteralPath (Join-Path $evidence 'SHA256SUMS') -Encoding utf8
Write-Output 'PASS Docker Linux C++20/C11: all root, shared and downstream consumer checks.'
