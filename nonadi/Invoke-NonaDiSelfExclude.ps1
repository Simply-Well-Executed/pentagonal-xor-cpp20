# SPDX-License-Identifier: 0BSD
# NONADI-001: Node/npm SELF_EXCLUDE. This scanner never starts Node.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
if([IO.Directory]::Exists($out) -or $out -ceq $root -or
   $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'NONADI|HOLD|unsafe_or_nonfresh_evidence'
}
[void][IO.Directory]::CreateDirectory($out)
$bad=[Collections.Generic.List[string]]::new()
$files=@(Get-ChildItem -LiteralPath $root -File -Recurse -Force|Sort-Object FullName)
foreach($f in $files){
 $rel=[IO.Path]::GetRelativePath($root,$f.FullName).Replace('\','/')
 if($rel.StartsWith('Archive/Retired/',[StringComparison]::OrdinalIgnoreCase)){continue}
 $leaf=[IO.Path]::GetFileName($rel)
 if($rel -match '(?i)(^|/)(node_modules|[.]npm)(/|$)' -or
   $leaf -match '(?i)([.]js|[.]mjs|[.]cjs|[.]jsx|[.]ts|[.]tsx|[.]mts|[.]cts)$' -or
   $leaf -match '(?i)^(package(-lock)?[.]json|yarn[.]lock|pnpm-lock[.]yaml|[.]npmrc)$'){
  $bad.Add(('SOURCE'+[char]9+$rel))
 }
 # 'uses:' is not prohibited generically; named Node-backed actions are.
 if($rel -match '^[.]github/workflows/.*[.]ya?ml$'){
  $content=[IO.File]::ReadAllText($f.FullName)
  if($content -match '(?im)^\s*uses\s*:\s*(actions/(checkout|cache|setup-node|setup-python|upload-artifact|download-artifact)@|[.]/[.]github/actions/|docker://)'){
   $bad.Add(('ACTION_REQUIRES_SEPARATE_REVIEW'+[char]9+$rel))
  }
 }
}
$receipt=Join-Path $out 'nona-node-self-exclusion.tsv'
$rows=[Collections.Generic.List[string]]::new()
$rows.Add('status'+[char]9+'path')
if($bad.Count -eq 0){$rows.Add('PASS'+[char]9+'NO_ACTIVE_NODE_NPM_SOURCE')}
else{foreach($entry in $bad){$rows.Add($entry)}}
[IO.File]::WriteAllLines($receipt,$rows,[Text.UTF8Encoding]::new($false))
Write-Host ('NONADI|NODE_NPM=SELF_EXCLUDE|CANDIDATES='+$bad.Count+
 '|RECEIPT_SHA256='+(Get-FileHash $receipt -Algorithm SHA256).Hash.ToLowerInvariant()+
 '|NO_NODE_EXECUTION=YES|RELEASE=HOLD')
if($bad.Count -gt 0){throw 'NONADI|HOLD|active_node_or_node_backed_action_requires_removal_or_review'}
