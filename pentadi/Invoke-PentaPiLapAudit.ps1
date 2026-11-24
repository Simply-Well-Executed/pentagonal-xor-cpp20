# SPDX-License-Identifier: 0BSD
# PENTAPI-001: five-gate review loop; evidence first, new proposal later.
# No auto-merge, no destructive rewrite, no repeated network probing.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$PentaDiEvidenceDirectory,
 [Parameter(Mandatory=$true)][string]$TcpEvidenceDirectory,
 [Parameter(Mandatory=$true)][string]$AuditDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$penta=[IO.Path]::GetFullPath($PentaDiEvidenceDirectory)
$tcp=[IO.Path]::GetFullPath($TcpEvidenceDirectory)
$out=[IO.Path]::GetFullPath($AuditDirectory)
foreach($p in @($penta,$tcp,$out)){
 if($p -ceq $root -or
    $p.StartsWith($root+[IO.Path]::DirectorySeparatorChar,
                  [StringComparison]::OrdinalIgnoreCase)){
  throw 'PENTAPI|FAIL|evidence_inside_checkout'
 }
}
if([IO.Directory]::Exists($out) -and
   @(Get-ChildItem -LiteralPath $out -Force).Count -gt 0){
 throw 'PENTAPI|FAIL|output_directory_not_empty'
}
[IO.Directory]::CreateDirectory($out)|Out-Null
$paths=@{
 'UPSTREAM'=Join-Path $root '.github/workflows/quaddi-ni-oracle-cpp20.yml'
 'DOWNSTREAM'=Join-Path $root '.github/workflows/pentadi-delphi7-asm.yml'
 'KEEP_PASS'=Join-Path $root 'pentadi/keep.pass'
 'PROFILER'=Join-Path $root 'pentadi/BuildSpeedProfiler.pas'
 'RUNNER'=Join-Path $root 'pentadi/Delphi007PinPipe.dpr'
 'UNIT'=Join-Path $root 'pentadi/BranchPinToTsht.pas'
 'CANONICAL'=Join-Path $root 'pentadi/PentaDiC11BranchRecords.reference.tsv'
 'C_SOURCE'=Join-Path $root 'pentadi/PentaDiC11BranchRecords.c'
 'PENTA_SCRIPT'=Join-Path $root 'pentadi/Invoke-PentaDiQualification.ps1'
 'TCP_SCRIPT'=Join-Path $root 'pentadi/Invoke-PentaPiTcp255.ps1'
 'THIS_SCRIPT'=$PSCommandPath
}
$original=@{}
foreach($k in $paths.Keys){
 if(-not [IO.File]::Exists($paths[$k])){
  throw ('PENTAPI|FAIL|source_missing|'+$k)
 }
 $original[$k]=(Get-FileHash -LiteralPath $paths[$k] -Algorithm SHA256).Hash
}
$checks=[System.Collections.Generic.List[object]]::new()
function Confirm-Gate([string]$name,[bool]$condition,[string]$scope){
 $status=if($condition){'PASS'}else{'FAIL'}
 $script:checks.Add([pscustomobject]@{
  gate=$name;status=$status;scope=$scope
 })
 Write-Host ('PENTAPI_GATE|'+$name+'|'+$status+'|'+$scope)
 if(-not $condition){throw ('PENTAPI|FAIL|'+$name)}
}
$up=[IO.File]::ReadAllText($paths['UPSTREAM'])
$down=[IO.File]::ReadAllText($paths['DOWNSTREAM'])
$gated=$up.Contains('needs: quaddi-concurrence',[StringComparison]::Ordinal) -and
 $up.Contains("needs.quaddi-concurrence.result == 'success'",[StringComparison]::Ordinal) -and
 $up.Contains('uses: ./.github/workflows/pentadi-delphi7-asm.yml',[StringComparison]::Ordinal) -and
 $down.Contains('workflow_call:',[StringComparison]::Ordinal) -and
 -not $down.Contains('workflow_dispatch:',[StringComparison]::Ordinal)
