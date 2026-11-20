[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$BaseImage,
 [string]$OutputRoot
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'Windows container smoke requires native Windows'}
if([Environment]::GetEnvironmentVariable('PROCESSOR_ARCHITECTURE') -cne 'AMD64'){
 throw 'Windows x64 container smoke requires an AMD64 Windows host'
}
if($BaseImage -notmatch '@sha256:[0-9a-fA-F]{64}$'){
 throw 'Windows base image must be a reviewed exact SHA256 digest'
}
if($env:DOCKER_HOST -and $env:DOCKER_HOST -notmatch '^npipe://'){
 throw 'Remote Docker daemon forbidden'
}
$ctx=@(& docker context inspect --format '{{.Endpoints.docker.Host}}')
if($LASTEXITCODE -ne 0 -or $ctx.Count -ne 1 -or $ctx[0] -notmatch '^npipe://'){
 throw 'Local npipe Docker context is required'
}
$os=@(& docker info --format '{{.OSType}}')
if($LASTEXITCODE -ne 0 -or $os.Count -ne 1 -or $os[0].Trim() -cne 'windows'){
 throw 'Local Docker daemon must use Windows containers, not Linux'
}
& docker image inspect $BaseImage *> $null
if($LASTEXITCODE -ne 0){throw 'SHA256-pinned Windows image not cached locally; no pull'}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
& git -C $root diff --quiet --exit-code HEAD --
if($LASTEXITCODE -ne 0){throw 'Tracked source is dirty'}
$dirty=@(& git -C $root status --porcelain --untracked-files=all)
if($LASTEXITCODE -ne 0 -or $dirty.Count -ne 0){throw 'Untracked/modified source'}
$shaLines=@(& git -C $root rev-parse --verify HEAD)
if($LASTEXITCODE -ne 0 -or $shaLines.Count -ne 1){throw 'Source commit missing'}
$sha=([string]$shaLines[0]).Trim()
if($sha -notmatch '^[0-9a-fA-F]{40}$'){throw 'Source commit invalid'}
if(-not $OutputRoot){$OutputRoot=Join-Path $root "build/gamp4-release/$sha"}
$out=[IO.Path]::GetFullPath($OutputRoot)
$receipt=Join-Path $out 'evidence/windows-x64/gamp4-gate.yml'
$package=Join-Path $out 'packages/windows-x64'
$checksum=Join-Path $out 'evidence/windows-x64/package-sha256sums.txt'
if(-not(Test-Path -LiteralPath $receipt -PathType Leaf)){throw 'Native GAMP4 receipt missing'}
foreach($expected in @(
 "source_commit: '$sha'",
 "status: 'PASS'",
 "ctest_status: 'PASS'",
 "gamp4_summary: 'PASS|cases=12|failures=0'"
)){
 if(-not(Select-String -LiteralPath $receipt -Pattern $expected -SimpleMatch -Quiet)){
  throw "Native GAMP4 evidence mismatch: $expected"
 }
}
if(-not(Test-Path -LiteralPath $checksum -PathType Leaf)){throw 'Package SHA256 manifest missing'}
$packageFull=[IO.Path]::GetFullPath($package)
foreach($line in Get-Content -LiteralPath $checksum){
 if($line -notmatch '^([a-fA-F0-9]{64})  (.+)$'){throw 'Invalid SHA256SUMS syntax'}
 $expectedHash=$Matches[1].ToLowerInvariant()
 $relative=$Matches[2]
 $target=[IO.Path]::GetFullPath((Join-Path $packageFull $relative))
 if(-not $target.StartsWith(($packageFull.TrimEnd([IO.Path]::DirectorySeparatorChar)+[IO.Path]::DirectorySeparatorChar),[StringComparison]::OrdinalIgnoreCase)){
  throw 'Package checksum path escapes package directory'
 }
 if(-not(Test-Path -LiteralPath $target -PathType Leaf)){throw "Missing artifact: $relative"}
 if((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash.ToLowerInvariant() -cne $expectedHash){
  throw "Artifact checksum changed: $relative"
 }
}
foreach($file in @('pentagonal_xor_std_free.exe','pentagonal_gamp4_validation_test.exe')){
 if(-not(Test-Path -LiteralPath (Join-Path $package "bin/$file") -PathType Leaf)){
  throw "Required Windows GAMP4 executable missing: $file"
 }
}
$output=Join-Path $out 'evidence/windows-x64'
$report=Join-Path $output 'windows-container-smoke.yml'
if(Test-Path -LiteralPath $report){throw 'Previous container evidence exists; choose new run batch'}
$tag='swe-gamp4-windows-smoke:sha-'+$sha.Substring(0,12)
$status='FAIL'
$stage='build'
$imageId='NOT_TESTED'
try {
 & docker build --pull=false --network=none --file (Join-Path $root 'docker/WindowsGamp4Smoke.Dockerfile') --build-arg "WINDOWS_BASE_IMAGE=$BaseImage" --tag $tag $package *> (Join-Path $output 'windows-container-smoke.log')
 if($LASTEXITCODE -ne 0){throw 'Windows container RUN smoke or C++ runtime dependencies failed'}
 $stage='inspect'
 $ids=@(& docker image inspect --format '{{.Id}}' $tag)
 if($LASTEXITCODE -ne 0 -or $ids.Count -ne 1){throw 'Windows container ID missing'}
 $imageId=([string]$ids[0]).Trim()
 $stage='complete'
 $status='PASS'
 Write-Host "IRON|WIN-DOCKER|PASS|$sha|$imageId"
}
finally {
 $rows=@(
 "schema: 'SWE-IRON-WINDOWS-CONTAINER-SMOKE-v1'"
 "source_commit: '$sha'"
 "profile: 'windows-x64'"
 "base_digest: '$BaseImage'"
 "status: '$status'"
 "stage: '$stage'"
 "local_image_id: '$imageId'"
 "registry_publication: 'NOT_PERFORMED'"
 "qa_status: 'NOT_APPROVED'"
 )
 $rows | Set-Content -LiteralPath $report -Encoding utf8
}
