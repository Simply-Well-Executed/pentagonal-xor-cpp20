# SPDX-License-Identifier: 0BSD
# NULL-OPTIMIZER-001: symbolic matrix and integrity probe, not runtime optimization.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
if(-not [IO.Directory]::Exists($root) -or $out -ceq $root -or
   $out.StartsWith(($root+[IO.Path]::DirectorySeparatorChar),[StringComparison]::OrdinalIgnoreCase)){
  throw 'NULL_OPTIMIZER|HOLD|UNSAFE_EVIDENCE_BOUNDARY'
}
if([IO.Directory]::Exists($out)){throw 'NULL_OPTIMIZER|HOLD|EVIDENCE_NOT_FRESH'}
$source=Join-Path $root 'ci/Invoke-NullOptimizer.ps1'
if(-not [IO.File]::Exists($source)){throw 'NULL_OPTIMIZER|HOLD|SOURCE_MISSING'}
$before=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant()
$utf8=[Text.UTF8Encoding]::new($false,$true)
# Separate source absence, explicit null, empty string, zero, unknown, and ∅ glyph.
$missing=@{}
$present=@{value=$null}
[string]$empty=''
[string]$unknown='?'
[char]$emptySetGlyph=[char]0x2205
if($missing.ContainsKey('value') -or -not $present.ContainsKey('value') -or
   $null -ne $present['value'] -or $empty.Length -ne 0 -or
   $unknown -cne '?' -or [int]$emptySetGlyph -ne 8709 -or 0 -eq $null){
  throw 'NULL_OPTIMIZER|HOLD|STATE_COLLISION'
}

# Exact original sentence; isolates balance and end before newline, never mutate it.
$phrase="I'M NOT GOING TO START A FIGHT WITH A WOMAN WHO KNOWS HOW TO KICK MY ASS SO HARD I'LL CRY I DON'T THINK IT SHOULD WORK BUT TRY PLAYING WITH IT AND:"
$lri=[char]0x2066
$rli=[char]0x2067
$fsi=[char]0x2068
$pdi=[char]0x2069
function Test-Isolates([string]$Value){
  [int]$depth=0
  foreach($unit in $Value.ToCharArray()){
    if($unit -ceq $lri -or $unit -ceq $rli -or $unit -ceq $fsi){
      $depth++
    }elseif($unit -ceq $pdi){
      if($depth -le 0){return $false}
      $depth--
    }elseif($unit -ceq [char]10 -or $unit -ceq [char]13){
      if($depth -ne 0){return $false}
    }
  }
  return ($depth -eq 0)
}
$wrapped=([string]$lri)+$phrase+([string]$pdi)
if(-not (Test-Isolates $wrapped) -or
   (Test-Isolates (([string]$lri)+$phrase)) -or
   (Test-Isolates ($phrase+([string]$pdi))) -or
   (Test-Isolates (([string]$lri)+'A'+([char]10)+([string]$pdi))) -or
   $wrapped.Substring(1,$wrapped.Length-2) -cne $phrase){
  throw 'NULL_OPTIMIZER|HOLD|BIDI_GUARD'
}

# Exhaustive 8-bit algebraic identity checks; no null-to-zero coercion.
for([int]$n=0;$n -lt 256;$n++){
  if(($n -bxor 0) -ne $n -or ($n -band 255) -ne $n -or
     ($n -bor 0) -ne $n -or ($n -bxor $n) -ne 0){
    throw ('NULL_OPTIMIZER|HOLD|BITWISE_IDENTITY|'+$n)
  }
}
# COREAO STANDARDIZER/1: check XOR before OR, including masked corruption.
[byte]$left=0x96
[byte]$right=0x69
[byte]$envelope=0x24
for([int]$n=1;$n -le 255;$n++){
  [byte]$core=$n
  [byte]$shell=($left -bxor ($right -bxor $core))
  [byte]$oracle=(($left -bxor $right) -bxor $core)
  [byte]$bad=($core -bxor 0x04)
  [byte]$badShell=($left -bxor ($right -bxor $bad))
  if($shell -ne $oracle -or $badShell -eq $shell){
    throw ('NULL_OPTIMIZER|HOLD|XOR_CANARY|'+$n)
  }
  if(($envelope -bor $shell) -ne ($envelope -bor $badShell)){
    throw ('NULL_OPTIMIZER|HOLD|UNEXPECTED_OR_MASK|'+$n)
  }
}

