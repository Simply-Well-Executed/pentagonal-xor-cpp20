# SPDX-License-Identifier: 0BSD
# PEEK V1: WTii-inspired test arenas plus Spin/Star-inspired bounded trials.
# Warcraft III GUI/JASS references inform test design, but no game code runs.
# Spin and Star both use a 0.03 game-time periodic trigger, NOT a Docker clock.
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
$match=[regex]::Match($Image,'^(?<registry>(?:ghcr[.]io/simply-well-executed/|paulcheek/)[a-z0-9-]+):(?<tag>[a-z0-9-]+)$')
if(-not $match.Success -or $Digest -cnotmatch '^sha256:[a-f0-9]{64}$'){
 throw 'PEEK|HOLD|image_or_digest_invalid'
}
$registry=$match.Groups['registry'].Value
$remoteRef=$registry+'@'+$Digest
if($env:GITHUB_ACTIONS -cne 'true' -or
   $env:GITHUB_SHA -cnotmatch '^[a-f0-9]{40}$' -or
   -not $Image.EndsWith('-sha-'+$env:GITHUB_SHA,[StringComparison]::Ordinal) -or
   -not $Image.Contains($Profile,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|source_commit_or_profile_invalid'
}
function Invoke-Docker([string[]]$Arguments,[int]$Expected,[string]$Arena){
 $psi=[Diagnostics.ProcessStartInfo]::new('docker')
 $psi.UseShellExecute=$false
 $psi.CreateNoWindow=$true
 $psi.RedirectStandardOutput=$true
 $psi.RedirectStandardError=$true
 foreach($arg in $Arguments){[void]$psi.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::new()
 $proc.StartInfo=$psi
 if(-not $proc.Start()){throw ('PEEK|HOLD|docker_process_start|'+$Arena)}
 try{
  $out=$proc.StandardOutput.ReadToEnd()
  $err=$proc.StandardError.ReadToEnd()
  if(-not $proc.WaitForExit(20000)){
   $proc.Kill($true)
   throw ('PEEK|HOLD|arena_timeout|'+$Arena)
  }
  if($proc.ExitCode -ne $Expected){
   throw ('PEEK|HOLD|arena_exit|'+$Arena+'|EXPECTED='+$Expected+'|GOT='+$proc.ExitCode)
  }
  return [pscustomobject]@{Out=$out;Err=$err;Exit=$proc.ExitCode}
 }finally{$proc.Dispose()}
}
# Check the published tag against the exact manifest, not only a local tag.
$pull=Invoke-Docker @('pull',$Image) 0 'pull_tag'
$remoteDigest=[regex]::Match(($pull.Out+[char]10+$pull.Err),'(?m)^Digest:\s*(sha256:[a-f0-9]{64})\s*$')
if(-not $remoteDigest.Success -or $remoteDigest.Groups[1].Value -cne $Digest){
 throw 'PEEK|HOLD|registry_tag_does_not_resolve_to_pinned_manifest'
}
[void](Invoke-Docker @('pull',$remoteRef) 0 'pull_remote_manifest')
$meta=Invoke-Docker @('image','inspect',$remoteRef) 0 'image_metadata'
try{$image=(@(ConvertFrom-Json -InputObject $meta.Out -Depth 20))[0]}catch{
 throw 'PEEK|HOLD|manifest_metadata_invalid'
}
if($image.Config.Labels.'org.opencontainers.image.revision' -cne $env:GITHUB_SHA -or
   $image.Config.Labels.'com.simplywellexecuted.runtime-profile' -cne $Profile -or
   $image.Config.Labels.'com.simplywellexecuted.native-openbsd-builds' -cne '0' -or
   @($image.Config.Entrypoint).Count -ne 1 -or
   $image.Config.Entrypoint[0] -cne '/opt/swe/app/bin/pentagonal_xor'){
 throw 'PEEK|HOLD|revision_profile_or_entrypoint_mismatch'
}
$evidence=[string]$image.Config.Labels.'com.simplywellexecuted.evidence-reference'
$expectedDigest=switch($Profile){
 'openbsd1' {'sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c'}
 'openbsd2' {'sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56'}
 'openbsd3' {'sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1'}
}
if(-not $evidence.EndsWith('@'+$expectedDigest,[StringComparison]::Ordinal)){
 throw 'PEEK|HOLD|original_evidence_digest_changed'
}
$runArgs=@('run','--rm','--network','none','--read-only','--cap-drop','ALL',
 '--security-opt','no-new-privileges','--pids-limit','16','--platform','linux/amd64')
$baselineArgs=@('0.80','0.70','0.90','0.60','0.95')
$baseline='Pentagonal XOR: 51.72800000%'
function Compare-Positive([string]$Name,[string[]]$Arguments,[string]$Expected){
 $result=Invoke-Docker ($runArgs+@($remoteRef)+$Arguments) 0 $Name
 if($result.Out.TrimEnd([char[]]@([char]13,[char]10)) -cne $Expected -or
    -not [string]::IsNullOrEmpty($result.Err)){
  throw ('PEEK|HOLD|positive_output_contract|'+$Name)
 }
}
Compare-Positive 'baseline_head' $baselineArgs $baseline
Compare-Positive 'zero_composition' @('0','0','0','0','0') 'Pentagonal XOR: 0.00000000%'
Compare-Positive 'odd_composition' @('1','1','1','0','0') 'Pentagonal XOR: 100.00000000%'
foreach($arena in @(
 @{name='hostile_out_of_range';args=@('0.8','0.7','1.2','0.6','0.95');error='Error:'},
 @{name='hostile_not_a_number';args=@('0.8','0.7','nan','0.6','0.95');error='Error:'},
 @{name='hostile_missing_arguments';args=@('0.8','0.7','0.9','0.6');error='Usage:'}
)){
 $result=Invoke-Docker ($runArgs+@($remoteRef)+$arena.args) 2 $arena.name
 if($result.Out.Trim().Length -ne 0 -or
    -not $result.Err.Contains($arena.error,[StringComparison]::Ordinal)){
  throw ('PEEK|HOLD|hostile_output_contract|'+$arena.name)
 }
}
# SPIN: nine back-to-back bounded invocations, no synthetic timer or pause.
$spinClock=[Diagnostics.Stopwatch]::StartNew()
for($i=0;$i -lt 9;$i++){
 Compare-Positive ('SPIN_'+$i) $baselineArgs $baseline
}
$spinClock.Stop()
# STAR: three ordered phases, nine invocations each (27 total). This tests
# sustained reproducibility, not an actual Warcraft game-time duration.
$starClock=[Diagnostics.Stopwatch]::StartNew()
for($phaseIndex=0;$phaseIndex -lt 3;$phaseIndex++){
 for($i=0;$i -lt 9;$i++){
  Compare-Positive ('STAR_'+$phaseIndex+'_'+$i) $baselineArgs $baseline
 }
}
$starClock.Stop()
Compare-Positive 'baseline_tail' $baselineArgs $baseline
# Independent typed JSON arena using the real installed control executable.
$json=Invoke-Docker ($runArgs+@('--entrypoint','/opt/swe/app/bin/pentagonal_control',
 $remoteRef,'0x3fffffff','0x3fffffff','or')) 0 'control_json'
if(-not [string]::IsNullOrEmpty($json.Err)){throw 'PEEK|HOLD|control_json_stderr'}
try{$unit=ConvertFrom-Json -InputObject $json.Out -AsHashtable -Depth 30}catch{
 throw 'PEEK|HOLD|control_json_parse'
}
if($unit.schema -cne 'SWE-PENTAGONAL-CXX20-experimental' -or
   $unit.source_mutated -isnot [bool] -or $unit.source_mutated -ne $false -or
   $unit.witness_supplied -isnot [bool] -or $unit.witness_supplied -ne $true -or
   @($unit.stages).Count -ne 6 -or @($unit.blocks).Count -ne 6){
 throw 'PEEK|HOLD|control_json_schema_or_types'
}
$names=@('xor','or','and','nop','cor','flop')
for($i=0;$i -lt 6;$i++){
 if($unit.stages[$i].name -cne $names[$i] -or
    $unit.stages[$i].value -isnot [bool] -or
    $unit.blocks[$i].xor -isnot [bool] -or
    $unit.blocks[$i].or -isnot [bool] -or
    $unit.blocks[$i].and -isnot [bool]){
  throw ('PEEK|HOLD|control_stage_types|'+$i)
 }
}
$canonical=@(
 'PEEK_V1','PROFILE='+$Profile,'DIGEST='+$Digest,'HEAD='+$baseline,
 'SPIN_RUNS=9','STAR_PHASES=3','STAR_RUNS=27',
 'HOSTILE_CASES=3','CONTROL_JSON_STAGES=6','ORIGINAL_EVIDENCE='+$evidence
) -join '|'
$seal=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
 [Text.UTF8Encoding]::new($false).GetBytes($canonical))).ToLowerInvariant()
