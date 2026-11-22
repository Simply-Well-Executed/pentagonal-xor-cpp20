# SPDX-License-Identifier: 0BSD
# Native PowerShell independent serial witness HEAD/TAIL + negative tests.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot,
      [Parameter(Mandatory=$true)][string]$TemporaryDirectory)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$temp=[IO.Path]::GetFullPath($TemporaryDirectory)
[IO.Directory]::CreateDirectory($temp)|Out-Null
$gate=Join-Path $root 'ci/Invoke-OpenBSDSerialCoreao.ps1'
$manifest=Join-Path $root 'ci/OPENBSD_SERIAL_COREAO.tsv'
$head=Join-Path $temp 'openbsd-head.jsonl'
$tail=Join-Path $temp 'openbsd-tail.jsonl'
& pwsh -NoLogo -NoProfile -File $gate -RepositoryRoot $root -ReportPath $head
if($LASTEXITCODE -ne 0){throw 'OPENBSD|SERIAL|TEST|HOLD|HEAD'}
& pwsh -NoLogo -NoProfile -File $gate -RepositoryRoot $root -ReportPath $tail
if($LASTEXITCODE -ne 0){throw 'OPENBSD|SERIAL|TEST|HOLD|TAIL'}
if(-not [StringComparer]::Ordinal.Equals([IO.File]::ReadAllText($head),[IO.File]::ReadAllText($tail))){
  throw 'OPENBSD|SERIAL|TEST|HOLD|HEAD_TAIL_CHANGED'
}
$rows=@([IO.File]::ReadAllLines($head)|ForEach-Object {$_|ConvertFrom-Json})
if($rows.Count -ne 10 -or $rows[4].stage -cne 'OPENBSD4_FIRST' -or
   $rows[6].stage -cne 'OPENBSD4_RETURN' -or $rows[7].stage -cne 'OPENBSD2_YIN' -or
   $rows[8].stage -cne 'OPENBSD2_YANG' -or $rows[9].stage -cne 'COREAO_OPENBSD2' -or
   $rows[7].core -cne 'OPENBSD2' -or $rows[8].core -cne 'OPENBSD2' -or
   $rows[9].coreao_core -cne 'OPENBSD2' -or
   $rows[9].yin_witness_sha256 -ceq $rows[9].yang_witness_sha256){
  throw 'OPENBSD|SERIAL|TEST|HOLD|ORDER_OR_CORE_ERASED'
}
foreach($r in $rows){
  if(-not $r.status.StartsWith('HOLD_',[StringComparison]::Ordinal) -or
     $r.native_openbsd_binary -cne 'NOT_BUILT' -or
     $r.precision_policy -cne 'PRESERVE_SOURCE_PRECISION'){
    throw 'OPENBSD|SERIAL|TEST|HOLD|FICTITIOUS_BUILD_OR_PRECISION'
  }
}
$corrupt=Join-Path $temp 'corrupt.tsv'
$original=[IO.File]::ReadAllText($manifest)
$bad=$original.Replace('OPENBSD4_RETURN','OPENBSD4_FIRST')
if([StringComparer]::Ordinal.Equals($bad,$original)){throw 'OPENBSD|SERIAL|TEST|HOLD|BAD_NEGATIVE_TEST'}
[IO.File]::WriteAllText($corrupt,$bad)
& pwsh -NoLogo -NoProfile -File $gate -RepositoryRoot $root -ManifestPath $corrupt *> $null
if($LASTEXITCODE -eq 0){throw 'OPENBSD|SERIAL|TEST|HOLD|TAMPER_ACCEPTED'}
Write-Host 'OPENBSD|SERIAL|TEST_PASS|HEAD_TAIL_EXACT|TEN_ORDERED_STAGES|CORE_OPENBSD2_PRESERVED|TAMPER_REJECTED|NO_NETWORK'
