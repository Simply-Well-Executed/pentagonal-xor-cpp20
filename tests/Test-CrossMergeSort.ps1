#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
# Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
# CROSS-MERGE-SORT offline disposable repository tests: never touches PR refs.
$ErrorActionPreference='Stop'
$script=Join-Path (Split-Path -Parent $PSScriptRoot) 'tools/Invoke-CrossMergeSort.ps1'
$root=Join-Path ([IO.Path]::GetTempPath()) ('cross-merge-sort-'+[guid]::NewGuid().ToString('N'))
$repo=Join-Path $root 'repo'
[void][IO.Directory]::CreateDirectory($repo)
function Run-Git([string[]]$Args) {
  & git -C $repo @Args
  if($LASTEXITCODE -ne 0){throw ('TEST_CROSS_MERGE_SORT|git_failed|'+($Args -join ' '))}
}
function Write-Source([string]$Name,[string]$Text) {
  [IO.File]::WriteAllText((Join-Path $repo $Name),$Text,[Text.UTF8Encoding]::new($false))
}
function Require([bool]$Condition,[string]$Reason) {
  if(-not $Condition){throw ('TEST_CROSS_MERGE_SORT|'+$Reason)}
}
try {
  Run-Git @('init','-q')
  Run-Git @('config','user.name','CROSS-MERGE-SORT fixture')
  Run-Git @('config','user.email','fixture@example.invalid')
  Write-Source 'common.txt' "base`n"
  Run-Git @('add','common.txt')
  Run-Git @('commit','-qm','base')
  $base=(& git -C $repo rev-parse HEAD).Trim()
  Run-Git @('switch','-q','-c','pr95')
  Write-Source '95.txt' "QUADDI source`n"
  Write-Source 'identical.txt' "shared independent content`n"
  Run-Git @('add','.')
  Run-Git @('commit','-qm','pr95')
  $left=(& git -C $repo rev-parse HEAD).Trim()
  Run-Git @('switch','-q','-c','pr316',$base)
  Write-Source '316.txt' "APPLICATION source`n"
  Write-Source 'identical.txt' "shared independent content`n"
  Run-Git @('add','.')
  Run-Git @('commit','-qm','pr316')
  $right=(& git -C $repo rev-parse HEAD).Trim()
  $evidence=Join-Path $root 'clean-evidence'
  & pwsh -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File $script -RepositoryRoot $repo -Pr95Commit $left -Pr316Commit $right -EvidenceDirectory $evidence
  Require ($LASTEXITCODE -eq 0) 'clean_merge_rejected'
  $r=Get-Content -LiteralPath (Join-Path $evidence 'cross-merge-sort.json') -Raw -Encoding utf8 | ConvertFrom-Json
  Require ($r.result -ceq 'CLEAN_PREVIEW' -and $r.total_paths -eq 3 -and
           $r.overlap_identical -eq 1 -and $r.overlap_different -eq 0) 'bad_clean_ledger'
  $pathList=@($r.rows | ForEach-Object path)
  Require (($pathList -join ',') -ceq '316.txt,95.txt,identical.txt') 'not_ordinal_sorted'
  Require ($r.source_mutated -eq $false -and $r.refs_updated -eq $false -and
           $r.common_ancestor -ceq $base -and
           $r.pr95_head -ceq $left -and $r.pr316_head -ceq $right) 'provenance_violation'
  # The tested preview must not move either branch tip.
  Require ((& git -C $repo rev-parse pr95).Trim() -ceq $left -and
           (& git -C $repo rev-parse pr316).Trim() -ceq $right) 'ref_mutation'
  # A second, independent repository demonstrates fail-closed content conflicts.
  $conflictRepo=Join-Path $root 'conflict'
  [void][IO.Directory]::CreateDirectory($conflictRepo)
  Run-Git @('init','-q',$conflictRepo)
  Run-Git @('-C',$conflictRepo,'config','user.name','fixture')
  Run-Git @('-C',$conflictRepo,'config','user.email','fixture@example.invalid')
  [IO.File]::WriteAllText((Join-Path $conflictRepo 'same.txt'),"base`n")
  Run-Git @('-C',$conflictRepo,'add','same.txt')
  Run-Git @('-C',$conflictRepo,'commit','-qm','base')
  $common2=(& git -C $conflictRepo rev-parse HEAD).Trim()
  Run-Git @('-C',$conflictRepo,'switch','-q','-c','one')
  [IO.File]::WriteAllText((Join-Path $conflictRepo 'same.txt'),"one`n")
  Run-Git @('-C',$conflictRepo,'commit','-qam','one')
  $one=(& git -C $conflictRepo rev-parse HEAD).Trim()
  Run-Git @('-C',$conflictRepo,'switch','-q','-c','two',$common2)
  [IO.File]::WriteAllText((Join-Path $conflictRepo 'same.txt'),"two`n")
  Run-Git @('-C',$conflictRepo,'commit','-qam','two')
  $two=(& git -C $conflictRepo rev-parse HEAD).Trim()
  $conflictEvidence=Join-Path $root 'conflict-evidence'
  & pwsh -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File $script -RepositoryRoot $conflictRepo -Pr95Commit $one -Pr316Commit $two -EvidenceDirectory $conflictEvidence
  Require ($LASTEXITCODE -eq 2) 'conflict_must_hold'
  $conf=Get-Content -LiteralPath (Join-Path $conflictEvidence 'cross-merge-sort.json') -Raw -Encoding utf8 | ConvertFrom-Json
  Require ($conf.result -ceq 'CONFLICT_HOLD' -and $conf.overlap_different -eq 1 -and
           $conf.refs_updated -eq $false -and $conf.rows[0].path -ceq 'same.txt') 'conflict_ledger_bad'
  Write-Host 'CROSS_MERGE_SORT|CLEAN=PASS|CONFLICT=HOLD_AS_REQUIRED|SORT=PASS|NO_REF_MUTATION=PASS'
} finally {
  # Retain disposable evidence for diagnosis; never touch source repository.
  Write-Host ('CROSS_MERGE_SORT|FIXTURE_ROOT='+$root)
}
