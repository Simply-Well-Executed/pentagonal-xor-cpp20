# SPDX-License-Identifier: 0BSD
# ARIRANG: three independent COREAO gauges followed by a fail-closed AIRGAP.
# This reads local Git objects only. It never fetches, merges, pushes, or moves a ref.
[CmdletBinding()]
param(
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{40}$')][string]$Yin,
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{40}$')][string]$Yang,
 [string]$RepositoryRoot='.',
 [string]$Target='refs/heads/main',
 [string]$SnapshotPath
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$utf8=[Text.UTF8Encoding]::new($false,$true)
function RunGit([string[]]$GitArgs) {
 $pinfo=[Diagnostics.ProcessStartInfo]::new()
 $pinfo.FileName='git'; $pinfo.UseShellExecute=$false
 $pinfo.RedirectStandardOutput=$true; $pinfo.RedirectStandardError=$true
 $pinfo.ArgumentList.Add('-C'); $pinfo.ArgumentList.Add($RepositoryRoot)
 foreach($arg in $GitArgs) { $pinfo.ArgumentList.Add($arg) }
 $p=[Diagnostics.Process]::new(); $p.StartInfo=$pinfo
 try {
  if(-not $p.Start()){throw 'ARIRANG|HOLD|GIT_START'}
  $errTask=$p.StandardError.ReadToEndAsync()
  $mem=[IO.MemoryStream]::new()
  try {
   $p.StandardOutput.BaseStream.CopyTo($mem)
   $p.WaitForExit()
   return [pscustomobject]@{Exit=$p.ExitCode;Bytes=$mem.ToArray();Error=$errTask.GetAwaiter().GetResult()}
  } finally {$mem.Dispose()}
 } finally {$p.Dispose()}
}
function GitSuccess([string[]]$Arguments) {
 $r=RunGit $Arguments
 if($r.Exit -ne 0) {throw ('ARIRANG|HOLD|GIT_EXIT|'+$r.Error.Trim())}
 return $r
}
function GitText([string[]]$Arguments) {
 $r=GitSuccess $Arguments
 return $utf8.GetString($r.Bytes).Trim()
}
function IsAncestor([string]$Ancestor,[string]$Descendant) {
 $r=RunGit @('merge-base','--is-ancestor',$Ancestor,$Descendant)
 if($r.Exit -eq 0){return $true}
 if($r.Exit -eq 1){return $false}
 throw ('ARIRANG|HOLD|ANCESTRY_INDETERMINATE|'+$r.Error)
}
function Paths([string]$From,[string]$To) {
 $r=GitSuccess @('-c','diff.renames=false','diff','--no-ext-diff','--no-textconv','--name-only','-z',$From,$To,'--')
 $set=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
 $bytes=$r.Bytes
 if($bytes.Length -eq 0){return ,$set}
 if($bytes[$bytes.Length-1] -ne 0){throw 'ARIRANG|HOLD|PATH_DELIMITER'}
 $first=0
 for($i=0;$i -lt $bytes.Length;$i++){
  if($bytes[$i] -ne 0){continue}
  if($i -eq $first){throw 'ARIRANG|HOLD|EMPTY_PATH'}
  $path=$utf8.GetString($bytes,$first,$i-$first)
  if(-not $set.Add($path)){throw 'ARIRANG|HOLD|DUPLICATE_PATH'}
  $first=$i+1
 }
 return ,$set
}
function Sorted([string[]]$Items) {
 $copy=[string[]]@($Items); [Array]::Sort($copy,[StringComparer]::Ordinal)
 return ,$copy
}
# Gauge 1: independent object and common-ancestor checks (YIN and YANG untrusted).
foreach($hash in @($Yin,$Yang)){
 if((GitText @('cat-file','-t',$hash)) -cne 'commit'){throw 'ARIRANG|HOLD|NOT_COMMIT'}
}
if($Yin -ceq $Yang){throw 'ARIRANG|HOLD|IDENTICAL_INPUTS'}
$yinFull=GitText @('rev-parse','--verify',"$Yin^{commit}")
$yangFull=GitText @('rev-parse','--verify',"$Yang^{commit}")
$baseCandidates=@((GitText @('merge-base','--all',$yinFull,$yangFull)) -split '\r?\n' | Where-Object {$_})
if($baseCandidates.Count -ne 1){throw 'ARIRANG|HOLD|AMBIGUOUS_COMMON_ANCESTOR'}
$base=$baseCandidates[0]
$targetCommit=GitText @('rev-parse','--verify',"$Target^{commit}")
if($targetCommit -cnotmatch '^[0-9a-f]{40}$'){throw 'ARIRANG|HOLD|INVALID_TARGET'}
# Gauge 2: presume YIN assertions false. Recompute its delta from Git and compare inclusion.
$yinChanges=Paths $base $yinFull
$yinIncluded=IsAncestor $yinFull $targetCommit
# Gauge 3: presume YANG assertions false. Recompute independently; no YIN-derived verdict.
$yangChanges=Paths $base $yangFull
$yangIncluded=IsAncestor $yangFull $targetCommit
$shared=[Collections.Generic.List[string]]::new()
foreach($path in $yinChanges){if($yangChanges.Contains($path)){$shared.Add($path)}}
$sharedSorted=Sorted $shared.ToArray()
# AIRGAP: inclusion is ancestry, not evidence of content or semantic parity.
# Current target's residual delta against each source head is informational only.
$yinResidual=Paths $yinFull $targetCommit
$yangResidual=Paths $yangFull $targetCommit
$clear=($yinIncluded -and $yangIncluded)
$report=[ordered]@{
 schema='ARIRANG-COREAO-THREE-GAUGES/1'
 evidence='LOCAL_GIT_OBJECTS'
 target_ref=$Target
 target_commit=$targetCommit
 source=[ordered]@{yin=$yinFull;yang=$yangFull;merge_base=$base}
 gauges=[ordered]@{
  provenance=[ordered]@{unique_base=$true;valid_commits=$true}
  distrust_yin=[ordered]@{changed_paths=$yinChanges.Count;independently_in_main=$yinIncluded}
  distrust_yang=[ordered]@{changed_paths=$yangChanges.Count;independently_in_main=$yangIncluded}
 }
 airgap=[ordered]@{
  both_heads_ancestors=$clear
  shared_paths=$sharedSorted
  yin_to_target_residual=Sorted @($yinResidual)
  yang_to_target_residual=Sorted @($yangResidual)
  semantics_qualified=$false
  tests_qualified=$false
 }
 decision=$(if($clear){'CLEAR_ANCESTRY_ONLY'}else{'HOLD'})
 release='HOLD'
}
$json=ConvertTo-Json -InputObject $report -Depth 8
Write-Output $json
if(-not $clear){throw 'ARIRANG|HOLD|PR_HEADS_NOT_BOTH_IN_TARGET'}
# Only a snapshot of verified ancestry is CLEAR; conflicts, CI and behavior remain unqualified.
if($SnapshotPath){
 $full=[IO.Path]::GetFullPath($SnapshotPath)
 $directory=[IO.Path]::GetDirectoryName($full)
 if(-not [IO.Directory]::Exists($directory)){throw 'ARIRANG|HOLD|SNAPSHOT_DIRECTORY_MISSING'}
 $file=[IO.File]::Open($full,[IO.FileMode]::CreateNew,[IO.FileAccess]::Write,[IO.FileShare]::None)
 try{
  $bytes=[Text.UTF8Encoding]::new($false).GetBytes($json+[Environment]::NewLine)
  $file.Write($bytes,0,$bytes.Length)
 }finally{$file.Dispose()}
}
