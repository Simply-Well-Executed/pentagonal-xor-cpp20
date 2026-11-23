# SPDX-License-Identifier: 0BSD
# TRIFORCE SigmaSigmaSigma: deterministic HEAD and TAIL for a named stage.
# Independent BigInteger oracle plus native TOOLONG executable when available.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$RepositoryRoot,
    [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
    [Parameter(Mandatory=$true)]
    [ValidatePattern('^[A-Za-z0-9_-]+$')][string]$Operation,
    [Parameter(Mandatory=$true)][ValidateSet('HEAD','TAIL')][string]$Phase,
    [string]$NativeExecutable = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
if(-not [IO.Directory]::Exists($root)){throw 'TRIFORCE|HOLD|source_missing'}
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $evidence
if($LASTEXITCODE -ne 0){throw 'TRIFORCE|HOLD|unsafe_evidence_path'}
if($evidence.StartsWith($root+[IO.Path]::DirectorySeparatorChar,
       [StringComparison]::OrdinalIgnoreCase)){
    throw 'TRIFORCE|HOLD|evidence_must_be_outside_source'
}
$required=@(
 'shared/toolong_q62_core.hpp',
 'shared/slidingtoolong.hpp',
 'shared/toolong_q62.hpp',
 'shared/toolong.hpp',
 'shared/toolong_bits.hpp',
 'shared/euler_pi_inverse_square.hpp',
 'shared/sigma_form_geometry.hpp',
 'shared/sigma_form_area_q62.hpp',
 'shared/right_triangle_f123_original.hpp',
 'shared/f123_eighth_fold_experiment.hpp',
 'shared/sigma_e_redaxualchoice.hpp',
 'shared/sigma_f_redaxualchoice.hpp',
 'shared/four_pyramid_reduxualchoice.hpp',
 'shared/u_circle_fit.hpp',
 'shared/xdr_rdx_fold_tork.hpp',
 'shared/minza_direct_ping_bidi.hpp',
 'shared/minza_direct_ping_tridi.hpp',
 'shared/three_geometric_folds.hpp',
 'shared/rectangloid_primitive_cxx20.hpp',
 'diaery/diaery.cpp',
 'diaery/model_bridge.hpp',
 'diaery/finite_head_to_head.hpp',
 'diaery/head_to_head.cpp',
 'diaery/Invoke-FiniteQualification.ps1',
 'diaery/unbound_head_to_head.cpp',
 'diaery/Invoke-UnboundQualification.ps1',
 'diaery/strict_profile.hpp',
 'diaery/parity_gate.hpp',
 'diaery/harmony.hpp',
 'diaery/fusion_watchdog.hpp',
 'diaery/fusion_qualification.hpp',
 'diaery/fusion_generator.hpp',
 'diaery/index.xhtml',
 'diaery/bad_harmony_missing.xhtml',
 'diaery/bad_harmony_mutated.xhtml',
 'diaery/bad_harmony_order.xhtml',
 'diaery/bad_doctype.xhtml',
 'diaery/bad_parity.xhtml',
 'diaery/bad_script.xhtml',
 'diaery/bad_stage.xhtml',
 'diaery/Invoke-ParityGate.ps1',
 'diaery/CMakeLists.txt',
 'diaery/README.md',
 'diaery/ERRATA.md',
 '.github/workflows/diaery-parity.yml',
 'shared/gammagamma.hpp',
 'tests/triforce_native_gate_test.cpp',
 'tests/toolong_arithmetic_test.cpp',
 'tests/gammagamma_pi_precision_test.cpp',
 'ci/triforce/CMakeLists.txt',
 'ci/Assert-TriforceHeadTail.ps1',
 'ci/Invoke-TriforcePipeline.ps1',
 'ci/Invoke-JstarProcessAudit.ps1',
 'ci/Assert-ExternalEvidencePath.ps1',
 'ci/Test-ExternalEvidenceBoundary.ps1',
 'ci/Invoke-AtomixHalfPhiping.ps1',
 'ci/Invoke-FullSourceAutostartNativeSuite.ps1',
 '.github/workflows/triforce-headtail.yml',
 '.github/workflows/magiccomb-headtail-oracle27-dotnet11-ps77.yml',
 '.github/workflows/openbsd-serial-coreao.yml',
 '.github/workflows/android-linux-no-python-source.yml',
 '.github/workflows/reelnative-fullsource-autostart.yml',
 'CMakeLists.txt',
 'shared/CMakeLists.txt',
 'tests/consumer/CMakeLists.txt',
 'docs/TOOLONG_TRIFORCE_Q62.md',
 'docs/SIGMA_FORM_GEOMETRY.md',
 'docs/F123_ORIGINAL_BASELINE.md',
 'docs/F123_EIGHTH_FOLD_EXPERIMENT.md',
 'docs/SIGMA_E_REDAXUALCHOICE.md',
 'docs/SIGMA_F_REDAXUALCHOICE.md',
 'docs/FOUR_PYRAMID_REDUXUALCHOICE.md',
 'docs/U_CIRCLE_FIT.md',
 'docs/XDR_RDX_FOLD_TORK.md',
 'docs/MINZA_DIRECT_PING_BIDI.md',
 'docs/MINZA_DIRECT_PING_TRIDI.md',
 'docs/THREE_GEOMETRIC_FOLDS.md',
 'docs/RECTANGLOID_PRIMITIVE_DIARY.md',
 'docs/ATOMIX_PR34_BACKPORT.md'
)
$header=Join-Path $root 'shared/toolong_q62_core.hpp'
if(-not [IO.File]::Exists($header)){throw 'TRIFORCE|HOLD|canonical_source_missing'}
$source=[IO.File]::ReadAllText($header,[Text.Encoding]::UTF8)
$records=@(
 [pscustomobject]@{symbol='euler';label='Euler';word='12535862302449814170';expected='271828182845904523'},
 [pscustomobject]@{symbol='pi';label='Pi';word='14488038916154245685';expected='314159265358979323'},
 [pscustomobject]@{symbol='intensity';label='I';word='315868046760674477';expected='6849296450333523'}
)
$factor=[System.Numerics.BigInteger]::Parse('100000000000000000')
$divisor=[System.Numerics.BigInteger]::Pow([System.Numerics.BigInteger]::Parse('2'),62)
foreach($r in $records) {
    $pattern='inline\s+constexpr\s+Record\s+'+$r.symbol+'\s*\{\s*"'+
             $r.label+'"\s*,\s*'+$r.word+'ULL\s*,\s*'+$r.expected+'ULL\s*\}'
    if(-not [regex]::IsMatch($source,$pattern)){
        throw ('TRIFORCE|HOLD|immutable_record_changed|'+$r.label)
    }
    $product=[System.Numerics.BigInteger]::Multiply(
        [System.Numerics.BigInteger]::Parse($r.word),$factor)
    $actual=[System.Numerics.BigInteger]::Divide($product,$divisor)
    $remainder=[System.Numerics.BigInteger]::Remainder($product,$divisor)
    if($actual.ToString() -cne $r.expected -or $remainder.Sign -lt 0 -or
       [System.Numerics.BigInteger]::Remainder($actual,
           [System.Numerics.BigInteger]::Parse('100')).ToString() -cne '23'){
        throw ('TRIFORCE|HOLD|Q62_or_suffix23_mismatch|'+$r.label)
    }
}
if(-not $source.Contains('static_assert(sigma_sigma_sigma()') -or
   -not $source.Contains('namespace circle_of_sanity') -or
   -not $source.Contains('static_assert(centrix.all_consistent') -or
   -not $source.Contains('static_assert(!centrix.release_authorized)') -or
   -not $source.Contains('externally_authenticated_approval = false')){
    throw 'TRIFORCE|HOLD|constexpr_or_unapproved_human_gate_missing'
}
# Fail closed for every source file, not just the named critical headers.
# Exact GitHub source ZIP has no .git directory. For local checkouts, .git
# metadata is excluded because it is not part of the authored source tree.
foreach($relative in $required){
    $file=Join-Path $root $relative
    if(-not [IO.File]::Exists($file)){
        throw ('TRIFORCE|HOLD|missing_file|'+$relative)
    }
}
$paths=[Collections.Generic.List[string]]::new()
foreach($item in Get-ChildItem -LiteralPath $root -Force -Recurse -File){
    $relative=[IO.Path]::GetRelativePath($root,$item.FullName).Replace([string][IO.Path]::DirectorySeparatorChar,'/')
    if($relative.StartsWith('.git/',[StringComparison]::OrdinalIgnoreCase)){
        continue
    }
    if(($item.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0){
        throw ('TRIFORCE|HOLD|source_reparse_point|'+$relative)
    }
    $paths.Add($relative)
}
$paths.Sort([StringComparer]::Ordinal)
if($paths.Count -lt $required.Count){
    throw 'TRIFORCE|HOLD|source_tree_incomplete'
}
$hashLines=[Collections.Generic.List[string]]::new()
foreach($relative in $paths){
    $file=Join-Path $root ($relative.Replace('/',[string][IO.Path]::DirectorySeparatorChar))
    $digest=(Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant()
    [void]$hashLines.Add($relative+[char]9+$digest)
}
$sourceLedger=[string]::Join("`n",$hashLines)
if(-not [IO.Directory]::Exists($evidence)){
    if($Phase -cne 'HEAD'){throw 'TRIFORCE|HOLD|TAIL_without_HEAD'}
    [void][IO.Directory]::CreateDirectory($evidence)
}
$statePath=Join-Path $evidence 'triforce-state.json'
if([IO.File]::Exists($statePath)){
    $state=Get-Content -LiteralPath $statePath -Raw | ConvertFrom-Json -AsHashtable
    if($state.ledger -cne $sourceLedger){
        throw 'TRIFORCE|HOLD|source_HEAD_TAIL_SHA256_drift'
    }
    if($env:GITHUB_SHA -and $state.commit -and
       $state.commit -cne $env:GITHUB_SHA) {
        throw 'TRIFORCE|HOLD|source_commit_changed'
    }
}else{
    if($Phase -cne 'HEAD'){throw 'TRIFORCE|HOLD|missing_first_HEAD'}
    $state=@{
       schema='SWE-TRIFORCE-HEADTAIL-v1'
       ledger=$sourceLedger
       commit=[string]$env:GITHUB_SHA
       last=''
       phase='TAIL'
       completed=0
    }
}
if($Phase -cne 'HEAD'){
    if($state.phase -cne 'HEAD' -or $state.last -cne $Operation){
        throw 'TRIFORCE|HOLD|TAIL_out_of_order_or_unpaired'
    }
}else{
    if($state.phase -cne 'TAIL'){
        throw 'TRIFORCE|HOLD|HEAD_before_previous_TAIL'
    }
}
if($NativeExecutable -ne ''){
    $executable=[IO.Path]::GetFullPath($NativeExecutable)
    if(-not [IO.File]::Exists($executable)){
        throw 'TRIFORCE|HOLD|missing_native_TOOLONG_test'
    }
    & $executable
    if($LASTEXITCODE -ne 0){
        throw ('TRIFORCE|HOLD|native_TOOLONG_gate_failed|exit='+$LASTEXITCODE)
    }
}
if($Phase -ceq 'HEAD') {
    $state.phase='HEAD'
    $state.last=$Operation
}else{
    $state.phase='TAIL'
    $state.completed=[int]$state.completed+1
}
$tmp=$statePath+'.tmp'
[IO.File]::WriteAllText($tmp,($state | ConvertTo-Json -Depth 5),
                        [Text.UTF8Encoding]::new($false))
[IO.File]::Move($tmp,$statePath,$true)
Write-Host ('TRIFORCE|'+$Phase+'|OP='+$Operation+
   '|EULER=271828182845904523|PI=314159265358979323|I=6849296450333523'+
   '|SIGMAS=3|SUFFIX=23|SOURCE_SHA256_STABLE|RELEASE=HOLD')
