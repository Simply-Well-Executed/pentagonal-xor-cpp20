# SPDX-License-Identifier: 0BSD
# MATCHB0Xn9 TANDEM253: 253 MODEL-ONLY nine-entrant waves, not PR validation.
# Nine parallel preparation workers converge on ONE exclusive shared lane.
# The actual BIDI driver still owns its own fixed internal exclusive lock.
# No real native admissions, merges or releases; Git graph is authoritative.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][ValidateSet('MASTER','WORKER')][string]$Mode,
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
  [ValidateRange(1,9)][int]$WorkerIndex=1
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'TANDEM253|HOLD|WINDOWS_REQUIRED'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$utf8=[Text.UTF8Encoding]::new($false,$true)
$repo='Simply-Well-Executed/pentagonal-xor-cpp20'
$api='https://api.github.com/repos/'+$repo
$driver=Join-Path $root 'ci/Invoke-Matchb0xn9Race.ps1'
if(-not [IO.File]::Exists($driver)){throw 'TANDEM253|HOLD|BIDI_DRIVER_MISSING'}
function Hash-Text([string]$inputText){
 return [Convert]::ToHexString(
  [Security.Cryptography.SHA256]::HashData(
   [Text.Encoding]::UTF8.GetBytes($inputText))).ToLowerInvariant()
}
if($Mode -ceq 'MASTER'){
 if([string]::IsNullOrWhiteSpace($env:GH_TOKEN)){throw 'TANDEM253|HOLD|GITHUB_TOKEN_UNAVAILABLE'}
 & pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $out
 if($LASTEXITCODE -ne 0 -or [IO.Directory]::Exists($out)){
  throw 'TANDEM253|HOLD|EVIDENCE_NOT_FRESH_OR_EXTERNAL'
 }
 [void][IO.Directory]::CreateDirectory($out)
 $headers=@{
  Authorization='Bearer '+$env:GH_TOKEN
  Accept='application/vnd.github+json'
  'X-GitHub-Api-Version'='2022-11-28'
  'User-Agent'='swe-tandem253-model-only'
 }
 $registry=@(Import-Csv -LiteralPath (Join-Path $root 'ci/MATCHB0Xn9_FESTIVAL_CUSTODY.tsv') -Delimiter ([char]9) -Encoding utf8)
 if($registry.Count -ne 35){throw 'TANDEM253|HOLD|LIVE_ROSTER_MISMATCH'}
 $gitRefs=Invoke-RestMethod -Uri ($api+'/branches?per_page=100') -Headers $headers
 $branchMap=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
 foreach($branch in $gitRefs){
  if(-not $branchMap.TryAdd([string]$branch.name,$branch)){
   throw 'TANDEM253|HOLD|DUPLICATE_LIVE_GIT_REF'
  }
 }
 if($branchMap.Count -ne 36 -or -not $branchMap.ContainsKey('main')){
  throw 'TANDEM253|HOLD|INVENTORY_DRIFT_OR_PAGINATION'
 }
 $mainSha=[string]$branchMap['main'].commit.sha
 if($mainSha -cnotmatch '^[a-f0-9]{40}$'){throw 'TANDEM253|HOLD|BAD_MAIN_COMMIT'}
 $historical=[Collections.Generic.List[object]]::new()
 $seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
 $behind=0;$zero=0
 foreach($item in $registry){
  if(-not $seen.Add($item.branch) -or -not $branchMap.ContainsKey($item.branch)){
   throw 'TANDEM253|HOLD|MISSING_OR_DUPLICATE_BRANCH'
  }
  if($item.pr -cne '-'){continue}
  $tip=[string]$branchMap[$item.branch].commit.sha
  if($tip -cnotmatch '^[a-f0-9]{40}$'){throw 'TANDEM253|HOLD|BAD_GIT_TIP'}
  $cmp=Invoke-RestMethod -Uri ($api+'/compare/'+$mainSha+'...'+$tip) -Headers $headers
  if($cmp.status -ceq 'behind' -and $cmp.ahead_by -eq 0){
   $class='CONTAINED_IN_MAIN';$behind++
  }elseif($cmp.status -ceq 'identical'){
   $class='CONTAINED_IN_MAIN';$behind++
  }elseif($cmp.status -ceq 'diverged' -and
          ($null -eq $cmp.files -or @($cmp.files).Count -eq 0)){
   $class='ZERO_NET_TREE_DELTA';$zero++
  }else{
   throw ('TANDEM253|HOLD|UNREVIEWED_BRANCH_GIT_DELTA|'+$item.branch)
  }
  if($item.branch -match '[\r\n\t|]'){throw 'TANDEM253|HOLD|UNSAFE_GIT_REF_NAME'}
  $historical.Add([pscustomobject]@{name=[string]$item.branch;git_sha=$tip;history_class=$class})
 }
 if($historical.Count -ne 26 -or $behind -ne 25 -or $zero -ne 1){
  throw 'TANDEM253|HOLD|HISTORICAL_REGISTRY_DRIFT'
 }
 $roster=Join-Path $out 'git-derived-model-roster.tsv'
 $rows=[Collections.Generic.List[string]]::new()
 $rows.Add('name'+[char]9+'git_sha'+[char]9+'history_class')
 foreach($e in $historical){
  $rows.Add($e.name+[char]9+$e.git_sha+[char]9+$e.history_class)
 }
 [IO.File]::WriteAllText($roster,([string]::Join([string][char]10,$rows)+[char]10),$utf8)
 $rosterSha=(Get-FileHash -LiteralPath $roster -Algorithm SHA256).Hash.ToLowerInvariant()
 [IO.File]::WriteAllText((Join-Path $out 'model-roster.sha256'),$rosterSha,$utf8)
 # The shared literal lane name cannot be supplied from a command argument.
 [IO.File]::WriteAllText((Join-Path $out 'singlelane.lock'),'',$utf8)
 [IO.File]::WriteAllText((Join-Path $out 'lane-leases.tsv'),'',$utf8)
 # Independent preflight of nine-admitted / tenth-rejected / duplicates.
 $fake=Join-Path $out 'synthetic-input.tsv'
 [IO.File]::WriteAllText($fake,('1'+[char]9+('a'*40)+[char]9+('b'*64)+[char]9+('c'*64)+[char]10),$utf8)
 & pwsh -NoLogo -NoProfile -NonInteractive -File $driver -Mode SELFTEST -ManifestPath $fake -EvidenceDirectory (Join-Path $out 'synthetic-selftest')
 if($LASTEXITCODE -ne 0){throw 'TANDEM253|HOLD|RACE_PREFLIGHT_FAILED'}
 Write-Host ('TANDEM253|START|WAVES=253|WORKERS=9|BATCH_ENTRANTS=9'+
   '|HISTORICAL_REFS=26|SOURCE=LIVE_GIT|MODE=MODEL_ONLY|RELEASE=HOLD')
 $children=[Collections.Generic.List[Diagnostics.Process]]::new()
 try{
  for([int]$i=1;$i -le 9;$i++){
   $psi=[Diagnostics.ProcessStartInfo]::new((Get-Command pwsh).Source)
   $psi.UseShellExecute=$false
   [void]$psi.Environment.Remove('GH_TOKEN')
   foreach($arg in @('-NoLogo','-NoProfile','-NonInteractive','-File',$PSCommandPath,
      '-Mode','WORKER','-RepositoryRoot',$root,'-EvidenceDirectory',$out,
      '-WorkerIndex',[string]$i)){
    [void]$psi.ArgumentList.Add([string]$arg)
   }
   $child=[Diagnostics.Process]::Start($psi)
   if($null -eq $child){throw 'TANDEM253|HOLD|WORKER_NOT_STARTED'}
   $children.Add($child)
  }
  foreach($child in $children){
   if(-not $child.WaitForExit(900000)){
    $child.Kill($true)
    throw 'TANDEM253|HOLD|WORKER_TIMEOUT'
   }
   if($child.ExitCode -ne 0){throw ('TANDEM253|HOLD|WORKER_EXIT_'+$child.ExitCode)}
  }
  $leases=@([IO.File]::ReadAllLines((Join-Path $out 'lane-leases.tsv'),$utf8))
  if($leases.Count -ne 253){throw ('TANDEM253|HOLD|LEASES_NOT_253|'+$leases.Count)}
  $seenWaves=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
  $manifest=[Collections.Generic.List[string]]::new()
  for([int]$i=0;$i -lt $leases.Count;$i++){
   $fields=$leases[$i].Split([char]9)
   if($fields.Count -ne 4 -or [int]$fields[0] -ne ($i+1) -or
      [int]$fields[1] -lt 1 -or [int]$fields[1] -gt 253 -or
      [int]$fields[2] -lt 1 -or [int]$fields[2] -gt 9 -or
      $fields[3] -cne 'MODEL_ONLY_EXCLUSIVE' -or
      -not $seenWaves.Add($fields[1])){
    throw ('TANDEM253|HOLD|INVALID_EXCLUSIVE_LEASE|INDEX='+$i)
   }
   $waveNumber=[int]$fields[1]
   $waveDir=Join-Path $out ('wave-'+('{0:000}' -f $waveNumber))
   $stamp=Join-Path $waveDir 'model-only-receipt.tsv'
   if(-not [IO.File]::Exists($stamp)){throw 'TANDEM253|HOLD|WAVE_RECEIPT_MISSING'}
   $data=[IO.File]::ReadAllLines($stamp,$utf8)
   if($data.Count -ne 1 -or $data[0] -cnotmatch '^MODEL_ONLY[	]'){
    throw 'TANDEM253|HOLD|INVALID_WAVE_RECEIPT'
   }
   $manifest.Add($data[0])
  }
  $final=Join-Path $out 'tandem253-ledger.tsv'
  [IO.File]::WriteAllText($final,
    ([string]::Join([string][char]10,$manifest)+[char]10),$utf8)
  $finalHash=(Get-FileHash -LiteralPath $final -Algorithm SHA256).Hash.ToLowerInvariant()
  # Re-resolve source of truth. No copied SHA from registry is authoritative.
  $after=Invoke-RestMethod -Uri ($api+'/branches?per_page=100') -Headers $headers
  $afterMap=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
  foreach($b in $after){if(-not $afterMap.TryAdd([string]$b.name,[string]$b.commit.sha)){throw 'TANDEM253|HOLD|DUPLICATE_FINAL_REF'}}
  if($afterMap.Count -ne $branchMap.Count -or $afterMap['main'] -cne $mainSha){
   throw 'TANDEM253|HOLD|FINAL_BRANCH_INVENTORY_CHANGED'
  }
  foreach($e in $historical){
   if(-not $afterMap.ContainsKey($e.name) -or
      $afterMap[$e.name] -cne $e.git_sha){
    throw ('TANDEM253|HOLD|HISTORICAL_REF_MOVED|'+$e.name)
   }
  }
  if((Get-FileHash -LiteralPath $roster -Algorithm SHA256).Hash.ToLowerInvariant() -cne $rosterSha){
   throw 'TANDEM253|HOLD|MODEL_ROSTER_CHANGED'
  }
  Write-Host ('TANDEM253|MODEL_PASS|WAVES=253|NINE_ENTRANTS_EACH'+
   '|PARALLEL_WORKERS=9|SHARED_SINGLE_LANE=SERIAL_EXCLUSIVE'+
   '|GIT_REFS_STABLE=YES|LEDGER_SHA256='+$finalHash+
   '|NATIVE_QUALIFICATION=NOT_CLAIMED|MERGE=NONE|RELEASE=HOLD')
 }finally{
  foreach($child in $children){
   if(-not $child.HasExited){$child.Kill($true)}
   $child.Dispose()
  }
 }
 exit 0
}
# WORKER mode: no network; every incoming ref is the master's live Git
# discovery as pinned by its independently rehashed roster file.
$rosterPath=Join-Path $out 'git-derived-model-roster.tsv'
$roster=@(Import-Csv -LiteralPath $rosterPath -Delimiter ([char]9) -Encoding utf8)
if($roster.Count -ne 26){throw 'TANDEM253|HOLD|ROSTER_WORKER_COUNT'}
$initialHash=[IO.File]::ReadAllText((Join-Path $out 'model-roster.sha256'),$utf8)
if((Get-FileHash -LiteralPath $rosterPath -Algorithm SHA256).Hash.ToLowerInvariant() -cne $initialHash){
 throw 'TANDEM253|HOLD|ROSTER_WORKER_HASH_DRIFT'
}
for([int]$wave=$WorkerIndex;$wave -le 253;$wave+=9){
 $waveDir=Join-Path $out ('wave-'+('{0:000}' -f $wave))
 if([IO.Directory]::Exists($waveDir)){throw 'TANDEM253|HOLD|DUPLICATE_WAVE'}
 [void][IO.Directory]::CreateDirectory($waveDir)
 $visual=[Collections.Generic.List[string]]::new()
 $synthetic=[Collections.Generic.List[string]]::new()
 for([int]$i=0;$i -lt 9;$i++){
  # Rotate across all 26 historical branches, never more than nine per wave.
  $entry=$roster[((($wave-1)*9)+$i)%$roster.Count]
  $visual.Add(([string]($i+1))+[char]9+$entry.name+[char]9+$entry.git_sha+
   [char]9+$entry.history_class+[char]9+'HISTORICAL_WITNESS_NOT_NATIVE')
  $synthetic.Add(([string]($i+1))+[char]9+$entry.git_sha+[char]9+
   (Hash-Text ('MODEL_ONLY_CUSTODY|'+$wave+'|'+$entry.name))+[char]9+
   (Hash-Text ('MODEL_ONLY_NOT_NATIVE_RECEIPT|'+$wave+'|'+$entry.git_sha)))
 }
 $rosterWave=Join-Path $waveDir 'historical-entrants.tsv'
 [IO.File]::WriteAllText($rosterWave,
  ([string]::Join([string][char]10,$visual)+[char]10),$utf8)
 # Deliberately labelled synthetic: never a qualified PR admission.
 $syntheticPath=Join-Path $waveDir 'MODEL_ONLY_DO_NOT_ADMIT.tsv'
 [IO.File]::WriteAllText($syntheticPath,
  ([string]::Join([string][char]10,$synthetic)+[char]10),$utf8)
 $lease=$null
 $timer=[Diagnostics.Stopwatch]::StartNew()
 while($null -eq $lease){
  try{
   $lease=[IO.File]::Open((Join-Path $out 'singlelane.lock'),
     [IO.FileMode]::Open,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
  }catch [IO.IOException]{
   if($timer.ElapsedMilliseconds -gt 900000){throw 'TANDEM253|HOLD|SHARED_LANE_WAIT_EXCEEDED'}
   Start-Sleep -Milliseconds 40
  }
 }
 try{
  $raceOut=Join-Path $waveDir 'model-only-race'
  & pwsh -NoLogo -NoProfile -NonInteractive -File $driver -Mode RACE -ManifestPath $syntheticPath -EvidenceDirectory $raceOut
  if($LASTEXITCODE -ne 0){throw ('TANDEM253|HOLD|SIMULATED_RACE_FAILED|WAVE='+$wave)}
  $forward=Join-Path $raceOut 'forward.tsv'
  $reverse=Join-Path $raceOut 'reverse.tsv'
  if(-not [IO.File]::Exists($forward) -or -not [IO.File]::Exists($reverse)){
   throw ('TANDEM253|HOLD|BIDI_PROOF_MISSING|WAVE='+$wave)
  }
  $f=(Get-FileHash -LiteralPath $forward -Algorithm SHA256).Hash.ToLowerInvariant()
  $r=(Get-FileHash -LiteralPath $reverse -Algorithm SHA256).Hash.ToLowerInvariant()
  $rosterHash=(Get-FileHash -LiteralPath $rosterWave -Algorithm SHA256).Hash.ToLowerInvariant()
  $receipt=Join-Path $waveDir 'model-only-receipt.tsv'
  [IO.File]::WriteAllText($receipt,
   ('MODEL_ONLY'+[char]9+$wave+[char]9+$WorkerIndex+[char]9+
    $rosterHash+[char]9+$f+[char]9+$r+[char]9+'NO_NATIVE_PASS'+[char]10),$utf8)
  # The global exclusive lease covers the entire local counterflow.
  $history=@([IO.File]::ReadAllLines((Join-Path $out 'lane-leases.tsv'),$utf8))
  $ordinal=$history.Count+1
  [IO.File]::AppendAllText((Join-Path $out 'lane-leases.tsv'),
   (([string]$ordinal)+[char]9+([string]$wave)+[char]9+
    ([string]$WorkerIndex)+[char]9+'MODEL_ONLY_EXCLUSIVE'+[char]10),$utf8)
 }finally{$lease.Dispose()}
 Write-Host ('TANDEM253|WAVE='+$wave+'|MODEL_PASS|NINE_BRANCH_WITNESSES'+
  '|SHARED_LANE=EXCLUSIVE|WORKER='+$WorkerIndex+'|RELEASE=HOLD')
}
exit 0
