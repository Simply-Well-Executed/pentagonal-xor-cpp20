# SPDX-License-Identifier: 0BSD
# SILVERHAND/1: sequential union of independent native diagnostics.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$RepositoryRoot,
    [Parameter(Mandatory=$true)][string]$BuildDirectory,
    [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'SILVERHAND|HOLD|native_Windows_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$build=[IO.Path]::GetFullPath($BuildDirectory)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
foreach($path in @($build,$out)){
    & pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $path
    if($LASTEXITCODE -ne 0){throw 'SILVERHAND|HOLD|invalid_evidence_boundary'}
}
$receipt=Join-Path $out 'SILVERHAND.json'
if([IO.File]::Exists($receipt)){throw 'SILVERHAND|HOLD|receipt_already_exists'}
$ctest=Join-Path $env:ProgramFiles 'CMake/bin/ctest.exe'
if(-not [IO.File]::Exists($ctest)){throw 'SILVERHAND|HOLD|canonical_ctest_missing'}
$auditPath=Join-Path $out 'jstar-process/build_toolong-process-audit.json'
if(-not [IO.File]::Exists($auditPath)){throw 'SILVERHAND|HOLD|missing_JSTAR_build_receipt'}
$audit=Get-Content -LiteralPath $auditPath -Raw | ConvertFrom-Json
if($audit.operation -cne 'build_toolong' -or $audit.exit_code -ne 0 -or
   $audit.process_integrity_status -cne 'SAMPLED_PASS' -or
   $audit.source_commit -cne [string]$env:GITHUB_SHA -or
   $audit.msbuild_max_workers -ne 1 -or
   $audit.command_arguments -cnotcontains 'SILVERHAND' -or
   $audit.command_arguments -cnotcontains $build){
    throw 'SILVERHAND|HOLD|JSTAR_build_not_qualified'
}
$names=@(
    'toolong_exact_arithmetic',
    'gammagamma_pi_precision_boundaries',
    'lcd_256_cycles_exact_quotient',
    'redux_exact_reduction',
    'redux_standalone_exact_reduction',
    'redux_source_arithmetic_equivalence',
    'ariari_fusion_exact_roundtrip',
    'ariari_standalone_exact_roundtrip',
    'ariari_toolong_gammagamma_composition',
    'namespace_free_composition_65025',
    'dakatana_physics_exact_batch',
    'talon_256_cycles_terminal_velocity'
)
$registryText=(& $ctest --test-dir $build -C Release --show-only=json-v1)
if($LASTEXITCODE -ne 0){throw 'SILVERHAND|HOLD|CTest_inventory_failed'}
$registry=($registryText -join [Environment]::NewLine)|ConvertFrom-Json
$actual=@($registry.tests|ForEach-Object{[string]$_.name})
if($actual.Count -ne $names.Count){throw 'SILVERHAND|HOLD|test_inventory_count'}
foreach($name in $names){
    if(@($actual|Where-Object{$_ -ceq $name}).Count -ne 1){
        throw ('SILVERHAND|HOLD|missing_or_duplicate_test|'+$name)
    }
}
$rows=[Collections.Generic.List[object]]::new()
$timer=[Diagnostics.Stopwatch]::StartNew()
$failed=0
Write-Host ('SILVERHAND|UNION|TASKS='+$names.Count+'|PARALLEL=1|RELEASE=HOLD')
foreach($name in $names){
    $lap=[Diagnostics.Stopwatch]::StartNew()
    & $ctest --test-dir $build -C Release --parallel 1 --timeout 300 --output-on-failure --no-tests=error -R ('^'+[regex]::Escape($name)+'$')
    $code=$LASTEXITCODE
    $lap.Stop()
    $status=if($code -eq 0){'PASS'}else{'HOLD'}
    if($code -ne 0){++$failed}
    $rows.Add([ordered]@{test=$name;exit_code=$code;status=$status;elapsed_ms=$lap.ElapsedMilliseconds})
    Write-Host ('SILVERHAND|'+$name+'|'+$status+'|EXIT='+$code+'|MS='+$lap.ElapsedMilliseconds)
}
$timer.Stop()
$summary=[ordered]@{
    schema='SILVERHAND-UNION-1'
    commit=[string]$env:GITHUB_SHA
    source_script_sha256=(Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash.ToLowerInvariant()
    jstar_sha256=(Get-FileHash -LiteralPath $auditPath -Algorithm SHA256).Hash.ToLowerInvariant()
    parallel=1
    expected=$names.Count
    completed=$rows.Count
    failed=$failed
    elapsed_ms=$timer.ElapsedMilliseconds
    tests=@($rows.ToArray())
    result=if($failed -eq 0 -and $rows.Count -eq $names.Count){'PASS'}else{'HOLD'}
    release='HOLD'
}
$bytes=[Text.UTF8Encoding]::new($false).GetBytes(($summary|ConvertTo-Json -Depth 9 -WarningAction Stop))
$stream=[IO.File]::Open($receipt,[IO.FileMode]::CreateNew,[IO.FileAccess]::Write,[IO.FileShare]::None)
try{$stream.Write($bytes,0,$bytes.Length)}finally{$stream.Dispose()}
$hash=(Get-FileHash -LiteralPath $receipt -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('SILVERHAND|'+$summary.result+'|COMPLETED='+$rows.Count+'|FAILED='+$failed+'|MS='+$timer.ElapsedMilliseconds+'|SHA256='+$hash+'|RELEASE=HOLD')
Write-Host 'SILVERHAND|RECEIPT_JSON_BEGIN'
Write-Host ([IO.File]::ReadAllText($receipt))
Write-Host 'SILVERHAND|RECEIPT_JSON_END'
if($summary.result -cne 'PASS'){throw 'SILVERHAND|HOLD|one_or_more_native_tests_failed'}
