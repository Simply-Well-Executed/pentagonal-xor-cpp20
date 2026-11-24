# SPDX-License-Identifier: 0BSD
# EXCITED-STAR-001: deterministic A* markdown, exact integer edge weights.
# All input edges, heuristics, and canonical labels are fixed by source.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$OutputDirectory,
  [Parameter(Mandatory=$true)][ValidatePattern('^[0-9a-f]{40}$')][string]$SourceSha
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($OutputDirectory)
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $out
if($LASTEXITCODE -ne 0 -or [IO.Directory]::Exists($out)){
  throw 'EXCITED|HOLD|unsafe_or_existing_output'
}
foreach($path in @('ci/Invoke-RepositoryCircuit.ps1','ci/Excite-Coreao.ps1',
                   '.github/workflows/EXCITE.yml',
                   '.github/workflows/EXCITED.yml')){
  if(-not [IO.File]::Exists((Join-Path $root $path))){
    throw ('EXCITED|HOLD|missing_source|'+$path)
  }
}
[void][IO.Directory]::CreateDirectory($out)
$graph=[ordered]@{
  Overview=@(@{to='Architecture';cost=2},@{to='Security';cost=4})
  Architecture=@(@{to='Benchmarks';cost=3},@{to='Summary';cost=5})
  Security=@(@{to='Summary';cost=2})
  Benchmarks=@(@{to='Summary';cost=1})
  Summary=@()
}
# Lower bounds to Summary; admissible and consistent for this graph.
$heuristic=@{Overview=4;Architecture=3;Security=2;Benchmarks=1;Summary=0}
$best=@{Overview=[long]0}
$paths=@{Overview=@('Overview')}
$frontier=[Collections.Generic.List[object]]::new()
$frontier.Add([pscustomobject]@{node='Overview';g=[long]0;f=[long]4;path='Overview'})
$visited=[Collections.Generic.List[string]]::new()
$goal=$null
while($frontier.Count -gt 0){
  # Small fixed graph: stable, deterministic priority order (f, g, node).
  $candidate=@($frontier | Sort-Object -Property f,g,node | Select-Object -First 1)[0]
  $frontier.Remove($candidate) | Out-Null
  $node=[string]$candidate.node
  if($candidate.g -ne $best[$node]){continue}
  if($node -ceq 'Summary'){$goal=$node;break}
  $visited.Add($node)
  foreach($edge in $graph[$node]){
    $next=[string]$edge.to
    $weight=[long]$edge.cost
    if($weight -lt 0 -or $candidate.g -gt ([long]::MaxValue-$weight)){
      throw 'EXCITED|HOLD|edge_cost_overflow'
    }
    $newCost=[long]$candidate.g+$weight
    if(-not $best.ContainsKey($next) -or $newCost -lt $best[$next]){
      $best[$next]=$newCost
      $paths[$next]=@($paths[$node])+@($next)
      $frontier.Add([pscustomobject]@{
        node=$next
        g=$newCost
        f=$newCost+[long]$heuristic[$next]
        path=([string]::Join('>',$paths[$next]))
      })
    }
  }
}
if($goal -cne 'Summary' -or $best['Summary'] -ne 6){
  throw 'EXCITED|HOLD|Astar_path_or_cost'
}
$route=@($paths['Summary'])
if($route.Count -lt 2 -or $route[0] -cne 'Overview' -or
   $route[-1] -cne 'Summary'){
  throw 'EXCITED|HOLD|Astar_route_unbounded'
}
# A* chooses a documentation selection, NOT a replacement for actual checks.
$lines=[Collections.Generic.List[string]]::new()
$lines.Add('# Sovieta Diao Xian ^.^ | EXCITED A* Markdown')
$lines.Add('')
$lines.Add('**Input:** successful EXCITE job on source SHA `'+$SourceSha+'`.')
$lines.Add('**Shortest cost:** '+$best['Summary']+' (dimensionless documentation edge weights).')
$lines.Add('**Chosen path:** '+([string]::Join(' → ',$route))+'.')
$lines.Add('**Scope:** the A* path assembles a guide. It does not certify source or release.')
$lines.Add('')
$lines.Add('## Sections')
$i=0
foreach($section in $route){
  $i++
  $lines.Add(('{0}. **{1}**: selected by deterministic A*.' -f $i,$section))
}
$lines.Add('')
$lines.Add('## COREAO code-block completion')
$lines.Add('')
$lines.Add('EXCITE processes each enumerated file, each of six independent subprojects, and the final repository sprint through QCHART → QLAM → QGRAPH → exact inverse-square witnesses → FRESH HAODA display metadata → QCODE.')
$lines.Add('The inverse-square witnesses are unitless fractions (1/1, 1/4, 1/9), not physical radiation measurements. Q-prefixed operations are experimental evidence envelopes rather than an established algorithm.')
$lines.Add('')
$lines.Add('## Verification and release')
$lines.Add('')
$lines.Add('- This guide is generated only by EXCITED after the upstream EXCITE job succeeds.')
$lines.Add('- A success does not create an INCEPTION acknowledgment, close an issue, merge a PR, or authorize release.')
$lines.Add('- **RELEASE=HOLD** unless separately reviewed and explicitly authorized.')
$lines.Add('')
$outPath=Join-Path $out 'sovieta-diao-xian-excited.md'
[IO.File]::WriteAllText($outPath,([string]::Join([char]10,$lines)+[char]10),
  [Text.UTF8Encoding]::new($false))
$markdown=[IO.File]::ReadAllText($outPath,[Text.Encoding]::UTF8)
if($markdown.Length -lt 500 -or
   $markdown -cnotmatch '^# Sovieta Diao Xian' -or
   $markdown -cnotmatch 'RELEASE=HOLD'){
  throw 'EXCITED|HOLD|generated_markdown_invalid'
}
$hash=(Get-FileHash -LiteralPath $outPath -Algorithm SHA256).Hash.ToLowerInvariant()
$receipt=[ordered]@{
  schema='EXCITED-STAR-001'
  source_sha=$SourceSha
  markdown_sha256=$hash
  relative_markdown='sovieta-diao-xian-excited.md'
  selected_nodes=@($route)
  total_cost=[long]$best['Summary']
  engine='PowerShell A* fixed nonnegative graph'
  promotion='NO'
  release='HOLD'
}
[IO.File]::WriteAllText((Join-Path $out 'excited-receipt.json'),
  (ConvertTo-Json -InputObject $receipt -Compress -Depth 9 -WarningAction Stop),
  [Text.UTF8Encoding]::new($false))
if(-not [string]::IsNullOrWhiteSpace($env:GITHUB_STEP_SUMMARY)){
  [IO.File]::AppendAllText($env:GITHUB_STEP_SUMMARY,
    ($markdown+[char]10+'EXCITED Markdown SHA-256: `'+$hash+'`'+[char]10),
    [Text.UTF8Encoding]::new($false))
}
Write-Host ('EXCITED|A_STAR=PASS|COST='+$best['Summary']+
            '|MARKDOWN_SHA256='+$hash+'|RELEASE=HOLD')
