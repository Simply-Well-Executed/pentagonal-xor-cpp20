# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Native PowerShell 7 evidence manifest: records failures without JS actions.
[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$EvidenceDir,
 [Parameter(Mandatory)][string]$Label
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$path=[IO.Path]::GetFullPath((Join-Path $root $EvidenceDir))
[void][IO.Directory]::CreateDirectory($path)
$manifestPath=Join-Path $path 'guard-record.json'
$head=(& git rev-parse HEAD 2>$null | Select-Object -First 1)
$files=@(Get-ChildItem -LiteralPath $path -File -Recurse -ErrorAction SilentlyContinue |
 Where-Object {$_.Name -notin @('SHA256SUMS','guard-record.json')} |
 Sort-Object FullName)
$records=@(foreach($file in $files) {
 [ordered]@{
  file=[IO.Path]::GetRelativePath($path,$file.FullName).Replace('\','/')
  size=$file.Length
  sha256=(Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
 }
})
[ordered]@{
 schema='SWE-GAMP4-Style-Guard-Record-v2'
 label=$Label
 github_sha=$env:GITHUB_SHA
 actual_checkout_sha=$head
 workflow=$env:GITHUB_WORKFLOW
 run_id=$env:GITHUB_RUN_ID
 run_attempt=$env:GITHUB_RUN_ATTEMPT
 os=$env:RUNNER_OS
 machine=$env:RUNNER_ARCH
 status='review job conclusion and test output'
 files=$records
} | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $manifestPath -Encoding utf8
$toHash=@(Get-ChildItem -LiteralPath $path -File -Recurse |
 Where-Object {$_.Name -ne 'SHA256SUMS'} |
 Sort-Object FullName)
$checks=@(foreach($file in $toHash){
 $rel=[IO.Path]::GetRelativePath($path,$file.FullName).Replace('\','/')
 '{0}  {1}' -f (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant(),$rel
})
$checks | Set-Content -LiteralPath (Join-Path $path 'SHA256SUMS') -Encoding utf8
Write-Output "Validation record $Label, source $head, $($checks.Count) hashes:"
$checks | ForEach-Object { Write-Output $_ }
if (![string]::IsNullOrWhiteSpace($env:GITHUB_STEP_SUMMARY)) {
 $lines=@(
  "### $Label", "",
  "Source checkout: $head", "",
  "Run: $env:GITHUB_RUN_ID", "",
  "Evidence files: $($checks.Count); SHA-256 logged and summarized.", "",
  "| SHA-256 | Evidence path |",
  "| --- | --- |"
 )
 foreach($f in $toHash) {
  $sha=(Get-FileHash $f.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
  $relative=[IO.Path]::GetRelativePath($path,$f.FullName).Replace('\','/')
  $lines += "| $sha | $relative |"
 }
 Add-Content -LiteralPath $env:GITHUB_STEP_SUMMARY -Value ($lines -join [Environment]::NewLine) -Encoding utf8
}
# Runner logs and job summary are retained without a JS upload action.
# Per-job binary files remain on the temporary runner, not a persistent archive.
