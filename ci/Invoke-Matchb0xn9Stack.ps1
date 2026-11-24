# SPDX-License-Identifier: 0BSD
# MATCHB0Xn9 stack circuit; no merge, release, source rewrite or history erase.
# Discover linear linked PRs from the verified live GitHub PR graph (<=9).
# Every independently pinned PR runs a *whole* native circuit before race.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$HarnessRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
  [Parameter(Mandatory=$true)][ValidatePattern('^[a-f0-9]{40}$')][string]$ExpectedTopSha
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'MATCHB0Xn9|HOLD|windows_native_required'}
$harness=[IO.Path]::GetFullPath($HarnessRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
if([IO.Directory]::Exists($out)){throw 'MATCHB0Xn9|HOLD|scratchpad_point0_not_fresh'}
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $harness 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $harness -EvidencePath $out
if($LASTEXITCODE -ne 0){throw 'MATCHB0Xn9|HOLD|unsafe_external_evidence'}
[void][IO.Directory]::CreateDirectory($out)
$utf8=[Text.UTF8Encoding]::new($false,$true)
$repo='Simply-Well-Executed/pentagonal-xor-cpp20'
$token=$env:GH_TOKEN
if([string]::IsNullOrWhiteSpace($token)){throw 'MATCHB0Xn9|HOLD|token_unavailable'}
$headers=@{
 Authorization='Bearer '+$token
 Accept='application/vnd.github+json'
 'X-GitHub-Api-Version'='2022-11-28'
 'User-Agent'='swe-matchb0xn9-sha-pinned'
}
$baseUri='https://api.github.com/repos/'+$repo
$open=@(Invoke-RestMethod -Uri ($baseUri+'/pulls?state=open&per_page=100') -Headers $headers)
if($open.Count -eq 0 -or $open.Count -ge 100){
  throw 'MATCHB0Xn9|HOLD|open_pr_inventory_missing_or_paginated'
}
$target=@($open|Where-Object {$_.number -eq 95})
if($target.Count -ne 1 -or $target[0].head.sha -cne $ExpectedTopSha){
  throw 'MATCHB0Xn9|HOLD|PR95_head_or_inventory_mismatch'
}
$chain=[Collections.Generic.List[object]]::new()
$visited=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$current=$target[0]
while($true){
  if(-not $visited.Add([string]$current.number) -or $chain.Count -ge 9){
    throw 'MATCHB0Xn9|HOLD|cycle_or_more_than_nine_circuits'
  }
  $chain.Insert(0,$current)
  if($current.base.ref -ceq 'main'){break}
  $matches=@($open|Where-Object {$_.head.ref -ceq $current.base.ref})
  if($matches.Count -ne 1 -or
     $matches[0].head.sha -cne $current.base.sha){
    throw 'MATCHB0Xn9|HOLD|missing_or_ambiguous_upstream_pr'
  }
  $current=$matches[0]
}
$current=$target[0]
while($true){
  $children=@($open|Where-Object {$_.base.ref -ceq $current.head.ref})
  if($children.Count -eq 0){break}
  if($children.Count -ne 1 -or
     $children[0].base.sha -cne $current.head.sha -or
     $chain.Count -ge 9 -or
     -not $visited.Add([string]$children[0].number)){
    throw 'MATCHB0Xn9|HOLD|branch_or_more_than_nine_circuits'
  }
  $current=$children[0]
  $chain.Add($current)
}
if($chain.Count -lt 2){throw 'MATCHB0Xn9|HOLD|insufficient_linked_prs'}
foreach($pr in $chain){
  if($pr.state -cne 'open' -or $pr.head.repo.full_name -cne $repo -or
     $pr.base.repo.full_name -cne $repo -or
     [string]$pr.head.sha -cnotmatch '^[a-f0-9]{40}$'){
    throw 'MATCHB0Xn9|HOLD|untrusted_pr_origin_or_sha'
  }
}
Write-Host ('MATCHB0Xn9|STACK_DISCOVERED|PR_COUNT='+$chain.Count+
  '|MAX_CIRCUITS=9|ORDER='+([string]::Join('>',@($chain|ForEach-Object {$_.number})))+
  '|SOURCE=LIVE_GITHUB_PR_GRAPH|RELEASE=HOLD')
# Synthetic tests are never treated as real PR proof.
$raceDriver=Join-Path $harness 'ci/Invoke-Matchb0xn9Race.ps1'
$fake=Join-Path $out 'synthetic-input.tsv'
$synthetic=@(
 ('34'+[char]9+('a'*40)+[char]9+('b'*64)+[char]9+('c'*64)),
 ('95'+[char]9+('d'*40)+[char]9+('e'*64)+[char]9+('f'*64)))
[IO.File]::WriteAllText($fake,([string]::Join([string][char]10,$synthetic)+[char]10),$utf8)
& pwsh -NoLogo -NoProfile -NonInteractive -File $raceDriver -Mode SELFTEST -ManifestPath $fake -EvidenceDirectory (Join-Path $out 'selftest')
if($LASTEXITCODE -ne 0){throw 'MATCHB0Xn9|HOLD|counterflow_selftest_failed'}
. (Join-Path $harness 'ci/Excite-Coreao.ps1')
$signal=Initialize-ExciteEvidence (Join-Path $out 'cross_pr_signals')
if($signal.Sequence -ne 0 -or $signal.PreviousCodeSha -cne ('0'*64)){
  throw 'MATCHB0Xn9|HOLD|point0_genesis_not_clean'
}
$manifest=[Collections.Generic.List[string]]::new()
$holds=[Collections.Generic.List[string]]::new()
foreach($pr in $chain){
  $number=[int]$pr.number
  $sha=[string]$pr.head.sha
  $prDir=Join-Path $out ('pr'+$number)
  [void][IO.Directory]::CreateDirectory($prDir)
  Write-Host ('MATCHB0Xn9|PR='+$number+'|POINT0|SOURCE='+$sha+'|RELEASE=HOLD')
  $custodySha='0'*64
  $receiptSha='0'*64
  $status='HOLD'
  $errorClass=''
  try{
    # Pinned HTTPS source zip only; every PR gets its own workspace.
    $zip=Join-Path $prDir 'source.zip'
    $extract=Join-Path $prDir 'source'
    Invoke-WebRequest -Uri ($baseUri+'/zipball/'+$sha) -Headers $headers -OutFile $zip -MaximumRedirection 5
    Expand-Archive -LiteralPath $zip -DestinationPath $extract
    $roots=@(Get-ChildItem -LiteralPath $extract -Directory)
    if($roots.Count -ne 1){throw 'archive_layout'}
    $source=$roots[0].FullName
    $files=@(Get-ChildItem -LiteralPath $source -File -Recurse -Force|Sort-Object FullName)
    if($files.Count -lt 340){throw 'incomplete_source_files'}
    $rows=[Collections.Generic.List[string]]::new()
    foreach($f in $files){
      $relative=[IO.Path]::GetRelativePath($source,$f.FullName).Replace('\','/')
      if($relative -match '[\r\n\t|]' -or $f.Length -gt 67108864){
        throw 'unsafe_file_path_or_size'
      }
      $digest=(Get-FileHash -LiteralPath $f.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
      $rows.Add($relative+[char]9+$digest)
      if(-not $relative.StartsWith('Archive/Retired/',[StringComparison]::OrdinalIgnoreCase) -and
         $relative -match '[.]ps(m1|d1|1)$'){
        $tokens=$null;$errors=$null
        [void][Management.Automation.Language.Parser]::ParseFile($f.FullName,[ref]$tokens,[ref]$errors)
        if(@($errors).Count -ne 0){throw ('powershell_syntax_'+$relative)}
      }
    }
    $custody=Join-Path $prDir 'source-custody.tsv'
    [IO.File]::WriteAllText($custody,([string]::Join([string][char]10,$rows)+[char]10),$utf8)
    $custodySha=(Get-FileHash -LiteralPath $custody -Algorithm SHA256).Hash.ToLowerInvariant()
    # Full actual circuits; an old PR without EXCITE runs its native
    # TRIFORCE and root/shared/installed consumer suite. Never call it
    # equivalent to the later EXCITE evidence envelope.
    $oldToken=$env:GH_TOKEN
    $env:GH_TOKEN=''
    try{
      $nativeCircuit=Join-Path $source 'ci/Invoke-RepositoryCircuit.ps1'
      if([IO.File]::Exists($nativeCircuit)){
        $evidence=Join-Path $prDir 'whole-circuit'
        & pwsh -NoLogo -NoProfile -NonInteractive -File $nativeCircuit -RepositoryRoot $source -EvidenceDirectory $evidence
        if($LASTEXITCODE -ne 0){throw 'EXCITE_full_circuit_failed'}
        $receipt=Join-Path $evidence 'race-receipt.json'
        if(-not [IO.File]::Exists($receipt)){throw 'EXCITE_receipt_missing'}
        $proof=ConvertFrom-Json -InputObject [IO.File]::ReadAllText($receipt,$utf8) -AsHashtable -Depth 9
        if($proof.status -cne 'PASS' -or $proof.excite_chain_audit -cne 'PASS' -or
           $proof.vehicle_signal_protocol -cne 'DOVIA.SIGNAL.3' -or
           $proof.vehicle_observer_count -ne 1 -or $proof.release -cne 'HOLD'){
          throw 'EXCITE_receipt_rejected'
        }
        $nativeBuilder=Join-Path $source 'ci/Build-ExcitedMarkdown.ps1'
        if(-not [IO.File]::Exists($nativeBuilder)){throw 'EXCITED_missing'}
        $markdownDir=Join-Path $prDir 'excited'
        & pwsh -NoLogo -NoProfile -NonInteractive -File $nativeBuilder -RepositoryRoot $source -OutputDirectory $markdownDir -SourceSha $sha
        if($LASTEXITCODE -ne 0){throw 'EXCITED_native_failed'}
        $tsv=Join-Path $markdownDir 'excited-receipt.tsv'
        $md=Join-Path $markdownDir 'sovieta-diao-xian-excited.md'
        if(-not [IO.File]::Exists($tsv) -or -not [IO.File]::Exists($md)){throw 'EXCITED_receipt_missing'}
        $check=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
        foreach($line in [IO.File]::ReadAllLines($tsv,$utf8)){
          $pair=$line.Split([char]9)
          if($pair.Count -ne 2 -or -not $check.TryAdd($pair[0],$pair[1])){
            throw 'EXCITED_receipt_format'
          }
        }
        if($check.Count -ne 24 -or $check['source_sha'] -cne $sha -or
           $check['schema'] -cne 'EXCITED-STAR-003' -or
           $check['release'] -cne 'HOLD' -or
           $check['markdown_sha256'] -cne
             (Get-FileHash -LiteralPath $md -Algorithm SHA256).Hash.ToLowerInvariant()){
          throw 'EXCITED_receipt_integrity'
        }
        $receiptSha=(Get-FileHash -LiteralPath $receipt -Algorithm SHA256).Hash.ToLowerInvariant()
      }else{
        $triforce=Join-Path $source 'ci/Invoke-TriforcePipeline.ps1'
        $whole=Join-Path $source 'ci/Invoke-FullSourceAutostartNativeSuite.ps1'
        if(-not [IO.File]::Exists($triforce) -or -not [IO.File]::Exists($whole)){
          throw 'legacy_whole_circuit_not_available'
        }
        & pwsh -NoLogo -NoProfile -NonInteractive -File $triforce -RepositoryRoot $source -EvidenceDirectory (Join-Path $prDir 'triforce')
        if($LASTEXITCODE -ne 0){throw 'legacy_triforce_failed'}
        & pwsh -NoLogo -NoProfile -NonInteractive -File $whole -RepositoryRoot $source -OutputDirectory (Join-Path $prDir 'whole-native') -Platform Windows
        if($LASTEXITCODE -ne 0){throw 'legacy_root_shared_consumer_failed'}
        $proofPath=Join-Path $prDir 'legacy-whole-receipt.tsv'
        [IO.File]::WriteAllText($proofPath,('pr'+[char]9+$number+[char]10+
          'source_sha'+[char]9+$sha+[char]10+'custody_sha256'+[char]9+$custodySha+[char]10+
          'native_whole_circuit'+[char]9+'PASS'+[char]10+'release'+[char]9+'HOLD'+[char]10),$utf8)
        $receiptSha=(Get-FileHash -LiteralPath $proofPath -Algorithm SHA256).Hash.ToLowerInvariant()
      }
    }finally{$env:GH_TOKEN=$oldToken}
    # Full immutable replay: neither compiler nor test may change source.
    $after=@(Get-ChildItem -LiteralPath $source -File -Recurse -Force|Sort-Object FullName)
    if($after.Count -ne $files.Count){throw 'source_count_mutated'}
    for([int]$i=0;$i -lt $after.Count;$i++){
      $path=[IO.Path]::GetRelativePath($source,$after[$i].FullName).Replace('\','/')
      $digest=(Get-FileHash -LiteralPath $after[$i].FullName -Algorithm SHA256).Hash.ToLowerInvariant()
      if(($path+[char]9+$digest) -cne $rows[$i]){throw 'source_bytes_mutated'}
    }
    $status='PASS'
  }catch{
    $errorClass=[string]$_.Exception.Message
    if($errorClass.Length -gt 220){$errorClass=$errorClass.Substring(0,220)}
    $holds.Add(('PR'+$number+'|'+$errorClass))
    Write-Host ('MATCHB0Xn9|PR='+$number+'|HOLD|REASON='+$errorClass+'|NO_BYPASS|RELEASE=HOLD')
  }
  $receiptLine=$number+[char]9+$sha+[char]9+$custodySha+[char]9+$receiptSha
  if($status -ceq 'PASS'){$manifest.Add($receiptLine)}
  # Per-PR supervising signal uses one canonical source; not a second camera.
  $sourceMarker=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
      [Text.Encoding]::UTF8.GetBytes($receiptLine))).ToLowerInvariant()
  $null=Add-ExciteVehicle -Context $signal -Kind REPOSITORY -Label ('STACK_PR_'+$number) -SourceSha $sourceMarker -Status $status -Detail ('MATCHB0Xn9_'+$status)
  $null=Assert-ExciteEvidence $signal
  Write-Host ('MATCHB0Xn9|PR='+$number+'|QUALIFICATION='+$status+
    '|CUSTODY_SHA256='+$custodySha+'|RECEIPT_SHA256='+$receiptSha+'|RELEASE=HOLD')
}
$null=Assert-ExciteEvidence $signal
# Re-read live PR SHAs after all circuits to reject drift.
foreach($pr in $chain){
  $fresh=Invoke-RestMethod -Uri ($baseUri+'/pulls/'+$pr.number) -Headers $headers
  if($fresh.head.sha -cne $pr.head.sha -or $fresh.base.sha -cne $pr.base.sha){
    throw ('MATCHB0Xn9|HOLD|PR_MOVED_DURING_TEST|PR='+$pr.number)
  }
}
if($holds.Count -ne 0 -or $manifest.Count -ne $chain.Count){
  throw ('MATCHB0Xn9|HOLD|PR_CIRCUITS_NOT_ALL_GREEN|'+
         [string]::Join(';',@($holds.ToArray())))
}
$manifestFile=Join-Path $out 'qualified-circuits.tsv'
[IO.File]::WriteAllText($manifestFile,
  ([string]::Join([string][char]10,$manifest)+[char]10),$utf8)
& pwsh -NoLogo -NoProfile -NonInteractive -File $raceDriver -Mode RACE -ManifestPath $manifestFile -EvidenceDirectory (Join-Path $out 'real-counterflow')
if($LASTEXITCODE -ne 0){throw 'MATCHB0Xn9|HOLD|real_counterflow_failed'}
Write-Host ('MATCHB0Xn9|STACK_PASS|CIRCUITS='+$chain.Count+
  '|OPPOSITE_HEAD_TAIL=VERIFIED|SINGLE_LANE=EXCLUSIVE|RELEASE=HOLD')
