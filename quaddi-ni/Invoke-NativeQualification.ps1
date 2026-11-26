# SPDX-License-Identifier: 0BSD
# QUADDI-NI native-only qualification, following DIAERY's isolated evidence gate.
# No Oracle substitution or cross-language PASS claim when the JDK is absent.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [ValidateRange(0,2147483647)][int]$Power=0,
 [string]$LinesFile='',
 [string]$SamplesFile='',
 [string]$MiscFile=''
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'QUADDI|HOLD|native_windows_MSVC_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$ev=[IO.Path]::GetFullPath($EvidenceDirectory)
$sep=[IO.Path]::DirectorySeparatorChar
if($ev -ceq $root -or $ev.StartsWith($root+$sep,[StringComparison]::OrdinalIgnoreCase)){
 throw 'QUADDI|HOLD|evidence_inside_checkout'
}
[IO.Directory]::CreateDirectory($ev)|Out-Null
if(-not $LinesFile){$LinesFile=Join-Path $root 'quaddi-ni/default-lines.tsv'}
if(-not $SamplesFile){$SamplesFile=Join-Path $root 'quaddi-ni/default-xs.tsv'}
if(-not $MiscFile){$MiscFile=Join-Path $root 'quaddi-ni/default-misc.tsv'}
$cpp=Join-Path $root 'quaddi-ni/QuadDiNI.cpp'
$render=Join-Path $root 'quaddi-ni/Invoke-Render.ps1'
$sources=@($cpp,$render,$LinesFile,$SamplesFile,$MiscFile)
foreach($path in $sources){
 if(-not [IO.File]::Exists($path)){throw ('QUADDI|HOLD|source_missing|'+$path)}
}
$before=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not [IO.File]::Exists($vswhere)){throw 'QUADDI|HOLD|vswhere_missing'}
$install=@(& $vswhere -latest -products '*' -requires 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' -property installationPath)
if($install.Count -ne 1 -or -not $install[0]){throw 'QUADDI|HOLD|MSVC_missing'}
& (Join-Path $install[0] 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
$cl=(Get-Command cl.exe -ErrorAction Stop).Source
$exe=Join-Path $ev 'quaddi-native.exe'
if([IO.File]::Exists($exe)){throw 'QUADDI|HOLD|binary_evidence_already_exists'}
& $cl /nologo /std:c++20 /O2 /W4 /permissive- /EHsc /fp:precise ('/Fe:'+$exe) $cpp
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($exe)){throw 'QUADDI|HOLD|native_compile_failed'}
function Count-Lines([string]$path){
 return @([IO.File]::ReadAllLines($path)|Where-Object{$_ -and -not $_.StartsWith('#')}).Count
}
function Native-Witness([int]$p,[string]$lines,[string]$samples,[string]$misc,[string]$label){
 $output=Join-Path $ev ($label+'.tsv')
 if([IO.File]::Exists($output)){throw 'QUADDI|HOLD|witness_already_exists'}
 & $exe $lines $samples ([string]$p) $misc > $output
 if($LASTEXITCODE -ne 0){throw ('QUADDI|HOLD|native_execution_failed|'+$label)}
 $rows=[IO.File]::ReadAllLines($output)
 $lc=Count-Lines $lines; $sc=Count-Lines $samples; $mc=Count-Lines $misc
 $pages=6+$lc+$mc
 $tab=[char]9
 $head=[string]::Join($tab,@('META','1','6',$lc,$mc,$pages,$p))
 $tail=[string]::Join($tab,@('END',$lc,$pages,$p,'RELEASE=HOLD'))
 $expected=2+$mc+7*$lc*$sc
 if($lc -lt 1 -or $sc -lt 1 -or $rows.Count -ne $expected -or
    $rows[0] -cne $head -or $rows[-1] -cne $tail){
  throw ('QUADDI|HOLD|count_or_identity_mismatch|'+$label)
 }
 Write-Host ('QUADDI|NATIVE_WITNESS=PASS|POWER='+$p+'|LINES='+$lc+'|SAMPLES='+$sc+'|PAGES='+$pages)
 return $output
}
# All seven seed powers, zero included, independently checked.
for($p=0;$p -le 6;$p++){
 $null=Native-Witness $p $LinesFile $SamplesFile $MiscFile ('default-power-'+$p)
}
# Requested seed power is also evaluated, or the power-0 witness is reused.
if($Power -gt 6){
 $chosen=Native-Witness $Power $LinesFile $SamplesFile $MiscFile ('selected-power-'+$Power)
}else{
 $chosen=Join-Path $ev ('default-power-'+$Power+'.tsv')
}
# A dynamic-line count AND a different number of appended miscellaneous pages.
$customLines=Join-Path $ev '17-independent-lines.tsv'
$customNotes=Join-Path $ev '4-misc-pages.tsv'
$lineData=for($i=0;$i -lt 17;$i++){('Independent_'+$i+'|'+$i+'|-3')}
[IO.File]::WriteAllLines($customLines,[string[]]$lineData,[Text.Encoding]::ASCII)
[IO.File]::WriteAllLines($customNotes,[string[]]@('COREAO_GammGamma','Audit_2','Audit_3','Audit_4'),[Text.Encoding]::ASCII)
$null=Native-Witness 0 $customLines $SamplesFile $customNotes 'custom-17-lines-4-misc'
# Negative canary proves invalid numerical input cannot silently pass.
$bad=Join-Path $ev 'bad-lines.tsv'
[IO.File]::WriteAllText($bad,('InvalidValue|not-a-number|-3'+[Environment]::NewLine),[Text.Encoding]::ASCII)
& $exe $bad $SamplesFile 0 $MiscFile 1>$null 2>$null
if($LASTEXITCODE -eq 0){throw 'QUADDI|HOLD|native_accepted_invalid_value'}
# Renderer uses the validated zero/default witness and keeps original rows intact.
$out=Join-Path $ev 'quaddi-paged.xhtml'
if([IO.File]::Exists($out)){throw 'QUADDI|HOLD|existing_XHTML_evidence'}
& pwsh -NoLogo -NoProfile -NonInteractive -File $render -WitnessFile $chosen -Destination $out
if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($out)){throw 'QUADDI|HOLD|XHTML_generation_failed'}
$after=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
for($i=0;$i -lt $sources.Count;$i++){
 if($before[$i] -cne $after[$i]){throw 'QUADDI|HOLD|source_changed'}
}
Write-Host ('QUADDI|NATIVE_QUALIFICATION=PASS|POWER='+$Power+'|XHTML=PASS|SOURCE_SHA256='+$before[0].ToLowerInvariant())
Write-Host 'QUADDI|ORACLE_PARITY=HOLD_UNLESS_SEPARATELY_RUN|RELEASE=HOLD'
