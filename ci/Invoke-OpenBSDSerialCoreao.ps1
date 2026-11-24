# SPDX-License-Identifier: 0BSD
# One-way, ten-step, strictly serial OpenBSD compatibility custody.
# COREAO retains CORE=OPENBSD2; no historical OS binary is claimed.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [string]$ReportPath,
  [string]$ManifestPath
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$utf8=[Text.UTF8Encoding]::new($false,$true)
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$path=if($ManifestPath){[IO.Path]::GetFullPath($ManifestPath)}else{Join-Path $root 'ci/OPENBSD_SERIAL_COREAO.tsv'}
if(-not [IO.File]::Exists($path)){throw 'OPENBSD|SERIAL|HOLD|manifest_missing'}
if(((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0){
  throw 'OPENBSD|SERIAL|HOLD|manifest_symlink'
}
$lines=[IO.File]::ReadAllLines($path,$utf8)
$header=@('ordinal','identity','release','pass','core','build_class','predecessor','source_pin') -join [char]9
if($lines.Count -ne 11 -or -not [StringComparer]::Ordinal.Equals($lines[0],$header)){
  throw 'OPENBSD|SERIAL|HOLD|manifest_schema'
}
$names=@('OPENBSD8','OPENBSD7','OPENBSD6','OPENBSD5','OPENBSD4_FIRST',
         'OPENBSD3','OPENBSD4_RETURN','OPENBSD2_YIN','OPENBSD2_YANG','COREAO_OPENBSD2')
$versions=@('8.0','7.9','6.9','5.9','4.9','3.9','4.9','2.9','2.9','2.9')
$passes=@('NONE','NONE','NONE','NONE','FIRST','NONE','RETURN','YIN','YANG','COREAO')
$cores=@('NONE','NONE','NONE','NONE','NONE','NONE','NONE','OPENBSD2','OPENBSD2','OPENBSD2')
$classes=@('CXX20_CANDIDATE','CXX20_CANDIDATE','CXX20_REVIEW','HISTORICAL_REVIEW',
           'HISTORICAL_REVIEW','HISTORICAL_REVIEW','HISTORICAL_REVIEW',
           'HISTORICAL_REVIEW','HISTORICAL_REVIEW','RECONCILE_ONLY')
$pins=@('BASE_8_0_LOCKED','SOURCE_UNPINNED','SOURCE_UNPINNED','SOURCE_UNPINNED',
        'SOURCE_UNPINNED','SOURCE_UNPINNED','SOURCE_UNPINNED',
        'SOURCE_UNPINNED','SOURCE_UNPINNED','DERIVED_ONLY')
$sourcePath=Join-Path $root 'ci/OPENBSD_80_SOURCE_LOCK.tsv'
if(-not [IO.File]::Exists($sourcePath)){throw 'OPENBSD|SERIAL|HOLD|openbsd8_source_lock_missing'}
if(((Get-Item -LiteralPath $sourcePath -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0){
  throw 'OPENBSD|SERIAL|HOLD|openbsd8_source_lock_symlink'
}
$sourceRows=[IO.File]::ReadAllLines($sourcePath,$utf8)
if($sourceRows.Count -ne 6 -or
   -not $sourceRows[1].StartsWith(('src.tar.gz'+[char]9),[StringComparison]::Ordinal) -or
   -not $sourceRows[2].StartsWith(('sys.tar.gz'+[char]9),[StringComparison]::Ordinal)){
  throw 'OPENBSD|SERIAL|HOLD|openbsd8_source_lock_schema'
}
$sourceSha=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
  [IO.File]::ReadAllBytes($sourcePath))).ToLowerInvariant()
$manifestSha=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
  [IO.File]::ReadAllBytes($path))).ToLowerInvariant()
