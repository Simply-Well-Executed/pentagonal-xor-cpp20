# SPDX-License-Identifier: 0BSD
# Complete-source native CMake/CTest + application startup test.
# Runs on GitHub-hosted Windows and INSIDE an operator-approved Linux Docker toolchain.
# This does not grant a GNU/POSIX/third-party component exception.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$OutputDirectory,
  [ValidateSet('Windows','Linux')][Parameter(Mandatory=$true)][string]$Platform
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$output=[IO.Path]::GetFullPath($OutputDirectory)
if(-not [IO.Directory]::Exists($root)){throw 'AUTOSTART|HOLD|repository_missing'}
if($Platform -cne 'Windows' -or -not $IsWindows){
  throw 'AUTOSTART|HOLD|native_MSVC_only_Linux_toolchain_not_approved'
}
if($output.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
  throw 'AUTOSTART|HOLD|build_output_must_be_outside_source_custody'
}
if([IO.Directory]::Exists($output)){throw 'AUTOSTART|HOLD|build_output_not_clean'}
[IO.Directory]::CreateDirectory($output)|Out-Null
$needed=@('CMakeLists.txt','LICENSE','LICENSES/LOCK.tsv','LICENSES/README.md',
  'LICENSES/text/0BSD.txt','.github/workflows/openbsd-serial-coreao.yml',
  'docs/THAILONGA_STAGE07_POSTFLIGHT_V1.md','ci/OPENBSD_SERIAL_COREAO.tsv',
  'ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv','ci/REELNATIVE_REGISTRY_PUBLISH_RECEIPTS_V1.tsv',
  'Archive/Retired/ISSUE_29_LEGACY_SHELLS.tsv','shared/CMakeLists.txt',
  'tests/consumer/CMakeLists.txt','pentagonal_xor_cpp20.cpp',
  'src/main.cpp','src/suite_main.cpp','android/app/src/main/AndroidManifest.xml',
  'java/PentagonalControl.java','.dockerignore')
foreach($relative in $needed){
  $path=Join-Path $root $relative
  if(-not [IO.File]::Exists($path)){throw ('AUTOSTART|HOLD|missing_source_or_license|'+$relative)}
}
$license=(Get-Content -LiteralPath (Join-Path $root 'LICENSE') -Raw)
if(-not $license.Contains('Permission to use, copy, modify, and/or distribute this software',[StringComparison]::Ordinal) -or
   -not $license.Contains('THE SOFTWARE IS PROVIDED "AS IS"',[StringComparison]::Ordinal)){
  throw 'AUTOSTART|HOLD|root_zero_clause_license_text_unrecognized'
}
$expectedCanonical='2c33d3cb2a4b42df5abf4868c370943feeeff274207f241aa3289e6c9487a308'
$observedCanonical=(Get-FileHash -LiteralPath (Join-Path $root 'ci/OPENBSD_SERIAL_COREAO.tsv') -Algorithm SHA256).Hash.ToLowerInvariant()
if($observedCanonical -cne $expectedCanonical){throw 'AUTOSTART|HOLD|canonical_manifest_drift'}
$sourceFileCount=@(Get-ChildItem -LiteralPath $root -Recurse -File -Force).Count
if($sourceFileCount -lt 340){throw ('AUTOSTART|HOLD|incomplete_source_tree|'+$sourceFileCount)}
Write-Host ('AUTOSTART|SOURCE_CUSTODY_PASS|TRACKED_ARCHIVE_FILE_COUNT='+$sourceFileCount+
            '|ROOT_LICENSE=0BSD|LICENSES_AND_WORKFLOWS_PRESENT|CANONICAL_SHA256='+$observedCanonical)
