# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Reproducible experimental compiler study: YANG 5, YIN 5 with no /GL.
# Original Windows no-op header is an unsafe negative fixture, never production.
[CmdletBinding()]
param([int]$Iterations=500000,[int]$Warmup=50000,[int]$Repeats=5,
      [int]$Trials=3,[string]$BuildRoot='build-v2-yin-yang',
      [string]$EvidenceDir='evidence-v2-yin-yang')
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$PSNativeCommandUseErrorActionPreference=$false
function Verify([bool]$condition,[string]$reason) {
 if(!$condition){ throw "V2_YIN_YANG: $reason" }
}
Verify ($IsWindows -and $env:RUNNER_OS -ceq 'Windows') 'Requires Windows MSVC runner'
Verify ($Iterations -ge 1000 -and $Warmup -ge 100 -and $Repeats -ge 3 -and
  $Repeats%2 -eq 1 -and $Trials -ge 3) 'Invalid measurement protocol'
$sha=((& git rev-parse HEAD)|Out-String).Trim()
Verify ($LASTEXITCODE -eq 0 -and $sha -match '^[a-f0-9]{40}$') 'Missing source commit'
Verify (!$env:GITHUB_SHA -or $env:GITHUB_SHA -ceq $sha) 'Commit mismatch'
New-Item -ItemType Directory -Force -Path $BuildRoot,$EvidenceDir | Out-Null
$columns=@(
 [ordered]@{name='A';opt='/Od';gl=$false;link='/LTCG:OFF';label='no speed optimization'},
 [ordered]@{name='B';opt='/O2';gl=$false;link='/LTCG:OFF';label='O2 alone'},
 [ordered]@{name='C';opt='/O2';gl=$true;link='/LTCG';label='O2 GL LTCG'},
 [ordered]@{name='D';opt='/O2';gl=$true;link='';label='O2 GL automatic linker LTCG'},
 [ordered]@{name='E';opt='/O2';gl=$false;link='/LTCG';label='O2 LTCG without GL'}
)
$builds=[ordered]@{}
$observations=[ordered]@{}
function Hash([string]$path) {
 if(!(Test-Path -LiteralPath $path -PathType Leaf)){return $null}
 return (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function Exec([string]$log,[string]$app,[string[]]$arguments) {
 & $app @arguments *> $log
 if($null -eq $LASTEXITCODE){return 99}
 return [int]$LASTEXITCODE
}
function Build([string]$side,[Collections.IDictionary]$column,[string]$variant) {
 $key=$side+'-'+$column.name+'-'+$variant
 $gl=($side -eq 'YANG' -and $column.gl)
 $cc=$column.opt+' '+$(if($gl){'/GL'}else{'/GL-'})+' /DNDEBUG'
 $ld='/INCREMENTAL:NO'+$(if($column.link){' '+$column.link}else{''})
 $dir=Join-Path $BuildRoot $key
 $cfg=Join-Path $EvidenceDir ($key+'-configure.log')
 $compile=Join-Path $EvidenceDir ($key+'-compile.log')
 $contract=Join-Path $EvidenceDir ($key+'-contract.log')
 $smoke=Join-Path $EvidenceDir ($key+'-smoke.log')
 $args=@('-S','.','-B',$dir,'-G','Visual Studio 17 2022','-A','x64',
  '-DBUILD_TESTING=ON',('-DSWE_LIGAZE_WINDOWS_AB_VARIANT='+$variant),
  ('-DCMAKE_CXX_FLAGS_RELEASE:STRING='+$cc),
  ('-DCMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='+$ld),
  '-DCMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE:BOOL=OFF')
 $config=Exec $cfg 'cmake' $args
 $r=[ordered]@{
  key=$key;side=$side;column=$column.name;variant=$variant;
  requested_compile=$cc;requested_link=$ld;configure_exit=$config;
  flags_verified=$false;bench_exit=$null;smoke_exit=$null;
  api_contract_exit=$null;rejection_confirmed=$false;
  binary=$null;binary_sha256=$null;
  auto_ltcg_diagnostic=$false;
  logs=[ordered]@{configure=$cfg;compile=$compile;contract=$contract;smoke=$smoke}
 }
 if($config -ne 0){return $r}
 $cache=Get-Content -LiteralPath (Join-Path $dir 'CMakeCache.txt') -Raw
 $r.flags_verified=$cache.Contains('CMAKE_CXX_FLAGS_RELEASE:STRING='+$cc) -and
  $cache.Contains('CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='+$ld)
 Verify $r.flags_verified "CMake modified flags in $key"
 $r.bench_exit=Exec $compile 'cmake' @('--build',$dir,'--config','Release',
    '--target','pentagonal_ligaze_benchmark','--parallel','2','--verbose')
 if($r.bench_exit -eq 0) {
  $binary=Join-Path (Join-Path $dir 'Release') 'pentagonal_ligaze_benchmark.exe'
  Verify (Test-Path -LiteralPath $binary -PathType Leaf) "Missing $binary"
  $r.binary=$binary
  $r.binary_sha256=Hash $binary
  $r.smoke_exit=Exec $smoke 'ctest' @('--test-dir',$dir,'-C','Release',
   '-R','^pentagonal_ligaze_benchmark_smoke$','--output-on-failure')
  $buildLog=Get-Content -LiteralPath $compile -Raw
  $r.auto_ltcg_diagnostic=($buildLog -match '(?i)(restart.{0,30}/LTCG|/LTCG.{0,30}restart)')
 }
 if($variant -eq 'full_header_v2'){
  $r.api_contract_exit=Exec $contract 'cmake' @('--build',$dir,'--config','Release',
   '--target','pentagonal_platform_v2_contract_test','--parallel','2','--verbose')
  $contractText=if(Test-Path -LiteralPath $contract){Get-Content -LiteralPath $contract -Raw}else{''}
  $r.rejection_confirmed=($r.api_contract_exit -ne 0 -and
   $contractText -match '(?i)(C2338|static.assert|UNSAFE:|ForgedSleepCall|ForgedCreateFileCall)')
 }
 return $r
}
function Read-Bench([Collections.IDictionary]$r){
 Verify ($r.bench_exit -eq 0 -and $r.smoke_exit -eq 0 -and $r.binary) "Not measured: $($r.key)"
 $text=@(& $r.binary ([string]$Iterations) ([string]$Warmup) ([string]$Repeats))
 Verify ($LASTEXITCODE -eq 0 -and $text.Count -gt 0) "Benchmark rejected: $($r.key)"
 $v=([string]$text[-1]|ConvertFrom-Json)
 Verify ($v.schema -ceq 'SWE-LIGAZE-BENCH-v1' -and
  $v.threat_fixtures -ceq 'PASS' -and $v.iterations -eq $Iterations -and
  $v.repeats -eq $Repeats -and [double]$v.median_ns_per_call -gt 0) "Invalid benchmark result $($r.key)"
 return $v
}
Write-Host "V2_YIN_YANG|SOURCE|$sha"
foreach($column in $columns) {
 foreach($side in @('YIN','YANG')) {
  foreach($variant in @('baseline','full_header_v2')) {
   $key=$side+'-'+$column.name+'-'+$variant
   Write-Host "V2_YIN_YANG|BUILD|$key"
   $builds[$key]=Build $side $column $variant
   Write-Host ("V2_YIN_YANG|COMPILE_RESULT|{0}|{1}" -f $key,$builds[$key].bench_exit)
  }
 }
}
foreach($key in $builds.Keys) {
 $r=$builds[$key]
 Verify ($r.configure_exit -eq 0 -and $r.flags_verified) "Configure failure: $key"
 if($r.variant -eq 'baseline') {
  Verify ($r.bench_exit -eq 0 -and $r.smoke_exit -eq 0) "Canonical baseline failed: $key"
 }else {
  Verify $r.rejection_confirmed "Expected unsafe WinAPI contract rejection missing: $key"
 }
}
$javaOut=Join-Path $BuildRoot 'java'
New-Item -ItemType Directory -Force -Path $javaOut|Out-Null
& javac --release 17 -encoding UTF-8 -d $javaOut java/PentagonalControl.java java/LigazeJavaBenchmark.java
Verify ($LASTEXITCODE -eq 0) 'Java 17 baseline compile failed'
foreach($column in $columns) {
 $trials=[System.Collections.Generic.List[object]]::new()
 for($trial=0;$trial -lt $Trials;$trial++) {
  $order=if($trial%2 -eq 0){@('YIN','YANG')}else{@('YANG','YIN')}
  $pair=[ordered]@{}
  foreach($side in $order){
   $base=$builds[$side+'-'+$column.name+'-baseline']
   $expanded=$builds[$side+'-'+$column.name+'-full_header_v2']
   if($expanded.bench_exit -ne 0 -or $expanded.smoke_exit -ne 0){
    $pair[$side]=[ordered]@{measured=$false;reason='expanded benchmark did not compile or pass smoke'}
    continue
   }
   $a=Read-Bench $base
   $b=Read-Bench $expanded
   Verify ([string]$a.checksum -ceq [string]$b.checksum) "COR changed: $side / $($column.name)"
   $pair[$side]=[ordered]@{
    measured=$true;baseline_ns=[double]$a.median_ns_per_call;
    expanded_ns=[double]$b.median_ns_per_call;
    expanded_over_baseline=([double]$b.median_ns_per_call/[double]$a.median_ns_per_call)
    checksum=[string]$a.checksum
   }
  }
  $javaOutput=@(& java -cp $javaOut LigazeJavaBenchmark ([string]$Iterations) ([string]$Warmup) ([string]$Repeats))
  Verify ($LASTEXITCODE -eq 0 -and $javaOutput.Count -gt 0) 'Java baseline failed'
  $java=([string]$javaOutput[-1]|ConvertFrom-Json)
  foreach($side in @('YIN','YANG')){
   if($pair[$side].measured){
    Verify ([string]$pair[$side].checksum -ceq [string]$java.checksum) "Java/native divergence: $side / $($column.name)"
   }
  }
  $ratio=if($pair.YIN.measured -and $pair.YANG.measured){
    $pair.YANG.expanded_ns/$pair.YIN.expanded_ns
  }else{$null}
  $trials.Add([ordered]@{index=$trial;order=$order -join ',';yin=$pair.YIN;yang=$pair.YANG;yang_over_yin=$ratio;java_ns=[double]$java.median_ns_per_call})
  Write-Host ("V2_YIN_YANG|TRIAL|{0}|{1}|YIN={2}|YANG={3}|ratio={4}" -f
   $column.name,$trial,$pair.YIN.expanded_ns,$pair.YANG.expanded_ns,$ratio)
 }
 $a=$builds['YIN-'+$column.name+'-full_header_v2']
 $b=$builds['YANG-'+$column.name+'-full_header_v2']
 $observations[$column.name]=[ordered]@{
  description=$column.label;yin_compile=$a.requested_compile;yang_compile=$b.requested_compile;
  linker=$b.requested_link;requested_flags_identical=($a.requested_compile -ceq $b.requested_compile)
  trials=$trials.ToArray()
 }
}
$report=[ordered]@{
 schema='SWE-V2-YIN-YANG-FIVE-PROFILE-v1';source_commit=$sha;source_mutated=$false
 platform='MSVC 2022 x64 Windows';yang='five requested optimization modes'
 yin='five matching modes with /GL disabled'
 yin_C_same_as_yin_E=$true;yin_A_B_E_match_yang_A_B_E=$true
 implicit_linker_ltcg_possible_when_gl_objects_linked=$true
 benchmark_results_do_not_override_winapi_contract=$true
 full_header_production_allowed=$false;promotion_performed=$false
 iterations=$Iterations;warmup=$Warmup;repeats=$Repeats;trials=$Trials
 builds=$builds;measurements=$observations
}
$out=Join-Path $EvidenceDir 'v2-yin-yang.json'
$report|ConvertTo-Json -Depth 16|Set-Content -LiteralPath $out -Encoding utf8
Write-Host ("V2_YIN_YANG|PASS|evidence_sha256={0}" -f (Hash $out))
