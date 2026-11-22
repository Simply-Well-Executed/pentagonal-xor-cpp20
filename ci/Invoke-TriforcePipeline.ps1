# SPDX-License-Identifier: 0BSD
# Serial MSVC-only stages; every controlled operation has independent HEAD and TAIL.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$RepositoryRoot,
    [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'TRIFORCE|HOLD|MSVC_requires_native_Windows'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
$gateScript=Join-Path $root 'ci/Assert-TriforceHeadTail.ps1'
$nativeBuild=Join-Path $evidence 'native'
$rootBuild=Join-Path $evidence 'root'
$nativeExe=Join-Path $nativeBuild 'Release/triforce_native_gate.exe'
function Invoke-Gate([string]$op,[string]$phase) {
    $args=@('-NoLogo','-NoProfile','-NonInteractive','-File',$gateScript,
            '-RepositoryRoot',$root,'-EvidenceDirectory',$evidence,
            '-Operation',$op,'-Phase',$phase)
    if([IO.File]::Exists($nativeExe)){
        $args+=@('-NativeExecutable',$nativeExe)
    }elseif($op -notin @('configure-native','build-native')){
        throw ('TRIFORCE|HOLD|native_test_not_built_before_stage|'+$op)
    }
    & pwsh @args
    if($LASTEXITCODE -ne 0){throw ('TRIFORCE|HOLD|'+$op+'|'+$phase)}
}
function Invoke-Stage([string]$name,[scriptblock]$operation) {
    Invoke-Gate $name 'HEAD'
    $global:LASTEXITCODE=0
    & $operation
    if($LASTEXITCODE -ne 0){
        throw ('TRIFORCE|HOLD|stage_command_failed|'+$name+
               '|exit='+$LASTEXITCODE)
    }
    Invoke-Gate $name 'TAIL'
}
# The exact commit archive is already fetched by the workflow bootstrap.
# No stage may proceed unless the independently recomputed owner records pass.
Invoke-Stage 'configure-native' {
    & cmake -S (Join-Path $root 'ci/triforce') -B $nativeBuild `
      -G 'Visual Studio 18 2026' -A x64
}
Invoke-Stage 'build-native' {
    & cmake --build $nativeBuild --config Release --target triforce_native_gate --parallel 1
}
Invoke-Stage 'execute-native' {
    & $nativeExe
}
Invoke-Stage 'configure-root' {
    & cmake -S $root -B $rootBuild -G 'Visual Studio 18 2026' -A x64 `
      -DBUILD_TESTING=ON -DSWE_STD_FREE_CORE_ONLY=OFF
}
Invoke-Stage 'build-toolong' {
    & cmake --build $rootBuild --config Release --parallel 1 `
      --target toolong_arithmetic_test gammagamma_pi_precision_test
}
Invoke-Stage 'ctest-toolong' {
    & ctest --test-dir $rootBuild -C Release `
      -R '^(toolong_exact_arithmetic|gammagamma_pi_precision_boundaries)$' `
      --output-on-failure --no-tests=error
}
Invoke-Stage 'release-hold' {
    $source=[IO.File]::ReadAllText(
        (Join-Path $root 'shared/toolong_q62_core.hpp'))
    if(-not $source.Contains('externally_authenticated_approval = false')){
        throw 'TRIFORCE|HOLD|human_approval_must_remain_explicit'
    }
    Write-Host 'TRIFORCE|HUMAN_CONSENT=NOT_AUTHENTICATED|RELEASE=HOLD'
}
$state=Get-Content -LiteralPath (Join-Path $evidence 'triforce-state.json') -Raw |
       ConvertFrom-Json
if($state.phase -cne 'TAIL' -or $state.completed -ne 7){
    throw 'TRIFORCE|HOLD|unpaired_or_incomplete_HEAD_TAIL'
}
Write-Host 'TRIFORCE|ΣΣΣ|HEAD_TAIL_PASSED|7_STAGES|MSVC_CXX20_O2|RELEASE=HOLD'