# Each native operation is bracketed by the same independent Q62/source gate.
# The nested session is separate from the calling GitHub workflow's outer gate.
$triforceScript=Join-Path $root 'ci/Assert-TriforceHeadTail.ps1'
$triforceEvidence=Join-Path $output 'triforce-inner-headtail'
$processAuditScript=Join-Path $root 'ci/Invoke-JstarProcessAudit.ps1'
$processEvidence=Join-Path $output 'jstar-process'
$env:MSBUILDDISABLENODEREUSE='1'
$env:DOTNET_CLI_DO_NOT_USE_MSBUILD_SERVER='1'
function Assert-Triforce([string]$Name,[string]$Phase) {
  $args=@('-NoLogo','-NoProfile','-NonInteractive','-File',$triforceScript,
          '-RepositoryRoot',$root,'-EvidenceDirectory',$triforceEvidence,
          '-Operation',('autostart_'+$Name),'-Phase',$Phase)
  # This native executable, once built, exercises the existing TOOLONG engine.
  $toolongTest=Join-Path $rootBuild 'Release/toolong_arithmetic_test.exe'
  if([IO.File]::Exists($toolongTest)){
    $args+=@('-NativeExecutable',$toolongTest)
  }
  & pwsh @args
  if($LASTEXITCODE -ne 0){throw ('AUTOSTART|TRIFORCE|HOLD|'+$Name+'|'+$Phase)}
}
function Invoke-Checked([string]$Executable,[string[]]$Arguments,[string]$Gate){
  Assert-Triforce $Gate 'HEAD'
  try {
    if($Arguments.Count -gt 1 -and $Arguments[0] -ceq '--build') {
      # Route all MSVC build stages through the signed-process custody gate.
      # The wrapper pins a single worker and disables retained MSBuild nodes.
      & $processAuditScript -RepositoryRoot $root -BuildDirectory $Arguments[1] `
        -EvidenceDirectory $processEvidence -Operation $Gate
    }else{
      & $Executable @Arguments
      if($LASTEXITCODE -ne 0){throw ('AUTOSTART|HOLD|'+$Gate+'|EXIT='+$LASTEXITCODE)}
    }
  }finally{
    Assert-Triforce $Gate 'TAIL'
  }
}
$cmake=Get-Command cmake -ErrorAction SilentlyContinue
$ctest=Get-Command ctest -ErrorAction SilentlyContinue
if($null -eq $cmake -or $null -eq $ctest){throw 'AUTOSTART|HOLD|cmake_or_ctest_unavailable'}
$info=@(& $cmake.Source --version)
if($LASTEXITCODE -ne 0 -or $info.Count -lt 1){throw 'AUTOSTART|HOLD|cmake_version_probe'}
Write-Host ('AUTOSTART|CMAKE|'+$info[0])
$rootBuild=Join-Path $output 'root'
$sharedBuild=Join-Path $output 'shared'
$consumerBuild=Join-Path $output 'consumer'
$stage=Join-Path $output 'install'
$config=@('-S',$root,'-B',$rootBuild,'-DBUILD_TESTING=ON','-DSWE_STD_FREE_CORE_ONLY=OFF')
if($Platform -ceq 'Windows'){$config+=@('-G','Visual Studio 18 2026','-A','x64')}
else{$config+=@('-DCMAKE_BUILD_TYPE=Release')}
Invoke-Checked $cmake.Source $config 'cmake_root_configure'
Invoke-Checked $cmake.Source @('--build',$rootBuild,'--config','Release','--parallel','1') 'cmake_root_full_build'
Invoke-Checked $ctest.Source @('--test-dir',$rootBuild,'-C','Release','--output-on-failure','--no-tests=error') 'ctest_root_all'
$sharedConfig=@('-S',(Join-Path $root 'shared'),'-B',$sharedBuild)
if($Platform -ceq 'Windows'){$sharedConfig+=@('-G','Visual Studio 18 2026','-A','x64')}
else{$sharedConfig+=@('-DCMAKE_BUILD_TYPE=Release')}
Invoke-Checked $cmake.Source $sharedConfig 'cmake_shared_configure'
Invoke-Checked $cmake.Source @('--build',$sharedBuild,'--config','Release','--parallel','1') 'cmake_shared_full_build'
Invoke-Checked $ctest.Source @('--test-dir',$sharedBuild,'-C','Release','--output-on-failure','--no-tests=error') 'ctest_shared_all'
Invoke-Checked $cmake.Source @('--install',$rootBuild,'--config','Release','--prefix',$stage) 'cmake_install'
$consumerConfig=@('-S',(Join-Path $root 'tests/consumer'),'-B',$consumerBuild,('-DSWE_AUDIT_PREFIX='+$stage))
if($Platform -ceq 'Windows'){$consumerConfig+=@('-G','Visual Studio 18 2026','-A','x64')}
else{$consumerConfig+=@('-DCMAKE_BUILD_TYPE=Release')}
Invoke-Checked $cmake.Source $consumerConfig 'cmake_consumer_configure'
Invoke-Checked $cmake.Source @('--build',$consumerBuild,'--config','Release','--parallel','1') 'cmake_consumer_full_build'
Invoke-Checked $ctest.Source @('--test-dir',$consumerBuild,'-C','Release','--output-on-failure','--no-tests=error') 'ctest_consumer_all'
$app=Join-Path $stage 'bin/pentagonal_xor'
if($Platform -ceq 'Windows'){$app+='.exe'}
if(-not [IO.File]::Exists($app)){throw 'AUTOSTART|HOLD|normal_main_executable_missing'}
$args=@('0.80','0.70','0.90','0.60','0.95')
Assert-Triforce 'normal_application' 'HEAD'
$run=@(& $app @args)
if($LASTEXITCODE -ne 0 -or $run.Count -ne 1 -or
   $run[0] -cne 'Pentagonal XOR: 51.72800000%'){
  throw ('AUTOSTART|HOLD|normal_application_output_mismatch|'+($run -join ';'))
}
Assert-Triforce 'normal_application' 'TAIL'
Write-Host ('AUTOSTART|NORMAL_STARTUP_PASS|PLATFORM='+$Platform+'|PROGRAM='+$app+'|OUTPUT='+$run[0])
Assert-Triforce 'typed_output_contracts' 'HEAD'
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-FullSourceOutputContractsV1.ps1') -RepositoryRoot $root -InstalledBinDirectory (Join-Path $stage 'bin') -Platform $Platform
if($LASTEXITCODE -ne 0){throw 'AUTOSTART|HOLD|typed_stdout_json_xml_contract_failed'}
Assert-Triforce 'typed_output_contracts' 'TAIL'

# New native NiteRex/Web9.X XHTML renderer is separate from the legacy CLI.
# Windows-only source projection; unsigned, no browser or CA conformance claim.
if($Platform -ceq 'Windows'){
 Assert-Triforce 'niterex_web9x' 'HEAD'
 & pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-NiteRexWeb9XOutputV1.ps1') -InstalledBinDirectory (Join-Path $stage 'bin')
 if($LASTEXITCODE -ne 0){throw 'AUTOSTART|HOLD|NiteRex_Web9X_XHTML_native_contract_failed'}
 Assert-Triforce 'niterex_web9x' 'TAIL'
}
$tests=@(
 'ci/Test-ReleaseSafety.ps1',
 'ci/Test-ReleaseSafetyPolicy.ps1',
 'ci/Test-OpenBSDSerialCoreao.ps1',
 'ci/Test-OpenBSDNativeEvidence.ps1',
 'ci/Test-OpenBSDThailongaPostflightV1.ps1'
)
$failedGates=[Collections.Generic.List[string]]::new()
foreach($relative in $tests){
  $argv=@('-NoLogo','-NoProfile','-File',(Join-Path $root $relative))
  switch($relative){
    'ci/Test-OpenBSDSerialCoreao.ps1' {$argv+=@('-RepositoryRoot',$root,'-TemporaryDirectory',(Join-Path $output 'serial-head-tail'))}
    'ci/Test-OpenBSDNativeEvidence.ps1' {$argv+=@('-RepositoryRoot',$root)}
    'ci/Test-OpenBSDThailongaPostflightV1.ps1' {$argv+=@('-RepositoryRoot',$root,'-TemporaryDirectory',(Join-Path $output 'thailonga-head-tail'))}
  }
  try{
    Assert-Triforce ('security_'+[IO.Path]::GetFileNameWithoutExtension($relative)) 'HEAD'
    & pwsh @argv
    if($LASTEXITCODE -ne 0){
      $failedGates.Add($relative+'|EXIT='+$LASTEXITCODE)
      break
    }else{
      Assert-Triforce ('security_'+[IO.Path]::GetFileNameWithoutExtension($relative)) 'TAIL'
      Write-Host ('AUTOSTART|INDEPENDENT_GATE_PASS|'+$relative)
    }
  }catch{
    $failedGates.Add($relative+'|ERROR='+$_.Exception.GetType().Name)
    break
  }
}
if($failedGates.Count -ne 0){
  foreach($failure in $failedGates){Write-Host ('AUTOSTART|INDEPENDENT_GATE_HOLD|'+$failure)}
  throw ('AUTOSTART|HOLD|independent_security_gates_failed|'+$failedGates.Count)
}
Write-Host 'AUTOSTART|NATIVE_SUITE_PASS|ROOT_CTEST|SHARED_CTEST|CONSUMER_CTEST|NORMAL_APP_EXECUTION|TYPED_OUTPUT_CONTRACTS|POWERSHELL_SECURITY_TESTS'
Write-Host 'AUTOSTART|COVERAGE_LIMIT|ANDROID_SDK_JAVA_DEVICE_TESTS_NOT_INCLUDED|DO_NOT_CLAIM_ALL_PROJECT_TESTS'
