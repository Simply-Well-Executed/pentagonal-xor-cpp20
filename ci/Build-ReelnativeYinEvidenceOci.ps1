# SPDX-License-Identifier: 0BSD
# Stage-08 YIN witness after distinct stage-07 OpenBSD4 return, plus Linux scratch OCI runtime smoke.
# REELNATIVE is the image display name, NOT an OpenBSD 2 or 2.9 OS binary.
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
  throw 'REELNATIVE|OCI|HOLD|requires_expected_github_hosted_linux'
}
if($ImageTag -cnotmatch '^ghcr[.]io/simply-well-executed/reelnative-yin-evidence:sha-[0-9a-f]{40}$'){
  throw 'REELNATIVE|OCI|HOLD|invalid_immutable_ghcr_image_tag'
}
if(-not [IO.Directory]::Exists($root)){throw 'REELNATIVE|OCI|HOLD|root_missing'}
[IO.Directory]::CreateDirectory($temp)|Out-Null
$ledger=Join-Path $temp 'serial-witnesses.jsonl'
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-OpenBSDSerialCoreao.ps1') -RepositoryRoot $root -TemporaryDirectory (Join-Path $temp 'magiccomb')
if($LASTEXITCODE -ne 0){throw 'REELNATIVE|OCI|HOLD|source_contract_failed'}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Test-OpenBSDNativeEvidence.ps1') -RepositoryRoot $root
if($LASTEXITCODE -ne 0){throw 'REELNATIVE|OCI|HOLD|native_evidence_failed'}
& pwsh -NoLogo -NoProfile -File (Join-Path $root 'ci/Invoke-OpenBSDSerialCoreao.ps1') -RepositoryRoot $root -ReportPath $ledger
if($LASTEXITCODE -ne 0){throw 'REELNATIVE|OCI|HOLD|reconciliation_failed'}
$rows=@([IO.File]::ReadAllLines($ledger,[Text.UTF8Encoding]::new($false,$true)) | ForEach-Object {$_|ConvertFrom-Json})
if($rows.Count -ne 10 -or
   $rows[6].stage -cne 'OPENBSD4_RETURN' -or $rows[6].pass -cne 'RETURN' -or
   $rows[7].previous_sha256 -cne $rows[6].witness_sha256 -or
   $rows[7].pass -cne 'YIN' -or $rows[7].core -cne 'OPENBSD2' -or
   $rows[8].previous_sha256 -cne $rows[7].witness_sha256 -or
   $rows[8].pass -cne 'YANG' -or $rows[8].core -cne 'OPENBSD2' -or
   $rows[7].stage -cne 'OPENBSD2_YIN' -or
   $rows[8].stage -cne 'OPENBSD2_YANG' -or
   $rows[9].stage -cne 'COREAO_OPENBSD2' -or
   $rows[9].coreao_core -cne 'OPENBSD2' -or $rows[9].approved -ne $false -or
   $rows[9].yin_witness_sha256 -cne $rows[7].witness_sha256 -or
   $rows[9].yang_witness_sha256 -cne $rows[8].witness_sha256 -or
   $rows[7].witness_sha256 -ceq $rows[8].witness_sha256){
  throw 'REELNATIVE|OCI|HOLD|lost_yin_yang_or_core'
}
foreach($row in $rows){
  if($row.native_openbsd_binary -cne 'NOT_BUILT' -or
     -not $row.status.StartsWith('HOLD_',[StringComparison]::Ordinal)){
    throw 'REELNATIVE|OCI|HOLD|false_native_build_claim'
  }
}
$source=Join-Path $root 'packaging/reelnative-yin/stage08_linux_amd64.cpp'
$sourceText=[IO.File]::ReadAllText($source,[Text.UTF8Encoding]::new($false,$true))
foreach($digest in @($rows[6].witness_sha256,$rows[7].witness_sha256)){
  if(-not $sourceText.Contains($digest,[StringComparison]::Ordinal)){
    throw ('REELNATIVE|OCI|HOLD|embedded_witness_drift|'+$digest)
  }
}
$compiler=Get-Command 'clang++-18' -ErrorAction SilentlyContinue
if($null -eq $compiler){throw 'REELNATIVE|OCI|HOLD|clang18_absent'}
$compilerVersion=@(& $compiler.Source --version)
if($LASTEXITCODE -ne 0 -or $compilerVersion[0] -cnotmatch 'clang version 18[.]'){
  throw 'REELNATIVE|OCI|HOLD|clang18_version_unverified'
}
$linker=Get-Command 'ld.lld-18' -ErrorAction SilentlyContinue
if($null -eq $linker){throw 'REELNATIVE|OCI|HOLD|lld18_absent'}
Write-Host ('REELNATIVE|OCI|COMPILER='+$compilerVersion[0]+'|COMPILER_SHA256='+((Get-FileHash -LiteralPath $compiler.Source -Algorithm SHA256).Hash.ToLowerInvariant()))
Write-Host ('REELNATIVE|OCI|LINKER_SHA256='+((Get-FileHash -LiteralPath $linker.Source -Algorithm SHA256).Hash.ToLowerInvariant()))
$context=Join-Path $temp 'image-context'
[IO.Directory]::CreateDirectory($context)|Out-Null
$binaryHead=Join-Path $temp 'reelnative-head'
$binaryTail=Join-Path $temp 'reelnative-tail'
$flags=@('-std=c++20','-O2','-ffreestanding','-fno-exceptions','-fno-rtti','-nostdlib',
         '-static','-no-pie','-fuse-ld=lld','-Wl,-e,_start',
         '-Wl,--build-id=none','-Wl,-z,noexecstack')
