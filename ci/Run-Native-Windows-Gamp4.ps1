[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)]
 [ValidateSet('windows-x86','windows-x64','windows-x86-64','windows-amd64','windows-arm64')]
 [string]$Profile,
 [Parameter(Mandatory=$true)][version]$LatestStablePowerShellVersion,
 [Parameter(Mandatory=$true)][string]$ReleaseEvidenceUri,
 [string]$OutputRoot
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows -or $PSVersionTable.PSVersion.Major -lt 7){throw 'Native Windows with PowerShell 7 required'}
$expectedUri="https://github.com/PowerShell/PowerShell/releases/tag/v$LatestStablePowerShellVersion"
if($ReleaseEvidenceUri -cne $expectedUri -or [version]$PSVersionTable.PSVersion -ne $LatestStablePowerShellVersion){
 throw 'PowerShell version does not match independently reviewed latest release'
}
$canonical=switch($Profile){
 'windows-x86'{'windows-x86'}
 'windows-arm64'{'windows-arm64'}
 default{'windows-x64'}
}
$arch=switch($canonical){
 'windows-x86'{'Win32'}
 'windows-arm64'{'ARM64'}
 default{'x64'}
}
$hostArch=[Environment]::GetEnvironmentVariable('PROCESSOR_ARCHITECTURE')
if($canonical -eq 'windows-arm64' -and $hostArch -cne 'ARM64'){throw 'ARM64 execution requires native ARM64 Windows'}
if($canonical -eq 'windows-x64' -and $hostArch -notin @('AMD64','x64')){throw 'x64 execution requires native AMD64 Windows'}
if($canonical -eq 'windows-x86' -and $hostArch -notin @('x86','AMD64','x64')){throw 'x86 test requires compatible native Windows'}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
foreach($tool in @('git','cmake','ctest')){
 if(-not(Get-Command -Name $tool -ErrorAction SilentlyContinue)){throw "Missing local tool: $tool"}
}
& git -C $root diff --quiet --exit-code HEAD --
if($LASTEXITCODE -ne 0){throw 'Tracked source differs from HEAD'}
$dirty=@(& git -C $root status --porcelain --untracked-files=all)
if($LASTEXITCODE -ne 0 -or $dirty.Count -ne 0){throw 'Dirty or untracked source; review before validation'}
$shaLines=@(& git -C $root rev-parse --verify HEAD)
if($LASTEXITCODE -ne 0 -or $shaLines.Count -ne 1){throw 'Cannot identify Git source'}
$sha=([string]$shaLines[0]).Trim()
if($sha -notmatch '^[0-9a-fA-F]{40}$'){throw 'Malformed Git SHA'}
if(-not $OutputRoot){$OutputRoot=Join-Path $root "build/gamp4-release/$sha"}
$out=[IO.Path]::GetFullPath($OutputRoot)
$evidence=Join-Path $out "evidence/$canonical"
$full=Join-Path $out "_build/$canonical/full"
$core=Join-Path $out "_build/$canonical/core"
$package=Join-Path $out "packages/$canonical"
[void][IO.Directory]::CreateDirectory($evidence)
$receipt=Join-Path $evidence 'gamp4-gate.yml'
if(Test-Path -LiteralPath $receipt){throw 'Existing evidence; select new OutputRoot to preserve it'}
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio/Installer/vswhere.exe'
if(-not(Test-Path -LiteralPath $vswhere -PathType Leaf)){throw 'Visual Studio discovery missing'}
$installedLines=@(& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion)
if($LASTEXITCODE -ne 0 -or $installedLines.Count -eq 0){throw 'Local MSVC toolchain missing'}
$installed=([string]$installedLines[0]).Trim()
$generator=switch(([version]$installed).Major){
 17{'Visual Studio 17 2022'}
 18{'Visual Studio 18 2026'}
 default{throw "Unsupported installed MSVC version $installed"}
}
$record=[ordered]@{
 schema='SWE-IRON-GAMP4-NATIVE-WINDOWS-v1'
 source_commit=$sha
 profile=$canonical
 native_host_cpu=$hostArch
 generator=$generator
 stage='preflight'
 status='FAIL'
 exit_code=1
 ctest_status='NOT_TESTED'
 gamp4_summary='NOT_TESTED'
 binary_sha256='NOT_TESTED'
 qa_status='NOT_APPROVED'
 pq_status='NOT_PERFORMED'
 failure=''
}
$previous=Get-Location
try {
 Set-Location -LiteralPath $root
 $record.stage='configure'
 $argsConfig=@('-S',$root,'-B',$full,'-G',$generator,'-A',$arch,
   '-DBUILD_TESTING=ON','-DCMAKE_CXX_STANDARD=20',
   '-DCMAKE_CXX_STANDARD_REQUIRED=ON','-DCMAKE_CXX_EXTENSIONS=OFF',
   '-DFETCHCONTENT_FULLY_DISCONNECTED=ON')
 & cmake @argsConfig *> (Join-Path $evidence 'configure.log')
 if($LASTEXITCODE -ne 0){throw 'Full MSVC configure failed'}
 $record.stage='build'
 & cmake --build $full --config Release --parallel 2 *> (Join-Path $evidence 'build.log')
 if($LASTEXITCODE -ne 0){throw 'Full MSVC C++20 build failed'}
 $record.stage='ctest'
 & ctest --test-dir $full -C Release --output-on-failure *> (Join-Path $evidence 'ctest.log')
 if($LASTEXITCODE -ne 0){throw 'Full native CTest failed'}
 $record.ctest_status='PASS'
 $record.stage='gamp4-urs'
 & (Join-Path $root 'ci/Invoke-PentagonalGamp4.ps1') -BuildDir $full -EvidenceDir $evidence -ExecutableName 'pentagonal_gamp4_validation_test'
 $reports=@(Get-ChildItem -LiteralPath $evidence -File -Filter 'pentagonal-gamp4-*.json')
 if($reports.Count -ne 1){throw 'Exactly one source-matched GAMP4 JSON report required'}
 $gamp=Get-Content -LiteralPath $reports[0].FullName -Raw | ConvertFrom-Json
 if(-not $gamp.pass -or $gamp.commit -cne $sha -or
    $gamp.test_count -ne 12 -or $gamp.observed_pass_ids.Count -ne 12 -or
    $gamp.oq_automated_status -cne 'PASS'){
   throw 'GAMP4 SHA, 12 URS or OQ report mismatch'
 }
 $logFile=Join-Path $evidence ([string]$gamp.text_log)
 if(-not(Test-Path -LiteralPath $logFile -PathType Leaf)){throw 'GAMP4 raw stdout log missing'}
 $summaryRows=@(Get-Content -LiteralPath $logFile | Where-Object { $_ -ceq 'GAMP4_STYLE_SUMMARY|PASS|cases=12|failures=0' })
 if($summaryRows.Count -ne 1){throw 'GAMP4 terminal PASS summary missing or repeated'}
 $record.gamp4_summary='PASS|cases=12|failures=0'
 $record.binary_sha256=[string]$gamp.executable_sha256
 $record.stage='core-archive'
 $argsCore=@('-S',$root,'-B',$core,'-G',$generator,'-A',$arch,
   '-DSWE_STD_FREE_CORE_ONLY=ON','-DBUILD_TESTING=OFF',
   '-DCMAKE_CXX_STANDARD=20','-DCMAKE_CXX_EXTENSIONS=OFF',
   '-DFETCHCONTENT_FULLY_DISCONNECTED=ON')
 & cmake @argsCore *> (Join-Path $evidence 'core-configure.log')
 if($LASTEXITCODE -ne 0){throw 'STD-free core configure failed'}
 & cmake --build $core --config Release --target swe_std_free_portable_core --parallel 2 *> (Join-Path $evidence 'core-build.log')
 if($LASTEXITCODE -ne 0){throw 'STD-free archive failed'}
 & cmake --install $core --config Release --prefix $package *> (Join-Path $evidence 'install.log')
 if($LASTEXITCODE -ne 0){throw 'Archive install failed'}
 $bin=Join-Path $package 'bin'
 [void][IO.Directory]::CreateDirectory($bin)
 foreach($name in @('pentagonal_xor','pentagonal_xor_std_free','pentagonal_xor_cli','pentagonal_suite_cli','pentagonal_gamp4_validation_test')){
   $exe=Join-Path (Join-Path $full 'Release') ($name+'.exe')
   if(-not(Test-Path -LiteralPath $exe -PathType Leaf)){throw "Missing native tested executable $exe"}
   Copy-Item -LiteralPath $exe -Destination (Join-Path $bin ($name+'.exe'))
 }
 $rows=@(Get-ChildItem -LiteralPath $package -File -Recurse | Sort-Object FullName |
   ForEach-Object {
     $relative=[IO.Path]::GetRelativePath($package,$_.FullName).Replace([char]92,'/')
     '{0}  {1}' -f (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant(),$relative
   })
 $rows | Set-Content -LiteralPath (Join-Path $evidence 'package-sha256sums.txt') -Encoding utf8
 $record.stage='complete'
 $record.status='PASS'
 $record.exit_code=0
 Write-Host "IRON|WINDOWS|GAMP4_PASS|$canonical|$sha"
}
catch {
 $record.failure=[string]$_.Exception.Message
 throw
}
finally {
 Set-Location $previous
 $lines=@(foreach($key in $record.Keys){
   $value=([string]$record[$key]).Replace("'", "''")
   '{0}: ''{1}''' -f $key,$value
 })
 $lines | Set-Content -LiteralPath $receipt -Encoding utf8
}
