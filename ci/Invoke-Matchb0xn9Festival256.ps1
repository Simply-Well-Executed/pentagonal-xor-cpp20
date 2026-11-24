# SPDX-License-Identifier: 0BSD
# MATCHB0Xn9 Festival. 255 offline model laps + 1 independent native circuit.
# The maximum nine circuits and single exclusive lane are code invariants.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [Parameter(Mandatory=$true)][ValidatePattern('^[a-f0-9]{40}$')][string]$ExpectedHarnessSha
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'FESTIVAL|HOLD|WINDOWS_REQUIRED'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$utf8=[Text.UTF8Encoding]::new($false,$true)
$repo='Simply-Well-Executed/pentagonal-xor-cpp20'
$api='https://api.github.com/repos/'+$repo
if([string]::IsNullOrWhiteSpace($env:GH_TOKEN)){throw 'FESTIVAL|HOLD|TOKEN_ABSENT'}
$headers=@{
 Authorization='Bearer '+$env:GH_TOKEN
 Accept='application/vnd.github+json'
 'X-GitHub-Api-Version'='2022-11-28'
 'User-Agent'='swe-festival256-native'
}
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $out
if($LASTEXITCODE -ne 0 -or [IO.Directory]::Exists($out)){throw 'FESTIVAL|HOLD|UNSAFE_OR_NONFRESH_EVIDENCE'}
[void][IO.Directory]::CreateDirectory($out)
# Registry is a map of branch names, PR ids and batches, NOT a SHA oracle.
# Discover refs and compare commit graphs against the live main ref.
$all=@(Import-Csv -LiteralPath (Join-Path $root 'ci/MATCHB0Xn9_FESTIVAL_CUSTODY.tsv') -Delimiter ([char]9) -Encoding utf8)
if($all.Count -ne 35){throw 'FESTIVAL|HOLD|REVIEW_ROSTER_INCOMPLETE'}
$names=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach($item in $all){
 if([string]::IsNullOrWhiteSpace($item.branch) -or -not $names.Add($item.branch)){
   throw 'FESTIVAL|HOLD|DUPLICATE_OR_EMPTY_BRANCH_NAME'
 }
}
$live=(Invoke-RestMethod -Uri ($api+'/branches?per_page=100') -Headers $headers)
$branchMap=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
foreach($b in $live){
 if([string]::IsNullOrWhiteSpace($b.name) -or
    [string]$b.commit.sha -cnotmatch '^[a-f0-9]{40}$' -or
    -not $branchMap.TryAdd([string]$b.name,$b)){
   throw 'FESTIVAL|HOLD|BAD_BRANCH_REF_LIST'
 }
}
if($branchMap.Count -ge 100 -or $branchMap.Count -ne ($names.Count+1) -or
   -not $branchMap.ContainsKey('main')){
 throw 'FESTIVAL|HOLD|MISSING_NEW_OR_PAGINATED_BRANCH'
}
$mainSha=[string]$branchMap['main'].commit.sha
$main=Invoke-RestMethod -Uri ($api+'/branches/main') -Headers $headers
if($main.commit.sha -cne $mainSha){throw 'FESTIVAL|HOLD|MAIN_REF_CHANGED_AT_DISCOVERY'}
$disposition=[Collections.Generic.List[string]]::new()
$participants=[Collections.Generic.List[object]]::new()
foreach($item in $all){
 if(-not $branchMap.ContainsKey($item.branch)){
   throw ('FESTIVAL|HOLD|BRANCH_REMOVED_OR_RENAMED|'+$item.branch)
 }
 $b=$branchMap[$item.branch]
 $currentSha=[string]$b.commit.sha
 # Git's comparison uses the reachable commit ancestry and tree delta.
 # The current source SHA is obtained live, NOT from the registry.
 $comparison=Invoke-RestMethod -Uri ($api+'/compare/'+$mainSha+'...'+$currentSha) -Headers $headers
 $relationship=[string]$comparison.status
 $fileCount=if($null -eq $comparison.files){0}else{@($comparison.files).Count}
 if($item.pr -cne '-'){
   if($item.batch -cne '001' -or $item.pr -cnotmatch '^[1-9][0-9]*$' -or
      $relationship -cnotin @('ahead','diverged') -or
      $comparison.ahead_by -lt 1 -or $fileCount -lt 1){
     throw ('FESTIVAL|HOLD|CANDIDATE_NO_NEW_GIT_TREE|'+$item.branch)
   }
   $participants.Add([pscustomobject]@{
     branch=[string]$item.branch
     number=[string]$item.pr
     sha=$currentSha
     relationship=$relationship
     changed_files=$fileCount
     audit_path=[string]$comparison.files[0].filename
   })
   $disposition.Add(($item.branch+[char]9+$currentSha+[char]9+
     'CANDIDATE'+[char]9+$item.pr+[char]9+$relationship+[char]9+$fileCount))
 }elseif($item.batch -ceq '-'){
   if($relationship -ceq 'behind' -and $comparison.ahead_by -eq 0){
     $kind='ALREADY_CONTAINED_IN_MAIN'
   }elseif($relationship -ceq 'identical'){
     $kind='ALREADY_CONTAINED_IN_MAIN'
   }elseif($relationship -ceq 'diverged' -and $fileCount -eq 0){
     $kind='NO_NET_TREE_DELTA'
   }else{
     throw ('FESTIVAL|HOLD|UNREVIEWED_NEW_BRANCH_CHANGES|'+$item.branch)
   }
   $disposition.Add(($item.branch+[char]9+$currentSha+[char]9+
     $kind+[char]9+'-'+[char]9+$relationship+[char]9+$fileCount))
 }else{throw 'FESTIVAL|HOLD|INVALID_BATCH_ASSIGNMENT'}
}
# This is a fixed literal safety boundary: NOT a variable, config or option.
if($participants.Count -lt 1 -or $participants.Count -gt 9){
 throw 'FESTIVAL|HOLD|NINE_CIRCUIT_MAX'
}
[IO.File]::WriteAllText((Join-Path $out 'git-derived-branch-disposition.tsv'),
 ('branch'+[char]9+'discovered_git_sha'+[char]9+'disposition'+
  [char]9+'pr'+[char]9+'comparison'+[char]9+'changed_files'+[char]10+
  [string]::Join([string][char]10,$disposition)+[char]10),$utf8)