Confirm-Gate '01_QUADDI_DEPENDENCY' $gated 'QuadDi four lanes gate PentaDi; no direct dispatch'

# The source remains a user-owned input; only a verified compiler can emit
# new assembler listing. Distinguish this generated ASM from Delphi output.
$asm=Join-Path $penta 'PentaDi_C11_MSVC_O2.asm'
$masm=Join-Path $penta 'PentaDiRel8_reference.obj'
$exe=Join-Path $penta 'PentaDi_C11_TEMP_EXECUTE_ONLY.exe'
$validAsm=[IO.File]::Exists($asm) -and
 [IO.File]::Exists($masm) -and
 -not [IO.File]::Exists($exe)
if($validAsm){
 $validAsm=([IO.FileInfo]::new($asm).Length -gt 200) -and
  ([IO.FileInfo]::new($masm).Length -gt 20) -and
  ([IO.File]::ReadAllText($asm) -match '(?i)\b(?:mov|lea|cmp|call|ret|add|sub|jmp)\b')
}
Confirm-Gate '02_ASM_PROVENANCE' $validAsm 'MSVC C11 FA listing; MASM syntax; no EXE artifact'

# Canonical source record is kept once; compare output EXACTLY and
# independently recalculate the unsigned 16-bit linear rel8 boundaries.
$canonical=[string[]]@(Get-Content -LiteralPath $paths['CANONICAL'] -Encoding ASCII)
$observedPath=Join-Path $penta 'PentaDi_C11_to_Delphi7.tsv'
$recordsOk=[IO.File]::Exists($observedPath) -and $canonical.Count -eq 9
$inside=0;$outside=0
if($recordsOk){
 $observed=[string[]]@(Get-Content -LiteralPath $observedPath -Encoding ASCII)
 $recordsOk=($observed.Count -eq $canonical.Count)
 if($recordsOk){
  for($i=0;$i -lt $canonical.Count;$i++){
   if($canonical[$i] -cne $observed[$i]){$recordsOk=$false;break}
   $fields=$canonical[$i].Split('|')
   if($fields.Count -ne 8){$recordsOk=$false;break}
   if($fields[4] -cnotin @('SHORT','LONG')){$recordsOk=$false;break}
   foreach($j in @(0,1,2,5,6,7)){
    if($fields[$j] -cnotmatch '^[0-9]+$'){
     $recordsOk=$false;break
    }
    if([long]$fields[$j] -gt 65535){$recordsOk=$false;break}
   }
   if(-not $recordsOk){break}
   $delta=[long]$fields[7]-[long]$fields[6]
   if($delta -ge -128 -and $delta -le 127){$inside++}
   else{$outside++}
  }
 }
}
Confirm-Gate '03_C11_RECORD_INTEGRITY' ($recordsOk -and $inside -eq 5 -and $outside -eq 4) '9 records; 5 in-range; 4 rejected'

$historical=[IO.File]::ReadAllText($paths['KEEP_PASS'])
$prof=[IO.File]::ReadAllText($paths['PROFILER'])
$runner=[IO.File]::ReadAllText($paths['RUNNER'])
$unit=[IO.File]::ReadAllText($paths['UNIT'])
$pc=[IO.File]::ReadAllText($paths['PENTA_SCRIPT'])
$profileOk=$historical.StartsWith('unit BuildSpeedProfiler;',[StringComparison]::Ordinal) -and
 $prof.Contains('procedure ApplyQualifiedSafety',[StringComparison]::Ordinal) -and
 $prof.Contains('function QualifiedBuildSafe',[StringComparison]::Ordinal) -and
 $prof.Contains('Opts.Delphi7RangeChecks := True;',[StringComparison]::Ordinal) -and
 $prof.Contains('Opts.Delphi7OverflowChecks := True;',[StringComparison]::Ordinal) -and
 $prof.Contains('Opts.Delphi7IOChecks := True;',[StringComparison]::Ordinal) -and
 $runner.Contains('ApplyQualifiedSafety(Qualified);',[StringComparison]::Ordinal) -and
 $runner.Contains('Profiler.DumpReport;',[StringComparison]::Ordinal) -and
 $pc.Contains('BuildSpeedProfiler.pas',[StringComparison]::Ordinal) -and
 $unit.Contains('{$R+}',[StringComparison]::Ordinal) -and
 $unit.Contains('{$Q+}',[StringComparison]::Ordinal) -and
 $unit.Contains('{$I+}',[StringComparison]::Ordinal)
