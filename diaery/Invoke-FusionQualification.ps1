# SPDX-License-Identifier: 0BSD
# DIAERY FUSION QUALIFICATION: FIFO==LAFIT, seven operators, native C++20.
# Output and temporary evidence are OUTSIDE the checkout, not a repo rewrite.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'FUSION|HOLD|native_Microsoft_CXX20_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$base=[IO.Path]::GetFullPath($EvidenceDirectory)
if($base.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'FUSION|HOLD|evidence_inside_repo'
}
[IO.Directory]::CreateDirectory($base) | Out-Null
$native=Join-Path $base 'diaery-native.exe'
$source=Join-Path $root 'diaery/index.xhtml'
if(-not [IO.File]::Exists($native)){throw 'FUSION|HOLD|native_source_uncompiled'}
if(-not [IO.File]::Exists($source)){throw 'FUSION|HOLD|missing_XHTML'}
$originHash=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
& $native --qualify $source
if($LASTEXITCODE -ne 0){throw 'FUSION|HOLD|seven_operator_qualification'}
$first=Join-Path $base 'fusion-first.cpp'
$second=Join-Path $base 'fusion-regenerated.cpp'
$recovered=Join-Path $base 'fusion-recovered.xhtml'
$fused=Join-Path $base 'fusion-generated.exe'
foreach($path in @($first,$second,$recovered,$fused)){
 if([IO.File]::Exists($path)){throw 'FUSION|HOLD|no_implicit_overwrite'}
}
& $native --fusion $source $first
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($first)){
 throw 'FUSION|HOLD|CXX20_generation_failed'
}
# Exact native MSVC toolchain, no project-authored Node/Python/GNU compiler.
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'FUSION|HOLD|vswhere_missing'}
$install=@(& $vswhere -latest -products '*' -requires `
 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($install.Count -ne 1 -or !$install[0]){throw 'FUSION|HOLD|MSVC_missing'}
$launch=Join-Path $install[0] 'Common7/Tools/Launch-VsDevShell.ps1'
& $launch -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=Get-Command cl.exe -ErrorAction Stop
& $cl.Source /nologo /std:c++20 /O2 /W4 /permissive- /EHsc `
  "/Fe:$fused" $first kernel32.lib
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($fused)){
 throw 'FUSION|HOLD|generated_CXX20_compile_failed'
}
# Binary-safe recovery: NEVER treat the XHTML stream as PowerShell lines.
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=$fused
$info.UseShellExecute=$false
$info.RedirectStandardOutput=$true
$info.RedirectStandardError=$true
$p=[Diagnostics.Process]::Start($info)
if($null -eq $p){throw 'FUSION|HOLD|generated_process_start_failed'}
try {
 $output=[IO.File]::Create($recovered)
 try { $p.StandardOutput.BaseStream.CopyTo($output) }
 finally {$output.Dispose()}
 $p.WaitForExit()
 if($p.ExitCode -ne 0){throw ('FUSION|HOLD|generated_emitter_exit_'+$p.ExitCode)}
} finally {$p.Dispose()}
$roundtripHash=(Get-FileHash -LiteralPath $recovered -Algorithm SHA256).Hash
if($roundtripHash -cne $originHash){
 throw 'FUSION|HOLD|FIFO_LAFIT_XHTML_bytes_differ'
}
& $native --qualify $recovered
if($LASTEXITCODE -ne 0){throw 'FUSION|HOLD|recovered_XHTML_not_qualified'}
& $native --fusion $recovered $second
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($second)){
 throw 'FUSION|HOLD|reverse_generation_failed'
}
$firstHash=(Get-FileHash -LiteralPath $first -Algorithm SHA256).Hash
$secondHash=(Get-FileHash -LiteralPath $second -Algorithm SHA256).Hash
if($firstHash -cne $secondHash){
 throw 'FUSION|HOLD|FIFO_LAFIT_generated_sources_differ'
}
# Bounded negative input matrix, no artifacts should be produced.
foreach($bad in @('bad_doctype.xhtml','bad_parity.xhtml','bad_stage.xhtml','bad_script.xhtml','bad_watchdog_missing.xhtml','bad_watchdog_mutated.xhtml','bad_watchdog_duplicate.xhtml','bad_watchdog_order.xhtml','bad_harmony_missing.xhtml','bad_harmony_mutated.xhtml','bad_harmony_order.xhtml')){
 $sourceBad=Join-Path $root ('diaery/'+$bad)
 $denied=Join-Path $base ('denied-'+$bad+'.cpp')
 & $native --qualify $sourceBad
 if($LASTEXITCODE -eq 0){throw ('FUSION|HOLD|negative_qualified|'+$bad)}
 & $native --fusion $sourceBad $denied
 if($LASTEXITCODE -eq 0 -or [IO.File]::Exists($denied)){
  throw ('FUSION|HOLD|rejected_XHTML_produced_CXX20|'+$bad)
 }
}
$altered=Join-Path $base 'fusion-tampered.xhtml'
$ascii=[Text.Encoding]::ASCII
$sourceText=$ascii.GetString([IO.File]::ReadAllBytes($recovered))
if(-not $sourceText.Contains('volume=13500')){throw 'FUSION|HOLD|no_volume_canary'}
[IO.File]::WriteAllBytes($altered,$ascii.GetBytes($sourceText.Replace('volume=13500','volume=13501')))
& $native --qualify $altered
if($LASTEXITCODE -eq 0){throw 'FUSION|HOLD|corrupted_value_qualified'}
$lastSourceHash=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
if($lastSourceHash -cne $originHash){throw 'FUSION|HOLD|source_mutation'}
Write-Host ('FUSION|FIFO==LAFIT|PASS|XHTML_SHA256='+$originHash.ToLowerInvariant())
Write-Host ('FUSION|REGENERATION=PASS|CXX20_SHA256='+$firstHash.ToLowerInvariant())
Write-Host 'FUSION|BiDi=PASS|TriDi=PASS|PiDi=PASS|PiPi=SYMBOLIC_ONLY|SigmaDi=PASS|SigmaSigma=PASS|GammaGamma=BOUNDED_PASS'
Write-Host 'FUSION|QUALIFICATION=PASS|RELEASE=HOLD|NO_SAFETY_CERTIFICATION'
