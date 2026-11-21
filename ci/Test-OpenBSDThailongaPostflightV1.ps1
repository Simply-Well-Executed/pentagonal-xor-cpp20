# SPDX-License-Identifier: 0BSD
# Independent THAILONGA v1 HEAD+TAIL, append-only and tamper-rejection checks.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$TemporaryDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$dir=[IO.Path]::GetFullPath($TemporaryDirectory)
if([IO.Directory]::Exists($dir)){throw 'THAILONGA|TEST|HOLD|preexisting_temp_directory'}
[IO.Directory]::CreateDirectory($dir)|Out-Null
$manifest=Join-Path $root 'ci/OPENBSD_SERIAL_COREAO.tsv'
$annex=Join-Path $root 'ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv'
$ledger=Join-Path $dir 'frozen-canonical-ten-stage.jsonl'
$head=Join-Path $dir 'thailonga-head-v1.jsonl'
$tail=Join-Path $dir 'thailonga-tail-v1.jsonl'
$gate=Join-Path $root 'ci/Invoke-OpenBSDThailongaPostflightV1.ps1'
$originalManifest=(Get-FileHash -LiteralPath $manifest -Algorithm SHA256).Hash
$originalAnnex=(Get-FileHash -LiteralPath $annex -Algorithm SHA256).Hash
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Invoke-OpenBSDSerialCoreao.ps1') -RepositoryRoot $root -ReportPath $ledger *> $null
if($LASTEXITCODE -ne 0){throw 'THAILONGA|TEST|HOLD|canonical_gate_failure'}
& pwsh -NoLogo -NoProfile -File $gate -RepositoryRoot $root -LedgerPath $ledger -ReportPath $head
if($LASTEXITCODE -ne 0){throw 'THAILONGA|TEST|HOLD|head_failure'}
& pwsh -NoLogo -NoProfile -File $gate -RepositoryRoot $root -LedgerPath $ledger -ReportPath $tail
if($LASTEXITCODE -ne 0){throw 'THAILONGA|TEST|HOLD|tail_failure'}
$h=[IO.File]::ReadAllText($head)
$t=[IO.File]::ReadAllText($tail)
if(-not [StringComparer]::Ordinal.Equals($h,$t)){throw 'THAILONGA|TEST|HOLD|head_tail_drift'}
$o=$h|ConvertFrom-Json
if($o.protocol -cne 'THAILONGA_POSTFLIGHT_V1' -or
   $o.canonical_core -cne 'NONE' -or $o.projection_core -cne 'OPENBSD2' -or
   $o.projection_mode -cne 'POSTFLIGHT_ONLY' -or
   $o.first_witness_sha256 -ceq $o.return_witness_sha256 -or
   $o.yin_witness_sha256 -ceq $o.yang_witness_sha256 -or
   $o.approved -ne $false -or $o.native_openbsd_binary -cne 'NOT_BUILT' -or
   $o.thailonga_sha256 -cnotmatch '^[a-f0-9]{64}$'){
  throw 'THAILONGA|TEST|HOLD|unsafe_core_or_identity'
}
$bad=Join-Path $dir 'deliberately_tampered_ledger.jsonl'
$raw=[IO.File]::ReadAllText($ledger)
$changed=$raw.Replace('d931ea94e80c62340b177555297501d4fc3d6bd69d2db170fd599662f32a7afc',('0'*64))
if([StringComparer]::Ordinal.Equals($changed,$raw)){throw 'THAILONGA|TEST|HOLD|negative_test_unapplicable'}
[IO.File]::WriteAllText($bad,$changed,[Text.UTF8Encoding]::new($false))
& pwsh -NoLogo -NoProfile -File $gate -RepositoryRoot $root -LedgerPath $bad *> $null
if($LASTEXITCODE -eq 0){throw 'THAILONGA|TEST|HOLD|tampered_yang_accepted'}
$global:LASTEXITCODE=0
if((Get-FileHash -LiteralPath $manifest -Algorithm SHA256).Hash -cne $originalManifest -or
   (Get-FileHash -LiteralPath $annex -Algorithm SHA256).Hash -cne $originalAnnex){
  throw 'THAILONGA|TEST|HOLD|frozen_input_mutated'
}
Write-Host ('THAILONGA|TEST_PASS|HEAD_TAIL_EXACT|TAMPER_REJECTED|STAGE07_FROZEN'+
 '|YIN_YANG_DISTINCT|POSTFLIGHT_ONLY|SHA256='+$o.thailonga_sha256+
 '|CANONICAL_CORE=NONE|PROJECTED_CORE=OPENBSD2|NATIVE_BUILDS=0|RELEASE=HOLD')