$previous='0'*64
$yinDigest=$null
$yangDigest=$null
$output=[Collections.Generic.List[string]]::new()
for($index=0;$index -lt 10;$index++){
  $f=$lines[$index+1].Split([char]9)
  $ordinal=($index+1).ToString('00',[Globalization.CultureInfo]::InvariantCulture)
  $from=if($index -eq 0){'START'}else{$names[$index-1]}
  if($f.Count -ne 8 -or
     -not [StringComparer]::Ordinal.Equals($f[0],$ordinal) -or
     -not [StringComparer]::Ordinal.Equals($f[1],$names[$index]) -or
     -not [StringComparer]::Ordinal.Equals($f[2],$versions[$index]) -or
     -not [StringComparer]::Ordinal.Equals($f[3],$passes[$index]) -or
     -not [StringComparer]::Ordinal.Equals($f[4],$cores[$index]) -or
     -not [StringComparer]::Ordinal.Equals($f[5],$classes[$index]) -or
     -not [StringComparer]::Ordinal.Equals($f[6],$from) -or
     -not [StringComparer]::Ordinal.Equals($f[7],$pins[$index])){
    throw ('OPENBSD|SERIAL|HOLD|wrong_stage_or_sequence|' + $ordinal)
  }
  # OS version is NOT a hardware ABI, word width or floating-point format.
  $status=if($index -eq 0){'HOLD_NATIVE_OPENBSD_BUILD_NOT_ATTESTED'}
          elseif($index -eq 9){'HOLD_COREAO_NATIVE_RECONCILIATION_PENDING'}
          elseif($index -ge 3){'HOLD_HISTORICAL_TOOLCHAIN_AND_ABI_UNATTESTED'}
          else{'HOLD_SOURCE_AND_NATIVE_COMPILER_UNATTESTED'}
  if($index -eq 9 -and ($null -eq $yinDigest -or $null -eq $yangDigest -or
                        $yinDigest -ceq $yangDigest)){
    throw 'OPENBSD|SERIAL|HOLD|coreao_requires_distinct_yin_yang'
  }
  $payload=(@($ordinal,$f[1],$f[2],$f[3],$f[4],$f[5],$from,$f[7],
               $status,$previous,$manifestSha,$sourceSha) -join '|')
  if($index -eq 9){$payload+='|'+$yinDigest+'|'+$yangDigest}
  $digest=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
    $utf8.GetBytes($payload))).ToLowerInvariant()
  $item=[ordered]@{
    ordinal=$ordinal;stage=$f[1];release=$f[2];pass=$f[3];
    core=$f[4];build_class=$f[5];predecessor=$from;source_pin=$f[7];
    precision_policy='PRESERVE_SOURCE_PRECISION';hardware_abi='UNATTESTED';
    native_openbsd_binary='NOT_BUILT';status=$status;
    previous_sha256=$previous;witness_sha256=$digest;
    openbsd8_source_lock_sha256=$sourceSha
  }
  if($index -eq 7){$yinDigest=$digest}
  if($index -eq 8){$yangDigest=$digest}
  if($index -eq 9){
    $item['yin_witness_sha256']=$yinDigest
    $item['yang_witness_sha256']=$yangDigest
    $item['coreao_core']='OPENBSD2'
    $item['approved']=$false
  }
  $output.Add(($item|ConvertTo-Json -Depth 3 -Compress))
  Write-Host ('OPENBSD|SERIAL|STEP='+$ordinal+'|TARGET='+$f[1]+'|RELEASE='+$f[2]+
              '|CORE='+$f[4]+'|STATUS='+$status)
  $previous=$digest
}
if($output.Count -ne 10){throw 'OPENBSD|SERIAL|HOLD|incomplete_ledger'}
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
  [IO.File]::WriteAllText([IO.Path]::GetFullPath($ReportPath),($output -join [char]10)+[char]10,$utf8)
}
Write-Host ('OPENBSD|SERIAL|SOURCE_CONTRACT_PASS|STEPS=10|OPENBSD_TARGET_PASSES=9'+
            '|COREAO=OPENBSD2|YIN_YANG_SEPARATE|NONPARALLEL|NATIVE_OPENBSD_BUILDS=0'+
            '|RELEASE=HOLD|MANIFEST_SHA256='+$manifestSha+'|TAIL_SHA256='+$previous)
