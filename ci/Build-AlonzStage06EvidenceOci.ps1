# SPDX-License-Identifier: 0BSD
# ALONZ ENVELOPE stage 06. A reproducible Linux/amd64 diagnostic, never OpenBSD.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$WorkingDirectory,
  [Parameter(Mandatory=$true)][string]$ImageTag
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$temp=[IO.Path]::GetFullPath($WorkingDirectory)
if($env:GITHUB_ACTIONS -cne 'true' -or
   $env:GITHUB_REPOSITORY -cne 'Simply-Well-Executed/pentagonal-xor-cpp20' -or
   $env:RUNNER_OS -cne 'Linux' -or
   $env:RUNNER_ENVIRONMENT -cne 'github-hosted'){
  throw 'ALONZ|OCI|HOLD|requires_expected_github_linux_host'
}
if($ImageTag -cnotmatch '^ghcr[.]io/simply-well-executed/alonz-envelope-stage06-evidence:sha-[0-9a-f]{40}$'){
  throw 'ALONZ|OCI|HOLD|unexpected_ghcr_tag'
}
if(-not [IO.Directory]::Exists($root)){throw 'ALONZ|OCI|HOLD|missing_root'}
[IO.Directory]::CreateDirectory($temp)|Out-Null
$ledger=Join-Path $temp 'serial-witnesses.jsonl'
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-OpenBSDSerialCoreao.ps1') -RepositoryRoot $root -TemporaryDirectory (Join-Path $temp 'magiccomb')
if($LASTEXITCODE -ne 0){throw 'ALONZ|OCI|HOLD|serial_magiccomb_failure'}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-OpenBSDNativeEvidence.ps1') -RepositoryRoot $root
if($LASTEXITCODE -ne 0){throw 'ALONZ|OCI|HOLD|source_evidence_failure'}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Invoke-OpenBSDSerialCoreao.ps1') -RepositoryRoot $root -ReportPath $ledger
if($LASTEXITCODE -ne 0){throw 'ALONZ|OCI|HOLD|ledger_generation_failed'}
$rows=@([IO.File]::ReadAllLines($ledger,[Text.UTF8Encoding]::new($false,$true))|ForEach-Object {$_|ConvertFrom-Json})
if($rows.Count -ne 10 -or
   $rows[4].stage -cne 'OPENBSD4_FIRST' -or $rows[4].release -cne '4.9' -or
   $rows[5].stage -cne 'OPENBSD3' -or $rows[5].release -cne '3.9' -or
   $rows[5].ordinal -cne '06' -or $rows[5].pass -cne 'NONE' -or
   $rows[5].core -cne 'NONE' -or $rows[5].predecessor -cne 'OPENBSD4_FIRST' -or
   $rows[5].previous_sha256 -cne $rows[4].witness_sha256 -or
   $rows[6].stage -cne 'OPENBSD4_RETURN' -or
   $rows[6].previous_sha256 -cne $rows[5].witness_sha256 -or
   $rows[7].stage -cne 'OPENBSD2_YIN' -or $rows[7].core -cne 'OPENBSD2' -or
   $rows[8].stage -cne 'OPENBSD2_YANG' -or $rows[8].core -cne 'OPENBSD2' -or
   $rows[7].witness_sha256 -ceq $rows[8].witness_sha256 -or
   $rows[9].stage -cne 'COREAO_OPENBSD2' -or
   $rows[9].coreao_core -cne 'OPENBSD2' -or
   $rows[9].yin_witness_sha256 -cne $rows[7].witness_sha256 -or
   $rows[9].yang_witness_sha256 -cne $rows[8].witness_sha256 -or
   $rows[9].approved -ne $false){
  throw 'ALONZ|OCI|HOLD|stage06_core_or_lineage_mismatch'
}
foreach($entry in $rows){
  if($entry.native_openbsd_binary -cne 'NOT_BUILT' -or
     -not $entry.status.StartsWith('HOLD_',[StringComparison]::Ordinal)){
    throw 'ALONZ|OCI|HOLD|false_native_claim'
  }
}
$source=Join-Path $root 'packaging/alonz-envelope-stage06/stage06_linux_amd64.cpp'
$sourceText=[IO.File]::ReadAllText($source,[Text.UTF8Encoding]::new($false,$true))
foreach($digest in @($rows[4].witness_sha256,$rows[5].witness_sha256,$rows[6].witness_sha256,
                     $rows[7].witness_sha256,$rows[8].witness_sha256)){
  if(-not $sourceText.Contains($digest,[StringComparison]::Ordinal)){
    throw ('ALONZ|OCI|HOLD|embedded_digest_drift|'+$digest)
  }
}
$compiler=Get-Command 'clang++-18' -ErrorAction SilentlyContinue
$linker=Get-Command 'ld.lld-18' -ErrorAction SilentlyContinue
if($null -eq $compiler -or $null -eq $linker){throw 'ALONZ|OCI|HOLD|clang18_toolchain_missing'}
$version=@(& $compiler.Source --version)
if($LASTEXITCODE -ne 0 -or $version[0] -cnotmatch 'clang version 18[.]'){
  throw 'ALONZ|OCI|HOLD|unexpected_compiler_version'
}
Write-Host ('ALONZ|TOOLCHAIN|COMPILER='+$version[0]+'|SHA256='+((Get-FileHash -LiteralPath $compiler.Source -Algorithm SHA256).Hash.ToLowerInvariant()))
Write-Host ('ALONZ|TOOLCHAIN|LINKER_SHA256='+((Get-FileHash -LiteralPath $linker.Source -Algorithm SHA256).Hash.ToLowerInvariant()))
$head=Join-Path $temp 'alonz-head'
$tail=Join-Path $temp 'alonz-tail'
$flags=@('-std=c++20','-O2','-ffreestanding','-fno-exceptions','-fno-rtti',
         '-nostdlib','-static','-no-pie','-fuse-ld=lld','-Wl,-e,_start',
         '-Wl,--build-id=none','-Wl,-z,noexecstack')
