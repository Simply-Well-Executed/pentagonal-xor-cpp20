# SPDX-License-Identifier: 0BSD
# PEEK V1: Warcraft III unit-tester-inspired, independent image arenas.
# This is a metaphor for fixture spawning and hostile scenarios, not a claim
# of executing Warcraft or the WTii .w3x map.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$Image,
 [Parameter(Mandatory=$true)][string]$Digest,
 [Parameter(Mandatory=$true)][ValidateSet('openbsd1','openbsd2','openbsd3')][string]$Profile,
 [Parameter(Mandatory=$true)][ValidateSet('GHCR_TRIAL','DOCKERHUB_TRIAL','DOCKERHUB_FINAL')][string]$Phase,
 [string]$ReportPath
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$imageMatch=[regex]::Match($Image,'^(?<registry>(?:ghcr[.]io/simply-well-executed/|paulcheek/)[a-z0-9-]+):(?<tag>[a-z0-9-]+)
$remoteRef=$repository+'@'+$Digest
if($env:GITHUB_SHA -cnotmatch '^[0-9a-f]{40}$' -or
   -not $Image.EndsWith('-sha-'+$env:GITHUB_SHA,[StringComparison]::Ordinal) -or
   -not $Image.Contains($Profile,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|source_commit_or_profile_drift'
}
if($env:GITHUB_ACTIONS -cne 'true'){throw 'PEEK|HOLD|GitHub_worker_expected'}
function Invoke-Docker([string[]]$Arguments,[int]$Expected,[string]$Arena){
 $psi=[Diagnostics.ProcessStartInfo]::new('docker')
 $psi.UseShellExecute=$false
 $psi.RedirectStandardOutput=$true
 $psi.RedirectStandardError=$true
 $psi.CreateNoWindow=$true
 foreach($arg in $Arguments){[void]$psi.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::new()
 $proc.StartInfo=$psi
 if(-not $proc.Start()){throw ('PEEK|HOLD|process_start|'+$Arena)}
 try{
  $stdout=$proc.StandardOutput.ReadToEnd()
  $stderr=$proc.StandardError.ReadToEnd()
  if(-not $proc.WaitForExit(60000)){
   $proc.Kill($true)
   throw ('PEEK|HOLD|arena_timeout|'+$Arena)
  }
  if($proc.ExitCode -ne $Expected){
   throw ('PEEK|HOLD|exit_code|'+$Arena+'|ACTUAL='+$proc.ExitCode+
          '|EXPECTED='+$Expected+'|STDERR='+$stderr.Substring(0,[Math]::Min(120,$stderr.Length)))
  }
  return [pscustomobject]@{Stdout=$stdout;Stderr=$stderr;Exit=$proc.ExitCode}
 }finally{$proc.Dispose()}
}
# First force the published tag to resolve remotely and compare its reported
# digest to the original candidate, then pull its exact content-addressed ref.
# Docker MAY reuse local cached layers: this is NOT a new empty Docker daemon.
# Every "unit" below executes in a NEW read-only isolated container.
$tagPull=Invoke-Docker @('pull',$Image) 0 'pull_published_tag'
$tagDigest=[regex]::Match(($tagPull.Stdout+[char]10+$tagPull.Stderr),'(?m)^Digest:\s*(sha256:[a-f0-9]{64})\s*
$inspect=Invoke-Docker @('image','inspect',$remoteRef) 0 'inspect_image'
try{$image=(ConvertFrom-Json $inspect.Stdout -Depth 20)[0]}catch{
 throw 'PEEK|HOLD|image_metadata_invalid'
}
if($image.Config.Labels.'org.opencontainers.image.revision' -cne $env:GITHUB_SHA -or
   $image.Config.Labels.'com.simplywellexecuted.runtime-profile' -cne $Profile -or
   $image.Config.Labels.'com.simplywellexecuted.native-openbsd-builds' -cne '0' -or
   @($image.Config.Entrypoint).Count -ne 1 -or
   $image.Config.Entrypoint[0] -cne '/opt/swe/app/bin/pentagonal_xor'){
 throw 'PEEK|HOLD|source_or_program_entrypoint_type'
}
$evidence=[string]$image.Config.Labels.'com.simplywellexecuted.evidence-reference'
$expectedEvidence=switch($Profile){
 'openbsd1' {'sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c'}
 'openbsd2' {'sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56'}
 'openbsd3' {'sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1'}
}
if(-not $evidence.EndsWith('@'+$expectedEvidence,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|original_evidence_digest_changed'
}
$restricted=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
 '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$arenas=@(
 @{name='baseline_head';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'},
 @{name='zero_composition';argv=@('0','0','0','0','0');exit=0;expected='Pentagonal XOR: 0.00000000%'},
 @{name='odd_composition';argv=@('1','1','1','0','0');exit=0;expected='Pentagonal XOR: 100.00000000%'},
 @{name='hostile_out_of_range';argv=@('0.8','0.7','1.2','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_not_a_number';argv=@('0.8','0.7','nan','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_missing_arguments';argv=@('0.8','0.7','0.9','0.6');exit=2;error='Usage:'},
 @{name='baseline_tail';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'}
)
$results=[Collections.Generic.List[string]]::new()
foreach($arena in $arenas){
 # Each stage is a fresh container, never the previous stage's mutable process.
 $result=Invoke-Docker ($restricted+@($remoteRef)+$arena.argv) $arena.exit $arena.name
 $got=$result.Stdout.TrimEnd([char[]]@([char]13,[char]10))
 if($arena.ContainsKey('expected')){
  if($got -cne $arena.expected -or -not [string]::IsNullOrEmpty($result.Stderr)){
   throw ('PEEK|HOLD|positive_fixture_wrong_units|'+$arena.name)
  }
 }else{
  if(-not [string]::IsNullOrEmpty($got) -or
     -not $result.Stderr.Contains($arena.error,[StringComparison]::Ordinal)){
   throw ('PEEK|HOLD|hostile_fixture_not_rejected|'+$arena.name)
  }
 }
 $results.Add($arena.name+'|'+$arena.exit+'|'+$got)
 Write-Host ('PEEK|ARENA_PASS|'+$Profile+'|'+$arena.name+'|EXIT='+$arena.exit)
}
# Independent JSON composition arena. JSON typed fields are not W3C-certified.
$jsonArgs=$restricted+@('--entrypoint','/opt/swe/app/bin/pentagonal_control',$remoteRef,
 '0x3fffffff','0x3fffffff','or')
$json=Invoke-Docker $jsonArgs 0 'control_json'
if(-not [string]::IsNullOrEmpty($json.Stderr)){throw 'PEEK|HOLD|control_json_stderr'}
try{$unit=ConvertFrom-Json $json.Stdout -AsHashtable -Depth 30}catch{
 throw 'PEEK|HOLD|control_json_parse'
}
if($unit.schema -cne 'SWE-PENTAGONAL-CXX20-experimental' -or
   $unit.source_mutated -isnot [bool] -or $unit.source_mutated -ne $false -or
   @($unit.stages).Count -ne 6 -or @($unit.blocks).Count -ne 6 -or
   $unit.witness_supplied -isnot [bool] -or $unit.witness_supplied -ne $true){
 throw 'PEEK|HOLD|control_json_types_or_stage_count'
}
$names=@('xor','or','and','nop','cor','flop')
for($i=0;$i -lt 6;$i++){
 if($unit.stages[$i].name -cne $names[$i] -or
    $unit.stages[$i].value -isnot [bool] -or
    $unit.blocks[$i].xor -isnot [bool] -or
    $unit.blocks[$i].or -isnot [bool] -or
    $unit.blocks[$i].and -isnot [bool]){
  throw ('PEEK|HOLD|control_unit_type|'+$i)
 }
}
$results.Add('json_stage_count=6|bool_fields_exact')
$bytes=[Text.UTF8Encoding]::new($false).GetBytes(($results -join [char]10))
$seal=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
 $path=[IO.Path]::GetFullPath($ReportPath)
 if([IO.File]::Exists($path)){throw 'PEEK|HOLD|existing_report_no_overwrite'}
 $record=[ordered]@{
  protocol='PEEK_WTII_INSPIRED_ARENA_TRIAL_V1'
  source_reference='https://www.hiveworkshop.com/threads/wtiis-unit-tester.239333/'
  profile=$Profile;phase=$Phase;source_commit=$env:GITHUB_SHA
  source_image=$Image;observed_registry_digest=$Digest
  original_evidence=$evidence;arenas=$arenas.Count;typed_json_stage_count=6
  result_sha256=$seal;status='PASS';scope='LINUX_AMD64_EXPERIMENT'
  full_native_openbsd_build=$false;warcraft_game_engine_used=$false
 }
 [IO.File]::WriteAllText($path,(ConvertTo-Json $record -Compress)+[char]10,[Text.UTF8Encoding]::new($false))
}
Write-Host ('PEEK|VIRGIN_TRIAL_PASS|PROFILE='+$Profile+'|PHASE='+$Phase+
 '|DIGEST='+$Digest+'|ARENAS='+$arenas.Count+'|JSON_STAGES=6|SHA256='+$seal+
 '|FRESH_ISOLATED_CONTAINERS|REMOTE_DIGEST_PULLED|NO_GAME_ENGINE')
)
if($Digest -cnotmatch '^sha256:[0-9a-f]{64}
$remoteRef=$repository+'@'+$Digest
if($env:GITHUB_SHA -cnotmatch '^[0-9a-f]{40}$' -or
   -not $Image.EndsWith('-sha-'+$env:GITHUB_SHA,[StringComparison]::Ordinal) -or
   -not $Image.Contains($Profile,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|source_commit_or_profile_drift'
}
if($env:GITHUB_ACTIONS -cne 'true'){throw 'PEEK|HOLD|GitHub_worker_expected'}
function Invoke-Docker([string[]]$Arguments,[int]$Expected,[string]$Arena){
 $psi=[Diagnostics.ProcessStartInfo]::new('docker')
 $psi.UseShellExecute=$false
 $psi.RedirectStandardOutput=$true
 $psi.RedirectStandardError=$true
 $psi.CreateNoWindow=$true
 foreach($arg in $Arguments){[void]$psi.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::new()
 $proc.StartInfo=$psi
 if(-not $proc.Start()){throw ('PEEK|HOLD|process_start|'+$Arena)}
 try{
  $stdout=$proc.StandardOutput.ReadToEnd()
  $stderr=$proc.StandardError.ReadToEnd()
  if(-not $proc.WaitForExit(60000)){
   $proc.Kill($true)
   throw ('PEEK|HOLD|arena_timeout|'+$Arena)
  }
  if($proc.ExitCode -ne $Expected){
   throw ('PEEK|HOLD|exit_code|'+$Arena+'|ACTUAL='+$proc.ExitCode+
          '|EXPECTED='+$Expected+'|STDERR='+$stderr.Substring(0,[Math]::Min(120,$stderr.Length)))
  }
  return [pscustomobject]@{Stdout=$stdout;Stderr=$stderr;Exit=$proc.ExitCode}
 }finally{$proc.Dispose()}
}
# Pull a fresh registry manifest reference. Docker MAY reuse local cached blobs.
# Every "unit" below executes in a NEW read-only isolated container.
[void](Invoke-Docker @('pull',$remoteRef) 0 'pull_remote_manifest')
$inspect=Invoke-Docker @('image','inspect',$remoteRef) 0 'inspect_image'
try{$image=(ConvertFrom-Json $inspect.Stdout -Depth 20)[0]}catch{
 throw 'PEEK|HOLD|image_metadata_invalid'
}
if($image.Config.Labels.'org.opencontainers.image.revision' -cne $env:GITHUB_SHA -or
   $image.Config.Labels.'com.simplywellexecuted.runtime-profile' -cne $Profile -or
   $image.Config.Labels.'com.simplywellexecuted.native-openbsd-builds' -cne '0' -or
   @($image.Config.Entrypoint).Count -ne 1 -or
   $image.Config.Entrypoint[0] -cne '/opt/swe/app/bin/pentagonal_xor'){
 throw 'PEEK|HOLD|source_or_program_entrypoint_type'
}
$evidence=[string]$image.Config.Labels.'com.simplywellexecuted.evidence-reference'
$expectedEvidence=switch($Profile){
 'openbsd1' {'sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c'}
 'openbsd2' {'sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56'}
 'openbsd3' {'sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1'}
}
if(-not $evidence.EndsWith('@'+$expectedEvidence,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|original_evidence_digest_changed'
}
$restricted=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
 '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$arenas=@(
 @{name='baseline_head';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'},
 @{name='zero_composition';argv=@('0','0','0','0','0');exit=0;expected='Pentagonal XOR: 0.00000000%'},
 @{name='odd_composition';argv=@('1','1','1','0','0');exit=0;expected='Pentagonal XOR: 100.00000000%'},
 @{name='hostile_out_of_range';argv=@('0.8','0.7','1.2','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_not_a_number';argv=@('0.8','0.7','nan','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_missing_arguments';argv=@('0.8','0.7','0.9','0.6');exit=2;error='Usage:'},
 @{name='baseline_tail';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'}
)
$results=[Collections.Generic.List[string]]::new()
foreach($arena in $arenas){
 # Each stage is a fresh container, never the previous stage's mutable process.
 $result=Invoke-Docker ($restricted+@($remoteRef)+$arena.argv) $arena.exit $arena.name
 $got=$result.Stdout.TrimEnd([char[]]@([char]13,[char]10))
 if($arena.ContainsKey('expected')){
  if($got -cne $arena.expected -or -not [string]::IsNullOrEmpty($result.Stderr)){
   throw ('PEEK|HOLD|positive_fixture_wrong_units|'+$arena.name)
  }
 }else{
  if(-not [string]::IsNullOrEmpty($got) -or
     -not $result.Stderr.Contains($arena.error,[StringComparison]::Ordinal)){
   throw ('PEEK|HOLD|hostile_fixture_not_rejected|'+$arena.name)
  }
 }
 $results.Add($arena.name+'|'+$arena.exit+'|'+$got)
 Write-Host ('PEEK|ARENA_PASS|'+$Profile+'|'+$arena.name+'|EXIT='+$arena.exit)
}
# Independent JSON composition arena. JSON typed fields are not W3C-certified.
$jsonArgs=$restricted+@('--entrypoint','/opt/swe/app/bin/pentagonal_control',$remoteRef,
 '0x3fffffff','0x3fffffff','or')
$json=Invoke-Docker $jsonArgs 0 'control_json'
if(-not [string]::IsNullOrEmpty($json.Stderr)){throw 'PEEK|HOLD|control_json_stderr'}
try{$unit=ConvertFrom-Json $json.Stdout -AsHashtable -Depth 30}catch{
 throw 'PEEK|HOLD|control_json_parse'
}
if($unit.schema -cne 'SWE-PENTAGONAL-CXX20-experimental' -or
   $unit.source_mutated -isnot [bool] -or $unit.source_mutated -ne $false -or
   @($unit.stages).Count -ne 6 -or @($unit.blocks).Count -ne 6 -or
   $unit.witness_supplied -isnot [bool] -or $unit.witness_supplied -ne $true){
 throw 'PEEK|HOLD|control_json_types_or_stage_count'
}
$names=@('xor','or','and','nop','cor','flop')
for($i=0;$i -lt 6;$i++){
 if($unit.stages[$i].name -cne $names[$i] -or
    $unit.stages[$i].value -isnot [bool] -or
    $unit.blocks[$i].xor -isnot [bool] -or
    $unit.blocks[$i].or -isnot [bool] -or
    $unit.blocks[$i].and -isnot [bool]){
  throw ('PEEK|HOLD|control_unit_type|'+$i)
 }
}
$results.Add('json_stage_count=6|bool_fields_exact')
$bytes=[Text.UTF8Encoding]::new($false).GetBytes(($results -join [char]10))
$seal=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
 $path=[IO.Path]::GetFullPath($ReportPath)
 if([IO.File]::Exists($path)){throw 'PEEK|HOLD|existing_report_no_overwrite'}
 $record=[ordered]@{
  protocol='PEEK_WTII_INSPIRED_ARENA_TRIAL_V1'
  source_reference='https://www.hiveworkshop.com/threads/wtiis-unit-tester.239333/'
  profile=$Profile;phase=$Phase;source_commit=$env:GITHUB_SHA
  source_image=$Image;observed_registry_digest=$Digest
  original_evidence=$evidence;arenas=$arenas.Count;typed_json_stage_count=6
  result_sha256=$seal;status='PASS';scope='LINUX_AMD64_EXPERIMENT'
  full_native_openbsd_build=$false;warcraft_game_engine_used=$false
 }
 [IO.File]::WriteAllText($path,(ConvertTo-Json $record -Compress)+[char]10,[Text.UTF8Encoding]::new($false))
}
Write-Host ('PEEK|VIRGIN_TRIAL_PASS|PROFILE='+$Profile+'|PHASE='+$Phase+
 '|DIGEST='+$Digest+'|ARENAS='+$arenas.Count+'|JSON_STAGES=6|SHA256='+$seal+
 '|FRESH_ISOLATED_CONTAINERS|REMOTE_DIGEST_PULLED|NO_GAME_ENGINE')
 -or -not $imageMatch.Success){
 throw 'PEEK|HOLD|image_or_digest_invalid'
}
$repository=$imageMatch.Groups['registry'].Value
$remoteRef=$repository+'@'+$Digest
if($env:GITHUB_SHA -cnotmatch '^[0-9a-f]{40}$' -or
   -not $Image.EndsWith('-sha-'+$env:GITHUB_SHA,[StringComparison]::Ordinal) -or
   -not $Image.Contains($Profile,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|source_commit_or_profile_drift'
}
if($env:GITHUB_ACTIONS -cne 'true'){throw 'PEEK|HOLD|GitHub_worker_expected'}
function Invoke-Docker([string[]]$Arguments,[int]$Expected,[string]$Arena){
 $psi=[Diagnostics.ProcessStartInfo]::new('docker')
 $psi.UseShellExecute=$false
 $psi.RedirectStandardOutput=$true
 $psi.RedirectStandardError=$true
 $psi.CreateNoWindow=$true
 foreach($arg in $Arguments){[void]$psi.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::new()
 $proc.StartInfo=$psi
 if(-not $proc.Start()){throw ('PEEK|HOLD|process_start|'+$Arena)}
 try{
  $stdout=$proc.StandardOutput.ReadToEnd()
  $stderr=$proc.StandardError.ReadToEnd()
  if(-not $proc.WaitForExit(60000)){
   $proc.Kill($true)
   throw ('PEEK|HOLD|arena_timeout|'+$Arena)
  }
  if($proc.ExitCode -ne $Expected){
   throw ('PEEK|HOLD|exit_code|'+$Arena+'|ACTUAL='+$proc.ExitCode+
          '|EXPECTED='+$Expected+'|STDERR='+$stderr.Substring(0,[Math]::Min(120,$stderr.Length)))
  }
  return [pscustomobject]@{Stdout=$stdout;Stderr=$stderr;Exit=$proc.ExitCode}
 }finally{$proc.Dispose()}
}
# Pull a fresh registry manifest reference. Docker MAY reuse local cached blobs.
# Every "unit" below executes in a NEW read-only isolated container.
[void](Invoke-Docker @('pull',$remoteRef) 0 'pull_remote_manifest')
$inspect=Invoke-Docker @('image','inspect',$remoteRef) 0 'inspect_image'
try{$image=(ConvertFrom-Json $inspect.Stdout -Depth 20)[0]}catch{
 throw 'PEEK|HOLD|image_metadata_invalid'
}
if($image.Config.Labels.'org.opencontainers.image.revision' -cne $env:GITHUB_SHA -or
   $image.Config.Labels.'com.simplywellexecuted.runtime-profile' -cne $Profile -or
   $image.Config.Labels.'com.simplywellexecuted.native-openbsd-builds' -cne '0' -or
   @($image.Config.Entrypoint).Count -ne 1 -or
   $image.Config.Entrypoint[0] -cne '/opt/swe/app/bin/pentagonal_xor'){
 throw 'PEEK|HOLD|source_or_program_entrypoint_type'
}
$evidence=[string]$image.Config.Labels.'com.simplywellexecuted.evidence-reference'
$expectedEvidence=switch($Profile){
 'openbsd1' {'sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c'}
 'openbsd2' {'sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56'}
 'openbsd3' {'sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1'}
}
if(-not $evidence.EndsWith('@'+$expectedEvidence,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|original_evidence_digest_changed'
}
$restricted=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
 '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$arenas=@(
 @{name='baseline_head';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'},
 @{name='zero_composition';argv=@('0','0','0','0','0');exit=0;expected='Pentagonal XOR: 0.00000000%'},
 @{name='odd_composition';argv=@('1','1','1','0','0');exit=0;expected='Pentagonal XOR: 100.00000000%'},
 @{name='hostile_out_of_range';argv=@('0.8','0.7','1.2','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_not_a_number';argv=@('0.8','0.7','nan','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_missing_arguments';argv=@('0.8','0.7','0.9','0.6');exit=2;error='Usage:'},
 @{name='baseline_tail';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'}
)
$results=[Collections.Generic.List[string]]::new()
foreach($arena in $arenas){
 # Each stage is a fresh container, never the previous stage's mutable process.
 $result=Invoke-Docker ($restricted+@($remoteRef)+$arena.argv) $arena.exit $arena.name
 $got=$result.Stdout.TrimEnd([char[]]@([char]13,[char]10))
 if($arena.ContainsKey('expected')){
  if($got -cne $arena.expected -or -not [string]::IsNullOrEmpty($result.Stderr)){
   throw ('PEEK|HOLD|positive_fixture_wrong_units|'+$arena.name)
  }
 }else{
  if(-not [string]::IsNullOrEmpty($got) -or
     -not $result.Stderr.Contains($arena.error,[StringComparison]::Ordinal)){
   throw ('PEEK|HOLD|hostile_fixture_not_rejected|'+$arena.name)
  }
 }
 $results.Add($arena.name+'|'+$arena.exit+'|'+$got)
 Write-Host ('PEEK|ARENA_PASS|'+$Profile+'|'+$arena.name+'|EXIT='+$arena.exit)
}
# Independent JSON composition arena. JSON typed fields are not W3C-certified.
$jsonArgs=$restricted+@('--entrypoint','/opt/swe/app/bin/pentagonal_control',$remoteRef,
 '0x3fffffff','0x3fffffff','or')
$json=Invoke-Docker $jsonArgs 0 'control_json'
if(-not [string]::IsNullOrEmpty($json.Stderr)){throw 'PEEK|HOLD|control_json_stderr'}
try{$unit=ConvertFrom-Json $json.Stdout -AsHashtable -Depth 30}catch{
 throw 'PEEK|HOLD|control_json_parse'
}
if($unit.schema -cne 'SWE-PENTAGONAL-CXX20-experimental' -or
   $unit.source_mutated -isnot [bool] -or $unit.source_mutated -ne $false -or
   @($unit.stages).Count -ne 6 -or @($unit.blocks).Count -ne 6 -or
   $unit.witness_supplied -isnot [bool] -or $unit.witness_supplied -ne $true){
 throw 'PEEK|HOLD|control_json_types_or_stage_count'
}
$names=@('xor','or','and','nop','cor','flop')
for($i=0;$i -lt 6;$i++){
 if($unit.stages[$i].name -cne $names[$i] -or
    $unit.stages[$i].value -isnot [bool] -or
    $unit.blocks[$i].xor -isnot [bool] -or
    $unit.blocks[$i].or -isnot [bool] -or
    $unit.blocks[$i].and -isnot [bool]){
  throw ('PEEK|HOLD|control_unit_type|'+$i)
 }
}
$results.Add('json_stage_count=6|bool_fields_exact')
$bytes=[Text.UTF8Encoding]::new($false).GetBytes(($results -join [char]10))
$seal=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
 $path=[IO.Path]::GetFullPath($ReportPath)
 if([IO.File]::Exists($path)){throw 'PEEK|HOLD|existing_report_no_overwrite'}
 $record=[ordered]@{
  protocol='PEEK_WTII_INSPIRED_ARENA_TRIAL_V1'
  source_reference='https://www.hiveworkshop.com/threads/wtiis-unit-tester.239333/'
  profile=$Profile;phase=$Phase;source_commit=$env:GITHUB_SHA
  source_image=$Image;observed_registry_digest=$Digest
  original_evidence=$evidence;arenas=$arenas.Count;typed_json_stage_count=6
  result_sha256=$seal;status='PASS';scope='LINUX_AMD64_EXPERIMENT'
  full_native_openbsd_build=$false;warcraft_game_engine_used=$false
 }
 [IO.File]::WriteAllText($path,(ConvertTo-Json $record -Compress)+[char]10,[Text.UTF8Encoding]::new($false))
}
Write-Host ('PEEK|VIRGIN_TRIAL_PASS|PROFILE='+$Profile+'|PHASE='+$Phase+
 '|DIGEST='+$Digest+'|ARENAS='+$arenas.Count+'|JSON_STAGES=6|SHA256='+$seal+
 '|FRESH_ISOLATED_CONTAINERS|REMOTE_DIGEST_PULLED|NO_GAME_ENGINE')
)
if(-not $tagDigest.Success -or $tagDigest.Groups[1].Value -cne $Digest){
 throw 'PEEK|HOLD|registry_tag_does_not_resolve_to_pinned_manifest'
}
[void](Invoke-Docker @('pull',$remoteRef) 0 'pull_remote_manifest')
$inspect=Invoke-Docker @('image','inspect',$remoteRef) 0 'inspect_image'
try{$image=(ConvertFrom-Json $inspect.Stdout -Depth 20)[0]}catch{
 throw 'PEEK|HOLD|image_metadata_invalid'
}
if($image.Config.Labels.'org.opencontainers.image.revision' -cne $env:GITHUB_SHA -or
   $image.Config.Labels.'com.simplywellexecuted.runtime-profile' -cne $Profile -or
   $image.Config.Labels.'com.simplywellexecuted.native-openbsd-builds' -cne '0' -or
   @($image.Config.Entrypoint).Count -ne 1 -or
   $image.Config.Entrypoint[0] -cne '/opt/swe/app/bin/pentagonal_xor'){
 throw 'PEEK|HOLD|source_or_program_entrypoint_type'
}
$evidence=[string]$image.Config.Labels.'com.simplywellexecuted.evidence-reference'
$expectedEvidence=switch($Profile){
 'openbsd1' {'sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c'}
 'openbsd2' {'sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56'}
 'openbsd3' {'sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1'}
}
if(-not $evidence.EndsWith('@'+$expectedEvidence,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|original_evidence_digest_changed'
}
$restricted=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
 '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$arenas=@(
 @{name='baseline_head';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'},
 @{name='zero_composition';argv=@('0','0','0','0','0');exit=0;expected='Pentagonal XOR: 0.00000000%'},
 @{name='odd_composition';argv=@('1','1','1','0','0');exit=0;expected='Pentagonal XOR: 100.00000000%'},
 @{name='hostile_out_of_range';argv=@('0.8','0.7','1.2','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_not_a_number';argv=@('0.8','0.7','nan','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_missing_arguments';argv=@('0.8','0.7','0.9','0.6');exit=2;error='Usage:'},
 @{name='baseline_tail';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'}
)
$results=[Collections.Generic.List[string]]::new()
foreach($arena in $arenas){
 # Each stage is a fresh container, never the previous stage's mutable process.
 $result=Invoke-Docker ($restricted+@($remoteRef)+$arena.argv) $arena.exit $arena.name
 $got=$result.Stdout.TrimEnd([char[]]@([char]13,[char]10))
 if($arena.ContainsKey('expected')){
  if($got -cne $arena.expected -or -not [string]::IsNullOrEmpty($result.Stderr)){
   throw ('PEEK|HOLD|positive_fixture_wrong_units|'+$arena.name)
  }
 }else{
  if(-not [string]::IsNullOrEmpty($got) -or
     -not $result.Stderr.Contains($arena.error,[StringComparison]::Ordinal)){
   throw ('PEEK|HOLD|hostile_fixture_not_rejected|'+$arena.name)
  }
 }
 $results.Add($arena.name+'|'+$arena.exit+'|'+$got)
 Write-Host ('PEEK|ARENA_PASS|'+$Profile+'|'+$arena.name+'|EXIT='+$arena.exit)
}
# Independent JSON composition arena. JSON typed fields are not W3C-certified.
$jsonArgs=$restricted+@('--entrypoint','/opt/swe/app/bin/pentagonal_control',$remoteRef,
 '0x3fffffff','0x3fffffff','or')
$json=Invoke-Docker $jsonArgs 0 'control_json'
if(-not [string]::IsNullOrEmpty($json.Stderr)){throw 'PEEK|HOLD|control_json_stderr'}
try{$unit=ConvertFrom-Json $json.Stdout -AsHashtable -Depth 30}catch{
 throw 'PEEK|HOLD|control_json_parse'
}
if($unit.schema -cne 'SWE-PENTAGONAL-CXX20-experimental' -or
   $unit.source_mutated -isnot [bool] -or $unit.source_mutated -ne $false -or
   @($unit.stages).Count -ne 6 -or @($unit.blocks).Count -ne 6 -or
   $unit.witness_supplied -isnot [bool] -or $unit.witness_supplied -ne $true){
 throw 'PEEK|HOLD|control_json_types_or_stage_count'
}
$names=@('xor','or','and','nop','cor','flop')
for($i=0;$i -lt 6;$i++){
 if($unit.stages[$i].name -cne $names[$i] -or
    $unit.stages[$i].value -isnot [bool] -or
    $unit.blocks[$i].xor -isnot [bool] -or
    $unit.blocks[$i].or -isnot [bool] -or
    $unit.blocks[$i].and -isnot [bool]){
  throw ('PEEK|HOLD|control_unit_type|'+$i)
 }
}
$results.Add('json_stage_count=6|bool_fields_exact')
$bytes=[Text.UTF8Encoding]::new($false).GetBytes(($results -join [char]10))
$seal=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
 $path=[IO.Path]::GetFullPath($ReportPath)
 if([IO.File]::Exists($path)){throw 'PEEK|HOLD|existing_report_no_overwrite'}
 $record=[ordered]@{
  protocol='PEEK_WTII_INSPIRED_ARENA_TRIAL_V1'
  source_reference='https://www.hiveworkshop.com/threads/wtiis-unit-tester.239333/'
  profile=$Profile;phase=$Phase;source_commit=$env:GITHUB_SHA
  source_image=$Image;observed_registry_digest=$Digest
  original_evidence=$evidence;arenas=$arenas.Count;typed_json_stage_count=6
  result_sha256=$seal;status='PASS';scope='LINUX_AMD64_EXPERIMENT'
  full_native_openbsd_build=$false;warcraft_game_engine_used=$false
 }
 [IO.File]::WriteAllText($path,(ConvertTo-Json $record -Compress)+[char]10,[Text.UTF8Encoding]::new($false))
}
Write-Host ('PEEK|VIRGIN_TRIAL_PASS|PROFILE='+$Profile+'|PHASE='+$Phase+
 '|DIGEST='+$Digest+'|ARENAS='+$arenas.Count+'|JSON_STAGES=6|SHA256='+$seal+
 '|FRESH_ISOLATED_CONTAINERS|REMOTE_DIGEST_PULLED|NO_GAME_ENGINE')
)
if($Digest -cnotmatch '^sha256:[0-9a-f]{64}
$remoteRef=$repository+'@'+$Digest
if($env:GITHUB_SHA -cnotmatch '^[0-9a-f]{40}$' -or
   -not $Image.EndsWith('-sha-'+$env:GITHUB_SHA,[StringComparison]::Ordinal) -or
   -not $Image.Contains($Profile,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|source_commit_or_profile_drift'
}
if($env:GITHUB_ACTIONS -cne 'true'){throw 'PEEK|HOLD|GitHub_worker_expected'}
function Invoke-Docker([string[]]$Arguments,[int]$Expected,[string]$Arena){
 $psi=[Diagnostics.ProcessStartInfo]::new('docker')
 $psi.UseShellExecute=$false
 $psi.RedirectStandardOutput=$true
 $psi.RedirectStandardError=$true
 $psi.CreateNoWindow=$true
 foreach($arg in $Arguments){[void]$psi.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::new()
 $proc.StartInfo=$psi
 if(-not $proc.Start()){throw ('PEEK|HOLD|process_start|'+$Arena)}
 try{
  $stdout=$proc.StandardOutput.ReadToEnd()
  $stderr=$proc.StandardError.ReadToEnd()
  if(-not $proc.WaitForExit(60000)){
   $proc.Kill($true)
   throw ('PEEK|HOLD|arena_timeout|'+$Arena)
  }
  if($proc.ExitCode -ne $Expected){
   throw ('PEEK|HOLD|exit_code|'+$Arena+'|ACTUAL='+$proc.ExitCode+
          '|EXPECTED='+$Expected+'|STDERR='+$stderr.Substring(0,[Math]::Min(120,$stderr.Length)))
  }
  return [pscustomobject]@{Stdout=$stdout;Stderr=$stderr;Exit=$proc.ExitCode}
 }finally{$proc.Dispose()}
}
# Pull a fresh registry manifest reference. Docker MAY reuse local cached blobs.
# Every "unit" below executes in a NEW read-only isolated container.
[void](Invoke-Docker @('pull',$remoteRef) 0 'pull_remote_manifest')
$inspect=Invoke-Docker @('image','inspect',$remoteRef) 0 'inspect_image'
try{$image=(ConvertFrom-Json $inspect.Stdout -Depth 20)[0]}catch{
 throw 'PEEK|HOLD|image_metadata_invalid'
}
if($image.Config.Labels.'org.opencontainers.image.revision' -cne $env:GITHUB_SHA -or
   $image.Config.Labels.'com.simplywellexecuted.runtime-profile' -cne $Profile -or
   $image.Config.Labels.'com.simplywellexecuted.native-openbsd-builds' -cne '0' -or
   @($image.Config.Entrypoint).Count -ne 1 -or
   $image.Config.Entrypoint[0] -cne '/opt/swe/app/bin/pentagonal_xor'){
 throw 'PEEK|HOLD|source_or_program_entrypoint_type'
}
$evidence=[string]$image.Config.Labels.'com.simplywellexecuted.evidence-reference'
$expectedEvidence=switch($Profile){
 'openbsd1' {'sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c'}
 'openbsd2' {'sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56'}
 'openbsd3' {'sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1'}
}
if(-not $evidence.EndsWith('@'+$expectedEvidence,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|original_evidence_digest_changed'
}
$restricted=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
 '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$arenas=@(
 @{name='baseline_head';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'},
 @{name='zero_composition';argv=@('0','0','0','0','0');exit=0;expected='Pentagonal XOR: 0.00000000%'},
 @{name='odd_composition';argv=@('1','1','1','0','0');exit=0;expected='Pentagonal XOR: 100.00000000%'},
 @{name='hostile_out_of_range';argv=@('0.8','0.7','1.2','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_not_a_number';argv=@('0.8','0.7','nan','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_missing_arguments';argv=@('0.8','0.7','0.9','0.6');exit=2;error='Usage:'},
 @{name='baseline_tail';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'}
)
$results=[Collections.Generic.List[string]]::new()
foreach($arena in $arenas){
 # Each stage is a fresh container, never the previous stage's mutable process.
 $result=Invoke-Docker ($restricted+@($remoteRef)+$arena.argv) $arena.exit $arena.name
 $got=$result.Stdout.TrimEnd([char[]]@([char]13,[char]10))
 if($arena.ContainsKey('expected')){
  if($got -cne $arena.expected -or -not [string]::IsNullOrEmpty($result.Stderr)){
   throw ('PEEK|HOLD|positive_fixture_wrong_units|'+$arena.name)
  }
 }else{
  if(-not [string]::IsNullOrEmpty($got) -or
     -not $result.Stderr.Contains($arena.error,[StringComparison]::Ordinal)){
   throw ('PEEK|HOLD|hostile_fixture_not_rejected|'+$arena.name)
  }
 }
 $results.Add($arena.name+'|'+$arena.exit+'|'+$got)
 Write-Host ('PEEK|ARENA_PASS|'+$Profile+'|'+$arena.name+'|EXIT='+$arena.exit)
}
# Independent JSON composition arena. JSON typed fields are not W3C-certified.
$jsonArgs=$restricted+@('--entrypoint','/opt/swe/app/bin/pentagonal_control',$remoteRef,
 '0x3fffffff','0x3fffffff','or')
$json=Invoke-Docker $jsonArgs 0 'control_json'
if(-not [string]::IsNullOrEmpty($json.Stderr)){throw 'PEEK|HOLD|control_json_stderr'}
try{$unit=ConvertFrom-Json $json.Stdout -AsHashtable -Depth 30}catch{
 throw 'PEEK|HOLD|control_json_parse'
}
if($unit.schema -cne 'SWE-PENTAGONAL-CXX20-experimental' -or
   $unit.source_mutated -isnot [bool] -or $unit.source_mutated -ne $false -or
   @($unit.stages).Count -ne 6 -or @($unit.blocks).Count -ne 6 -or
   $unit.witness_supplied -isnot [bool] -or $unit.witness_supplied -ne $true){
 throw 'PEEK|HOLD|control_json_types_or_stage_count'
}
$names=@('xor','or','and','nop','cor','flop')
for($i=0;$i -lt 6;$i++){
 if($unit.stages[$i].name -cne $names[$i] -or
    $unit.stages[$i].value -isnot [bool] -or
    $unit.blocks[$i].xor -isnot [bool] -or
    $unit.blocks[$i].or -isnot [bool] -or
    $unit.blocks[$i].and -isnot [bool]){
  throw ('PEEK|HOLD|control_unit_type|'+$i)
 }
}
$results.Add('json_stage_count=6|bool_fields_exact')
$bytes=[Text.UTF8Encoding]::new($false).GetBytes(($results -join [char]10))
$seal=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
 $path=[IO.Path]::GetFullPath($ReportPath)
 if([IO.File]::Exists($path)){throw 'PEEK|HOLD|existing_report_no_overwrite'}
 $record=[ordered]@{
  protocol='PEEK_WTII_INSPIRED_ARENA_TRIAL_V1'
  source_reference='https://www.hiveworkshop.com/threads/wtiis-unit-tester.239333/'
  profile=$Profile;phase=$Phase;source_commit=$env:GITHUB_SHA
  source_image=$Image;observed_registry_digest=$Digest
  original_evidence=$evidence;arenas=$arenas.Count;typed_json_stage_count=6
  result_sha256=$seal;status='PASS';scope='LINUX_AMD64_EXPERIMENT'
  full_native_openbsd_build=$false;warcraft_game_engine_used=$false
 }
 [IO.File]::WriteAllText($path,(ConvertTo-Json $record -Compress)+[char]10,[Text.UTF8Encoding]::new($false))
}
Write-Host ('PEEK|VIRGIN_TRIAL_PASS|PROFILE='+$Profile+'|PHASE='+$Phase+
 '|DIGEST='+$Digest+'|ARENAS='+$arenas.Count+'|JSON_STAGES=6|SHA256='+$seal+
 '|FRESH_ISOLATED_CONTAINERS|REMOTE_DIGEST_PULLED|NO_GAME_ENGINE')
 -or -not $imageMatch.Success){
 throw 'PEEK|HOLD|image_or_digest_invalid'
}
$repository=$imageMatch.Groups['registry'].Value
$remoteRef=$repository+'@'+$Digest
if($env:GITHUB_SHA -cnotmatch '^[0-9a-f]{40}$' -or
   -not $Image.EndsWith('-sha-'+$env:GITHUB_SHA,[StringComparison]::Ordinal) -or
   -not $Image.Contains($Profile,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|source_commit_or_profile_drift'
}
if($env:GITHUB_ACTIONS -cne 'true'){throw 'PEEK|HOLD|GitHub_worker_expected'}
function Invoke-Docker([string[]]$Arguments,[int]$Expected,[string]$Arena){
 $psi=[Diagnostics.ProcessStartInfo]::new('docker')
 $psi.UseShellExecute=$false
 $psi.RedirectStandardOutput=$true
 $psi.RedirectStandardError=$true
 $psi.CreateNoWindow=$true
 foreach($arg in $Arguments){[void]$psi.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::new()
 $proc.StartInfo=$psi
 if(-not $proc.Start()){throw ('PEEK|HOLD|process_start|'+$Arena)}
 try{
  $stdout=$proc.StandardOutput.ReadToEnd()
  $stderr=$proc.StandardError.ReadToEnd()
  if(-not $proc.WaitForExit(60000)){
   $proc.Kill($true)
   throw ('PEEK|HOLD|arena_timeout|'+$Arena)
  }
  if($proc.ExitCode -ne $Expected){
   throw ('PEEK|HOLD|exit_code|'+$Arena+'|ACTUAL='+$proc.ExitCode+
          '|EXPECTED='+$Expected+'|STDERR='+$stderr.Substring(0,[Math]::Min(120,$stderr.Length)))
  }
  return [pscustomobject]@{Stdout=$stdout;Stderr=$stderr;Exit=$proc.ExitCode}
 }finally{$proc.Dispose()}
}
# Pull a fresh registry manifest reference. Docker MAY reuse local cached blobs.
# Every "unit" below executes in a NEW read-only isolated container.
[void](Invoke-Docker @('pull',$remoteRef) 0 'pull_remote_manifest')
$inspect=Invoke-Docker @('image','inspect',$remoteRef) 0 'inspect_image'
try{$image=(ConvertFrom-Json $inspect.Stdout -Depth 20)[0]}catch{
 throw 'PEEK|HOLD|image_metadata_invalid'
}
if($image.Config.Labels.'org.opencontainers.image.revision' -cne $env:GITHUB_SHA -or
   $image.Config.Labels.'com.simplywellexecuted.runtime-profile' -cne $Profile -or
   $image.Config.Labels.'com.simplywellexecuted.native-openbsd-builds' -cne '0' -or
   @($image.Config.Entrypoint).Count -ne 1 -or
   $image.Config.Entrypoint[0] -cne '/opt/swe/app/bin/pentagonal_xor'){
 throw 'PEEK|HOLD|source_or_program_entrypoint_type'
}
$evidence=[string]$image.Config.Labels.'com.simplywellexecuted.evidence-reference'
$expectedEvidence=switch($Profile){
 'openbsd1' {'sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c'}
 'openbsd2' {'sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56'}
 'openbsd3' {'sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1'}
}
if(-not $evidence.EndsWith('@'+$expectedEvidence,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|original_evidence_digest_changed'
}
$restricted=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
 '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$arenas=@(
 @{name='baseline_head';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'},
 @{name='zero_composition';argv=@('0','0','0','0','0');exit=0;expected='Pentagonal XOR: 0.00000000%'},
 @{name='odd_composition';argv=@('1','1','1','0','0');exit=0;expected='Pentagonal XOR: 100.00000000%'},
 @{name='hostile_out_of_range';argv=@('0.8','0.7','1.2','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_not_a_number';argv=@('0.8','0.7','nan','0.6','0.95');exit=2;error='Error:'},
 @{name='hostile_missing_arguments';argv=@('0.8','0.7','0.9','0.6');exit=2;error='Usage:'},
 @{name='baseline_tail';argv=@('0.80','0.70','0.90','0.60','0.95');exit=0;expected='Pentagonal XOR: 51.72800000%'}
)
$results=[Collections.Generic.List[string]]::new()
foreach($arena in $arenas){
 # Each stage is a fresh container, never the previous stage's mutable process.
 $result=Invoke-Docker ($restricted+@($remoteRef)+$arena.argv) $arena.exit $arena.name
 $got=$result.Stdout.TrimEnd([char[]]@([char]13,[char]10))
 if($arena.ContainsKey('expected')){
  if($got -cne $arena.expected -or -not [string]::IsNullOrEmpty($result.Stderr)){
   throw ('PEEK|HOLD|positive_fixture_wrong_units|'+$arena.name)
  }
 }else{
  if(-not [string]::IsNullOrEmpty($got) -or
     -not $result.Stderr.Contains($arena.error,[StringComparison]::Ordinal)){
   throw ('PEEK|HOLD|hostile_fixture_not_rejected|'+$arena.name)
  }
 }
 $results.Add($arena.name+'|'+$arena.exit+'|'+$got)
 Write-Host ('PEEK|ARENA_PASS|'+$Profile+'|'+$arena.name+'|EXIT='+$arena.exit)
}
# Independent JSON composition arena. JSON typed fields are not W3C-certified.
$jsonArgs=$restricted+@('--entrypoint','/opt/swe/app/bin/pentagonal_control',$remoteRef,
 '0x3fffffff','0x3fffffff','or')
$json=Invoke-Docker $jsonArgs 0 'control_json'
if(-not [string]::IsNullOrEmpty($json.Stderr)){throw 'PEEK|HOLD|control_json_stderr'}
try{$unit=ConvertFrom-Json $json.Stdout -AsHashtable -Depth 30}catch{
 throw 'PEEK|HOLD|control_json_parse'
}
if($unit.schema -cne 'SWE-PENTAGONAL-CXX20-experimental' -or
   $unit.source_mutated -isnot [bool] -or $unit.source_mutated -ne $false -or
   @($unit.stages).Count -ne 6 -or @($unit.blocks).Count -ne 6 -or
   $unit.witness_supplied -isnot [bool] -or $unit.witness_supplied -ne $true){
 throw 'PEEK|HOLD|control_json_types_or_stage_count'
}
$names=@('xor','or','and','nop','cor','flop')
for($i=0;$i -lt 6;$i++){
 if($unit.stages[$i].name -cne $names[$i] -or
    $unit.stages[$i].value -isnot [bool] -or
    $unit.blocks[$i].xor -isnot [bool] -or
    $unit.blocks[$i].or -isnot [bool] -or
    $unit.blocks[$i].and -isnot [bool]){
  throw ('PEEK|HOLD|control_unit_type|'+$i)
 }
}
$results.Add('json_stage_count=6|bool_fields_exact')
$bytes=[Text.UTF8Encoding]::new($false).GetBytes(($results -join [char]10))
$seal=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
 $path=[IO.Path]::GetFullPath($ReportPath)
 if([IO.File]::Exists($path)){throw 'PEEK|HOLD|existing_report_no_overwrite'}
 $record=[ordered]@{
  protocol='PEEK_WTII_INSPIRED_ARENA_TRIAL_V1'
  source_reference='https://www.hiveworkshop.com/threads/wtiis-unit-tester.239333/'
  profile=$Profile;phase=$Phase;source_commit=$env:GITHUB_SHA
  source_image=$Image;observed_registry_digest=$Digest
  original_evidence=$evidence;arenas=$arenas.Count;typed_json_stage_count=6
  result_sha256=$seal;status='PASS';scope='LINUX_AMD64_EXPERIMENT'
  full_native_openbsd_build=$false;warcraft_game_engine_used=$false
 }
 [IO.File]::WriteAllText($path,(ConvertTo-Json $record -Compress)+[char]10,[Text.UTF8Encoding]::new($false))
}
Write-Host ('PEEK|VIRGIN_TRIAL_PASS|PROFILE='+$Profile+'|PHASE='+$Phase+
 '|DIGEST='+$Digest+'|ARENAS='+$arenas.Count+'|JSON_STAGES=6|SHA256='+$seal+
 '|FRESH_ISOLATED_CONTAINERS|REMOTE_DIGEST_PULLED|NO_GAME_ENGINE')
