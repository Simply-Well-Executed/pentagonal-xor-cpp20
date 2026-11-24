[CmdletBinding()]
param(
  [switch]$Publish,
  [string]$Gamp4EvidenceJson,
  [string]$CtestLog,
  [string]$ReleaseApprovalJson,
  [string]$AcknowledgeLatestReplacement
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$repository = 'paulcheek/android-source-study-v1'
$sourceTag = $repository + ':sha-07eb2a4561f0'
$targetTag = $repository + ':latest'
$sourceCommit = '07eb2a4561f0f5e82db4668d2b4fa453e2a4d733'
$expectedDigest = 'sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30'
$ackExpected = $targetTag + '@' + $expectedDigest

function Get-Manifest([string]$Ref) {
  $raw = @(& docker buildx imagetools inspect $Ref --format '{{json .Manifest}}' 2>&1)
  if ($LASTEXITCODE -ne 0 -or $raw.Count -eq 0) {
    throw "Cannot inspect Docker Hub manifest for $Ref"
  }
  try { $object = ($raw -join [Environment]::NewLine) | ConvertFrom-Json -ErrorAction Stop }
  catch { throw "Invalid registry manifest JSON for $Ref" }
  if ([string]$object.digest -notmatch '^sha256:[0-9a-fA-F]{64}$') {
    throw "Missing digest from registry manifest for $Ref"
  }
  return $object
}

function Assert-ValidatedRelease {
  foreach ($path in @($Gamp4EvidenceJson,$CtestLog,$ReleaseApprovalJson)) {
    if ([string]::IsNullOrWhiteSpace($path) -or
        -not (Test-Path -LiteralPath $path -PathType Leaf)) {
      throw 'Source-matched GAMP4 evidence, CTest log and independently approved JSON required'
    }
  }
  $gamp = Get-Content -LiteralPath $Gamp4EvidenceJson -Raw | ConvertFrom-Json
  if ([string]$gamp.schema -cne 'SWE-PENTAGONAL-GAMP4-SUPPLIER-VALIDATION-v1' -or
      [string]$gamp.commit -cne $sourceCommit -or
      $gamp.pass -cne $true -or
      [int]$gamp.test_count -ne 12 -or
      [int]$gamp.process_exit -ne 0 -or
      [string]$gamp.oq_automated_status -cne 'PASS' -or
      @($gamp.rejected_ids).Count -ne 0) {
    throw 'GAMP4 supplier report does not satisfy required source and 12-URS scope'
  }
  $ids = @($gamp.observed_pass_ids | Sort-Object -Unique)
  if ($ids.Count -ne 12) { throw 'GAMP4 report missing distinct 12 URS' }
  foreach ($n in 11..22) {
    $id = 'URS-{0:D2}' -f $n
    if ($ids -cnotcontains $id) { throw "GAMP4 requirement missing: $id" }
  }
  $ctest = Get-Content -LiteralPath $CtestLog -Raw
  $match = [regex]::Match($ctest,'100% tests passed,\s*0 tests failed out of (\d+)')
  if (-not $match.Success -or [int]$match.Groups[1].Value -lt 1 -or
      $ctest -match 'The following tests FAILED|Errors while running CTest|No tests were found') {
    throw 'Full CTest log does not report zero failures and at least one passing test'
  }
  $approval = Get-Content -LiteralPath $ReleaseApprovalJson -Raw | ConvertFrom-Json
  if ([string]$approval.schema -cne 'SWE-ANDROID-DOCKERHUB-PROMOTION-APPROVAL-v1' -or
      [string]$approval.source_commit -cne $sourceCommit -or
      [string]$approval.source_tag -cne $sourceTag -or
      [string]$approval.target_tag -cne $targetTag -or
      [string]$approval.manifest_digest -cne $expectedDigest) {
    throw 'Approval is not bound to this exact source commit, manifest and destination'
  }
  foreach ($field in @('operator_approved','qa_release_approved','license_reviewed',
                       'sbom_reviewed','image_origin_reviewed')) {
    if ($approval.$field -cne $true) {
      throw "Independent human review not approved: $field"
    }
  }
  $gampHash = (Get-FileHash -LiteralPath $Gamp4EvidenceJson -Algorithm SHA256).Hash.ToLowerInvariant()
  $ctestHash = (Get-FileHash -LiteralPath $CtestLog -Algorithm SHA256).Hash.ToLowerInvariant()
  if ([string]$approval.gamp4_report_sha256 -cne $gampHash -or
      [string]$approval.ctest_log_sha256 -cne $ctestHash) {
    throw 'Release approval does not match validation evidence SHA256 values'
  }
  Write-Host "IRON|GAMP4|EVIDENCE_ACCEPTED_FOR_REVIEW|$sourceCommit|URS=12"
}

if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
  throw 'Docker CLI not available; nothing pushed'
}
if ($env:DOCKER_HOST -and $env:DOCKER_HOST -notmatch '^(unix|npipe)://') {
  throw 'Remote Docker daemon prohibited'
}
$context = @(& docker context inspect --format '{{.Endpoints.docker.Host}}')
if ($LASTEXITCODE -ne 0 -or $context.Count -ne 1 -or
    [string]$context[0] -notmatch '^(unix|npipe)://') {
  throw 'Local Unix/npipe Docker daemon required'
}
& docker info *> $null
if ($LASTEXITCODE -ne 0) { throw 'Local Docker daemon unavailable' }

