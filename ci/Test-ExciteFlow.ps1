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
  if([string]$_.Exception.Message -cmatch '^EXCITE\|HOLD\|(integrity|dovia_signal)\|'){
    $rejected=$true
  }else{throw}
}
if(-not $rejected){throw 'EXCITE|HOLD|corrupted_qledger_was_accepted'}
# Restore the synthetic qlam to isolate the DOVIA signal corruption canary.
$lines[0]=$lines[0].Replace('EXCITE.QLAM.2','EXCITE.QLAM.1')
[IO.File]::WriteAllText($lamPath,([string]::Join([char]10,$lines)+[char]10),
  [Text.UTF8Encoding]::new($false))
if((Assert-ExciteEvidence $ctx) -cne $expected){
  throw 'DOVIA|HOLD|restored_ledger_not_verified'
}
# A train attempting to jump straight from QCHART to QGRAPH meets RED.
$redObserved=$false
try {
  Assert-DoviaRailTransition -From 'QCHART' -To 'QGRAPH' -Ordinal 1
} catch {
  if([string]$_.Exception.Message -cmatch '^EXCITE\|HOLD\|dovia_signal\|'){
    $redObserved=$true
  }else{throw}
}
if(-not $redObserved){throw 'DOVIA|HOLD|illegal_transition_accepted'}
# Corrupt only the synthetic DOVIA signal ledger: the bot must identify
# a SIGNAL_RECEIPT_OR_PROVENANCE_TAMPER, not silently accept a green light.
$signalPath=Join-Path $evidence 'signal.jsonl'
$signalLines=[IO.File]::ReadAllLines($signalPath,[Text.UTF8Encoding]::new($false,$true))
$signalLines[0]=$signalLines[0].Replace('DOVIA.SIGNAL.1','DOVIA.SIGNAL.9')
[IO.File]::WriteAllText($signalPath,
  ([string]::Join([char]10,$signalLines)+[char]10),
  [Text.UTF8Encoding]::new($false))
$signalRejected=$false
try {
  $null=Assert-ExciteEvidence $ctx
} catch {
  if([string]$_.Exception.Message -cmatch
     '^EXCITE\|HOLD\|dovia_signal\|ordinal=1\|stage=QCODE\|rule=SIGNAL_RECEIPT_OR_PROVENANCE_TAMPER'){
    $signalRejected=$true
  }else{throw}
}
if(-not $signalRejected){throw 'DOVIA|HOLD|signal_tamper_accepted'}
Write-Host ('DOVIA|RAIL_AUTOPILOT|SELFTEST=PASS|RED_ILLEGAL_ROUTE=REJECTED'+
  '|RED_SIGNAL_TAMPER=REJECTED|AMBER_INPUT_HOLD=PRESERVED|RELEASE=HOLD')
Write-Host ('EXCITE|SELFTEST=PASS|NEGATIVE_TAMPER=REJECTED|QCODE_SHA256='+$expected+'|RELEASE=HOLD')
