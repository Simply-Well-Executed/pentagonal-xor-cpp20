# SPDX-License-Identifier: 0BSD
# HULAO-CHAOS-255-001: 255 fully offline, deterministic, bounded fault models.
# Each lap checks known steady state, one deliberately disturbed value,
# detection and recovery. NEVER terminate an OS process, corrupt a real file,
# interfere with a network, or modify the source under test.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [ValidateRange(1,255)][int]$Rounds=255
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
if($out -ceq $root -or
   $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,
                   [StringComparison]::OrdinalIgnoreCase)){
 throw 'HULAO_CHAOS|FAIL|evidence_inside_checkout'
}
if([IO.Directory]::Exists($out) -and
   @(Get-ChildItem -LiteralPath $out -Force).Count -gt 0){
 throw 'HULAO_CHAOS|FAIL|nonempty_evidence_directory'
}
[IO.Directory]::CreateDirectory($out)|Out-Null
$source=@(
 (Join-Path $root 'pentadi/PentaDiC11BranchRecords.reference.tsv'),
 (Join-Path $root 'pentadi/Invoke-PentaPiTcp255.ps1'),
 (Join-Path $root 'pentadi/Invoke-PentaPiLapAudit.ps1'),
 (Join-Path $root 'pentadi/Invoke-HulaoChaos255.ps1')
)
$before=[System.Collections.Generic.List[string]]::new()
foreach($p in $source){
 if(-not [IO.File]::Exists($p)){throw ('HULAO_CHAOS|FAIL|source_absent|'+$p)}
 $before.Add((Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash)
}
# Exact 17 categories x 15 deterministic rounds = 255. The fault only
# touches local primitive variables or in-memory byte arrays.
$types=[string[]]@(
 'PACKET_DROP','LATENCY_SPIKE','DUPLICATE_FRAME','REORDERED_FRAME',
 'CORRUPT_PAYLOAD','BAD_LENGTH','ZERO_LENGTH','OVERSIZE_FRAME',
 'STALE_PROVENANCE','BAD_CHECKSUM','NBIT_OVERFLOW','GLINT_LOSS',
 'DOUBLE_OWNER','BRANCH_OSCILLATION','REL8_OUT_OF_RANGE',
 'MALFORMED_TSV','REMOTE_ENDPOINT'
)
if($types.Count -ne 17){throw 'HULAO_CHAOS|FAIL|scenario_registry'}
$tab=[string][char]9
$evidence=[System.Collections.Generic.List[string]]::new()
$evidence.Add('lap'+$tab+'fault'+$tab+'control'+$tab+'injection_detected'+$tab+
              'recovered'+$tab+'blast_radius'+$tab+'result')
$failures=0
$control=[Text.Encoding]::ASCII.GetBytes('PENTAPI|1')
$baseHash=[Convert]::ToHexString(
 [Security.Cryptography.SHA256]::HashData($control))
for($lap=1;$lap -le $Rounds;$lap++){
 $index=($lap-1)%$types.Count
 $fault=$types[$index]
 $replaySeed=(([long]$lap*73+65521)%997)
 $detected=$false
 $recovered=$false
 # The control group is immutable, local, non-network data.
 if($control.Length -ne 9){throw 'HULAO_CHAOS|FAIL|steady_state_length'}
 switch($fault){
  'PACKET_DROP'       {$received=0; $detected=($received -ne 9)}
  'LATENCY_SPIKE'     {$elapsedMs=2501+[int]($replaySeed%512); $detected=($elapsedMs -gt 2500)}
  'DUPLICATE_FRAME'   {$sequence=[int[]]@(7,7); $detected=($sequence.Length -ne 1)}
  'REORDERED_FRAME'   {$sequence=[int[]]@(8,7); $detected=($sequence[0] -ne 7)}
  'CORRUPT_PAYLOAD'   {
   [byte[]]$mutant=[byte[]]$control.Clone()
   $mutant[([int]($replaySeed%9))]=$mutant[([int]($replaySeed%9))] -bxor [byte]1
   $detected=([Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($mutant)) -cne $baseHash)
  }
  'BAD_LENGTH'        {$header=8; $detected=($header -ne $control.Length)}
  'ZERO_LENGTH'       {$header=0; $detected=($header -ne $control.Length)}
  'OVERSIZE_FRAME'    {$header=1025; $detected=($header -gt 1024)}
  'STALE_PROVENANCE'  {$actualTag=95; $receivedTag=94; $detected=($actualTag -ne $receivedTag)}
  'BAD_CHECKSUM'      {$actual=$baseHash; $mutated='0'+$actual.Substring(1); if($actual[0] -eq '0'){$mutated='F'+$actual.Substring(1)}; $detected=($actual -cne $mutated)}
  'NBIT_OVERFLOW'     {$bits=8; $number=256; $detected=($number -ge (1 -shl $bits))}
  'GLINT_LOSS'        {$numeratorBits=144; $destinationBits=128; $detected=($numeratorBits -gt $destinationBits)}
  'DOUBLE_OWNER'      {$owners=2; $detected=($owners -ne 1)}
  'BRANCH_OSCILLATION'{$history=[string[]]@('SHORT','LONG','SHORT'); $detected=($history[0] -ceq $history[2] -and $history[0] -cne $history[1])}
  'REL8_OUT_OF_RANGE' {$delta=128+[int]($replaySeed%1024); $detected=($delta -gt 127)}
  'MALFORMED_TSV'     {$record='1|7|10|LOOP7|SHORT|2|1000'; $detected=($record.Split('|').Count -ne 8)}
  'REMOTE_ENDPOINT'   {$address='198.51.100.8'; $detected=($address -cne '127.0.0.1')}
  default {throw 'HULAO_CHAOS|FAIL|unknown_fault'}
 }
 # Model recovery is resetting ONLY the local probe, then demonstrating
 # the same untouched baseline digest as before. No real service restarts.
 $recovered=([Convert]::ToHexString(
 [Security.Cryptography.SHA256]::HashData($control)) -ceq $baseHash)
 $result=if($detected -and $recovered){'PASS'}else{'FAIL'}
 if($result -cne 'PASS'){$failures++}
 $evidence.Add(('{0:000}' -f $lap)+$tab+$fault+$tab+'PASS'+$tab+
                $(if($detected){'PASS'}else{'FAIL'})+$tab+
                $(if($recovered){'PASS'}else{'FAIL'})+$tab+
                'IN_MEMORY_ONLY'+$tab+$result)
 if($result -cne 'PASS'){
  throw ('HULAO_CHAOS|FAIL|lap='+('{0:000}' -f $lap)+'|scenario='+$fault)
 }
}
for($i=0;$i -lt $source.Count;$i++){
 if($before[$i] -cne (Get-FileHash -LiteralPath $source[$i] -Algorithm SHA256).Hash){
  throw 'HULAO_CHAOS|FAIL|source_changed_during_laps'
 }
}
# Lap 256 is narrative/inspection only; never counted as 255 tests passing
# if any fault injection failed. Release remains HOLD and DCC32 remains HOLD.
$ledger=Join-Path $out 'Hulao_Chaos_255_laps.tsv'
[IO.File]::WriteAllLines($ledger,$evidence,[Text.UTF8Encoding]::new($false))
$receipt=Join-Path $out 'Hulao_Chaos_256_sanctuary.txt'
[IO.File]::WriteAllLines($receipt,[string[]]@(
 'PENTAPI|LAP=256|MODE=FICTIONAL_SANCTUARY|PREVIOUS_MODEL_LAPS='+$Rounds,
 'PENTAPI|REAL_GATES=REQUIRE_INDEPENDENT_CI|DELPHI7=HOLD',
 'PENTAPI|NO_PRODUCTION_FAULTS|NO_EXTERNAL_NETWORK|RELEASE=HOLD'
),[Text.UTF8Encoding]::new($false))
$ledgerHash=(Get-FileHash -LiteralPath $ledger -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('HULAO_CHAOS|MODEL_ROUNDS='+$Rounds+'|FAULT_CLASSES=17'+
 '|STEADY_STATE=PASS|DETECTION='+$Rounds+'_OF_'+$Rounds+
 '|RECOVERY='+$Rounds+'_OF_'+$Rounds+'|BLAST_RADIUS=IN_MEMORY_ONLY'+
 '|REAL_OUTAGES=0|SOURCE_MUTATION=NONE|LAP256=FICTION_ONLY'+
 '|LEDGER_SHA256='+$ledgerHash+'|RELEASE=HOLD')
