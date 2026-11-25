#requires -Version 7.0
# SPDX-License-Identifier: 0BSD
# Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
# CROSS-MERGE-SORT | THAILONGA = visible, reviewable provenance.
# Two independent Git commit heads in; sorted path and merge-tree evidence out.
# No network, no checkout, no merge commit, no ref update, no silent resolution.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$Pr95Commit,
  [Parameter(Mandatory=$true)][string]$Pr316Commit,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
function Require([bool]$Okay,[string]$Reason) {
  if(-not $Okay){throw ('CROSS_MERGE_SORT|HOLD|'+$Reason)}
}
function Git([string[]]$Arguments) {
  $processInfo=[Diagnostics.ProcessStartInfo]::new()
  $processInfo.FileName='git'
  $processInfo.UseShellExecute=$false
  $processInfo.RedirectStandardOutput=$true
  $processInfo.RedirectStandardError=$true
  $processInfo.CreateNoWindow=$true
  $processInfo.ArgumentList.Add('-C')
  $processInfo.ArgumentList.Add($root)
  foreach($argument in $Arguments){$processInfo.ArgumentList.Add($argument)}
  $process=[Diagnostics.Process]::Start($processInfo)
  Require ($null -ne $process) 'GIT_NOT_STARTED'
  try {
    $stdout=$process.StandardOutput.ReadToEnd()
    $stderr=$process.StandardError.ReadToEnd()
    $process.WaitForExit()
    return [pscustomobject]@{
      Exit=$process.ExitCode
      Out=$stdout
      Err=$stderr
    }
  } finally {$process.Dispose()}
}
function One([string[]]$Arguments,[string]$Reason) {
  $r=Git $Arguments
  Require ($r.Exit -eq 0) $Reason
  return $r.Out.Trim()
}
function Blob([string]$Commit,[string]$Path) {
  # A missing file is a first-class distinction, not an empty source.
  $r=Git @('rev-parse','--verify',('--end-of-options'),($Commit+':'+$Path))
  if($r.Exit -ne 0){return 'ABSENT'}
  $oid=$r.Out.Trim()
  Require ($oid -cmatch '^[0-9a-f]{40,64}$') 'INVALID_OBJECT_ID'
  return $oid
}
function Paths([string]$Common,[string]$Head) {
  # -z ensures Unicode, spaces, tabs and newlines cannot confuse the parser.
  $r=Git @('diff','--no-ext-diff','--name-only','-z',$Common,$Head,'--')
  Require ($r.Exit -eq 0) 'DIFF_FAILED'
  if($r.Out.Length -eq 0){return @()}
  Require ($r.Out.EndsWith([string][char]0)) 'UNTERMINATED_GIT_PATH'
  return @($r.Out.Split([char]0,[StringSplitOptions]::RemoveEmptyEntries))
}
function Escape-Field([string]$Value) {
  # Human TSV escapes only. JSON retains the original path exactly.
  return $Value.Replace('\\','\\\\').Replace("`t",'\\t').Replace("`r",'\\r').Replace("`n",'\\n')
}
$root=[IO.Path]::GetFullPath($RepositoryRoot).TrimEnd([IO.Path]::DirectorySeparatorChar)
$out=[IO.Path]::GetFullPath($EvidenceDirectory).TrimEnd([IO.Path]::DirectorySeparatorChar)
$cmp=[StringComparison]::OrdinalIgnoreCase
Require ($out -cne $root -and -not $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,$cmp)) 'EVIDENCE_INSIDE_REPOSITORY'
Require (-not [IO.Directory]::Exists($out) -and -not [IO.File]::Exists($out)) 'EVIDENCE_MUST_BE_FRESH'
Require ($Pr95Commit -cmatch '^[0-9a-f]{40}$' -and $Pr316Commit -cmatch '^[0-9a-f]{40}$') 'UNPINNED_HEAD'
Require ($Pr95Commit -cne $Pr316Commit) 'HEADS_IDENTICAL'
$inside=One @('rev-parse','--is-inside-work-tree') 'NOT_A_GIT_REPOSITORY'
Require ($inside -ceq 'true') 'NOT_A_GIT_WORKTREE'
$h95=One @('rev-parse','--verify',($Pr95Commit+'^{commit}')) 'PR95_COMMIT_MISSING'
$h316=One @('rev-parse','--verify',($Pr316Commit+'^{commit}')) 'PR316_COMMIT_MISSING'
Require ($h95 -ceq $Pr95Commit -and $h316 -ceq $Pr316Commit) 'COMMIT_PIN_DRIFT'
$common=Git @('merge-base','--all',$h95,$h316)
Require ($common.Exit -eq 0) 'NO_MERGE_BASE'
$ancestors=@($common.Out.Trim().Split("`n",[StringSplitOptions]::RemoveEmptyEntries))
Require ($ancestors.Count -eq 1 -and $ancestors[0].Trim() -cmatch '^[0-9a-f]{40,64}$') 'MULTIPLE_OR_INVALID_MERGE_BASE'
$base=$ancestors[0].Trim()
$left=@(Paths $base $h95)
$right=@(Paths $base $h316)
$leftSet=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$rightSet=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach($p in $left){[void]$leftSet.Add($p)}
foreach($p in $right){[void]$rightSet.Add($p)}
$all=[Collections.Generic.SortedSet[string]]::new([StringComparer]::Ordinal)
foreach($p in $leftSet){[void]$all.Add($p)}
foreach($p in $rightSet){[void]$all.Add($p)}
$rows=[Collections.Generic.List[object]]::new()
foreach($path in $all) {
  $a=$leftSet.Contains($path)
  $b=$rightSet.Contains($path)
  $aOid=Blob $h95 $path
  $bOid=Blob $h316 $path
  $class=if($a -and $b) {
    if($aOid -ceq $bOid){'BOTH_IDENTICAL'} else {'BOTH_DIFFERENT'}
  } elseif($a){'PR95_ONLY'}else{'PR316_ONLY'}
  $rows.Add([pscustomobject][ordered]@{
    path=$path;class=$class;pr95_changed=$a;pr316_changed=$b
    pr95_object=$aOid;pr316_object=$bOid
  })
}
# Git's merge machinery is the authority. A path overlap is only a
# candidate conflict; do not assume overlap == conflict or resolve by order.
$merge=Git @('merge-tree','--write-tree','--messages',$h95,$h316)
Require ($merge.Exit -in @(0,1)) 'MERGE_TREE_INFRA_FAILURE'
$lines=@($merge.Out.Split("`n",[StringSplitOptions]::RemoveEmptyEntries))
$tree=if($lines.Count){$lines[0].Trim()}else{''}
Require ($tree -cmatch '^[0-9a-f]{40,64}$') 'MISSING_MERGE_TREE_ID'
$state=if($merge.Exit -eq 0){'CLEAN_PREVIEW'}else{'CONFLICT_HOLD'}
$report=[ordered]@{
 protocol='CROSS-MERGE-SORT-THAILONGA-v1'
 mode='read_only_ref_safe_git_merge_tree_preview'
 thailong='HIDES'
 thailonga='PROVENANCE_OPEN'
 repository=$root
 pr95_head=$h95
 pr316_head=$h316
 common_ancestor=$base
 preview_tree=$tree
 result=$state
 total_paths=$rows.Count
 pr95_changed_paths=$leftSet.Count
 pr316_changed_paths=$rightSet.Count
 overlap_different=@($rows | Where-Object class -eq 'BOTH_DIFFERENT').Count
 overlap_identical=@($rows | Where-Object class -eq 'BOTH_IDENTICAL').Count
 conflict_diagnostics=$merge.Out
 source_mutated=$false
 refs_updated=$false
 approval_inferred=$false
 rows=@($rows)
}
[void][IO.Directory]::CreateDirectory($out)
$json=Join-Path $out 'cross-merge-sort.json'
$tsv=Join-Path $out 'cross-merge-sort.tsv'
$txt=Join-Path $out 'merge-tree-evidence.txt'
[IO.File]::WriteAllText($json,($report | ConvertTo-Json -Depth 8),
  [Text.UTF8Encoding]::new($false))
$linesTsv=[Collections.Generic.List[string]]::new()
$linesTsv.Add("path`tclass`tpr95_changed`tpr316_changed`tpr95_object`tpr316_object")
foreach($r in $rows){
  $linesTsv.Add((Escape-Field $r.path)+"`t"+$r.class+"`t"+$r.pr95_changed+
   "`t"+$r.pr316_changed+"`t"+$r.pr95_object+"`t"+$r.pr316_object)
}
[IO.File]::WriteAllLines($tsv,$linesTsv,[Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($txt,$merge.Out,[Text.UTF8Encoding]::new($false))
Write-Host ('CROSS_MERGE_SORT|'+$state+'|PATHS='+$rows.Count+
  '|PR95='+$leftSet.Count+'|PR316='+$rightSet.Count+
  '|TREE='+$tree+'|PROVENANCE=OPEN|REFS_UNCHANGED')
if($state -cne 'CLEAN_PREVIEW'){exit 2}
exit 0
