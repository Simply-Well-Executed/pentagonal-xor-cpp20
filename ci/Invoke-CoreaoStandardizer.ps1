# SPDX-License-Identifier: 0BSD
# COREAO-STANDARDIZER/1. OR alone cannot prove XOR input integrity.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][ValidateSet('Preflight','Round')][string]$Mode,
  [Parameter(Mandatory=$true)][string]$SourceRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
  [ValidateRange(1,255)][int]$Round=1
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'STANDARDIZER|HOLD|WINDOWS_REQUIRED'}
$root=[IO.Path]::GetFullPath($SourceRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
if(-not [IO.Directory]::Exists($root) -or $out -ceq $root -or
  $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
  throw 'STANDARDIZER|HOLD|UNSAFE_EVIDENCE_BOUNDARY'
}
$utf8=[Text.UTF8Encoding]::new($false,$true)
if($Mode -ceq 'Preflight'){
  if([IO.Directory]::Exists($out)){throw 'STANDARDIZER|HOLD|EVIDENCE_NOT_FRESH'}
  foreach($p in @('CMakeLists.txt','ci/Invoke-CoreaoStandardizer.ps1',
    'ci/Invoke-PRParity.ps1','.github/workflows/PR-PARITY.yml',
    '.github/workflows/COREAO-STANDARDIZER.yml')){
    if(-not [IO.File]::Exists((Join-Path $root $p))){
      throw ('STANDARDIZER|HOLD|MISSING|'+$p)
    }
  }
  $cmake=[IO.File]::ReadAllText((Join-Path $root 'CMakeLists.txt'),$utf8)
  foreach($p in @('native_MSVC_only','set(CMAKE_CXX_STANDARD 20)',
    'set(CMAKE_CXX_STANDARD_REQUIRED ON)','set(CMAKE_CXX_EXTENSIONS OFF)',
    'set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)',
    'set(CMAKE_CXX_FLAGS_${_config} "/MD /O2 /DNDEBUG"')){
    if(-not $cmake.Contains($p,[StringComparison]::Ordinal)){
      throw ('STANDARDIZER|HOLD|TOOLCHAIN_MISMATCH|'+$p)
    }
  }
  if($cmake.Contains('"-O3 -DNDEBUG"',[StringComparison]::Ordinal) -or
     $cmake.Contains('"/Od"',[StringComparison]::Ordinal)){
    throw 'STANDARDIZER|HOLD|UNAPPROVED_OPTIMIZATION'
  }
  [void][IO.Directory]::CreateDirectory($out)
  Write-Host 'STANDARDIZER|PREFLIGHT=PASS|NATIVE_MSVC_CXX20_O2_POLICY|RELEASE=HOLD'
  return
}
if(-not [IO.Directory]::Exists($out)){throw 'STANDARDIZER|HOLD|PREFLIGHT_MISSING'}
# RIGHT_CORE = RIGHT XOR CORE; XOR_SHELL = LEFT XOR RIGHT_CORE;
# STANDARD = ENVELOPE OR XOR_SHELL. Integer domain: unsigned 8-bit.
[byte]$left=0x96
[byte]$right=0x69
[byte]$core=$Round
[byte]$envelope=0x24
[byte]$rightCore=($right -bxor $core)
[byte]$shell=($left -bxor $rightCore)
[byte]$standard=($envelope -bor $shell)
[byte]$independent=(($left -bxor $right) -bxor $core)
[byte]$recovered=($left -bxor ($right -bxor $shell))
if($shell -ne $independent -or $recovered -ne $core -or
   ($standard -band $envelope) -ne $envelope -or
   ($standard -band $shell) -ne $shell){
  throw ('STANDARDIZER|HOLD|ALGEBRA|ROUND='+$Round)
}
# Deliberately corrupt an OR-masked bit. The XOR boundary must detect it.
[byte]$badCore=($core -bxor 0x04)
[byte]$badShell=($left -bxor ($right -bxor $badCore))
[byte]$badWrapped=($envelope -bor $badShell)
if($badShell -eq $shell -or $badWrapped -ne $standard){
  throw ('STANDARDIZER|HOLD|MASKING_CANARY|ROUND='+$Round)
}
$fields=@(('{0:000}' -f $Round),('{0:X2}' -f $core),
  ('{0:X2}' -f $rightCore),('{0:X2}' -f $shell),
  ('{0:X2}' -f $standard),'OR_MASKED_BUT_XOR_DETECTED','PASS')
$payload=[string]::Join([string][char]9,$fields)
$sha=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($utf8.GetBytes($payload))).ToLowerInvariant()
$leaf=Join-Path $out ('round-{0:000}.tsv' -f $Round)
$fs=[IO.File]::Open($leaf,[IO.FileMode]::CreateNew,[IO.FileAccess]::Write,[IO.FileShare]::None)
try{
  $bytes=$utf8.GetBytes($payload+[char]9+$sha+[char]10)
  $fs.Write($bytes,0,$bytes.Length)
}finally{$fs.Dispose()}
Write-Host ('STANDARDIZER|ROUND={0:000}|XOR_CORE={1:X2}|XOR_SHELL={2:X2}|OR={3:X2}|CANARY=DETECTED|PASS' -f $Round,$rightCore,$shell,$standard)