& $compiler.Source @flags $source '-o' $head
if($LASTEXITCODE -ne 0){throw 'ALONZ|OCI|HOLD|head_compile'}
& $compiler.Source @flags $source '-o' $tail
if($LASTEXITCODE -ne 0){throw 'ALONZ|OCI|HOLD|tail_compile'}
$headSha=(Get-FileHash -LiteralPath $head -Algorithm SHA256).Hash.ToLowerInvariant()
$tailSha=(Get-FileHash -LiteralPath $tail -Algorithm SHA256).Hash.ToLowerInvariant()
if($headSha -cne $tailSha){throw 'ALONZ|OCI|HOLD|binary_not_reproducible'}
$context=Join-Path $temp 'image-context'
[IO.Directory]::CreateDirectory($context)|Out-Null
[IO.File]::Copy($head,(Join-Path $context 'stage06'),$true)
[IO.File]::Copy($ledger,(Join-Path $context 'serial-witnesses.jsonl'),$true)
[IO.File]::Copy((Join-Path $root 'ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv'),(Join-Path $context 'native-evidence.tsv'),$true)
[IO.File]::Copy((Join-Path $root 'LICENSE'),(Join-Path $context 'LICENSE'),$true)
[IO.File]::Copy((Join-Path $root 'packaging/alonz-envelope-stage06/Dockerfile'),(Join-Path $context 'Dockerfile'),$true)
Write-Host ('ALONZ|OCI|SOURCE_BINARY_HEAD_TAIL_PASS|SHA256='+$headSha)
& docker build --network=none --pull=false --no-cache --platform linux/amd64 -f (Join-Path $context 'Dockerfile') -t $ImageTag $context
if($LASTEXITCODE -ne 0){throw 'ALONZ|OCI|HOLD|docker_build_failed'}
$expected=@(
  'ALONZ|STAGE=06|IDENTITY=OPENBSD3|RELEASE=3.9|PASS=NONE|CORE=NONE|HOST=LINUX_AMD64',
  'ALONZ|ADDRESSING|NAME_DISTINCT_FROM_ADDRESS|ADDRESS_DISTINCT_FROM_ROUTE|NO_IMPLIED_AUTHORITY',
  ('ALONZ|PREDECESSOR=OPENBSD4_FIRST|SHA256='+$rows[4].witness_sha256),
  ('ALONZ|STAGE06_WITNESS=OPENBSD3|SHA256='+$rows[5].witness_sha256),
  ('ALONZ|SUCCESSOR=OPENBSD4_RETURN|SHA256='+$rows[6].witness_sha256),
  ('ALONZ|POSTFLIGHT_REFERENCE_ONLY|LATER_COREAO=OPENBSD2|NOT_STAGE06_INPUT=1|YIN='+$rows[7].witness_sha256+'|YANG='+$rows[8].witness_sha256),
  'ALONZ|NATIVE_OPENBSD_BUILDS=0|LICENSE_REVIEW=HOLD|RELEASE_STATUS=HOLD|NETWORK_RUNTIME=NONE'
)
$runFlags=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
            '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$actualHead=@(& docker @runFlags $ImageTag)
if($LASTEXITCODE -ne 0){throw 'ALONZ|OCI|HOLD|docker_head_failed'}
$actualTail=@(& docker @runFlags $ImageTag)
if($LASTEXITCODE -ne 0){throw 'ALONZ|OCI|HOLD|docker_tail_failed'}
if($actualHead.Count -ne $expected.Count -or $actualTail.Count -ne $expected.Count){
  throw 'ALONZ|OCI|HOLD|wrong_runtime_line_count'
}
for($i=0;$i -lt $expected.Count;$i++){
  if(-not [StringComparer]::Ordinal.Equals($actualHead[$i],$expected[$i]) -or
     -not [StringComparer]::Ordinal.Equals($actualTail[$i],$expected[$i])){
    throw ('ALONZ|OCI|HOLD|witness_drift|line='+$i)
  }
}
foreach($line in $actualHead){Write-Host $line}
Write-Host ('ALONZ|OCI|HEAD_TAIL_RUNTIME_PASS|BINARY_SHA256='+$headSha+'|STAGE=06|ALIAS=openbsd4|NATIVE_OPENBSD_BUILDS=0|RELEASE=HOLD')