$sourceManifest = Get-Manifest $sourceTag
$digest = ([string]$sourceManifest.digest).ToLowerInvariant()
if ($digest -cne $expectedDigest) {
  throw "Source tag digest disagrees with operator-supplied SHA256: $digest"
}
$media = [string]$sourceManifest.mediaType
if ($media -notin @('application/vnd.docker.distribution.manifest.v2+json',
                    'application/vnd.oci.image.manifest.v1+json')) {
  throw "Source is an image index ($media). docker tag/push can change its platform set; separate multi-platform promotion required."
}
Write-Host "IRON|DOCKERHUB|IMMUTABLE_SOURCE_VERIFIED|$sourceTag|$digest"
$latest = Get-Manifest $targetTag
$latestBefore = ([string]$latest.digest).ToLowerInvariant()
if ($latestBefore -ceq $expectedDigest) {
  Write-Host "IRON|DOCKERHUB|ALREADY_TARGET_DIGEST|$targetTag|$digest"
  return
}
if (-not $Publish) {
  Write-Host "IRON|DOCKERHUB|HOLD_DRY_RUN|latest=$latestBefore|no push"
  return
}
if ($AcknowledgeLatestReplacement -cne $ackExpected) {
  throw "Exact tag-replacement acknowledgement required: $ackExpected"
}
if ($env:DOCKERHUB_USERNAME -cne 'paulcheek') {
  throw 'DOCKERHUB_USERNAME must equal paulcheek; authenticate separately with docker login'
}
Assert-ValidatedRelease
$sourceLocal = @(& docker image inspect --format '{{json .RepoDigests}}' $sourceTag)
if ($LASTEXITCODE -ne 0 -or $sourceLocal.Count -ne 1) {
  throw 'No local image with the immutable tag; no automatic Docker pull permitted'
}
try { $localDigests = @($sourceLocal[0] | ConvertFrom-Json) }
catch { throw 'Cannot decode local registry RepoDigests list' }
$want = $repository + '@' + $expectedDigest
$normalized = @($localDigests | ForEach-Object {
  ([string]$_).Replace('index.docker.io/','').Replace('docker.io/','')
})
if ($normalized -cnotcontains $want) {
  throw 'Local source image has no matching RepoDigest; image ID is not manifest digest'
}
$latestRecheck = Get-Manifest $targetTag
if (([string]$latestRecheck.digest).ToLowerInvariant() -cne $latestBefore) {
  throw 'Mutable latest tag changed during preflight; restart approval'
}
& docker tag $sourceTag $targetTag
if ($LASTEXITCODE -ne 0) { throw 'Local Docker tag failed' }
Write-Host "IRON|DOCKERHUB|PUSH_START|$targetTag"
& docker push $targetTag
if ($LASTEXITCODE -ne 0) {
  throw 'Push failed or uncertain; registry must be checked independently'
}
$latestAfter = Get-Manifest $targetTag
$publishedDigest = ([string]$latestAfter.digest).ToLowerInvariant()
if ($publishedDigest -cne $expectedDigest) {
  throw "RELEASE DEVIATION: latest now resolves to $publishedDigest; expected $expectedDigest. Not a verified publication."
}
Write-Host "IRON|DOCKERHUB|PUSH_VERIFIED|$targetTag@$publishedDigest"
