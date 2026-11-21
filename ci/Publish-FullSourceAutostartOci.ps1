# SPDX-License-Identifier: 0BSD
# Docker autostart publisher, called only after full Windows native test job.
# Fail-closed for historical OpenBSD/GNU/POSIX license HOLD. No latest tags.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][ValidateSet('openbsd1','openbsd2','openbsd3')][string]$Profile
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
if($env:GITHUB_ACTIONS -cne 'true' -or
   $env:GITHUB_REPOSITORY -cne 'Simply-Well-Executed/pentagonal-xor-cpp20' -or
   $env:GITHUB_REF -cne 'refs/heads/feat/fresh-artistic-eye-chain-20261120' -or
   $env:RUNNER_OS -cne 'Linux' -or
   $env:RUNNER_ENVIRONMENT -cne 'github-hosted' -or
   $env:GITHUB_SHA -cnotmatch '^[a-f0-9]{40}$'){
 throw 'AUTOSTART|OCI|HOLD|expected_exact_branch_source_runner'
}
if($env:SWE_AUTOSTART_BASES_APPROVED -cne 'true' -or
   $env:SWE_AUTOSTART_ALL_TESTS_ATTESTED -cne 'true' -or
   $env:SWE_AUTOSTART_RELEASE_APPROVED -cne 'true'){
 throw 'AUTOSTART|OCI|HOLD|explicit_separate_release_approvals_missing'
}
foreach($base in @($env:TOOLCHAIN_IMAGE,$env:RUNTIME_IMAGE)){
 if([string]::IsNullOrWhiteSpace($base) -or
    $base -cnotmatch '^[a-z0-9./_-]+@sha256:[a-f0-9]{64}$'){
  throw 'AUTOSTART|OCI|HOLD|base_image_not_digest_pinned'
 }
}
if(-not [IO.File]::Exists((Join-Path $root 'LICENSE')) -or
   -not [IO.File]::Exists((Join-Path $root 'LICENSES/LOCK.tsv'))){
 throw 'AUTOSTART|OCI|HOLD|full_source_or_licenses_missing'
}
# The existing release HOLD is not lifted by a human-friendly tag or a test pass.
$forbidden= @(Get-ChildItem -LiteralPath $root -Recurse -File -Filter '*.sh' |
 Where-Object {
   $relative=[IO.Path]::GetRelativePath($root,$_.FullName).Replace([char]92,'/')
   -not $relative.StartsWith('Archive/Retired/',[StringComparison]::Ordinal)
 })
if($forbidden.Count -ne 0){
 throw ('AUTOSTART|POLICY_HOLD|active_POSIX_shell_sources='+$forbidden.Count)
}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Assert-LocalLicenseRegistry.ps1') -RepositoryRoot $root
if($LASTEXITCODE -ne 0){throw 'AUTOSTART|POLICY_HOLD|license_custody_check_failed'}
# Component/toolchain license inventory and all non-CMake tests must be approved
# by independent human review. The env gate alone never overrides the active POSIX hold.
switch($Profile){
 'openbsd1' {$witness='ghcr.io/simply-well-executed/openbsd1@sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c'}
 'openbsd2' {$witness='ghcr.io/simply-well-executed/openbsd2@sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56'}
 'openbsd3' {$witness='ghcr.io/simply-well-executed/openbsd3@sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1'}
}
$ghcr='ghcr.io/simply-well-executed/'+$Profile+'-autostart:sha-'+$env:GITHUB_SHA
$ghcrAlias='ghcr.io/simply-well-executed/autostart:'+$Profile+'-sha-'+$env:GITHUB_SHA
$hub='paulcheek/autostart:'+$Profile+'-sha-'+$env:GITHUB_SHA
foreach($image in @($env:TOOLCHAIN_IMAGE,$env:RUNTIME_IMAGE)){
 & docker pull $image
 if($LASTEXITCODE -ne 0){throw 'AUTOSTART|OCI|HOLD|approved_base_unavailable'}
}
$dockerfile=Join-Path $root 'docker/FullSource-ReelnativeAutostart.Dockerfile'
& docker build --network=none --pull=false --no-cache --platform linux/amd64 -f $dockerfile `
  --build-arg ('TOOLCHAIN_IMAGE='+$env:TOOLCHAIN_IMAGE) `
  --build-arg ('RUNTIME_IMAGE='+$env:RUNTIME_IMAGE) `
  --build-arg ('SOURCE_SHA='+$env:GITHUB_SHA) `
  --build-arg ('PROFILE='+$Profile) `
  --build-arg ('EVIDENCE_REF='+$witness) -t $ghcr $root
if($LASTEXITCODE -ne 0){throw 'AUTOSTART|OCI|HOLD|docker_build_ctest_failed'}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Verify-FullSourceAutostartImage.ps1') `
 -RepositoryRoot $root -Image $ghcr -WorkingDirectory (Join-Path $env:RUNNER_TEMP ('full-source-inspection-'+$Profile))
if($LASTEXITCODE -ne 0){throw 'AUTOSTART|OCI|HOLD|complete_source_or_default_startup_failed'}
function Push-Exact([string]$image){
 $out=@(& docker push $image 2>&1)
 foreach($line in $out){Write-Host $line}
 if($LASTEXITCODE -ne 0){throw ('AUTOSTART|OCI|HOLD|push_failed|'+$image)}
 $m=[regex]::Match(($out -join [char]10),'digest: (sha256:[a-f0-9]{64})')
 if(-not $m.Success){throw ('AUTOSTART|OCI|HOLD|remote_digest_missing|'+$image)}
 return $m.Groups[1].Value
}
if([string]::IsNullOrWhiteSpace($env:GH_TOKEN)){throw 'AUTOSTART|GHCR|HOLD|token_missing'}
$env:GH_TOKEN | docker login ghcr.io --username $env:GITHUB_ACTOR --password-stdin
if($LASTEXITCODE -ne 0){throw 'AUTOSTART|GHCR|HOLD|login_failed'}
$canonicalDigest=Push-Exact $ghcr
& docker tag $ghcr $ghcrAlias
if($LASTEXITCODE -ne 0){throw 'AUTOSTART|GHCR|HOLD|alias_tag_failed'}
if((Push-Exact $ghcrAlias) -cne $canonicalDigest){throw 'AUTOSTART|GHCR|HOLD|digest_mismatch'}
& docker logout ghcr.io | Out-Null
if($env:DOCKERHUB_USERNAME -cne 'paulcheek' -or
   [string]::IsNullOrWhiteSpace($env:DOCKERHUB_TOKEN)){
 throw 'AUTOSTART|DOCKERHUB|HOLD|credentials_unavailable'
}
& docker tag $ghcr $hub
if($LASTEXITCODE -ne 0){throw 'AUTOSTART|DOCKERHUB|HOLD|tag_failed'}
$env:DOCKERHUB_TOKEN | docker login --username $env:DOCKERHUB_USERNAME --password-stdin
if($LASTEXITCODE -ne 0){throw 'AUTOSTART|DOCKERHUB|HOLD|login_failed'}
if((Push-Exact $hub) -cne $canonicalDigest){throw 'AUTOSTART|DOCKERHUB|HOLD|digest_mismatch'}
& docker logout | Out-Null
Write-Host ('AUTOSTART|PINNED_PUBLICATION_PASS|PROFILE='+$Profile+
 '|DIGEST='+$canonicalDigest+'|SOURCE_SHA='+$env:GITHUB_SHA+
 '|FULL_NATIVE_CTEST_AND_NORMAL_START|ORIGINAL_EVIDENCE_IMAGES_UNTOUCHED|NO_LATEST_TAG')
