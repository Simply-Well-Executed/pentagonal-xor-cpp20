# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Evidence for unsigned release-candidate output. No signing credentials.
[CmdletBinding()]
param([string]$EvidenceDir='evidence-android')
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence=[IO.Path]::GetFullPath((Join-Path $root $EvidenceDir))
[void][IO.Directory]::CreateDirectory($evidence)
$apk=Join-Path $root 'android/app/build/outputs/apk/release/app-unsigned.apk'
$buildLog=Join-Path $evidence 'build-and-test.log'
$passed=(Test-Path $buildLog -PathType Leaf) -and
  [bool](Select-String -LiteralPath $buildLog -Quiet -SimpleMatch '26654 independent Java checks')
$apkPresent=Test-Path $apk -PathType Leaf
$fingerprints=@(foreach($name in @('javac','java','jar','cmake','pwsh')) {
 $cmd=Get-Command $name -ErrorAction SilentlyContinue | Select-Object -First 1
 if($cmd -and (Test-Path -LiteralPath $cmd.Source -PathType Leaf)) {
   [ordered]@{name=$name;file=$cmd.Source;sha256=(Get-FileHash $cmd.Source -Algorithm SHA256).Hash.ToLowerInvariant()}
 }
})
$manifest=[ordered]@{
 schema='SWE-Android-Unsigned-GAMP4-Style-v1'
 github_sha=$env:GITHUB_SHA
 source_head=((& git rev-parse HEAD 2>$null) | Select-Object -First 1)
 run_id=$env:GITHUB_RUN_ID
 build_target='unsigned-release-candidate'
 signed=$false
 installable=$false
 java_tests_observed=$passed
 artifact_present=$apkPresent
 apk_sha256=if($apkPresent){(Get-FileHash $apk -Algorithm SHA256).Hash.ToLowerInvariant()}else{$null}
 tool_fingerprints=$fingerprints
}
$manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $evidence 'manifest.json') -Encoding utf8
$checks=@(Get-ChildItem -LiteralPath $evidence -File -Recurse |
 Where-Object {$_.Name -ne 'SHA256SUMS'} | Sort-Object FullName |
 ForEach-Object {
  $relative=[IO.Path]::GetRelativePath($evidence,$_.FullName).Replace('\','/')
  '{0}  {1}' -f (Get-FileHash $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant(),$relative
 })
$checks | Set-Content -LiteralPath (Join-Path $evidence 'SHA256SUMS') -Encoding utf8
Write-Output ('Unsigned Android candidate present: {0}; Java checks observed: {1}' -f $apkPresent,$passed)
if(!$apkPresent){Write-Warning 'No Android candidate produced; evidence keeps failure status.'}
