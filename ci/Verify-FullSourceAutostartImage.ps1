# SPDX-License-Identifier: 0BSD
# Exact full-source Docker copy comparison and normal default-entrypoint smoke.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$Image,
 [Parameter(Mandatory=$true)][string]$WorkingDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$working=[IO.Path]::GetFullPath($WorkingDirectory)
if([IO.Directory]::Exists($working)){throw 'AUTOSTART|VALIDATION|HOLD|working_directory_not_clean'}
[IO.Directory]::CreateDirectory($working)|Out-Null
if($Image -cnotmatch '^(?:ghcr[.]io/simply-well-executed/|paulcheek/)[a-z0-9-]+:sha-[0-9a-f]{40}$'){
 throw 'AUTOSTART|VALIDATION|HOLD|invalid_immutable_source_tag'
}
$run=@(& docker run --rm --network none --read-only --cap-drop ALL --security-opt no-new-privileges --pids-limit 16 --platform linux/amd64 $Image)
if($LASTEXITCODE -ne 0 -or $run.Count -ne 1 -or
   $run[0] -cne 'Pentagonal XOR: 51.72800000%'){
  throw ('AUTOSTART|VALIDATION|HOLD|normal_start_failed|'+($run -join ';'))
}
$container=(@(& docker create --platform linux/amd64 $Image))[0]
if($LASTEXITCODE -ne 0 -or $container -cnotmatch '^[0-9a-f]{64}$'){
 throw 'AUTOSTART|VALIDATION|HOLD|unable_to_create_validation_container'
}
try{
 $dst=Join-Path $working 'source'
 [IO.Directory]::CreateDirectory($dst)|Out-Null
 & docker cp ($container+':/opt/swe/source/.') $dst
 if($LASTEXITCODE -ne 0){throw 'AUTOSTART|VALIDATION|HOLD|full_source_copy_missing'}
 $expected=@(Get-ChildItem -LiteralPath $root -File -Recurse -Force)
 $actual=@(Get-ChildItem -LiteralPath $dst -File -Recurse -Force)
 $expectedPaths=@{}
 foreach($item in $expected){
  $relative=[IO.Path]::GetRelativePath($root,$item.FullName).Replace('\\','/')
  # Only explicitly private material is excluded by .dockerignore. If an
  # ordinary tracked file is absent, fail closed instead of calling it complete.
  if($relative -match '^(?:\.git/|build(?:-[^/]*)?/|dist/|stage/|evidence(?:-[^/]*)?/|android/app/build/)' -or
     $relative -match '(?:^|/)[^/]*(?:secret|token)[^/]*$' -or
     $relative -match '\.(?:p12|jks|keystore)$'){continue}
  $expectedPaths[$relative]=(Get-FileHash -LiteralPath $item.FullName -Algorithm SHA256).Hash
 }
 $actualPaths=@{}
 foreach($item in $actual){
  $relative=[IO.Path]::GetRelativePath($dst,$item.FullName).Replace('\\','/')
  $actualPaths[$relative]=(Get-FileHash -LiteralPath $item.FullName -Algorithm SHA256).Hash
 }
 if($actualPaths.Count -ne $expectedPaths.Count){
  throw ('AUTOSTART|VALIDATION|HOLD|source_file_count_mismatch|EXPECTED='+$expectedPaths.Count+'|ACTUAL='+$actualPaths.Count)
 }
 foreach($key in $expectedPaths.Keys){
  if(-not $actualPaths.ContainsKey($key) -or $actualPaths[$key] -cne $expectedPaths[$key]){
   throw ('AUTOSTART|VALIDATION|HOLD|file_missing_or_changed|'+$key)
  }
 }
 if(-not $actualPaths.ContainsKey('LICENSES/LOCK.tsv') -or
    -not $actualPaths.ContainsKey('.github/workflows/openbsd-serial-coreao.yml') -or
    -not $actualPaths.ContainsKey('ci/REELNATIVE_REGISTRY_PUBLISH_RECEIPTS_V1.tsv')){
  throw 'AUTOSTART|VALIDATION|HOLD|provenance_file_missing'
 }
 Write-Host ('AUTOSTART|VALIDATION_PASS|FULL_SOURCE_SHA256_EXACT|FILES='+$actualPaths.Count+
             '|LICENSES_PRESENT|NORMAL_PROGRAM_START_PASS|IMAGE='+$Image)
}
finally{
 & docker rm -f $container *> $null
}