Confirm-Gate '04_PROFILER_CUSTODY' $profileOk 'keep.pass immutable; checked Pascal; stage measurements'

$tcpPath=Join-Path $tcp 'PentaPi_TCP_255.tsv'
$tcpOk=[IO.File]::Exists($tcpPath)
if($tcpOk){
 $t=[IO.File]::ReadAllText($tcpPath)
 $tcpOk=$t.Contains('initial_ip_ttl'+[char]9+'255',[StringComparison]::Ordinal) -and
  $t.Contains('scope'+[char]9+'127.0.0.1_only',[StringComparison]::Ordinal) -and
  $t.Contains('echo_exact'+[char]9+'PASS',[StringComparison]::Ordinal) -and
  $t.Contains('on_wire_ttl_capture'+[char]9+'NOT_TESTED',[StringComparison]::Ordinal) -and
  $t.Contains('application_retention_years'+[char]9+'NOT_APPLICABLE',[StringComparison]::Ordinal)
}
foreach($k in $paths.Keys){
 if($original[$k] -cne (Get-FileHash -LiteralPath $paths[$k] -Algorithm SHA256).Hash){
  $tcpOk=$false
 }
}
Confirm-Gate '05_TCP255_AND_NO_MUTATION' $tcpOk 'IPv4 loopback framed echo; no external IP; source stable'

$tab=[string][char]9
$csv=Join-Path $out 'PentaPi_lap_001.tsv'
$lines=[System.Collections.Generic.List[string]]::new()
$lines.Add('gate'+$tab+'status'+$tab+'scope')
foreach($c in $checks){
 $lines.Add($c.gate+$tab+$c.status+$tab+$c.scope)
}
[IO.File]::WriteAllLines($csv,$lines,[Text.UTF8Encoding]::new($false))
$proposals=Join-Path $out 'PentaPi_next_lap_proposals.md'
$md=@'
# PentaPi | Next lap, proposed only

Five preflight gates are assessed from exact source, CI dependencies,
C11 output, historical and checked Pascal profiler, and a local TCP echo.
Passing these gates does not claim Delphi 7 compilation.

- HOLD: Obtain an independently licensed, hash-pinned DCC32 and run the
  Delphi7 Pascal compile, branch optimizer, and profiler tests.
- HOLD: Measure actual C11/Delphi speed on pinned hardware and compiler.
- HOLD: Full XHTML 1.0 Strict DTD validation has not been demonstrated.
- HOLD: Packet capture proving outgoing IPv4 TTL on wire is not included.
- APPROVAL: No automatic rewrite, merge, release or self-modifying code.
- AUDIT: A changed experiment uses a new revision and fresh witnesses.
'@
[IO.File]::WriteAllText($proposals,$md,[Text.UTF8Encoding]::new($false))
$sha=(Get-FileHash -LiteralPath $csv -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('PENTAPI|LAP=001|PRECHECK=5_OF_5|PROFILER=WIRED'+
            '|TCP_INITIAL_IPV4_TTL=255|DELPHI7_COMPILER=HOLD'+
            '|NO_AUTO_MUTATION=PASS|LEDGER_SHA256='+$sha+'|RELEASE=HOLD')
