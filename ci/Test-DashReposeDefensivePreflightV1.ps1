# SPDX-License-Identifier: 0BSD
# DASH-REPOSE V1: bounded defensive validation, before MagicComb and airgap.
# GitHub source retrieval happens before this guard; no physical offline claim.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$ExactSourceArchive
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$archive=[IO.Path]::GetFullPath($ExactSourceArchive)
if($env:GITHUB_ACTIONS -cne 'true' -or
   $env:GITHUB_REPOSITORY -cne 'Simply-Well-Executed/pentagonal-xor-cpp20' -or
   $env:GITHUB_SHA -cnotmatch '^[a-f0-9]{40}$' -or
   $env:RUNNER_OS -cne 'Linux' -or
   $env:RUNNER_ENVIRONMENT -cne 'github-hosted'){
 throw 'DASH-REPOSE|DEFENSE_HOLD|runner_or_source_untrusted'
}
if(-not [IO.Directory]::Exists($root) -or -not [IO.File]::Exists($archive)){
 throw 'DASH-REPOSE|DEFENSE_HOLD|missing_source_archive'
}
$required=@(
 'LICENSE','LICENSES/LOCK.tsv','CMakeLists.txt',
 'ci/OPENBSD_SERIAL_COREAO.tsv','ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv',
 'ci/Test-OpenBSDSerialCoreao.ps1','ci/Test-OpenBSDNativeEvidence.ps1',
 'ci/Invoke-OpenBSDThailongaPostflightV1.ps1',
 'ci/Invoke-CoreaoAirgapMagicCombV1.ps1',
 'ci/Test-PeekGateStaticV1.ps1',
 'packaging/coreao-airgap-v1/Dockerfile',
 'packaging/coreao-airgap-v1/tick_linux_amd64.cpp'
)
foreach($rel in $required){
 $file=Join-Path $root $rel
 if(-not [IO.File]::Exists($file)){throw ('DASH-REPOSE|DEFENSE_HOLD|missing_file|'+$rel)}
 if(([IO.FileInfo]::new($file).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0){
  throw ('DASH-REPOSE|DEFENSE_HOLD|symlink_file|'+$rel)
 }
}
$manifest=(Get-FileHash -LiteralPath (Join-Path $root 'ci/OPENBSD_SERIAL_COREAO.tsv') -Algorithm SHA256).Hash.ToLowerInvariant()
$annex=(Get-FileHash -LiteralPath (Join-Path $root 'ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv') -Algorithm SHA256).Hash.ToLowerInvariant()
if($manifest -cne '2c33d3cb2a4b42df5abf4868c370943feeeff274207f241aa3289e6c9487a308' -or
   $annex -cnotmatch '^[a-f0-9]{64}$'){
 throw 'DASH-REPOSE|DEFENSE_HOLD|canonical_source_hash_mismatch'
}
$df=[IO.File]::ReadAllText((Join-Path $root 'packaging/coreao-airgap-v1/Dockerfile'))
if(-not $df.Contains('FROM scratch AS trial',[StringComparison]::Ordinal) -or
   -not $df.Contains('USER 65534:65534',[StringComparison]::Ordinal) -or
   -not $df.Contains('source.zip /custody/exact-github-source.zip',[StringComparison]::Ordinal)){
 throw 'DASH-REPOSE|DEFENSE_HOLD|unexpected_container_contract'
}
Add-Type -AssemblyName System.IO.Compression
$zip=[IO.Compression.ZipFile]::OpenRead($archive)
try{
 $entries=@($zip.Entries)
 if($entries.Count -lt 340 -or $entries.Count -gt 12000){
  throw ('DASH-REPOSE|DEFENSE_HOLD|zip_entry_count|'+$entries.Count)
 }
 [long]$expanded=0
 $names=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
 foreach($entry in $entries){
  $name=$entry.FullName
  if($name.StartsWith('/',[StringComparison]::Ordinal) -or
     $name.Contains('\',[StringComparison]::Ordinal) -or
     $name.Contains('//',[StringComparison]::Ordinal) -or
     @($name.Split('/')) -contains '..'){
   throw 'DASH-REPOSE|DEFENSE_HOLD|unsafe_zip_entry'
  }
  if(-not $names.Add($name)){throw 'DASH-REPOSE|DEFENSE_HOLD|duplicate_zip_entry'}
  $expanded+=$entry.Length
  if($entry.Length -gt 67108864 -or $expanded -gt 268435456){
   throw 'DASH-REPOSE|DEFENSE_HOLD|zip_size_limit'
  }
 }
}finally{$zip.Dispose()}
$archiveHash=(Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('DASH-REPOSE|DEFENSE_PASS|GIT_SHA='+$env:GITHUB_SHA+
 '|SOURCE_ZIP_SHA256='+$archiveHash+'|MANIFEST_SHA256='+$manifest+
 '|NATIVE_ANNEX_SHA256='+$annex+'|BOUNDED_ARCHIVE|RELEASE=HOLD')
