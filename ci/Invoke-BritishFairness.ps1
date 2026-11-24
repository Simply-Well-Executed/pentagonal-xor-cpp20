# SPDX-License-Identifier: 0BSD
# BRITISH = FAIR
# BRITISHISH-FAIR/1 | TRI HORN | synthetic adversarial ranking only
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
  [Parameter(Mandatory=$true)][ValidatePattern('^[a-f0-9]{40}$')][string]$SourceSha
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$enc=[Text.UTF8Encoding]::new($false,$true)
if(-not [IO.Directory]::Exists($root) -or [IO.Directory]::Exists($out) -or
  $out -ceq $root -or
  $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
  throw 'BRITISHISH|HOLD|EVIDENCE_BOUNDARY'
}
$source=Join-Path $root 'ci/Invoke-BritishFairness.ps1'
if(-not [IO.File]::Exists($source)){throw 'BRITISHISH|HOLD|SOURCE_MISSING'}
$sourceHash=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant()
$colors=[string[]]@('RED','GREEN','BLUE')
$baseline=[int[]]@(9,9,9)
if($colors.Length -ne 3 -or $baseline.Length -ne 3 -or
  $baseline[0] -ne $baseline[1] -or $baseline[1] -ne $baseline[2]){
  throw 'BRITISHISH|HOLD|THREE_EQUALITY_LINES'
}
[void][IO.Directory]::CreateDirectory($out)
$tab=[string][char]9
$lines=[Collections.Generic.List[string]]::new()
$lines.Add((@('trial','seed_sha256','red_baseline','green_baseline','blue_baseline',
 'selected','red_score','green_score','blue_score','predicted','observed','status') -join $tab))
$counts=[int[]]@(0,0,0)
# Each block of three uses a distinct, reproducible pseudorandom permutation.
# This guarantees equal candidate exposure (85 each), without pretending
# the resulting 255 trials are independent random samples.
for([int]$block=0;$block -lt 85;$block++){
  $digest=[Security.Cryptography.SHA256]::HashData(
    $enc.GetBytes($SourceSha+'|BRITISHISH-FAIR/1|BLOCK='+$block))
  $permutation=[int[]]@(0,1,2)
  for([int]$i=2;$i -ge 1;$i--){
    $j=[int]($digest[2-$i] % ($i+1))
    $swap=$permutation[$i]
    $permutation[$i]=$permutation[$j]
    $permutation[$j]=$swap
  }
  if(@($permutation | Sort-Object -Unique).Count -ne 3){
    throw 'BRITISHISH|HOLD|INVALID_PERMUTATION'
  }
  for([int]$slot=0;$slot -lt 3;$slot++){
    $trial=3*$block+$slot+1
    $selected=$permutation[$slot]
    $counts[$selected]++
    $scores=[int[]]@($baseline[0],$baseline[1],$baseline[2])
    $scores[$selected]+=1
    $predicted=$colors[$selected]
    # Independent sorted-score check, rather than trusting the selected index.
    $max=($scores | Measure-Object -Maximum).Maximum
    $leaders=@(for([int]$k=0;$k -lt 3;$k++){
      if($scores[$k] -eq $max){$colors[$k]}
    })
    $actual=if($leaders.Count -eq 1){$leaders[0]}else{'TIE'}
    $pass=($actual -ceq $predicted -and
      $baseline[0] -eq 9 -and $baseline[1] -eq 9 -and $baseline[2] -eq 9)
    # Counterfactual canary: original unmodified scores remain a three-way tie.
    $controlLeaders=@(for([int]$k=0;$k -lt 3;$k++){
      if($baseline[$k] -eq 9){$colors[$k]}
    })
    if($controlLeaders.Count -ne 3){$pass=$false}
    if(-not $pass){throw ('BRITISHISH|HOLD|RANKING_TRIAL='+$trial)}
    $lines.Add((@($trial,[Convert]::ToHexString($digest).ToLowerInvariant(),9,9,9,
      $colors[$selected],$scores[0],$scores[1],$scores[2],
      $predicted,$actual,'PASS') -join $tab))
  }
}
if($lines.Count -ne 256 -or $counts[0] -ne 85 -or
   $counts[1] -ne 85 -or $counts[2] -ne 85){
  throw 'BRITISHISH|HOLD|UNBALANCED_OR_INCOMPLETE'
}
if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant() -cne $sourceHash){
  throw 'BRITISHISH|HOLD|SOURCE_MUTATED'
}
$receipt=Join-Path $out 'britishish-fair-trihorn.tsv'
[IO.File]::WriteAllLines($receipt,$lines,$enc)
if([IO.File]::ReadAllLines($receipt,$enc).Count -ne 256){
  throw 'BRITISHISH|HOLD|RECEIPT_ROUNDTRIP'
}
$sha=(Get-FileHash -LiteralPath $receipt -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('BRITISHISH|THREE_EQUALITY_LINES=RED_GREEN_BLUE|BASELINE=9_9_9'+
 '|TRIALS=255|ALLOCATIONS=85_85_85|PREDICTION=PASS'+
 '|SYNTHETIC_BIAS=DISCLOSED|SOURCE_UNCHANGED=YES|SHA256='+$sha+'|RELEASE=HOLD')
