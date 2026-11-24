# SPDX-License-Identifier: 0BSD
# EXCITED-STAR-002: build/run C++20 A* plus SPT Finland verification, MSVC /O2.
# Only source bootstrap contacts GitHub; this stage compiles offline.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$OutputDirectory,
  [Parameter(Mandatory=$true)][ValidatePattern('^[a-f0-9]{40}$')][string]$SourceSha
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows) {throw 'EXCITED|HOLD|Windows_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($OutputDirectory)
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $out
if($LASTEXITCODE -ne 0 -or [IO.Directory]::Exists($out)){
  throw 'EXCITED|HOLD|unsafe_or_existing_output'
}
$required=@(
  'ci/Invoke-RepositoryCircuit.ps1',
  'ci/Excite-Coreao.ps1',
  'ci/excited-native/astar_raw.cpp',
  'ci/excited-native/CMakeLists.txt',
  '.github/workflows/EXCITE.yml',
  '.github/workflows/EXCITED.yml'
)
foreach($relative in $required){
  if(-not [IO.File]::Exists((Join-Path $root $relative))){
    throw ('EXCITED|HOLD|missing_source|'+$relative)
  }
}
$code=Join-Path $root 'ci/excited-native/astar_raw.cpp'
$sourceDigest=(Get-FileHash -LiteralPath $code -Algorithm SHA256).Hash.ToLowerInvariant()
[void][IO.Directory]::CreateDirectory($out)
$build=Join-Path $out 'build'
$cmake=(Get-Command cmake -ErrorAction Stop).Source
& $cmake -S (Join-Path $root 'ci/excited-native') -B $build -DBUILD_TESTING=ON
if($LASTEXITCODE -ne 0){throw 'EXCITED|HOLD|cmake_configure'}
& $cmake --build $build --config Release --target swe_excited_astar --parallel 1
if($LASTEXITCODE -ne 0){throw 'EXCITED|HOLD|msvc_o2_build'}
& ctest --test-dir $build -C Release --output-on-failure -R '^excited_(astar_exact_path|spt_finland_verifier)$'
if($LASTEXITCODE -ne 0){throw 'EXCITED|HOLD|astar_ctest'}
$binary=''
foreach($candidate in @(
  (Join-Path $build 'Release/swe_excited_astar.exe'),
  (Join-Path $build 'swe_excited_astar.exe')
)){
  if([IO.File]::Exists($candidate)){$binary=$candidate;break}
}
if(-not $binary){throw 'EXCITED|HOLD|native_executable_missing'}
$start=[Diagnostics.ProcessStartInfo]::new($binary)
$start.UseShellExecute=$false
$start.RedirectStandardOutput=$true
$start.RedirectStandardError=$true
$start.StandardOutputEncoding=[Text.UTF8Encoding]::new($false,$true)
$start.StandardErrorEncoding=[Text.UTF8Encoding]::new($false,$true)
$process=[Diagnostics.Process]::Start($start)
if($null -eq $process){throw 'EXCITED|HOLD|native_process_start'}
try{
  $markdown=$process.StandardOutput.ReadToEnd()
  $stderr=$process.StandardError.ReadToEnd()
  if(-not $process.WaitForExit(15000)){
    $process.Kill($true)
    throw 'EXCITED|HOLD|native_timeout'
  }
  if($process.ExitCode -ne 0 -or $stderr.Length -gt 0){
    throw ('EXCITED|HOLD|native_exit='+$process.ExitCode)
  }
}finally{$process.Dispose()}
if($markdown.Length -lt 500 -or $markdown.Length -gt 4096 -or
   -not $markdown.StartsWith('# Sovieta Diao Xian ^.^ | EXCITED v2 A* Markdown') -or
   -not $markdown.Contains('**Optimal path:** Overview -> Security -> Summary') -or
   -not $markdown.Contains('**Total path cost:** 6') -or
   -not $markdown.Contains('## SPT Finland shortest-path tree cross-check') -or
   -not $markdown.Contains('**SPT total goal cost:** 6') -or
   -not $markdown.Contains('Summary: cost 6, parent Security') -or
   -not $markdown.Contains('**RELEASE=HOLD**')){
  throw 'EXCITED|HOLD|native_markdown_or_route_invalid'
}
# Metadata is added by the trusted orchestrator, not inferred by the A* graph.
$markdown += [char]10+'**EXCITE source revision:** `'+$SourceSha+'`.'+[char]10
$path=Join-Path $out 'sovieta-diao-xian-excited.md'
[IO.File]::WriteAllText($path,$markdown,[Text.UTF8Encoding]::new($false))
$mdSha=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
$binSha=(Get-FileHash -LiteralPath $binary -Algorithm SHA256).Hash.ToLowerInvariant()
$after=(Get-FileHash -LiteralPath $code -Algorithm SHA256).Hash.ToLowerInvariant()
if($sourceDigest -cne $after){throw 'EXCITED|HOLD|source_mutated_after_build'}
# Intentionally TSV rather than JSON, with no user-controlled fields.
$rows=@(
  'schema'+"`t"+'EXCITED-STAR-002',
  'source_sha'+"`t"+$SourceSha,
  'native_source_sha256'+"`t"+$sourceDigest,
  'native_binary_sha256'+"`t"+$binSha,
  'markdown_file'+"`t"+'sovieta-diao-xian-excited.md',
  'markdown_sha256'+"`t"+$mdSha,
  'route'+"`t"+'Overview>Security>Summary',
  'path_cost'+"`t"+'6',
  'spt_protocol'+"`t"+'SHORTEST_PATH_TREE_NOT_ETHERNET_STP',
  'spt_verified'+"`t"+'YES',
  'spt_vertex_costs'+"`t"+'0,2,4,5,6',
  'spt_parent_nodes'+"`t"+'-1,0,0,1,2',
  'compiler'+"`t"+'MSVC_CXX20_O2',
  'no_std_in_source'+"`t"+'YES',
  'no_json'+"`t"+'YES',
  'promotion'+"`t"+'NO',
  'release'+"`t"+'HOLD'
)
$tsv=Join-Path $out 'excited-receipt.tsv'
[IO.File]::WriteAllText($tsv,([string]::Join([char]10,$rows)+[char]10),
  [Text.UTF8Encoding]::new($false))
if(-not [string]::IsNullOrWhiteSpace($env:GITHUB_STEP_SUMMARY)){
  [IO.File]::AppendAllText($env:GITHUB_STEP_SUMMARY,
    ($markdown+[char]10+'EXCITED v2 Markdown SHA-256: `'+$mdSha+'`'+[char]10),
    [Text.UTF8Encoding]::new($false))
}
Write-Host ('EXCITED|V2|NATIVE_ASTAR_SPT_PASS|COST=6|MARKDOWN_SHA256='+$mdSha+
            '|BINARY_SHA256='+$binSha+'|RELEASE=HOLD')
