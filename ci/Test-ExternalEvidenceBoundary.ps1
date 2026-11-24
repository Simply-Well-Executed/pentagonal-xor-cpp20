# SPDX-License-Identifier: 0BSD
# Regression: source equality, descendants, and external Windows junctions HOLD.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'EVIDENCE|HOLD|Windows_test_required'}
$root=[IO.Path]::TrimEndingDirectorySeparator([IO.Path]::GetFullPath($RepositoryRoot))
$gate=Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1'
if(-not [IO.File]::Exists($gate)){throw 'EVIDENCE|HOLD|guard_missing'}
$parent=Join-Path ([IO.Path]::GetTempPath()) ('swe-evidence-guard-'+[Guid]::NewGuid().ToString('N'))
[void][IO.Directory]::CreateDirectory($parent)
$link=Join-Path $parent 'junction'
function Assert-Rejected([string]$candidate,[string]$expected){
  $output=(& pwsh -NoLogo -NoProfile -NonInteractive -File $gate -RepositoryRoot $root -EvidencePath $candidate 2>&1 | Out-String)
  $result=$LASTEXITCODE
  if($result -eq 0 -or $output -notmatch $expected){
    throw ('EVIDENCE|HOLD|negative_case_accepted|'+$candidate+'|exit='+$result)
  }
}
try {
  $legal=Join-Path $parent 'fresh-evidence'
  & pwsh -NoLogo -NoProfile -NonInteractive -File $gate -RepositoryRoot $root -EvidencePath $legal
  if($LASTEXITCODE -ne 0){throw 'EVIDENCE|HOLD|valid_external_evidence_rejected'}
  Assert-Rejected $root 'output_inside_source'
  Assert-Rejected (Join-Path $root 'src') 'output_inside_source'
  $junction=New-Item -ItemType Junction -Path $link -Target $root -ErrorAction Stop
  if($null -eq $junction -or
     ($junction.Attributes -band [IO.FileAttributes]::ReparsePoint) -eq 0){
    throw 'EVIDENCE|HOLD|junction_test_setup_failed'
  }
  Assert-Rejected (Join-Path $link 'escape') 'reparse_point_ancestor'
} finally {
  # Delete only the junction itself, NEVER the repository target.
  if([IO.Directory]::Exists($link)){[IO.Directory]::Delete($link)}
  if([IO.Directory]::Exists($parent)){[IO.Directory]::Delete($parent)}
}
Write-Host 'EVIDENCE|REGRESSION_PASS|VALID_EXTERNAL_AND_3_NEGATIVE_CASES|RELEASE=HOLD'
