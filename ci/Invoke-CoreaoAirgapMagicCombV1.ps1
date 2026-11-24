# SPDX-License-Identifier: 0BSD
# DASH-REPOSE: defensive MagicComb first, network-none Docker envelope,
# SPIN-9/YANG, STAR-27/YIN, then derived COREAO seal; no historical mutation.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$ExactSourceArchive,
 [Parameter(Mandatory=$true)][string]$WorkingDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$archive=[IO.Path]::GetFullPath($ExactSourceArchive)
$work=[IO.Path]::GetFullPath($WorkingDirectory)
$gitSha=$env:GITHUB_SHA
if($env:GITHUB_ACTIONS -cne 'true' -or
   $env:GITHUB_REPOSITORY -cne 'Simply-Well-Executed/pentagonal-xor-cpp20' -or
   $env:RUNNER_OS -cne 'Linux' -or
   $env:RUNNER_ENVIRONMENT -cne 'github-hosted' -or
   $gitSha -cnotmatch '^[a-f0-9]{40}$'){
 throw 'COREAO|AIRGAP|HOLD|untrusted_runner_or_source'
}
$manifest=Join-Path $root 'ci/OPENBSD_SERIAL_COREAO.tsv'
$annex=Join-Path $root 'ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv'
$canonical=Join-Path $root 'packaging/openbsd2-coreao/stage10_linux_amd64.cpp'
foreach($path in @($archive,$manifest,$annex,$canonical,
 (Join-Path $root 'LICENSE'),(Join-Path $root 'LICENSES/LOCK.tsv'),
 (Join-Path $root 'ci/REELNATIVE_REGISTRY_PUBLISH_RECEIPTS_V1.tsv'))){
 if(-not [IO.File]::Exists($path)){throw ('COREAO|AIRGAP|HOLD|missing_source|'+$path)}
}
if([IO.Directory]::Exists($work) -or
 $work.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::Ordinal)){
 throw 'COREAO|AIRGAP|HOLD|working_directory_not_clean'
}
[IO.Directory]::CreateDirectory($work)|Out-Null
function Hash-String([string]$value){
 return [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
  [Text.UTF8Encoding]::new($false).GetBytes($value))).ToLowerInvariant()
}
function Hash-File([string]$path){
 return (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
}
$hashes=@{}
foreach($path in @($manifest,$annex,$canonical)){$hashes[$path]=Hash-File $path}
$archiveSha=Hash-File $archive
Write-Host ('DASH-REPOSE|DEFENSE_FIRST|GIT_SHA='+$gitSha+'|ARCHIVE_SHA256='+$archiveSha)
# The MagicComb and postflight tests include HEAD/TAIL and negative corruption.
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-OpenBSDSerialCoreao.ps1') -RepositoryRoot $root -TemporaryDirectory (Join-Path $work 'magiccomb')
if($LASTEXITCODE -ne 0){throw 'COREAO|AIRGAP|HOLD|magiccomb_failed'}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-OpenBSDNativeEvidence.ps1') -RepositoryRoot $root
if($LASTEXITCODE -ne 0){throw 'COREAO|AIRGAP|HOLD|native_evidence_failed'}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-OpenBSDThailongaPostflightV1.ps1') -RepositoryRoot $root -TemporaryDirectory (Join-Path $work 'thailonga-test')
if($LASTEXITCODE -ne 0){throw 'COREAO|AIRGAP|HOLD|thailonga_failed'}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-PeekGateStaticV1.ps1') -RepositoryRoot $root
if($LASTEXITCODE -ne 0){throw 'COREAO|AIRGAP|HOLD|static_negative_failed'}
foreach($path in $hashes.Keys){
 if((Hash-File $path) -cne $hashes[$path]){throw 'COREAO|AIRGAP|HOLD|canonical_mutated'}
}
Write-Host 'DASH-REPOSE|MAGICCOMB_DEFENSE_PASS|HEAD_TAIL|NEGATIVE_TESTS|CANONICAL_UNCHANGED'
$ledger=Join-Path $work 'serial-witnesses.jsonl'
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Invoke-OpenBSDSerialCoreao.ps1') -RepositoryRoot $root -ReportPath $ledger
if($LASTEXITCODE -ne 0){throw 'COREAO|AIRGAP|HOLD|ledger_failed'}
$rows=@([IO.File]::ReadAllLines($ledger,[Text.UTF8Encoding]::new($false,$true))|ForEach-Object{$_|ConvertFrom-Json})
if($rows.Count -ne 10 -or $rows[7].stage -cne 'OPENBSD2_YIN' -or
   $rows[8].stage -cne 'OPENBSD2_YANG' -or
   $rows[9].stage -cne 'COREAO_OPENBSD2' -or
   $rows[9].coreao_core -cne 'OPENBSD2' -or $rows[9].approved -ne $false -or
   $rows[9].yin_witness_sha256 -cne $rows[7].witness_sha256 -or
   $rows[9].yang_witness_sha256 -cne $rows[8].witness_sha256 -or
   $rows[9].witness_sha256 -cne 'efed3f09265eb3f5e506cf310a9905b12befa3b642f0085b3a089fb856ddbcdf'){
 throw 'COREAO|AIRGAP|HOLD|canonical_identity_mismatch'
}
$compiler=Get-Command 'clang++-18' -ErrorAction SilentlyContinue
$linker=Get-Command 'ld.lld-18' -ErrorAction SilentlyContinue
if($null -eq $compiler -or $null -eq $linker){throw 'COREAO|AIRGAP|HOLD|clang_lld_18_missing'}
$version=@(& $compiler.Source --version)
if($LASTEXITCODE -ne 0 -or $version[0] -cnotmatch 'clang version 18[.]'){
 throw 'COREAO|AIRGAP|HOLD|compiler_version'
}
$source=Join-Path $root 'packaging/coreao-airgap-v1/tick_linux_amd64.cpp'
if(-not [IO.File]::ReadAllText($source).Contains($rows[9].witness_sha256,[StringComparison]::Ordinal)){
 throw 'COREAO|AIRGAP|HOLD|tick_source_witness_drift'
}
$head=Join-Path $work 'tick-head'
$tail=Join-Path $work 'tick-tail'
$flags=@('-std=c++20','-O2','-ffreestanding','-fno-exceptions','-fno-rtti',
 '-nostdlib','-static','-no-pie','-fuse-ld=lld','-Wl,-e,_start',
 '-Wl,--build-id=none','-Wl,-z,noexecstack')
& $compiler.Source @flags $source '-o' $head
if($LASTEXITCODE -ne 0){throw 'COREAO|AIRGAP|HOLD|compile_head_failed'}
& $compiler.Source @flags $source '-o' $tail
if($LASTEXITCODE -ne 0 -or (Hash-File $head) -cne (Hash-File $tail)){
 throw 'COREAO|AIRGAP|HOLD|compile_tail_mismatch'
}
$ctx=Join-Path $work 'context'
[IO.Directory]::CreateDirectory($ctx)|Out-Null
[IO.File]::Copy($head,(Join-Path $ctx 'coreao-tick'))
[IO.File]::Copy($archive,(Join-Path $ctx 'source.zip'))
[IO.File]::Copy($ledger,(Join-Path $ctx 'serial-witnesses.jsonl'))
[IO.File]::Copy($annex,(Join-Path $ctx 'native-evidence.tsv'))
[IO.File]::Copy((Join-Path $root 'LICENSE'),(Join-Path $ctx 'LICENSE'))
$df=Join-Path $root 'packaging/coreao-airgap-v1/Dockerfile'
$trial='coreao-airgap:trial-'+$gitSha
$final='coreao-airgap:postflight-'+$gitSha
& docker build --network=none --pull=false --no-cache --platform linux/amd64 --target trial -f $df --build-arg ('SOURCE_SHA='+$gitSha) -t $trial $ctx
if($LASTEXITCODE -ne 0){throw 'COREAO|AIRGAP|HOLD|networkless_build_failed'}
$expected=@(
 'COREAO|AIRGAP_V1|TICK=1|ENTRY=FREESTANDING_CXX20|COMPILER_O2=REQUIRED',
 ('COREAO|CANONICAL_STAGE10_WITNESS_SHA256='+$rows[9].witness_sha256),
 'COREAO|NATIVE_OPENBSD_BUILDS=0|RELEASE_STATUS=HOLD|HOST=LINUX_AMD64'
)
$run=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
 '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
function Tick([string]$image,[string]$name){
 $output=@(& docker @run $image)
 if($LASTEXITCODE -ne 0 -or $output.Count -ne $expected.Count){
  throw ('COREAO|AIRGAP|HOLD|tick_exit_or_count|'+$name)
 }
 for($i=0;$i -lt $expected.Count;$i++){
  if(-not [StringComparer]::Ordinal.Equals($output[$i],$expected[$i])){
   throw ('COREAO|AIRGAP|HOLD|tick_mismatch|'+$name+'|'+$i)
  }
 }
 return ($output -join [char]10)
}
$first=Tick $trial 'DEFENSE_HEAD'
Write-Host 'DASH-REPOSE|AIRGAP_PASS|BUILD_NETWORK_NONE|RUNTIME_NETWORK_NONE|READ_ONLY|NO_CAPABILITIES'
$spin=[Collections.Generic.List[string]]::new()
$spinClock=[Diagnostics.Stopwatch]::StartNew()
for($i=0;$i -lt 9;$i++){$spin.Add((Tick $trial ('SPIN9_YANG_'+$i)))}
$spinClock.Stop()
$yang=Hash-String ('COREAO_AIRGAP_V1|YANG|SPIN9|'+($spin -join [char]10))
$star=[Collections.Generic.List[string]]::new()
$starClock=[Diagnostics.Stopwatch]::StartNew()
for($p=0;$p -lt 3;$p++){
 for($i=0;$i -lt 9;$i++){$star.Add((Tick $trial ('STAR27_YIN_'+$p+'_'+$i)))}
}
$starClock.Stop()
$yin=Hash-String ('COREAO_AIRGAP_V1|YIN|STAR27|'+($star -join [char]10))
if($spin.Count -ne 9 -or $star.Count -ne 27 -or $yin -ceq $yang){
 throw 'COREAO|AIRGAP|HOLD|YIN_YANG_incomplete_or_merged'
}
if(-not [StringComparer]::Ordinal.Equals($first,(Tick $trial 'DEFENSE_TAIL'))){
 throw 'COREAO|AIRGAP|HOLD|head_tail_drift'
}
$payload=@(
 'COREAO_AIRGAP_MAGICCOMB_DASH_REPOSE_V1',
 ('SOURCE_SHA='+$gitSha),('SOURCE_ARCHIVE_SHA256='+$archiveSha),
 ('CANONICAL_STAGE10='+$rows[9].witness_sha256),
 ('YANG_SPIN9='+$yang),('YIN_STAR27='+$yin)
) -join '|'
$coreao=Hash-String $payload
$receipt=[ordered]@{
 protocol='COREAO_AIRGAP_MAGICCOMB_DASH_REPOSE_V1'
 status='PASS'
 source_sha=$gitSha
 source_archive_sha256=$archiveSha
 canonical_core='OPENBSD2'
 canonical_stage10_witness_sha256=$rows[9].witness_sha256
 historical_yin_sha256=$rows[7].witness_sha256
 historical_yang_sha256=$rows[8].witness_sha256
 magiccomb='HEAD_TAIL_NEGATIVE_PASS'
 airgap_scope='DOCKER_BUILD_AND_RUNTIME_NETWORK_NONE_NOT_PHYSICAL_AIRGAP'
 defense_order='MAGICCOMB_THEN_AIRGAP_THEN_YANG_YIN_THEN_POSTFLIGHT'
 yang_stream='SPIN-9'
 yang_count=9
 yang_sha256=$yang
 yin_stream='STAR-27'
 yin_count=27
 yin_phases=3
 yin_sha256=$yin
 coreao_postflight_sha256=$coreao
 spin_ms=$spinClock.ElapsedMilliseconds
 star_ms=$starClock.ElapsedMilliseconds
 native_openbsd_builds=0
 release_status='HOLD'
 published=$false
}
$receiptPath=Join-Path $ctx 'coreao-airgap-receipt-v1.json'
[IO.File]::WriteAllText($receiptPath,(ConvertTo-Json $receipt -Compress)+[char]10,
 [Text.UTF8Encoding]::new($false))
& docker build --network=none --pull=false --no-cache --platform linux/amd64 --target postflight -f $df --build-arg ('SOURCE_SHA='+$gitSha) -t $final $ctx
if($LASTEXITCODE -ne 0){throw 'COREAO|AIRGAP|HOLD|postflight_build_failed'}
if(-not [StringComparer]::Ordinal.Equals($first,(Tick $final 'POSTFLIGHT_TAIL'))){
 throw 'COREAO|AIRGAP|HOLD|postflight_runtime_drift'
}
$container=(@(& docker create --platform linux/amd64 $final))[0]
if($LASTEXITCODE -ne 0 -or $container -cnotmatch '^[a-f0-9]{64}$'){
 throw 'COREAO|AIRGAP|HOLD|postflight_container_create'
}
try{
 $copyZip=Join-Path $work 'verified-source.zip'
 $copyReceipt=Join-Path $work 'verified-receipt.json'
 & docker cp ($container+':/custody/exact-github-source.zip') $copyZip
 if($LASTEXITCODE -ne 0 -or (Hash-File $copyZip) -cne $archiveSha){
  throw 'COREAO|AIRGAP|HOLD|source_archive_content_mismatch'
 }
 & docker cp ($container+':/custody/coreao-airgap-receipt-v1.json') $copyReceipt
 if($LASTEXITCODE -ne 0 -or (Hash-File $copyReceipt) -cne (Hash-File $receiptPath)){
  throw 'COREAO|AIRGAP|HOLD|postflight_receipt_content_mismatch'
 }
}finally{& docker rm -f $container *> $null}
foreach($path in $hashes.Keys){
 if((Hash-File $path) -cne $hashes[$path]){
  throw 'COREAO|AIRGAP|HOLD|historical_mutation_after_run'
 }
}
Write-Host ('DASH-REPOSE|COREAO_AIRGAP_PASS|MAGICCOMB_DEFENSE_FIRST|SPIN9_YANG_SHA256='+$yang+
 '|STAR27_YIN_SHA256='+$yin+'|DERIVED_COREAO_SHA256='+$coreao+
 '|SOURCE_ZIP_SHA256='+$archiveSha+'|DOCKER_NETWORK_NONE|NO_REGISTRY_PUSH|RELEASE=HOLD')
