# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Fail-closed policy check for the isolated guard workflow.
[CmdletBinding()]
param()
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$workflow=Get-Content -LiteralPath (Join-Path $root '.github/workflows/shared-guard.yml') -Raw
$license=Get-Content -LiteralPath (Join-Path $root 'LICENSE') -Raw
if ($license -notmatch '0BSD|Zero Clause BSD') {
    throw 'Project license does not advertise Zero Clause BSD.'
}
$forbidden=[ordered]@{
 'Actions that require a JavaScript runtime'='(?im)^\s*uses\s*:'
 'JS package tooling'='(?i)\b(?:node|npm|npx|yarn)\b'
 'Android developmental signing/build output'='(?i)android.{0,100}debug|debug.{0,100}android'
}
foreach($rule in $forbidden.GetEnumerator()){
 if([regex]::IsMatch($workflow,$rule.Value)){throw "Guard violates policy: $($rule.Key)"}
}
foreach($required in @('docker-android:','docker-leap:',
    'paulcheek/android-source-study-v1:latest',
    'paulcheek/opensuse-leap-hardened:latest',
    'OS_LICENSE_REVIEW_APPROVED',
    'ci/Record-GuardEvidence.ps1')) {
 if(!$workflow.Contains($required)){throw "Missing guard invariant: $required"}
}
Write-Output 'PASS shared guard: zero JS-based actions, unsigned Android, dual pinned validation, project 0BSD.'
