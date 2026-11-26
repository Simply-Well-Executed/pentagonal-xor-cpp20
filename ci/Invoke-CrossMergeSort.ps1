# SPDX-License-Identifier: 0BSD
# COREAO-365: offline, non-mutating cross-merge-sort inspector.
[CmdletBinding()]
param(
  [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{40}$')][string]$Yin,
  [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{40}$')][string]$Yang,
  [string]$RepositoryRoot='.',
  [switch]$HoldOnOverlap
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
function GitBytes([string[]]$GitArgs) {
  $start=[Diagnostics.ProcessStartInfo]::new()
  $start.FileName='git'
  $start.UseShellExecute=$false
  $start.RedirectStandardOutput=$true
  $start.RedirectStandardError=$true
  $start.ArgumentList.Add('-C'); $start.ArgumentList.Add($RepositoryRoot)
  foreach($arg in $GitArgs) { $start.ArgumentList.Add($arg) }
  $p=[Diagnostics.Process]::new()
  $p.StartInfo=$start
  try {
    if(-not $p.Start()) { throw 'COREAO|HOLD|GIT_NOT_STARTED' }
    $err=$p.StandardError.ReadToEndAsync()
    $stream=[IO.MemoryStream]::new()
    try {
      $p.StandardOutput.BaseStream.CopyTo($stream)
      $p.WaitForExit()
      if($p.ExitCode -ne 0) {
        throw ('COREAO|HOLD|GIT_FAILED|'+$err.GetAwaiter().GetResult())
      }
      return ,$stream.ToArray()
    } finally { $stream.Dispose() }
  } finally { $p.Dispose() }
}
$utf8=[Text.UTF8Encoding]::new($false,$true)
function GitText([string[]]$GitArgs) {
  return $utf8.GetString((GitBytes $GitArgs)).Trim()
}
function ChangedPaths([string]$Origin,[string]$Tip) {
  # NUL framing retains spaces, tab, newline, quotes and other legal path characters.
  $raw=GitBytes @('-c','diff.renames=false','diff','--no-ext-diff','--no-textconv','--name-only','-z',$Origin,$Tip,'--')
  $set=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
  if($raw.Length -eq 0) { return ,$set }
  if($raw[$raw.Length-1] -ne 0) { throw 'COREAO|HOLD|PATH_FRAMING' }
  $start=0
  for($i=0;$i -lt $raw.Length;$i++) {
    if($raw[$i] -ne 0) { continue }
    if($i -eq $start) { throw 'COREAO|HOLD|EMPTY_PATH' }
    $name=$utf8.GetString($raw,$start,$i-$start)
    if(-not $set.Add($name)) { throw 'COREAO|HOLD|REPEATED_PATH' }
    $start=$i+1
  }
  return ,$set
}
foreach($sha in @($Yin,$Yang)) {
  if((GitText @('cat-file','-t',$sha)) -cne 'commit') { throw 'COREAO|HOLD|NOT_COMMIT' }
}
$bases=@((GitText @('merge-base','--all',$Yin,$Yang)) -split '\r?\n' | Where-Object { $_ })
if($bases.Count -ne 1 -or $bases[0] -cnotmatch '^[0-9a-f]{40}$') { throw 'COREAO|HOLD|AMBIGUOUS_BASE' }
$base=$bases[0]
$a=ChangedPaths $base $Yin
$b=ChangedPaths $base $Yang
$yinOnly=[Collections.Generic.List[string]]::new()
$yangOnly=[Collections.Generic.List[string]]::new()
$shared=[Collections.Generic.List[string]]::new()
foreach($path in $a) {
  if($b.Contains($path)) { $shared.Add($path) } else { $yinOnly.Add($path) }
}
foreach($path in $b) {
  if(-not $a.Contains($path)) { $yangOnly.Add($path) }
}
function Sorted([string[]]$items) {
  $copy=[string[]]@($items)
  [Array]::Sort($copy,[StringComparer]::Ordinal)
  return ,$copy
}
$yinList=Sorted $yinOnly.ToArray()
$yangList=Sorted $yangOnly.ToArray()
$sharedList=Sorted $shared.ToArray()
$report=[ordered]@{
  schema='COREAO-CROSS-MERGE-SORT/1'
  source='local Git commit objects'
  angles=[ordered]@{yin=180;yang=180;coreao=5;full=365}
  yin=$Yin.ToLowerInvariant()
  yang=$Yang.ToLowerInvariant()
  merge_base=$base
  yin_only=$yinList
  yang_only=$yangList
  shared_paths=$sharedList
  counts=[ordered]@{yin_only=$yinList.Length;yang_only=$yangList.Length;shared=$sharedList.Length}
  rename_review='REQUIRED: rename detection disabled; delete/add may represent rename'
  integration='NOT_ATTEMPTED'
  decision='HOLD'
  release='HOLD'
}
$json=ConvertTo-Json -InputObject $report -Depth 6
Write-Output $json
if($HoldOnOverlap -and $sharedList.Length -gt 0) { throw 'COREAO|HOLD|OVERLAP_REVIEW' }