& $compiler.Source @flags $source '-o' $binaryHead
if($LASTEXITCODE -ne 0){throw 'REELNATIVE|OCI|HOLD|clang_head_failed'}
& $compiler.Source @flags $source '-o' $binaryTail
if($LASTEXITCODE -ne 0){throw 'REELNATIVE|OCI|HOLD|clang_tail_failed'}
$headHash=(Get-FileHash -LiteralPath $binaryHead -Algorithm SHA256).Hash.ToLowerInvariant()
$tailHash=(Get-FileHash -LiteralPath $binaryTail -Algorithm SHA256).Hash.ToLowerInvariant()
if($headHash -cne $tailHash){throw 'REELNATIVE|OCI|HOLD|nonreproducible_native_linux_compile'}
[IO.File]::Copy($binaryHead,(Join-Path $context 'stage08'),$true)
[IO.File]::Copy($ledger,(Join-Path $context 'serial-witnesses.jsonl'),$true)
[IO.File]::Copy((Join-Path $root 'ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv'),(Join-Path $context 'native-evidence.tsv'),$true)
[IO.File]::Copy((Join-Path $root 'LICENSE'),(Join-Path $context 'LICENSE'),$true)
[IO.File]::Copy((Join-Path $root 'packaging/reelnative-yin/Dockerfile'),(Join-Path $context 'Dockerfile'),$true)
Write-Host ('REELNATIVE|OCI|LINUX_AMD64_BINARY_REPRODUCIBLE|SHA256='+$headHash)
& docker build --network=none --pull=false --no-cache --platform linux/amd64 -f (Join-Path $context 'Dockerfile') -t $ImageTag $context
if($LASTEXITCODE -ne 0){throw 'REELNATIVE|OCI|HOLD|docker_build_failed'}
$expected=@(
  'REELNATIVE|STAGE=08|IDENTITY=OPENBSD2_YIN|CORE=OPENBSD2|RELEASE=2.9',
  ('REELNATIVE|PREDECESSOR=OPENBSD4_RETURN|SHA256='+$rows[6].witness_sha256),
  ('REELNATIVE|YIN_WITNESS_SHA256='+$rows[7].witness_sha256),
  'REELNATIVE|NATIVE_OPENBSD_BUILDS=0|RELEASE_STATUS=HOLD|HOST=LINUX_AMD64'
)
$runFlags=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
            '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$head=@(& docker @runFlags $ImageTag)
if($LASTEXITCODE -ne 0){throw 'REELNATIVE|OCI|HOLD|docker_head_failed'}
$tail=@(& docker @runFlags $ImageTag)
if($LASTEXITCODE -ne 0){throw 'REELNATIVE|OCI|HOLD|docker_tail_failed'}
if($head.Count -ne $expected.Count -or $tail.Count -ne $expected.Count){
  throw 'REELNATIVE|OCI|HOLD|wrong_runtime_output_line_count'
}
for($i=0;$i -lt $expected.Count;$i++){
  if(-not [StringComparer]::Ordinal.Equals($head[$i],$expected[$i]) -or
     -not [StringComparer]::Ordinal.Equals($tail[$i],$expected[$i])){
    throw ('REELNATIVE|OCI|HOLD|runtime_witness_drift|'+$i)
  }
}
foreach($line in $head){Write-Host $line}
Write-Host ('REELNATIVE|OCI|DOCKER_RUNTIME_PASS|HEAD_TAIL_EXACT|LINUX_AMD64|STAGE=08|CORE=OPENBSD2|RELEASE=HOLD|IMAGE='+$ImageTag)
