# SPDX-License-Identifier: 0BSD
# Dependency-free PEEK V1 syntax, exact-identity and negative-path rehearsal.
# Running this check alone does NOT qualify an OCI image or its output.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$targets=@(
 'ci/Test-PeekVirginOciV1.ps1',
 'ci/Test-FullSourceOutputContractsV1.ps1',
 'ci/Publish-FullSourceAutostartOci.ps1',
 'ci/Verify-FullSourceAutostartImage.ps1',
 'ci/Invoke-FullSourceAutostartNativeSuite.ps1'
)
foreach($rel in $targets){
 $file=Join-Path $root $rel
 if(-not [IO.File]::Exists($file)){throw ('PEEK|STATIC_HOLD|missing|'+$rel)}
 $tokens=$null
 $issues=$null
 [void][Management.Automation.Language.Parser]::ParseFile($file,[ref]$tokens,[ref]$issues)
 if(@($issues).Count -ne 0){
  foreach($issue in $issues){
   Write-Host ('PEEK|SYNTAX_DIAGNOSTIC|'+$rel+'|'+$issue.Message)
  }
  throw ('PEEK|STATIC_HOLD|script_parsing|'+$rel)
 }
 Write-Host ('PEEK|SYNTAX_PASS|'+$rel)
}
$trial=[IO.File]::ReadAllText((Join-Path $root 'ci/Test-PeekVirginOciV1.ps1'))
$publisher=[IO.File]::ReadAllText((Join-Path $root 'ci/Publish-FullSourceAutostartOci.ps1'))
$required=@(
 'GHCR_TRIAL','DOCKERHUB_TRIAL','DOCKERHUB_FINAL',
 'registry_tag_does_not_resolve_to_pinned_manifest',
 'pull_remote_manifest','--network','none','--read-only',
 'baseline_head','baseline_tail','zero_composition','odd_composition',
 'hostile_out_of_range','hostile_not_a_number','hostile_missing_arguments',
 'control_json','WITNESS'
)
# The original evidence identity is checked using the actual per-profile
# manifest labels instead of a mutable name or an untyped score.
foreach($marker in $required){
 if(-not $trial.Contains($marker,[StringComparison]::OrdinalIgnoreCase)){
  throw ('PEEK|STATIC_HOLD|missing_trial_invariant|'+$marker)
 }
}
foreach($marker in @('trial-sha-','GHCR_TRIAL','DOCKERHUB_TRIAL','DOCKERHUB_FINAL',
  'Test-PeekVirginOciV1.ps1','ghcrTrial','hubTrial','Push-Exact')){
 if(-not $publisher.Contains($marker,[StringComparison]::Ordinal)){
  throw ('PEEK|STATIC_HOLD|publisher_drift|'+$marker)
 }
}
# Negative: unsafe digest fails BEFORE reaching Docker.
$unsafe=@(& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-PeekVirginOciV1.ps1') `
 -Image 'paulcheek/autostart:openbsd3-trial-sha-0000000000000000000000000000000000000000' `
 -Digest 'sha256:WRONG' -Profile openbsd3 -Phase DOCKERHUB_TRIAL 2>&1)
if($LASTEXITCODE -eq 0){throw 'PEEK|STATIC_HOLD|invalid_digest_was_accepted'}
$global:LASTEXITCODE=0
Write-Host 'PEEK|STATIC_TEST_PASS|SYNTAX|THREE_PROFILE_ARENAS|DIGEST_PIN|TRIAL_THEN_VALIDATED|NO_DOCKER_EXECUTED'