# Cross-language entries are HOLD unless separately qualified by LadderDi.
$lanes=@(
  'HeptaDi|Lua 3.2.2|heptadi/HeptaDiLua3L.lua|HOLD_UNTESTED',
  'OctaDi|Python 2.7.18 and independent Python 3|octadi/G.py|HOLD_UNTESTED',
  'NonaDi|Node/npm|nonadi/Invoke-NonaDiSelfExclude.ps1|SELF_EXCLUDE',
  'EnneaDi|native rustc O2|enneadi/EnneaDi.rs|HOLD_UNTESTED',
  'DecaDi|Go direct from Google only|decadi/DecaDi.go|HOLD_UNTESTED',
  'UnDecaDi|Microsoft MASM x64|undecadi/UnDecaDi.asm|HOLD_UNTESTED',
  'DoDecaDi|Perl|dodecadi/DoDecaDi.pl|HOLD_UNTESTED',
  'TriDecaDi|native OCaml|tridecadi/TriDecaDi.ml|HOLD_UNTESTED'
)
$families=@('Di','Pi','PiPi','Sigma','SigmaSigma','SigmaSigmaSigma','SigmaSigmaSigmanull','PiSigmaPiSigma')
$lines=[Collections.Generic.List[string]]::new()
$lines.Add([string]::Join([string][char]9,@('lane','family','runtime','source','status','null_semantics')))
$unique=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach($lane in $lanes){
  $parts=$lane.Split([char]'|')
  if($parts.Count -ne 4){throw 'NULL_OPTIMIZER|HOLD|LANE_SCHEMA'}
  foreach($family in $families){
    if(-not $unique.Add($parts[0]+'|'+$family)){throw 'NULL_OPTIMIZER|HOLD|DUPLICATE_CELL'}
    $lines.Add([string]::Join([string][char]9,@(
      $parts[0],$family,$parts[1],$parts[2],$parts[3],
      'UNDEFINED_REQUIRES_EXPLICIT_OPERAND_AND_NULL_RULE'
    )))
  }
}
if($unique.Count -ne 64 -or $lines.Count -ne 65){throw 'NULL_OPTIMIZER|HOLD|CELL_COUNT'}
[void][IO.Directory]::CreateDirectory($out)
$receipt=Join-Path $out 'NULL_OPTIMIZER_MATRIX.tsv'
[IO.File]::WriteAllText($receipt,([string]::Join([string][char]10,$lines)+[char]10),$utf8)
$digest=(Get-FileHash -LiteralPath $receipt -Algorithm SHA256).Hash.ToLowerInvariant()
$after=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant()
if($after -cne $before){throw 'NULL_OPTIMIZER|HOLD|SOURCE_MUTATED'}
$tail=Join-Path $out 'NULL_OPTIMIZER_HEAD_TAIL.txt'
$summary=@('id=NULL-OPTIMIZER-001','contract=PASS', 'runtime_matrix=HOLD',
 'cells=64','exhaustive_bitwise=256','xor_or_canary=255','bidi=PASS',
 'node=SELF_EXCLUDE',('source_sha256='+$before),('matrix_sha256='+$digest),
 'release=HOLD')
[IO.File]::WriteAllText($tail,([string]::Join([string][char]10,$summary)+[char]10),$utf8)
Write-Host ('NULL_OPTIMIZER|CONTRACT_PASS|CELLS=64|BITWISE=256|BIDI=PASS|LANGUAGES=HOLD|RELEASE=HOLD|TSV_SHA256='+$digest)
