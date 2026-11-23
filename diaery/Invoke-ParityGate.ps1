# SPDX-License-Identifier: 0BSD
# DIAERY: native MSVC C++20 /O2 and 1:1 XHTML Strict-profile concurrence.
# No project-authored Node, Python, npm, network requests or container use.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [string]$EvidenceDirectory = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
if(!$EvidenceDirectory){$EvidenceDirectory=Join-Path $env:RUNNER_TEMP 'diaery-evidence'}
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
if($evidence.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
  throw 'DIAERY|HOLD|evidence_must_be_outside_source'
}
[IO.Directory]::CreateDirectory($evidence) | Out-Null
if(-not $IsWindows) { throw 'DIAERY|HOLD|Windows_native_required' }
$files=@(
 'diaery/diaery.cpp','diaery/model_bridge.hpp',
 'diaery/strict_profile.hpp','diaery/parity_gate.hpp',
 'diaery/fusion_watchdog.hpp',
 'diaery/harmony.hpp','shared/slidingtoolong.hpp',
 'diaery/bad_harmony_missing.xhtml',
 'diaery/bad_harmony_mutated.xhtml',
 'diaery/bad_harmony_order.xhtml',
 'diaery/fusion_qualification.hpp','diaery/fusion_generator.hpp',
 'diaery/Invoke-FusionQualification.ps1',
 'diaery/index.xhtml','diaery/bad_watchdog_missing.xhtml',
 'diaery/bad_watchdog_mutated.xhtml',
 'diaery/bad_watchdog_duplicate.xhtml',
 'diaery/bad_watchdog_order.xhtml',
 'diaery/bad_doctype.xhtml',
 'diaery/bad_parity.xhtml','diaery/bad_script.xhtml',
 'diaery/bad_stage.xhtml',
 'diaery/CMakeLists.txt','diaery/README.md',
 'diaery/Invoke-ParityGate.ps1',
 '.github/workflows/diaery-parity.yml',
 'shared/sigma_form_geometry.hpp','shared/sigma_form_area_q62.hpp',
 'shared/right_triangle_f123_original.hpp',
 'shared/f123_eighth_fold_experiment.hpp',
 'shared/sigma_e_redaxualchoice.hpp',
 'shared/sigma_f_redaxualchoice.hpp',
 'shared/four_pyramid_reduxualchoice.hpp',
 'shared/u_circle_fit.hpp','shared/xdr_rdx_fold_tork.hpp',
 'shared/minza_direct_ping_bidi.hpp',
 'shared/minza_direct_ping_tridi.hpp',
 'shared/three_geometric_folds.hpp',
 'shared/rectangloid_primitive_cxx20.hpp'
)
function Get-SourceDigest {
  $entries=[Collections.Generic.List[string]]::new()
  foreach($path in $files){
    $full=Join-Path $root $path
    if(-not [IO.File]::Exists($full)){throw "DIAERY|HOLD|source_missing|$path"}
    $sha=(Get-FileHash -LiteralPath $full -Algorithm SHA256).Hash.ToLowerInvariant()
    $entries.Add($path+'|'+$sha)
  }
  $joined=[string]::Join("`n",$entries)
  return [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
      [Text.Encoding]::UTF8.GetBytes($joined))).ToLowerInvariant()
}
$head=Get-SourceDigest
Write-Host "DIAERY|HEAD|SHA256=$head"
# Use the installed native Microsoft toolchain; do not download dependencies.
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'DIAERY|HOLD|vswhere_missing'}
$installation=@(& $vswhere -latest -products '*' -requires `
 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($installation.Count -ne 1 -or !$installation[0]){
 throw 'DIAERY|HOLD|native_MSVC_unavailable'
}
$launch=Join-Path $installation[0] 'Common7/Tools/Launch-VsDevShell.ps1'
if(-not [IO.File]::Exists($launch)){throw 'DIAERY|HOLD|DevShell_missing'}
& $launch -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$compiler=Get-Command cl.exe -ErrorAction Stop
if(-not $compiler.Source.EndsWith('cl.exe',[StringComparison]::OrdinalIgnoreCase)){
 throw 'DIAERY|HOLD|native_compiler_invalid'
}
$exe=Join-Path $evidence 'diaery-native.exe'
$src=Join-Path $root 'diaery/diaery.cpp'
& $compiler.Source /nologo /std:c++20 /O2 /W4 /permissive- /EHsc `
  "/Fe:$exe" $src user32.lib gdi32.lib kernel32.lib
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($exe)){
 throw 'DIAERY|HOLD|native_CXX20_compile_failed'
}
$positive=Join-Path $root 'diaery/index.xhtml'
& $exe --check $positive
if($LASTEXITCODE -ne 0){throw 'DIAERY|HOLD|1to1_parity_failed'}
foreach($bad in @('bad_doctype.xhtml','bad_parity.xhtml','bad_script.xhtml','bad_stage.xhtml')){
  & $exe --reject (Join-Path $root ('diaery/'+$bad))
  if($LASTEXITCODE -ne 0){throw "DIAERY|HOLD|negative_gate_failed|$bad"}
}
$tail=Get-SourceDigest
Write-Host "DIAERY|TAIL|SHA256=$tail"
if($head -cne $tail){throw 'DIAERY|HOLD|HEAD_TAIL_mismatch'}
Write-Host 'DIAERY|1:1-PARITY-GATE=PASS|CONCURRENCE=PASS|RELEASE=HOLD'