$openResponse=Invoke-RestMethod -Uri ($api+'/pulls?state=open&per_page=100') -Headers $headers
$open=[Collections.Generic.List[object]]::new()
foreach($candidatePr in $openResponse){
 if($null -eq $candidatePr -or $null -eq $candidatePr.number){
   throw 'FESTIVAL|HOLD|INVALID_PR_LIST_SHAPE'
 }
 $open.Add($candidatePr)
}
if($open.Count -ge 100){throw 'FESTIVAL|HOLD|PR_PAGINATION_REQUIRED'}
$seenPR=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$resolved=[Collections.Generic.List[object]]::new()
$lineage=[Collections.Generic.List[string]]::new()
foreach($entry in $participants){
 if(-not $seenPR.Add($entry.number)){throw 'FESTIVAL|HOLD|DUPLICATE_PR_ID'}
 $matches=@($open | Where-Object {[string]$_.number -ceq $entry.number})
 if($matches.Count -ne 1){throw ('FESTIVAL|HOLD|OPEN_PR_MISSING|'+$entry.number)}
 $pr=$matches[0]
 if($pr.head.sha -cne $entry.sha -or
    $pr.head.ref -cne $entry.branch -or
    $pr.head.repo.full_name -cne $repo -or
    $pr.base.repo.full_name -cne $repo -or
    $pr.state -cne 'open' -or -not $pr.draft){
   throw ('FESTIVAL|HOLD|PR_OR_GIT_REF_DISAGREES|'+$entry.number)
 }
 # The workflow source is a GitHub event commit, not a hand-authored hash.
 if($entry.number -ceq '95' -and $entry.sha -cne $ExpectedHarnessSha){
   throw 'FESTIVAL|HOLD|HARNESS_COMMIT_DRIFT'
 }
 if($entry.number -ceq '95'){
   if($pr.base.ref -cne 'feat/jstar-msvc-process-provenance-20261121'){
     throw 'FESTIVAL|HOLD|STACK_BASE_NOT_APPROVED'
   }
   $prior=@($open | Where-Object {$_.head.ref -ceq $pr.base.ref})
   if($prior.Count -ne 1){throw 'FESTIVAL|HOLD|UPSTREAM_AMBIGUOUS'}
   if($pr.base.sha -cne $prior[0].head.sha){
     $lineage.Add('UPSTREAM_PR34_TO_PR95_CURRENT_SHA_MISMATCH')
   }
 }elseif($pr.base.ref -cne 'main'){
   throw ('FESTIVAL|HOLD|CANDIDATE_BASE_IS_NOT_MAIN|'+$entry.number)
 }
 # Path-scoped commit history: newest modifying commit is *provenance*,
 # not a source-of-truth override. File contents are rehashed later.
 $path=[uri]::EscapeDataString([string]$entry.audit_path)
 $url=$api+'/commits?sha='+$entry.sha+'&path='+$path+'&per_page=1'
 $pathEvents=Invoke-RestMethod -Uri $url -Headers $headers
 $historySha=if($null -ne $pathEvents -and @($pathEvents).Count -gt 0){
   [string](@($pathEvents)[0].sha)
 }else{'UNAVAILABLE'}
 Write-Host ('FESTIVAL|GIT_PATH_HISTORY|PR='+$entry.number+
   '|PATH='+$entry.audit_path+'|LAST_TOUCH='+$historySha+
   '|AUTHORITY=CURRENT_GIT_COMMIT_TREE|RELEASE=HOLD')
 $resolved.Add([pscustomobject]@{
   number=$entry.number;branch=$entry.branch;sha=$entry.sha
   path_commit=$historySha
 })
}
Write-Host ('FESTIVAL|DISCOVERED|REGISTERED_BRANCHES='+$all.Count+
 '|BATCH_001_CIRCUITS='+$resolved.Count+'|GIT_GRAPH_SOURCE=LIVE'+
 '|ROUNDS=255_MODEL_PLUS_1_NATIVE|RELEASE=HOLD')