if(-not [string]::IsNullOrWhiteSpace($ReportPath)){
 $report=[IO.Path]::GetFullPath($ReportPath)
 if([IO.File]::Exists($report)){throw 'PEEK|HOLD|existing_report_no_overwrite'}
 $record=[ordered]@{
  protocol='PEEK_WTII_SPIN_STAR_V1'
  source_references=@(
   'https://www.hiveworkshop.com/threads/wtiis-unit-tester.239333/',
   'https://www.hiveworkshop.com/threads/spin-version-1-1.219769/',
   'https://www.hiveworkshop.com/threads/star-v1-4.198361/'
  )
  profile=$Profile;phase=$Phase;source_commit=$env:GITHUB_SHA
  image=$Image;manifest_digest=$Digest;original_evidence=$evidence
  spin_runs=9;spin_elapsed_ms=$spinClock.ElapsedMilliseconds
  star_phases=3;star_runs=27;star_elapsed_ms=$starClock.ElapsedMilliseconds
  hostile_cases=3;control_json_stages=6;result_sha256=$seal
  status='PASS';native_openbsd_build=$false;warcraft_engine_used=$false
 }
 [IO.File]::WriteAllText($report,(ConvertTo-Json $record -Compress -Depth 5)+[char]10,
  [Text.UTF8Encoding]::new($false))
}
Write-Host ('PEEK|VIRGIN_TRIAL_PASS|PROFILE='+$Profile+'|PHASE='+$Phase+
 '|DIGEST='+$Digest+'|SPIN=9|STAR=3x9|HOSTILE=3|JSON_STAGES=6'+
 '|SPIN_MS='+$spinClock.ElapsedMilliseconds+'|STAR_MS='+$starClock.ElapsedMilliseconds+
 '|SHA256='+$seal+'|LINUX_AMD64|NO_GAME_ENGINE')
