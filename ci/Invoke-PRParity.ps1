# SPDX-License-Identifier: 0BSD
# PR-PARITY/1: one identical fail-closed qualification contract for every PR.
# Source and Git history determine truth; a ledger hash is only a witness.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'PR_PARITY|HOLD|WINDOWS_MSVC_REQUIRED'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$utf8=[Text.UTF8Encoding]::new($false,$true)
if(-not [IO.Directory]::Exists($root) -or $out -ceq $root -or
   $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase) -or
   [IO.Directory]::Exists($out)){
 throw 'PR_PARITY|HOLD|NONFRESH_OR_UNSAFE_EVIDENCE'
}
[void][IO.Directory]::CreateDirectory($out)
$results=[Collections.Generic.List[object]]::new()
function Record([string]$phase,[string]$status,[string]$detail){
 if($detail.Length -gt 220){$detail=$detail.Substring(0,220)}
 $detail=$detail.Replace([char]9,' ').Replace([char]13,' ').Replace([char]10,' ')
 $results.Add([pscustomobject]@{Phase=$phase;Status=$status;Detail=$detail})
 Write-Host ('PR_PARITY|'+$phase+'|'+$status+'|'+$detail)
}
function Audit([string]$phase,[scriptblock]$body){
 try{& $body; Record $phase 'PASS' 'verified'}
 catch{Record $phase 'HOLD' ([string]$_.Exception.Message)}
}
# Requirements come from BOTH existing PR #34 and PR #95. A file's
# presence is not proof that its workflow executed or passed.
$required=[ordered]@{
 'HEADTAIL_TRIFORCE'=@('.github/workflows/triforce-headtail.yml','ci/Invoke-TriforcePipeline.ps1','ci/Assert-TriforceHeadTail.ps1')
 'NATIVE_FULLSOURCE'=@('.github/workflows/reelnative-fullsource-autostart.yml','ci/Invoke-FullSourceAutostartNativeSuite.ps1','shared/CMakeLists.txt','tests/consumer/CMakeLists.txt')
 'PYTHON_EXCLUSION'=@('.github/workflows/android-linux-no-python-source.yml','ci/Build-Android.ps1')
 'LICENSE_CUSTODY'=@('LICENSE','LICENSES/LOCK.tsv','LICENSES/README.md','LICENSES/text/0BSD.txt')
 'OPENBSD_SERIAL'=@('.github/workflows/openbsd-serial-coreao.yml','ci/OPENBSD_SERIAL_COREAO.tsv')
 'EXCITE_REPOSITORY'=@('.github/workflows/EXCITE.yml','ci/Invoke-RepositoryCircuit.ps1','ci/Excite-Coreao.ps1')
 'DOVIA_NATIVE'=@('.github/workflows/DOVIA.yml','ci/excited-native/CMakeLists.txt')
 'QUADDI_NATIVE'=@('.github/workflows/quaddi-ni-oracle-cpp20.yml','quaddi-ni/QuadDiNI.cpp')
 'HEXADI_CORE_PWSH_CSHARP'=@('.github/workflows/HEXA-DI.yml','ci/Install-HexaDiPowerShell.ps1','ci/Invoke-HexaDiQualification.ps1','pentadi/Invoke-PentaDiQualification.ps1','pentadi/PentaDiC11BranchRecords.reference.tsv')
 'MATCHB0XN9'=@('.github/workflows/MATCHB0Xn9.yml','ci/Invoke-Matchb0xn9Race.ps1')
 'FESTIVAL'=@('.github/workflows/MATCHB0Xn9-Festival.yml','ci/Invoke-Matchb0xn9Festival256.ps1')
}
Audit 'SELF_SOURCE' {
 foreach($rel in @('.github/workflows/PR-PARITY.yml','ci/Invoke-PRParity.ps1','CMakeLists.txt')){
  if(-not [IO.File]::Exists((Join-Path $root $rel))){throw ('missing_self_source|'+$rel)}
 }
}
foreach($name in $required.Keys){
 $missing=@($required[$name] | Where-Object {-not [IO.File]::Exists((Join-Path $root $_))})
 if($missing.Count -gt 0){
  Record ('COVERAGE_'+$name) 'HOLD' ('missing='+[string]::Join(';',$missing))
 }else{
  Record ('COVERAGE_'+$name) 'PASS' 'required_files_present_not_execution_proof'
 }
}
Audit 'ZERO_CLAUSE_LICENSE' {
 $license=Join-Path $root 'LICENSE'
 if(-not [IO.File]::Exists($license)){throw 'LICENSE_missing'}
 $s=[IO.File]::ReadAllText($license,$utf8)
 if(-not $s.Contains('Permission to use, copy, modify, and/or distribute this software',[StringComparison]::Ordinal) -or
    -not $s.Contains('THE SOFTWARE IS PROVIDED "AS IS"',[StringComparison]::Ordinal)){
  throw 'unrecognized_0BSD_root_license'
 }
}
Audit 'SOURCE_PATH_AND_PYTHON_EXCLUSION' {
 $files=@(Get-ChildItem -LiteralPath $root -Recurse -File -Force)
 if($files.Count -lt 30){throw 'source_tree_incomplete'}
 foreach($f in $files){
  $relative=[IO.Path]::GetRelativePath($root,$f.FullName).Replace('\','/')
  if($relative -match '[\r\n\t|]' -or $relative.StartsWith('../',[StringComparison]::Ordinal)){
   throw ('untrusted_source_path|'+$relative)
  }
  if($relative -notmatch '^(?i:Archive/Retired/)' -and
     $relative -match '(?i)(^|/)__pycache__(/|$)|[.]py[ic]?$|[.]whl$|(^|/)(pyproject[.]toml|requirements[.]txt|Pipfile)$'){
    throw ('python_source_not_approved|'+$relative)
  }
 }
}
Audit 'POWERSHELL_SYNTAX' {
 $scripts=@(Get-ChildItem -LiteralPath (Join-Path $root 'ci') -Recurse -File -Filter '*.ps1' -ErrorAction Stop)
 if($scripts.Count -eq 0){throw 'no_CI_powershell_scripts'}
 foreach($script in $scripts){
  $tokens=$null;$errors=$null
  [void][Management.Automation.Language.Parser]::ParseFile($script.FullName,[ref]$tokens,[ref]$errors)
  if(@($errors).Count -gt 0){
   throw ('invalid_powershell|'+[IO.Path]::GetRelativePath($root,$script.FullName))
  }
 }
}
# All build outputs are outside source custody. Native Release MSVC uses
# default /O2; no Debug, /Od or -O0 configuration is requested.
$originalFiles=@(Get-ChildItem -LiteralPath $root -Recurse -Force -File | Sort-Object FullName)
$before=[Collections.Generic.List[string]]::new()
foreach($f in $originalFiles){
 $before.Add(([IO.Path]::GetRelativePath($root,$f.FullName).Replace('\','/'))+
  [char]9+(Get-FileHash -LiteralPath $f.FullName -Algorithm SHA256).Hash.ToLowerInvariant())
}
Audit 'ROOT_MSVC_CXX20_RELEASE_BUILD' {
 $build=Join-Path $out 'root-msvc'
 & cmake -S $root -B $build -A x64 -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF
 if($LASTEXITCODE -ne 0){throw 'CMake_configure_failed'}
 $cache=Join-Path $build 'CMakeCache.txt'
 if(-not [IO.File]::Exists($cache)){throw 'CMAKE_CACHE_MISSING'}
 $flags=[IO.File]::ReadAllText($cache,$utf8)
 if($flags -cnotmatch '(?m)^CMAKE_CXX_FLAGS_RELEASE:STRING=.*\/O2' -or
    $flags -cmatch '(?m)^CMAKE_CXX_FLAGS_RELEASE:STRING=.*(\/Od|-O0)'){
  throw 'MSVC_RELEASE_OPTIMIZATION_NOT_O2'
 }
 & cmake --build $build --config Release --parallel 1
 if($LASTEXITCODE -ne 0){throw 'MSVC_CXX20_Release_build_failed'}
 & ctest --test-dir $build -C Release --output-on-failure
 if($LASTEXITCODE -ne 0){throw 'root_CTest_failed'}
 $listing=(& ctest --test-dir $build -C Release --show-only=json-v1 | Out-String)
 if($LASTEXITCODE -ne 0){throw 'CTest_list_failed'}
 $tests=ConvertFrom-Json -InputObject $listing -Depth 8
 if(@($tests.tests).Count -lt 1){throw 'zero_native_tests'}
}
Audit 'TRIFORCE_HEAD_TAIL_NATIVE' {
 $native=Join-Path $root 'ci/Invoke-TriforcePipeline.ps1'
 if(-not [IO.File]::Exists($native)){throw 'TRIFORCE_pipeline_unavailable'}
 & pwsh -NoLogo -NoProfile -NonInteractive -File $native -RepositoryRoot $root -EvidenceDirectory (Join-Path $out 'triforce')
 if($LASTEXITCODE -ne 0){throw 'TRIFORCE_native_stage_failed'}
}
Audit 'EXCITE_WHOLE_REPOSITORY_NATIVE' {
 $native=Join-Path $root 'ci/Invoke-RepositoryCircuit.ps1'
 if(-not [IO.File]::Exists($native)){throw 'EXCITE_whole_repository_unavailable'}
 & pwsh -NoLogo -NoProfile -NonInteractive -File $native -RepositoryRoot $root -EvidenceDirectory (Join-Path $out 'excite')
 if($LASTEXITCODE -ne 0){throw 'EXCITE_whole_repository_failed'}
}
Audit 'DOVIA_NATIVE_CTEST' {
 $src=Join-Path $root 'ci/excited-native'
 if(-not [IO.File]::Exists((Join-Path $src 'CMakeLists.txt'))){throw 'DOVIA_native_project_unavailable'}
 $build=Join-Path $out 'dovia-msvc'
 & cmake -S $src -B $build -A x64 -DBUILD_TESTING=ON
 if($LASTEXITCODE -ne 0){throw 'DOVIA_CMake_configure_failed'}
 $cache=Join-Path $build 'CMakeCache.txt'
 if(-not [IO.File]::Exists($cache)){throw 'CMAKE_CACHE_MISSING'}
 $flags=[IO.File]::ReadAllText($cache,$utf8)
 if($flags -cnotmatch '(?m)^CMAKE_CXX_FLAGS_RELEASE:STRING=.*\/O2' -or
    $flags -cmatch '(?m)^CMAKE_CXX_FLAGS_RELEASE:STRING=.*(\/Od|-O0)'){
  throw 'MSVC_RELEASE_OPTIMIZATION_NOT_O2'
 }
 & cmake --build $build --config Release --parallel 1
 if($LASTEXITCODE -ne 0){throw 'DOVIA_native_build_failed'}
 & ctest --test-dir $build -C Release --output-on-failure
 if($LASTEXITCODE -ne 0){throw 'DOVIA_ctest_failed'}
}
Audit 'NO_SOURCE_MUTATION' {
 $after=@(Get-ChildItem -LiteralPath $root -Recurse -Force -File | Sort-Object FullName)
 if($after.Count -ne $originalFiles.Count){throw 'source_count_mutated'}
 for([int]$i=0;$i -lt $after.Count;$i++){
  $record=[IO.Path]::GetRelativePath($root,$after[$i].FullName).Replace('\','/')+
   [char]9+(Get-FileHash -LiteralPath $after[$i].FullName -Algorithm SHA256).Hash.ToLowerInvariant()
  if($record -cne $before[$i]){throw 'source_digest_mutated'}
 }
}
$receipt=Join-Path $out 'PR_PARITY_RECEIPT.tsv'
$lines=[Collections.Generic.List[string]]::new()
$lines.Add('phase'+[char]9+'status'+[char]9+'detail')
foreach($item in $results){$lines.Add($item.Phase+[char]9+$item.Status+[char]9+$item.Detail)}
[IO.File]::WriteAllText($receipt,([string]::Join([string][char]10,$lines)+[char]10),$utf8)
$hash=(Get-FileHash -LiteralPath $receipt -Algorithm SHA256).Hash.ToLowerInvariant()
$held=@($results|Where-Object {$_.Status -cne 'PASS'})
if($held.Count -gt 0){
 Write-Host ('PR_PARITY|HOLD|FAILED_GATES='+$held.Count+'|RECEIPT_SHA256='+$hash+
  '|NO_FALSE_PARITY|NO_MERGE|NO_RELEASE')
 throw 'PR_PARITY|HOLD|MANDATORY_GATES_NOT_ALL_GREEN'
}
Write-Host ('PR_PARITY|PASS|GATES='+$results.Count+
 '|RECEIPT_SHA256='+$hash+'|PASS_NOT_MERGE_APPROVAL|RELEASE=HOLD')