# Exercises the hard-coded maximum and the one-lane exclusive lock.
$fake=Join-Path $out 'synthetic-input.tsv'
$fakeLine=('1'+[char]9+('a'*40)+[char]9+('b'*64)+[char]9+('c'*64)+[char]10)
[IO.File]::WriteAllText($fake,$fakeLine,$utf8)
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Invoke-Matchb0xn9Race.ps1') -Mode SELFTEST -ManifestPath $fake -EvidenceDirectory (Join-Path $out 'selftest')
if($LASTEXITCODE -ne 0){throw 'FESTIVAL|HOLD|INVARIANT_NEGATIVES_FAILED'}
$passing=[Collections.Generic.List[string]]::new()
$failed=[Collections.Generic.List[string]]::new()
$receipts=[Collections.Generic.List[string]]::new()
foreach($p in $resolved){
 $number=[string]$p.number
 $sha=[string]$p.sha
 $prDir=Join-Path $out ('pr'+$number)
 [void][IO.Directory]::CreateDirectory($prDir)
 $status='HOLD';$phase='255_OFFLINE_MODEL';$reason=''
 $custodySha='0'*64;$proofSha='0'*64
 Write-Host ('FESTIVAL|START|PR='+$number+'|SHA='+$sha+'|RELEASE=HOLD')
 try{
   # The fixed harness proves 255 in-memory MODEL tests only.
   & pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'pentadi/Invoke-HulaoChaos255.ps1') -RepositoryRoot $root -EvidenceDirectory (Join-Path $prDir 'model255') -Rounds 255
   if($LASTEXITCODE -ne 0){throw 'MODEL255_RUN_FAILED'}
   $ledger=Join-Path $prDir 'model255/Hulao_Chaos_255_laps.tsv'
   $sanctuary=Join-Path $prDir 'model255/Hulao_Chaos_256_sanctuary.txt'
   if(-not [IO.File]::Exists($ledger) -or -not [IO.File]::Exists($sanctuary)){throw 'MODEL_RECEIPT_MISSING'}
   $records=@(Import-Csv -LiteralPath $ledger -Delimiter ([char]9) -Encoding utf8)
   if($records.Count -ne 255){throw 'MODEL255_INCOMPLETE'}
   for([int]$n=0;$n -lt 255;$n++){
     if($records[$n].lap -cne ('{0:000}' -f ($n+1)) -or
        $records[$n].result -cne 'PASS' -or
        $records[$n].injection_detected -cne 'PASS' -or
        $records[$n].recovered -cne 'PASS'){
       throw ('MODEL255_LAP_'+($n+1)+'_FAILED')
     }
   }
   if(-not [IO.File]::ReadAllText($sanctuary,$utf8).Contains(
     'LAP=256|MODE=FICTIONAL_SANCTUARY',[StringComparison]::Ordinal)){
     throw 'MODEL256_MISLABELLED'
   }
   $phase='256_NATIVE_CIRCUIT'
   $zip=Join-Path $prDir 'source.zip'
   $extract=Join-Path $prDir 'source'
   Invoke-WebRequest -Uri ($api+'/zipball/'+$sha) -Headers $headers -OutFile $zip -MaximumRedirection 5
   Expand-Archive -LiteralPath $zip -DestinationPath $extract
   $directories=@(Get-ChildItem -LiteralPath $extract -Directory)
   if($directories.Count -ne 1){throw 'PINNED_ZIP_STRUCTURE'}
   $src=$directories[0].FullName
   $files=@(Get-ChildItem -LiteralPath $src -File -Recurse -Force | Sort-Object FullName)
   if($files.Count -lt 250){throw 'INCOMPLETE_PINNED_SOURCE'}
   $rows=[Collections.Generic.List[string]]::new()
   foreach($f in $files){
     $relative=[IO.Path]::GetRelativePath($src,$f.FullName).Replace('\','/')
     if($relative -match '[\r\n\t|]' -or $relative.StartsWith('../',[StringComparison]::Ordinal) -or
        $f.Length -gt 67108864){throw 'UNSAFE_SOURCE_PATH'}
     $digest=(Get-FileHash -LiteralPath $f.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
     $rows.Add($relative+[char]9+$digest)
     if($relative -match '[.]ps(1|m1|d1)$' -and
        -not $relative.StartsWith('Archive/Retired/',[StringComparison]::OrdinalIgnoreCase)){
       $tokens=$null;$errors=$null
       [void][Management.Automation.Language.Parser]::ParseFile($f.FullName,[ref]$tokens,[ref]$errors)
       if(@($errors).Count -gt 0){throw ('SOURCE_PS_SYNTAX_'+$relative)}
     }
   }
   $custody=Join-Path $prDir 'source-custody.tsv'
   [IO.File]::WriteAllText($custody,([string]::Join([string][char]10,$rows)+[char]10),$utf8)
   $custodySha=(Get-FileHash -LiteralPath $custody -Algorithm SHA256).Hash.ToLowerInvariant()
   $saved=$env:GH_TOKEN;$env:GH_TOKEN=''
   try{
     $full=Join-Path $src 'ci/Invoke-RepositoryCircuit.ps1'
     if([IO.File]::Exists($full)){
       $native=Join-Path $prDir 'native256'
       & pwsh -NoLogo -NoProfile -NonInteractive -File $full -RepositoryRoot $src -EvidenceDirectory $native
       if($LASTEXITCODE -ne 0){throw 'NATIVE256_WHOLE_CIRCUIT_FAILED'}
       $proof=Join-Path $native 'race-receipt.json'
       if(-not [IO.File]::Exists($proof)){throw 'NATIVE256_RECEIPT_MISSING'}
       $data=ConvertFrom-Json -InputObject ([IO.File]::ReadAllText($proof,$utf8)) -AsHashtable -Depth 9
       if($data.status -cne 'PASS' -or $data.excite_chain_audit -cne 'PASS' -or
          @($data.stages).Count -lt 7 -or @($data.subproject_failures).Count -ne 0 -or
          @($data.file_failures).Count -ne 0 -or $data.release -cne 'HOLD'){
         throw 'NATIVE256_FAILED_LEDGER'
       }
       foreach($stage in $data.stages){
         if($stage.status -cne 'PASS' -or
            ($null -ne $stage.exit_code -and $stage.exit_code -ne 0)){
           throw ('NATIVE256_STAGE_FAILED_'+$stage.lane)
         }
       }
       $proofSha=(Get-FileHash -LiteralPath $proof -Algorithm SHA256).Hash.ToLowerInvariant()
     }else{
       $tri=Join-Path $src 'ci/Invoke-TriforcePipeline.ps1'
       $whole=Join-Path $src 'ci/Invoke-FullSourceAutostartNativeSuite.ps1'
       if(-not [IO.File]::Exists($tri) -or -not [IO.File]::Exists($whole)){
         throw 'ORIGINAL_WHOLE_SUITE_UNAVAILABLE'
       }
       & pwsh -NoLogo -NoProfile -NonInteractive -File $tri -RepositoryRoot $src -EvidenceDirectory (Join-Path $prDir 'triforce256')
       if($LASTEXITCODE -ne 0){throw 'NATIVE256_TRIFORCE_FAILED'}
       & pwsh -NoLogo -NoProfile -NonInteractive -File $whole -RepositoryRoot $src -OutputDirectory (Join-Path $prDir 'fullnative256') -Platform Windows
       if($LASTEXITCODE -ne 0){throw 'NATIVE256_ROOT_SHARED_CONSUMER_FAILED'}
       $proof=Join-Path $prDir 'legacy256.tsv'
       [IO.File]::WriteAllText($proof,('native256'+[char]9+'PASS'+[char]10+'release'+[char]9+'HOLD'+[char]10),$utf8)
       $proofSha=(Get-FileHash -LiteralPath $proof -Algorithm SHA256).Hash.ToLowerInvariant()
     }
   }finally{$env:GH_TOKEN=$saved}
   $after=@(Get-ChildItem -LiteralPath $src -File -Recurse -Force | Sort-Object FullName)
   if($after.Count -ne $files.Count){throw 'SOURCE_COUNT_MUTATED'}
   for([int]$i=0;$i -lt $after.Count;$i++){
     $name=[IO.Path]::GetRelativePath($src,$after[$i].FullName).Replace('\','/')
     $hash=(Get-FileHash -LiteralPath $after[$i].FullName -Algorithm SHA256).Hash.ToLowerInvariant()
     if(($name+[char]9+$hash) -cne $rows[$i]){throw 'SOURCE_DIGEST_MUTATED'}
   }
   $status='PASS'
   $passing.Add(($number+[char]9+$sha+[char]9+$custodySha+[char]9+$proofSha))
 }catch{
   $reason=[string]$_.Exception.Message
   if($reason.Length -gt 160){$reason=$reason.Substring(0,160)}
   $failed.Add(('PR'+$number+'|'+$phase+'|'+$reason))
   Write-Host ('FESTIVAL|PR='+$number+'|HOLD|PHASE='+$phase+'|REASON='+$reason)
 }
 $receipts.Add(('PR'+$number+[char]9+$sha+[char]9+$status+
                [char]9+$custodySha+[char]9+$proofSha+[char]9+$phase))
 Write-Host ('FESTIVAL|PR='+$number+'|MODEL255=MODEL_ONLY|NATIVE256='+$status+'|RELEASE=HOLD')
}
[IO.File]::WriteAllText((Join-Path $out 'festival-qualification.tsv'),
 ([string]::Join([string][char]10,$receipts)+[char]10),$utf8)
$endMain=Invoke-RestMethod -Uri ($api+'/branches/main') -Headers $headers
if($endMain.commit.sha -cne $mainSha){throw 'FESTIVAL|HOLD|MAIN_ADVANCED'}
# Re-read live Git refs after the complete native run. Never trust the
# earlier registry or a GitHub PR's cached head as current state.
$liveFinal=Invoke-RestMethod -Uri ($api+'/branches?per_page=100') -Headers $headers
$endMap=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
foreach($branch in $liveFinal){
 if(-not $endMap.TryAdd([string]$branch.name,[string]$branch.commit.sha)){
   throw 'FESTIVAL|HOLD|DUPLICATE_REF_IN_FINAL_INVENTORY'
 }
}
if($endMap.Count -ne $branchMap.Count){throw 'FESTIVAL|HOLD|BRANCH_INVENTORY_MOVED'}
foreach($p in $resolved){
 $latest=Invoke-RestMethod -Uri ($api+'/pulls/'+$p.number) -Headers $headers
 if($latest.state -cne 'open' -or $latest.head.sha -cne $p.sha -or
    -not $endMap.ContainsKey($p.branch) -or $endMap[$p.branch] -cne $p.sha){
   throw ('FESTIVAL|HOLD|PR_MOVED|'+$p.number)
 }
}
if($lineage.Count -ne 0){
 throw ('FESTIVAL|HOLD|STACK_ANCESTRY_STALE|'+[string]::Join(';',$lineage))
}
if($failed.Count -ne 0 -or $passing.Count -ne $resolved.Count){
 throw ('FESTIVAL|HOLD|CANDIDATES_NOT_ALL_QUALIFIED|'+[string]::Join(';',$failed))
}
$admitted=Join-Path $out 'qualified-circuits.tsv'
[IO.File]::WriteAllText($admitted,([string]::Join([string][char]10,$passing)+[char]10),$utf8)
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Invoke-Matchb0xn9Race.ps1') -Mode RACE -ManifestPath $admitted -EvidenceDirectory (Join-Path $out 'exclusive-lane')
if($LASTEXITCODE -ne 0){throw 'FESTIVAL|HOLD|BIDI_SHARED_LANE_FAILED'}
Write-Host ('FESTIVAL|PASS|CIRCUITS='+$resolved.Count+
 '|EACH=255_OFFLINE_MODEL_PLUS_1_NATIVE|BIDI=EXCLUSIVE_HALF_DUPLEX'+
 '|MAIN_MERGE_REVIEW_REQUIRED|NO_AUTOMERGE|RELEASE=HOLD')
