# SPDX-License-Identifier: 0BSD
# Serial MSVC-only stages; every controlled operation has independent HEAD and TAIL.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$RepositoryRoot,
    [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'TRIFORCE|HOLD|MSVC_requires_native_Windows'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
$gateScript=Join-Path $root 'ci/Assert-TriforceHeadTail.ps1'
$nativeBuild=Join-Path $evidence 'native'
$rootBuild=Join-Path $evidence 'precision'
$script:nativeProvenancePassed=$false
$nativeExe=Join-Path $nativeBuild 'Release/triforce_native_gate.exe'
$processAuditScript=Join-Path $root 'ci/Invoke-JstarProcessAudit.ps1'
$processEvidence=Join-Path $evidence 'jstar-process'
$env:MSBUILDDISABLENODEREUSE='1'
$env:DOTNET_CLI_DO_NOT_USE_MSBUILD_SERVER='1'
function Invoke-Gate([string]$op,[string]$phase) {
    $args=@('-NoLogo','-NoProfile','-NonInteractive','-File',$gateScript,
            '-RepositoryRoot',$root,'-EvidenceDirectory',$evidence,
            '-Operation',$op,'-Phase',$phase)
    if($script:nativeProvenancePassed -and [IO.File]::Exists($nativeExe)){
        $args+=@('-NativeExecutable',$nativeExe)
    }elseif($op -notin @('configure-native','build-native','guard-external-evidence','atomix-half-phiping')){
        throw ('TRIFORCE|HOLD|native_test_not_built_before_stage|'+$op)
    }
    & pwsh @args
    if($LASTEXITCODE -ne 0){throw ('TRIFORCE|HOLD|'+$op+'|'+$phase)}
}
function Invoke-Stage([string]$name,[scriptblock]$operation) {
    Invoke-Gate $name 'HEAD'
    try {
        $global:LASTEXITCODE=0
        & $operation
        if($LASTEXITCODE -ne 0){
            throw ('TRIFORCE|HOLD|stage_command_failed|'+$name+
                   '|exit='+$LASTEXITCODE)
        }
    } finally {
        # Preserve a paired TAIL even when the compiler reports a failure.
        # Failed native compilation remains a HOLD, never a bypass.
        Invoke-Gate $name 'TAIL'
    }
}
# The exact commit archive is already fetched by the workflow bootstrap.
# No stage may proceed unless the independently recomputed owner records pass.
Invoke-Stage 'configure-native' {
    & cmake -S (Join-Path $root 'ci/triforce') -B $nativeBuild `
      -G 'Visual Studio 18 2026' -A x64 -DSWE_TRIFORCE_LANE=CORE
}
# Backported from bookstore PR #1 and fixed after upstream PR #79 findings.
# These stages are bracketed by the original authoritative TRIFORCE gate.
Invoke-Stage 'guard-external-evidence' {
    & pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Test-ExternalEvidenceBoundary.ps1') -RepositoryRoot $root
}
Invoke-Stage 'atomix-half-phiping' {
    $atomixDir=Join-Path $evidence ('atomix-'+[Guid]::NewGuid().ToString('N'))
    & pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Invoke-AtomixHalfPhiping.ps1') -RepositoryRoot $root -EvidenceDirectory $atomixDir
}
Invoke-Stage 'build-native' {
    & $processAuditScript -RepositoryRoot $root -BuildDirectory $nativeBuild `
      -EvidenceDirectory $processEvidence -Operation 'build_native' `
      -Targets @('triforce_native_gate')
    # A build exit alone is insufficient; JSTAR must return successfully.
    $script:nativeProvenancePassed=$true
}
Invoke-Stage 'execute-native' {
    & $nativeExe
}

Invoke-Stage 'configure-precision' {
    & cmake -S (Join-Path $root 'ci/triforce') -B $rootBuild `
      -G 'Visual Studio 18 2026' -A x64 -DSWE_TRIFORCE_LANE=PRECISION
}
Invoke-Stage 'build-toolong' {
    & $processAuditScript -RepositoryRoot $root -BuildDirectory $rootBuild `
      -EvidenceDirectory $processEvidence -Operation 'build_toolong' `
      -Targets @('toolong_arithmetic_test','gammagamma_pi_precision_test')
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
if($state.phase -cne 'TAIL' -or $state.completed -ne 9){
    throw 'TRIFORCE|HOLD|unpaired_or_incomplete_HEAD_TAIL'
}
Write-Host 'TRIFORCE|ΣΣΣ|HEAD_TAIL_PASSED|9_STAGES|MSVC_CXX20_O2|RELEASE=HOLD'
