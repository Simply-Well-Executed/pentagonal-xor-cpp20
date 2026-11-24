# SPDX-License-Identifier: 0BSD
# EXCITE deterministic head/tail verification with a negative corruption witness.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'EXCITE|HOLD|native_windows_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $evidence
if($LASTEXITCODE -ne 0 -or [IO.Directory]::Exists($evidence)){
  throw 'EXCITE|HOLD|test_evidence_path'
}
. (Join-Path $root 'ci/Excite-Coreao.ps1')
$ctx=Initialize-ExciteEvidence $evidence
$first=Add-ExciteVehicle -Context $ctx -Kind FILE -Label 'A/fixture.cpp' `
  -SourceSha (Get-ExciteDigest 'immutable-synthetic-A') -Status PASS `
  -ElapsedMs 0 -Bytes 9 -Detail 'TEST_ONLY'
$second=Add-ExciteVehicle -Context $ctx -Kind SUBPROJECT -Label 'synthetic_failure' `
  -SourceSha (Get-ExciteDigest 'immutable-synthetic-B') -Status HOLD `
  -ElapsedMs 2 -Bytes 17 -Detail 'EXPECTED_NEGATIVE_WITNESS'
$expected=Assert-ExciteEvidence $ctx
if($expected -cne $second -or $ctx.Sequence -ne 2 -or
   $ctx.FileCount -ne 1 -or $ctx.SubprojectCount -ne 1){
  throw 'EXCITE|HOLD|selftest_incorrect_ledger'
}
# Never modify actual source, only the synthetic test stream outside the checkout.
$lamPath=Join-Path $evidence 'qlam.jsonl'
$lines=[IO.File]::ReadAllLines($lamPath,[Text.UTF8Encoding]::new($false,$true))
$lines[0]=$lines[0].Replace('EXCITE.QLAM.1','EXCITE.QLAM.2')
[IO.File]::WriteAllText($lamPath,([string]::Join([char]10,$lines)+[char]10),
  [Text.UTF8Encoding]::new($false))
$rejected=$false
try{
  $null=Assert-ExciteEvidence $ctx
}catch{
  if([string]$_.Exception.Message -cmatch '^EXCITE\|HOLD\|integrity\|'){
    $rejected=$true
  }else{throw}
}
if(-not $rejected){throw 'EXCITE|HOLD|corrupted_qledger_was_accepted'}
Write-Host ('EXCITE|SELFTEST=PASS|NEGATIVE_TAMPER=REJECTED|QCODE_SHA256='+$expected+'|RELEASE=HOLD')
