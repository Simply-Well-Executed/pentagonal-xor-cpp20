# SPDX-License-Identifier: 0BSD
# MATCHB0Xn9.1: 1..9 independently qualified PR circuits may share one
# exclusive simplex lane, with two opposing BIDI traversals. Half-duplex,
# not physically simultaneous traffic. These are simulation receipts.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][ValidateSet('RACE','WORKER','SELFTEST')][string]$Mode,
  [Parameter(Mandatory=$true)][string]$ManifestPath,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
  [ValidateSet('FORWARD','REVERSE')][string]$Direction='FORWARD',
  [string]$ManifestSha=''
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$utf8=[Text.UTF8Encoding]::new($false,$true)
$manifest=[IO.Path]::GetFullPath($ManifestPath)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
if(-not [IO.File]::Exists($manifest)){throw 'MATCHB0Xn9|HOLD|manifest_missing'}
$sha=(Get-FileHash -LiteralPath $manifest -Algorithm SHA256).Hash.ToLowerInvariant()
if($ManifestSha -and $ManifestSha -cne $sha){throw 'MATCHB0Xn9|HOLD|manifest_sha_changed'}
$lines=[IO.File]::ReadAllLines($manifest,$utf8)
if($lines.Count -lt 1 -or $lines.Count -gt 9){
  throw ('MATCHB0Xn9|HOLD|MAX_NINE|ACTUAL='+$lines.Count)
}
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$entries=[Collections.Generic.List[object]]::new()
foreach($line in $lines){
  $fields=$line.Split([char]9)
  if($fields.Count -ne 4 -or $fields[0] -cnotmatch '^[1-9][0-9]{0,7}$' -or
     $fields[1] -cnotmatch '^[a-f0-9]{40}$' -or
     $fields[2] -cnotmatch '^[a-f0-9]{64}$' -or
     $fields[3] -cnotmatch '^[a-f0-9]{64}$' -or
     -not $seen.Add($fields[0])){
    throw 'MATCHB0Xn9|HOLD|malformed_duplicate_or_unaudited_source'
  }
  $entries.Add($fields)
}
function Expected-Stream([string]$way){
  $records=[Collections.Generic.List[string]]::new()
  for([int]$step=0;$step -lt $entries.Count;$step++){
    $index=if($way -ceq 'FORWARD'){$step}else{$entries.Count-1-$step}
    $item=$entries[$index]
    $records.Add($way+[char]9+($step+1)+[char]9+
                 $item[0]+[char]9+$item[1]+[char]9+$item[2]+[char]9+$item[3])
  }
  return ([string]::Join([string][char]10,$records)+[char]10)
}
if($Mode -ceq 'WORKER'){
  if(-not [IO.Directory]::Exists($out)){throw 'MATCHB0Xn9|HOLD|lane_missing'}
  $lockPath=Join-Path $out 'singlelane.lock'
  if(-not [IO.File]::Exists($lockPath)){throw 'MATCHB0Xn9|HOLD|lock_missing'}
  $watch=[Diagnostics.Stopwatch]::StartNew()
  $lease=$null
  while($null -eq $lease){
    try {
      # The exclusive handle covers the WHOLE traversal: no overtaking.
      $lease=[IO.File]::Open($lockPath,[IO.FileMode]::Open,
                              [IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
    }catch [IO.IOException]{
      if($watch.ElapsedMilliseconds -gt 90000){throw 'MATCHB0Xn9|HOLD|lane_lease_timeout'}
      Start-Sleep -Milliseconds 40
    }
  }
  try{
    if((Get-FileHash -LiteralPath $manifest -Algorithm SHA256).Hash.ToLowerInvariant() -cne $sha){
      throw 'MATCHB0Xn9|HOLD|manifest_changed_during_traversal'
    }
    $target=Join-Path $out ($Direction.ToLowerInvariant()+'.tsv')
    if([IO.File]::Exists($target)){throw 'MATCHB0Xn9|HOLD|duplicate_direction'}
    [IO.File]::WriteAllText($target,(Expected-Stream $Direction),$utf8)
    [IO.File]::AppendAllText((Join-Path $out 'lane-leases.tsv'),
      ($Direction+[char]9+$sha+[char]9+'EXCLUSIVE_NO_OVERTAKE'+[char]10),$utf8)
  }finally{$lease.Dispose()}
  Write-Host ('MATCHB0Xn9|'+$Direction+'|LAPS='+$entries.Count+
              '|SINGLE_LANE=EXCLUSIVE|BIDI=HALF_DUPLEX|RELEASE=HOLD')
  exit 0
}
if($Mode -ceq 'SELFTEST'){
  if([IO.Directory]::Exists($out)){throw 'MATCHB0Xn9|HOLD|selftest_dir_not_fresh'}
  [void][IO.Directory]::CreateDirectory($out)
  # A 10th independent circuit must always be rejected, no fallback.
  $bad=[Collections.Generic.List[string]]::new()
  for([int]$i=1;$i -le 10;$i++){
    $bad.Add($i+[char]9+('a'*40)+[char]9+('b'*64)+[char]9+('c'*64))
  }
  $ten=Join-Path $out 'ten-circuits.tsv'
  [IO.File]::WriteAllText($ten,([string]::Join([string][char]10,$bad)+[char]10),$utf8)
  & pwsh -NoLogo -NoProfile -NonInteractive -File $PSCommandPath -Mode RACE -ManifestPath $ten -EvidenceDirectory (Join-Path $out 'forbidden-ten')
  if($LASTEXITCODE -eq 0){throw 'MATCHB0Xn9|HOLD|ten_admitted'}
  # Exercise the exact capacity 9 with both independent processes.
  $nine=Join-Path $out 'nine-circuits.tsv'
  [IO.File]::WriteAllText($nine,
    ([string]::Join([string][char]10,@($bad.ToArray()[0..8]))+[char]10),$utf8)
  & pwsh -NoLogo -NoProfile -NonInteractive -File $PSCommandPath -Mode RACE -ManifestPath $nine -EvidenceDirectory (Join-Path $out 'nine-race')
  if($LASTEXITCODE -ne 0){throw 'MATCHB0Xn9|HOLD|nine_race_failed'}
  # Reject duplicate identities even when a manifest has <=9 entries.
  $duplicate=Join-Path $out 'duplicate.tsv'
  [IO.File]::WriteAllText($duplicate,($bad[0]+[char]10+$bad[0]+[char]10),$utf8)
  & pwsh -NoLogo -NoProfile -NonInteractive -File $PSCommandPath -Mode RACE -ManifestPath $duplicate -EvidenceDirectory (Join-Path $out 'forbidden-duplicate')
  if($LASTEXITCODE -eq 0){throw 'MATCHB0Xn9|HOLD|duplicate_admitted'}
  Write-Host 'MATCHB0Xn9|SELFTEST=PASS|NINE_ADMITTED=YES|TEN_REJECTED=YES|DUPLICATES_REJECTED=YES|RELEASE=HOLD'
  exit 0
}
if([IO.Directory]::Exists($out)){throw 'MATCHB0Xn9|HOLD|race_dir_not_fresh'}
[void][IO.Directory]::CreateDirectory($out)
[IO.File]::WriteAllText((Join-Path $out 'singlelane.lock'),'',$utf8)
[IO.File]::WriteAllText((Join-Path $out 'lane-leases.tsv'),'',$utf8)
$children=[Collections.Generic.List[Diagnostics.Process]]::new()
try{
  foreach($way in @('FORWARD','REVERSE')){
    $start=[Diagnostics.ProcessStartInfo]::new((Get-Command pwsh -ErrorAction Stop).Source)
    $start.UseShellExecute=$false
    foreach($arg in @('-NoLogo','-NoProfile','-NonInteractive','-File',$PSCommandPath,
                      '-Mode','WORKER','-ManifestPath',$manifest,'-EvidenceDirectory',$out,
                      '-Direction',$way,'-ManifestSha',$sha)){
      [void]$start.ArgumentList.Add([string]$arg)
    }
    $p=[Diagnostics.Process]::Start($start)
    if($null -eq $p){throw ('MATCHB0Xn9|HOLD|worker_start_'+$way)}
    $children.Add($p)
  }
  foreach($p in $children){
    if(-not $p.WaitForExit(120000)){
      $p.Kill($true)
      throw 'MATCHB0Xn9|HOLD|worker_timeout'
    }
    if($p.ExitCode -ne 0){throw ('MATCHB0Xn9|HOLD|worker_exit_'+$p.ExitCode)}
  }
  foreach($way in @('FORWARD','REVERSE')){
    $target=Join-Path $out ($way.ToLowerInvariant()+'.tsv')
    if(-not [IO.File]::Exists($target) -or
       [IO.File]::ReadAllText($target,$utf8) -cne (Expected-Stream $way)){
      throw ('MATCHB0Xn9|HOLD|opposite_direction_parity_'+$way)
    }
  }
  $leases=[IO.File]::ReadAllLines((Join-Path $out 'lane-leases.tsv'),$utf8)
  if($leases.Count -ne 2){throw 'MATCHB0Xn9|HOLD|lease_count'}
  $directions=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
  foreach($record in $leases){
    $parts=$record.Split([char]9)
    if($parts.Count -ne 3 -or $parts[0] -cnotin @('FORWARD','REVERSE') -or
       $parts[1] -cne $sha -or $parts[2] -cne 'EXCLUSIVE_NO_OVERTAKE' -or
       -not $directions.Add($parts[0])){
      throw 'MATCHB0Xn9|HOLD|lease_record_invalid'
    }
  }
  $forward=(Get-FileHash -LiteralPath (Join-Path $out 'forward.tsv') -Algorithm SHA256).Hash.ToLowerInvariant()
  $reverse=(Get-FileHash -LiteralPath (Join-Path $out 'reverse.tsv') -Algorithm SHA256).Hash.ToLowerInvariant()
  Write-Host ('MATCHB0Xn9|PASS|CIRCUITS='+$entries.Count+
              '|MAX_CIRCUITS=9|BIDI_WORKERS=2|SINGLE_LANE=EXCLUSIVE'+
              '|NO_PASSING=YES|FORWARD_SHA256='+$forward+'|REVERSE_SHA256='+$reverse+
              '|MANIFEST_SHA256='+$sha+'|RELEASE=HOLD')
}finally{
  foreach($p in $children){
    if(-not $p.HasExited){$p.Kill($true)}
    $p.Dispose()
  }
}
