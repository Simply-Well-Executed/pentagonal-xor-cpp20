# SPDX-License-Identifier: 0BSD
# Supplemental evidence contract v1. No native OpenBSD compilation is claimed.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [string]$EvidencePath
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$utf8=[Text.UTF8Encoding]::new($false,$true)
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$evidence=if($EvidencePath){[IO.Path]::GetFullPath($EvidencePath)}else{Join-Path $root 'ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv'}
$manifest=Join-Path $root 'ci/OPENBSD_SERIAL_COREAO.tsv'
$lock8=Join-Path $root 'ci/OPENBSD_80_SOURCE_LOCK.tsv'
$lock7=Join-Path $root 'ci/OPENBSD_79_SOURCE_LOCK.tsv'
foreach($path in @($evidence,$manifest,$lock8,$lock7)){
  if(-not [IO.File]::Exists($path)){throw ('OPENBSD|EVIDENCE|HOLD|missing_file|'+$path)}
  if(((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0){
    throw 'OPENBSD|EVIDENCE|HOLD|symlink'
  }
}
$old=@([IO.File]::ReadAllLines($manifest,$utf8))
$lines=@([IO.File]::ReadAllLines($evidence,$utf8))
$expectedHeader=@('ordinal','identity','release','pass','fixed_release_ref','published_src_tar_sha256','source_evidence_state','native_host','native_toolchain','native_compile','binary_sha256','test_result','status') -join [char]9
if($lines.Count -ne 11 -or $old.Count -ne 11 -or
   -not [StringComparer]::Ordinal.Equals($lines[0],$expectedHeader)){
  throw 'OPENBSD|EVIDENCE|HOLD|schema_or_row_count'
}
$lockHeader=@('asset','sha256','area','custody_state') -join [char]9
$sourceChecksums=@{}
foreach($lock in @(@{release='8.0';path=$lock8},@{release='7.9';path=$lock7})){
  $items=@([IO.File]::ReadAllLines($lock.path,$utf8))
  if($items.Count -ne 6 -or -not [StringComparer]::Ordinal.Equals($items[0],$lockHeader)){
    throw ('OPENBSD|EVIDENCE|HOLD|release_lock_schema|'+$lock.release)
  }
  $sourceRow=$items[1].Split([char]9)
  if($sourceRow.Count -ne 4 -or $sourceRow[0] -cne 'src.tar.gz' -or
     $sourceRow[1] -cnotmatch '^[0-9a-f]{64}$'){
    throw ('OPENBSD|EVIDENCE|HOLD|release_source_hash|'+$lock.release)
  }
  $sourceChecksums[$lock.release]=$sourceRow[1]
}
# These two identifiers were independently transcribed from the official release
# SHA256.sig publications. This DOES NOT verify signatures or archive contents.
if($sourceChecksums['8.0'] -cne '9984c74ff875b5ff041e4676a7f3e4e1ead2cc47a284d705eb9f37cbec4fe795' -or
   $sourceChecksums['7.9'] -cne 'fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b'){
  throw 'OPENBSD|EVIDENCE|HOLD|published_source_hash_drift'
}
$expected7=@(
  'src.tar.gz|fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b',
  'sys.tar.gz|c9ef294021ef7aafd5f18ffe8ebfed63394e5a86d65d7436d6db78551f9d57f1',
  'ports.tar.gz|937aef3d19bc288a838bfa168733872c1a33064db7fe00caf60ea29d9476c6db',
  'xenocara.tar.gz|da1f8ab900591f546d6b268877abb260e6acae5bb55265a0967e27d764e9cb36',
  'openbsd-79-base.pub|b7ee8e7981549a1f707a4ed05115a99959a5f9e34b9bed9f9a098be17dd7e4b1'
)
$lock7Rows=[IO.File]::ReadAllLines($lock7,$utf8)
for($i=0;$i -lt 5;$i++){
  $f=$lock7Rows[$i+1].Split([char]9)
  if($f.Count -ne 4 -or (($f[0]+'|'+$f[1]) -cne $expected7[$i]) -or
     ($i -lt 4 -and $f[3] -cne 'PUBLISHED_SHA256_SIG_METADATA_ONLY') -or
     ($i -eq 4 -and $f[3] -cne 'TRUST_ANCHOR_UNATTESTED')){
    throw ('OPENBSD|EVIDENCE|HOLD|79_source_lock_drift|'+($i+1))
  }
}
$distinct=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
for($i=1;$i -le 10;$i++){
  $m=$old[$i].Split([char]9)
  $e=$lines[$i].Split([char]9)
  if($m.Count -ne 8 -or $e.Count -ne 13 -or -not $distinct.Add($e[1])){
    throw ('OPENBSD|EVIDENCE|HOLD|invalid_stage|'+$i)
  }
  for($col=0;$col -lt 4;$col++){
    if(-not [StringComparer]::Ordinal.Equals($e[$col],$m[$col])){
      throw ('OPENBSD|EVIDENCE|HOLD|canonical_stage_drift|'+$i)
    }
  }
  $tag='OPENBSD_'+$m[2].Replace('.','_')+'_BASE'
  $mode=if($i -eq 10){'DERIVED_FROM_YIN_YANG_NO_NEW_BUILD'}
        elseif($i -le 2){'PUBLISHED_HASH_ONLY'}
        else{'CVS_TAG_REFERENCE_ONLY'}
  $hash=if($i -le 2){$sourceChecksums[$m[2]]}else{'UNVERIFIED'}
  $compile=if($i -eq 10){'NOT_APPLICABLE'}else{'NOT_EXECUTED'}
  if($e[4] -cne $tag -or $e[5] -cne $hash -or $e[6] -cne $mode -or
     $e[7] -cne 'UNATTESTED' -or $e[8] -cne 'UNATTESTED' -or
     $e[9] -cne $compile -or $e[10] -cne 'NONE' -or
     $e[11] -cne 'NOT_EXECUTED' -or $e[12] -cne 'HOLD'){
    throw ('OPENBSD|EVIDENCE|HOLD|unattested_promotion_or_source_drift|'+$i)
  }
}
if($old[8].Split([char]9)[4] -cne 'OPENBSD2' -or
   $old[9].Split([char]9)[4] -cne 'OPENBSD2' -or
   $old[10].Split([char]9)[4] -cne 'OPENBSD2'){
  throw 'OPENBSD|EVIDENCE|HOLD|original_core_discarded'
}
function Get-FileSha256([string]$path){
  return [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData([IO.File]::ReadAllBytes($path))).ToLowerInvariant()
}
Write-Host ('OPENBSD|EVIDENCE|METADATA_VALID|STAGES=10|NATIVE_BUILDS=0|CORE=OPENBSD2|RELEASE=HOLD|SIGNATURES_NOT_VERIFIED|79_LOCK_SHA256='+(Get-FileSha256 $lock7)+'|EVIDENCE_SHA256='+(Get-FileSha256 $evidence))
