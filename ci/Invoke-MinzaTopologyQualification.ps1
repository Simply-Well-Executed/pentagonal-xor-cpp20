# SPDX-License-Identifier: 0BSD
# Compile and execute the scoped nine-face topology and byte-lane witnesses.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'MINZA_TOPOLOGY|HOLD|native_windows_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $evidence
if($LASTEXITCODE -ne 0){throw 'MINZA_TOPOLOGY|HOLD|unsafe_evidence_path'}
if([IO.Directory]::Exists($evidence)){throw 'MINZA_TOPOLOGY|HOLD|evidence_must_be_fresh'}
$sourceNames=@(
  'shared/minza_topology_incidence.hpp',
  'shared/u_circle_fit.hpp',
  'shared/xdr_rdx_fold_tork.hpp',
  'shared/minza_direct_ping_bidi.hpp',
  'shared/minza_direct_ping_tridi.hpp',
  'shared/three_geometric_folds.hpp',
  'shared/sigma_penta_ping.hpp',
  'shared/minza_ninza_ping_unfold.hpp',
  'tests/minza_topology_design_test.cpp',
  'tests/minza_byte_pipeline_test.cpp'
)
$sources=@($sourceNames | ForEach-Object {Join-Path $root $_})
foreach($path in $sources){if(-not [IO.File]::Exists($path)){throw ('MINZA_TOPOLOGY|HOLD|missing_source|'+$path)}}
$before=@($sources | ForEach-Object {(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash.ToLowerInvariant()})
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'MINZA_TOPOLOGY|HOLD|vswhere_missing'}
$install=@(& $vswhere -latest -products '*' -requires 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($install.Count -ne 1 -or -not $install[0]){throw 'MINZA_TOPOLOGY|HOLD|MSVC_missing'}
& (Join-Path $install[0] 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$compiler=(Get-Command cl.exe -ErrorAction Stop).Source
[void][IO.Directory]::CreateDirectory($evidence)
Push-Location -LiteralPath $evidence
try {
  foreach($name in @('minza_topology_design_test','minza_byte_pipeline_test')){
    $cpp=Join-Path $root ('tests/'+$name+'.cpp')
    $exe=Join-Path $evidence ($name+'.exe')
    $obj=Join-Path $evidence ($name+'.obj')
    & $compiler /nologo /std:c++20 /O2 /W4 /permissive- /EHsc /constexpr:steps2000000 ('/Fo'+$obj) ('/Fe'+$exe) $cpp
    if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($exe)){throw ('MINZA_TOPOLOGY|HOLD|compile_failed|'+$name)}
    & $exe
    if($LASTEXITCODE -ne 0){throw ('MINZA_TOPOLOGY|HOLD|runtime_failed|'+$name+'|'+$LASTEXITCODE)}
    Write-Host ('MINZA_TOPOLOGY|'+$name+'|PASS|CXX20|O2')
  }
} finally {
  Pop-Location
}
$after=@($sources | ForEach-Object {(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash.ToLowerInvariant()})
for($i=0;$i -lt $sources.Count;$i++){
  if($before[$i] -cne $after[$i]){throw ('MINZA_TOPOLOGY|HOLD|source_changed|'+$sourceNames[$i])}
  Write-Host ('MINZA_TOPOLOGY|SOURCE_SHA256|'+$sourceNames[$i]+'|'+$after[$i])
}
Write-Host 'MINZA_TOPOLOGY|NATIVE_TESTS=PASS|SOURCE_STABLE|RELEASE=HOLD'
