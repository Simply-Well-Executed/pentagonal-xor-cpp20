# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# CORE: new, explicitly experimental C++20 YIN-side /GL+PGO build.
# Not the established no-/GL YIN protocol. No WinAPI shadow is promotable.
[CmdletBinding()]
param(
  [int]$TrainingIterations=500000,
  [int]$TrainingWarmup=50000,
  [int]$TrainingRepeats=5,
  [string]$BuildRoot='build-core-yin-pgo',
  [string]$Evidence='evidence-core-yin-pgo'
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$PSNativeCommandUseErrorActionPreference=$false
function Guard([bool]$ok,[string]$message){
  if(!$ok){throw ('CORE_YIN: '+$message)}
}
function Log-Command([string]$out,[string]$cmd,[string[]]$arguments){
  & $cmd @arguments *> $out
  if($null -eq $LASTEXITCODE){return 99}
  return [int]$LASTEXITCODE
}
function Digest([string]$path){
  if(!(Test-Path -LiteralPath $path -PathType Leaf)){return $null}
  (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
}
Guard ($IsWindows -and $env:RUNNER_OS -ceq 'Windows') 'MSVC Windows runner required'
Guard ($TrainingIterations -ge 1000 -and $TrainingWarmup -ge 100 -and
  $TrainingRepeats -ge 3 -and $TrainingRepeats % 2 -eq 1) 'Invalid training corpus'
$sha=((& git rev-parse HEAD)|Out-String).Trim()
Guard ($LASTEXITCODE -eq 0 -and $sha -match '^[a-f0-9]{40}$') 'Git SHA missing'
Guard (!$env:GITHUB_SHA -or $sha -ceq $env:GITHUB_SHA) 'Git SHA mismatch'
New-Item -ItemType Directory -Path $BuildRoot,$Evidence -Force | Out-Null
$cpuAvx2=[System.Runtime.Intrinsics.X86.Avx2]::IsSupported
$requestedCompiler='/std:c++20 /O2 /Ob3 /GL /Gw /fp:fast /arch:AVX2 /Qpar /EHsc'
$compiler=$requestedCompiler+' /DNDEBUG'
$requestedLinker='/LTCG:PGOptimize /OPT:REF /OPT:ICF=5'
$commonLinker='/INCREMENTAL:NO /OPT:REF /OPT:ICF=5'
$trainingLinker='/LTCG /GENPROFILE '+$commonLinker
$exactLinker='/LTCG:PGOptimize '+$commonLinker
$supportedLinker='/LTCG /USEPROFILE '+$commonLinker
$record=[ordered]@{
  schema='SWE-LIGAZE-CORE-YIN-PGO-v1'
  name='CORE';layer='YIN exploratory new branch, explicitly allows GL'
  canonical_yin_no_gl_unchanged=$true;source_commit=$sha
  requested_compiler=$requestedCompiler;requested_linker=$requestedLinker
  effective_compiler=$compiler
  profile_generation_linker=$trainingLinker
  legacy_exact_linker_attempt=$exactLinker
  modern_supported_linker=$supportedLinker
  AVX2_host_supported=$cpuAvx2
  floating_point_fast_not_canonical=$true
  unsafe_full_header_prod_approved=$false
  production_promotion=$false;math_source_mutated=$false
  training_iterations=$TrainingIterations;training_warmup=$TrainingWarmup
  training_repeats=$TrainingRepeats
  variants=[ordered]@{}
}
function Save-Record{
  $file=Join-Path $Evidence 'core-yin-pgo.json'
  $record|ConvertTo-Json -Depth 18|Set-Content -LiteralPath $file -Encoding utf8
  Write-Host ('CORE_YIN|EVIDENCE|'+(Digest $file))
}
# Compile with AVX2 even on unsupported hosts, but never execute its binary.
if(!$cpuAvx2){
  $record.status='UNSUPPORTED_HOST_AVX2'
  Save-Record
  throw 'CORE_YIN: AVX2 not supported by host; cannot train or time this binary'
}
function Configure([string]$dir,[string]$variant,[string]$linker,[string]$log){
  $configureArgs=@('-S','.','-B',$dir,
    '-DBUILD_TESTING=ON',
    ('-DSWE_LIGAZE_WINDOWS_AB_VARIANT='+$variant),
    ('-DCMAKE_CXX_FLAGS_RELEASE:STRING='+$compiler),
    ('-DCMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='+$linker),
    '-DCMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE:BOOL=OFF')
  $rc=Log-Command $log 'cmake' $configureArgs
  Guard ($rc -eq 0) ("CMake configure rejected "+$variant+': '+$log)
  $cache=Get-Content -LiteralPath (Join-Path $dir 'CMakeCache.txt') -Raw
  Guard ($cache.Contains('CMAKE_CXX_FLAGS_RELEASE:STRING='+$compiler) -and
    $cache.Contains('CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='+$linker)) 'CMake did not preserve exact compiler/linker flags'
}
function Build-Target([string]$dir,[string]$target,[string]$log){
  Log-Command $log 'cmake' @('--build',$dir,'--config','Release',
    '--target',$target,'--parallel','2','--verbose')
}
function Measure([string]$exe){
  $result=@(& $exe ([string]$TrainingIterations) ([string]$TrainingWarmup) ([string]$TrainingRepeats))
  Guard ($LASTEXITCODE -eq 0 -and $result.Count -gt 0) 'CORE runtime rejected'
  $m=([string]$result[-1]|ConvertFrom-Json)
  Guard ($m.schema -ceq 'SWE-LIGAZE-BENCH-v1' -and $m.threat_fixtures -ceq 'PASS' -and
    $m.iterations -eq $TrainingIterations -and $m.repeats -eq $TrainingRepeats) 'CORE numerical witness invalid'
  $m
}
$variants=@('baseline','full_header_v2')
try {
  foreach($variant in $variants){
    $dir=Join-Path $BuildRoot $variant
    $v=[ordered]@{
      variant=$variant;status='STARTED';training_binary_sha256=$null
      trained_pgc_count=0;trained_pgd_count=0;profile_member_hashes=@()
      instrument_build_exit=$null;legacy_optimized_build_exit=$null
      modern_optimized_build_exit=$null;optimizer_used=$null
      strict_winapi_contract_rejected=$null
      optimized_binary_sha256=$null;trained_checksum=$null
      optimized_checksum=$null;optimized_median_ns=$null
    }
    $record.variants[$variant]=$v
    Write-Host ('CORE_YIN|INSTRUMENT|'+$variant)
    Configure $dir $variant $trainingLinker (Join-Path $Evidence ($variant+'-configure-instrument.log'))
    $v.instrument_build_exit=Build-Target $dir 'pentagonal_ligaze_benchmark' (Join-Path $Evidence ($variant+'-instrument-build.log'))
    Guard ($v.instrument_build_exit -eq 0) ('Instrumented build failed: '+$variant)
    $exe=[IO.Path]::GetFullPath((Join-Path $dir 'Release/pentagonal_ligaze_benchmark.exe'))
    Guard (Test-Path -LiteralPath $exe -PathType Leaf) ('Missing instrumented binary: '+$variant)
    $v.training_binary_sha256=Digest $exe
    $binaryDir=Split-Path -Parent $exe
    Push-Location -LiteralPath $binaryDir
    try {
      $measured=Measure $exe
    }finally{Pop-Location}
    $v.trained_checksum=[string]$measured.checksum
    $pgc=@(Get-ChildItem -LiteralPath $dir -Recurse -File -Filter '*.pgc')
    $pgd=@(Get-ChildItem -LiteralPath $dir -Recurse -File -Filter '*.pgd')
    $v.trained_pgc_count=$pgc.Count
    $v.trained_pgd_count=$pgd.Count
    $v.profile_member_hashes=@(($pgc+$pgd)|ForEach-Object {
      [ordered]@{file=$_.Name;sha256=(Digest $_.FullName)}
    })
    Guard ($pgc.Count -gt 0 -and $pgd.Count -gt 0) ('PGO training data not generated: '+$variant)
    Save-Record
    # Attempt exact requested legacy syntax, but never invent PGO data.
    Write-Host ('CORE_YIN|PGO_LEGACY_EXACT|'+$variant)
    Configure $dir $variant $exactLinker (Join-Path $Evidence ($variant+'-configure-legacy.log'))
    $v.legacy_optimized_build_exit=Build-Target $dir 'pentagonal_ligaze_benchmark' (Join-Path $Evidence ($variant+'-legacy-link.log'))
    if($v.legacy_optimized_build_exit -eq 0){
      $v.optimizer_used='legacy-requested-LTCG-PGOptimize'
    }else{
      Write-Host ('CORE_YIN|LEGACY_REJECTED|'+$variant+'|retry supported /LTCG /USEPROFILE')
      Configure $dir $variant $supportedLinker (Join-Path $Evidence ($variant+'-configure-supported.log'))
      $v.modern_optimized_build_exit=Build-Target $dir 'pentagonal_ligaze_benchmark' (Join-Path $Evidence ($variant+'-supported-link.log'))
      Guard ($v.modern_optimized_build_exit -eq 0) ('Supported PGO linking failed: '+$variant)
      $v.optimizer_used='modern-supported-LTCG-USEPROFILE'
    }
    Guard (Test-Path -LiteralPath $exe -PathType Leaf) ('Optimized binary missing: '+$variant)
    $v.optimized_binary_sha256=Digest $exe
    $optimized=Measure $exe
    $v.optimized_checksum=[string]$optimized.checksum
    $v.optimized_median_ns=[double]$optimized.median_ns_per_call
    Guard ($v.optimized_checksum -ceq $v.trained_checksum) ('PGO transformed COR checksum: '+$variant)
    if($variant -eq 'full_header_v2'){
      $v.contract_build_exit=Build-Target $dir 'pentagonal_platform_v2_contract_test' (Join-Path $Evidence ($variant+'-contract.log'))
      $contract=Get-Content -LiteralPath (Join-Path $Evidence ($variant+'-contract.log')) -Raw
      $v.strict_winapi_contract_rejected=($v.contract_build_exit -ne 0 -and $contract -match '(?i)(C2338|static.assert|ForgedSleepCall|ForgedCreateFileCall)')
      Guard $v.strict_winapi_contract_rejected 'Unsafe WinAPI contract unexpectedly passed or failed for unrelated reason'
    }
    $v.status='PGO_COMPILED_MEASURED_EXPERIMENTAL_NOT_RELEASE'
    Save-Record
  }
  $a=$record.variants.baseline
  $b=$record.variants.full_header_v2
  Guard ($a.optimized_checksum -ceq $b.optimized_checksum) 'Expanded header altered CORE numerical semantics'
  $javaDir=Join-Path $BuildRoot 'java'
  New-Item -ItemType Directory -Force -Path $javaDir|Out-Null
  & javac --release 17 -encoding UTF-8 -d $javaDir java/PentagonalControl.java java/LigazeJavaBenchmark.java
  Guard ($LASTEXITCODE -eq 0) 'CORE Java 17 source checksum reference failed to compile'
  $j=@(& java -cp $javaDir LigazeJavaBenchmark ([string]$TrainingIterations) ([string]$TrainingWarmup) ([string]$TrainingRepeats))
  Guard ($LASTEXITCODE -eq 0 -and $j.Count -gt 0) 'CORE Java reference failed'
  $java=([string]$j[-1]|ConvertFrom-Json)
  $record.java17_reference_checksum=[string]$java.checksum
  Guard ($a.optimized_checksum -ceq $record.java17_reference_checksum) 'CORE disagrees with original Java/native checksum'

  $record.header_over_baseline_ratio=([double]$b.optimized_median_ns/[double]$a.optimized_median_ns)
  $record.status='PASS_EXPERIMENTAL_STRICT_WINAPI_REJECTION_RETAINED'
}finally{Save-Record}
Write-Host ('CORE_YIN|COMPLETE|'+$record.status)
