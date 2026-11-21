# SPDX-License-Identifier: 0BSD
# THAILONGA v1: append-only derived projection AFTER the frozen ten-stage sequence.
# Do not insert future stages 08/09 as causal inputs to the earlier stage 07.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$LedgerPath,
  [string]$ReportPath
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$utf8=[Text.UTF8Encoding]::new($false,$true)
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$ledger=[IO.Path]::GetFullPath($LedgerPath)
$manifest=Join-Path $root 'ci/OPENBSD_SERIAL_COREAO.tsv'
$evidence=Join-Path $root 'ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv'
foreach($path in @($manifest,$evidence,$ledger)){
  if(-not [IO.File]::Exists($path)){throw 'THAILONGA|V1|HOLD|input_missing'}
  if(((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0){
    throw 'THAILONGA|V1|HOLD|symlink_or_reparse_point'
  }
}
function Sha256Bytes([byte[]]$bytes){
  return [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
}
$manifestSha=Sha256Bytes ([IO.File]::ReadAllBytes($manifest))
$evidenceSha=Sha256Bytes ([IO.File]::ReadAllBytes($evidence))
# Frozen source/provenance anchors established before introducing THAILONGA v1.
if($manifestSha -cne '2c33d3cb2a4b42df5abf4868c370943feeeff274207f241aa3289e6c9487a308' -or
   $evidenceSha -cne 'd5db2528c8995ff1138ea2d9eae687ce04f5903b0fe56f5ba90940312e88f56a'){
  throw 'THAILONGA|V1|HOLD|frozen_manifest_or_native_annex_changed'
}
$lines=@([IO.File]::ReadAllLines($ledger,$utf8))
if($lines.Count -ne 10){throw 'THAILONGA|V1|HOLD|ledger_row_count'}
$rows=@($lines|ForEach-Object{$_|ConvertFrom-Json})
$expectedStages=@('OPENBSD8','OPENBSD7','OPENBSD6','OPENBSD5','OPENBSD4_FIRST',
                 'OPENBSD3','OPENBSD4_RETURN','OPENBSD2_YIN','OPENBSD2_YANG','COREAO_OPENBSD2')
for($i=0;$i -lt 10;$i++){
  $ordinal=($i+1).ToString('00',[Globalization.CultureInfo]::InvariantCulture)
  if($rows[$i].ordinal -cne $ordinal -or $rows[$i].stage -cne $expectedStages[$i] -or
     $rows[$i].witness_sha256 -cnotmatch '^[a-f0-9]{64}$' -or
     $rows[$i].status -cnotmatch '^HOLD_' -or
     $rows[$i].native_openbsd_binary -cne 'NOT_BUILT'){
    throw ('THAILONGA|V1|HOLD|row_invalid|'+$ordinal)
  }
  if($i -gt 0 -and $rows[$i].previous_sha256 -cne $rows[$i-1].witness_sha256){
    throw ('THAILONGA|V1|HOLD|broken_chain|'+$ordinal)
  }
}
if($rows[4].pass -cne 'FIRST' -or $rows[4].release -cne '4.9' -or
   $rows[5].release -cne '3.9' -or $rows[6].pass -cne 'RETURN' -or
   $rows[6].release -cne '4.9' -or $rows[6].core -cne 'NONE' -or
   $rows[6].witness_sha256 -ceq $rows[4].witness_sha256 -or
   $rows[7].pass -cne 'YIN' -or $rows[8].pass -cne 'YANG' -or
   $rows[7].core -cne 'OPENBSD2' -or $rows[8].core -cne 'OPENBSD2' -or
   $rows[9].coreao_core -cne 'OPENBSD2' -or $rows[9].approved -ne $false -or
   $rows[9].yin_witness_sha256 -cne $rows[7].witness_sha256 -or
   $rows[9].yang_witness_sha256 -cne $rows[8].witness_sha256 -or
   $rows[7].witness_sha256 -ceq $rows[8].witness_sha256){
  throw 'THAILONGA|V1|HOLD|core_or_first_return_yin_yang_drift'
}
if($rows[4].witness_sha256 -cne '1e91defa5aa33b5b613b0ffc8c62da3eee826fb02a2fc2d296d6a01d6c417095' -or
   $rows[5].witness_sha256 -cne 'd220105c5b3fc0ca018e2ace6e0b7b02461852e28557ae4a28251731ca28fb22' -or
   $rows[6].witness_sha256 -cne 'e8690d3c9cfcf1993810e8847aead545b0461467a3d1e4f74047e6b0a1f394fd' -or
   $rows[7].witness_sha256 -cne '5397a209ab64bb9b194b97bb6e15f0c228b5a86f6bc0eb25c7dfcc9fc73bd7f6' -or
   $rows[8].witness_sha256 -cne 'd931ea94e80c62340b177555297501d4fc3d6bd69d2db170fd599662f32a7afc' -or
   $rows[9].witness_sha256 -cne 'efed3f09265eb3f5e506cf310a9905b12befa3b642f0085b3a089fb856ddbcdf'){
  throw 'THAILONGA|V1|HOLD|frozen_stage_witness_drift'
}
$payload=@(
  'THAILONGA','V1','POSTFLIGHT_STAGE07','OPENBSD4_RETURN',
  ('CANONICAL_CORE='+$rows[6].core),('PROJECTED_CORE='+$rows[9].coreao_core),
  ('RETURN='+$rows[6].witness_sha256),
  ('YIN='+$rows[7].witness_sha256),
  ('YANG='+$rows[8].witness_sha256),
  ('COREAO='+$rows[9].witness_sha256),
  ('MANIFEST='+$manifestSha)
) -join '|'
$seal=Sha256Bytes ($utf8.GetBytes($payload))
$item=[ordered]@{
  protocol='THAILONGA_POSTFLIGHT_V1'
  subject_stage='OPENBSD4_RETURN'
  subject_ordinal='07'
  canonical_release='4.9'
  canonical_core='NONE'
  projection_core='OPENBSD2'
  projection_mode='POSTFLIGHT_ONLY'
  first_witness_sha256=$rows[4].witness_sha256
  predecessor_witness_sha256=$rows[5].witness_sha256
  return_witness_sha256=$rows[6].witness_sha256
  yin_witness_sha256=$rows[7].witness_sha256
  yang_witness_sha256=$rows[8].witness_sha256
  coreao_witness_sha256=$rows[9].witness_sha256
  manifest_sha256=$manifestSha
  source_native_annex_sha256=$evidenceSha
  thailonga_sha256=$seal
  native_openbsd_binary='NOT_BUILT'
  release_status='HOLD'
  approved=$false
}
$line=$item|ConvertTo-Json -Compress -Depth 3
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
  $out=[IO.Path]::GetFullPath($ReportPath)
  if([IO.File]::Exists($out)){throw 'THAILONGA|V1|HOLD|report_already_exists_append_only'}
  [IO.File]::WriteAllText($out,$line+[char]10,$utf8)
}
Write-Host ('THAILONGA|POSTFLIGHT_V1|STAGE=07|CANONICAL_CORE=NONE'+
  '|PROJECTED_CORE=OPENBSD2|YIN='+$rows[7].witness_sha256+
  '|YANG='+$rows[8].witness_sha256+'|SHA256='+$seal+
  '|FROZEN_LEDGER_UNCHANGED|NATIVE_BUILDS=0|RELEASE=HOLD')
