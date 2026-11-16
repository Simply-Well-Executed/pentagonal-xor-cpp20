# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Execute only inside disposable SDK-bearing isolated validation stage.
[CmdletBinding()]
param()
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence = Join-Path $root 'evidence-android'
[void][IO.Directory]::CreateDirectory($evidence)
$log = Join-Path $evidence 'build-and-test.log'
$passed = $false
try {
    & (Join-Path $PSScriptRoot 'Test-ReleaseSafety.ps1') *>&1 |
        Tee-Object -FilePath (Join-Path $evidence 'preflight.log')
    if (!$?) { throw 'Source pre-flight failed.' }
    & (Join-Path $PSScriptRoot 'Build-AndroidUnsigned.ps1') *>&1 |
        Tee-Object -FilePath $log
    if (!$?) { throw 'Unsigned Android release-candidate compilation or structural validation failed.' }
    $passed = $true
} finally {
    & (Join-Path $PSScriptRoot 'Write-AndroidUnsignedEvidence.ps1') -EvidenceDir 'evidence-android'
}
if (!$passed) { throw 'Disposable Android validation stage did not pass.' }
$apk = Join-Path $root 'android/app/build/outputs/apk/release/app-unsigned.apk'
if (!(Test-Path -LiteralPath $apk -PathType Leaf)) {
    throw 'Validation produced no unsigned release-candidate APK.'
}
Write-Output ('PASS Docker Android SDK-only APK, SHA256 {0}' -f (
    Get-FileHash -LiteralPath $apk -Algorithm SHA256).Hash.ToLowerInvariant())
