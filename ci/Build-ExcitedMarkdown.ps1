# SPDX-License-Identifier: 0BSD
# EXCITED-STAR-003: A* + SPT Finland + DOVIA* RUSPT COREAO, MSVC /O2.
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
  'ci/excited-native/dovia_star.hpp',
  'ci/excited-native/dovia_policy_test.cpp',
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
& $cmake --build $build --config Release --target swe_excited_astar swe_dovia_policy --parallel 1
if($LASTEXITCODE -ne 0){throw 'EXCITED|HOLD|msvc_o2_build'}
& ctest --test-dir $build -C Release --output-on-failure -R '^excited_'
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
   -not $markdown.Contains('**DOVIA YANG and RUSPT YIN verified**') -or
   -not $markdown.Contains('**DOVIA VLAN range:** 1-4094 (4096 rejected)') -or
   -not $markdown.Contains('**DOVIA protected path cost:** 6') -or
   -not $markdown.Contains('3 x 45 + 1 x 45 = 180 degrees') -or
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
# Exact canonical key/value records. No delimiter or array-expression inference.
# Reconstruct and validate every row, then re-read the emitted UTF-8 TSV.
$receiptValues=[ordered]@{
  schema='EXCITED-STAR-003'
  source_sha=$SourceSha
  native_source_sha256=$sourceDigest
  native_binary_sha256=$binSha
  markdown_file='sovieta-diao-xian-excited.md'
  markdown_sha256=$mdSha
  route='Overview>Security>Summary'
  path_cost='6'
  spt_protocol='SHORTEST_PATH_TREE_NOT_ETHERNET_STP'
  spt_verified='YES'
  spt_vertex_costs='0,2,4,5,6'
  spt_parent_nodes='-1,0,0,1,2'
  dovia_yang='RFC2328_1998_SECTION16_1_BASE_NO_ERRATA'
  dovia_yin='RUSPT_ROOT_GUARD_ASTAR'
  dovia_verified='YES'
  dovia_vlan_range='1-4094'
  dovia_root='DSW-RING1'
  dovia_rotation_degrees='180'
  dovia_policy_negative_ctest='PASS'
  compiler='MSVC_CXX20_O2'
  no_std_in_source='YES'
  no_json='YES'
  promotion='NO'
  release='HOLD'
}
if($receiptValues.Count -ne 24){throw 'EXCITED|HOLD|receipt_schema_field_count'}
$recordLines=[Collections.Generic.List[string]]::new()
foreach($entry in $receiptValues.GetEnumerator()){
  $key=[string]$entry.Key
  $value=[string]$entry.Value
  if($key -cnotmatch '^[a-z][a-z0-9_]*$' -or
     [string]::IsNullOrEmpty($value) -or $value.IndexOf([char]9) -ge 0 -or
     $value.IndexOf([char]10) -ge 0 -or $value.IndexOf([char]13) -ge 0){
    throw ('EXCITED|HOLD|unsafe_receipt_value|key='+$key)
  }
  $recordLines.Add($key+[char]9+$value)
}
$tsv=Join-Path $out 'excited-receipt.tsv'
$encoding=[Text.UTF8Encoding]::new($false,$true)
[IO.File]::WriteAllText($tsv,([string]::Join([string][char]10,$recordLines)+[char]10),$encoding)
$readBack=[IO.File]::ReadAllLines($tsv,$encoding)
if($readBack.Count -ne $recordLines.Count){
  throw 'EXCITED|HOLD|receipt_roundtrip_count'
}
for([int]$index=0;$index -lt $recordLines.Count;$index++){
  if($readBack[$index] -cne $recordLines[$index]){
    throw ('EXCITED|HOLD|receipt_roundtrip_row|row='+($index+1))
  }
}
$receiptSha=(Get-FileHash -LiteralPath $tsv -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('EXCITED|TSV_WRITER|SCHEMA=EXCITED-STAR-003|ROWS='+$recordLines.Count+
  '|RECEIPT_SHA256='+$receiptSha+'|RELEASE=HOLD')
if(-not [string]::IsNullOrWhiteSpace($env:GITHUB_STEP_SUMMARY)){
  [IO.File]::AppendAllText($env:GITHUB_STEP_SUMMARY,
    ($markdown+[char]10+'EXCITED v3 Markdown SHA-256: `'+$mdSha+'`'+[char]10),
    [Text.UTF8Encoding]::new($false))
}
Write-Host ('EXCITED|V3|NATIVE_ASTAR_SPT_DOVIA_PASS|COST=6|MARKDOWN_SHA256='+$mdSha+
            '|BINARY_SHA256='+$binSha+'|RELEASE=HOLD')
