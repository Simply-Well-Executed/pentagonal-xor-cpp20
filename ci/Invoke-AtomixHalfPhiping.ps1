# SPDX-License-Identifier: 0BSD
# ATOMIX backport from introduction-to-codeql PR #1, hardened after PR #79.
# Keep original PR #34 Q62 TRIFORCE gate authoritative. Local test only.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'ATOMIX|HOLD|native_MSVC_Windows_required'}
$root=[IO.Path]::TrimEndingDirectorySeparator([IO.Path]::GetFullPath($RepositoryRoot))
$evidence=[IO.Path]::TrimEndingDirectorySeparator([IO.Path]::GetFullPath($EvidenceDirectory))
$guard=Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1'
$gate=Join-Path $root 'ci/Assert-TriforceHeadTail.ps1'
foreach($file in @($guard,$gate)){
  if(-not [IO.File]::Exists($file)){throw ('ATOMIX|HOLD|missing_guard|'+$file)}
}
& pwsh -NoLogo -NoProfile -NonInteractive -File $guard -RepositoryRoot $root -EvidencePath $evidence
if($LASTEXITCODE -ne 0){throw 'ATOMIX|HOLD|external_evidence_boundary'}
if([IO.Directory]::Exists($evidence)){throw 'ATOMIX|HOLD|evidence_must_be_fresh'}
[void][IO.Directory]::CreateDirectory($evidence)
$gateEvidence=Join-Path $evidence 'triforce-state'
$protocol='ATOMIX-PINOY-HALF-PHIPING-PR34-001'
$utf8=[Text.UTF8Encoding]::new($false,$true)
$phases=@(
  [pscustomobject]@{name='HEAD';marker='Σ-Euler-א';expected='271828182845904523'},
  [pscustomobject]@{name='AIRGAP';marker='π-Pi-ب';expected='314159265358979323'},
  [pscustomobject]@{name='TAIL';marker='I-Intensity-한';expected='6849296450333523'}
)
function Get-Digest([byte[]]$bytes){
  return [Convert]::ToHexString(
    [Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
}
$records=[Collections.Generic.List[object]]::new()
foreach($phase in $phases){
  $op='atomix_'+$phase.name.ToLowerInvariant()
  & pwsh -NoLogo -NoProfile -NonInteractive -File $gate -RepositoryRoot $root -EvidenceDirectory $gateEvidence -Operation $op -Phase 'HEAD'
  if($LASTEXITCODE -ne 0){throw ('ATOMIX|HOLD|triforce_HEAD_failed|'+$op)}
  try {
    # Exactly one ASCII bit in a synthetic SARIF Unicode message, copied
    # in memory. The controlled phase variation is deterministic.
    $message=$phase.marker+'|ATOMIX:A:Q62='+$phase.expected
    $fixture=[ordered]@{
      version='2.1.0'
      runs=@([ordered]@{
        tool=@{driver=@{name='AtomixOfflineFixture';version='1'}}
        results=@([ordered]@{
          ruleId='ATOMIX/UNICODE_PIPE'
          level='note'
          message=@{text=$message}
        })
      })
    }
    $json=ConvertTo-Json -InputObject $fixture -Compress -Depth 9 -WarningAction Stop
    $needle='ATOMIX:A:'
    $at=$json.IndexOf($needle,[StringComparison]::Ordinal)
    if($at -lt 0 -or
       $json.IndexOf($needle,$at+1,[StringComparison]::Ordinal) -ge 0){
      throw 'ATOMIX|HOLD|nonunique_test_coordinate'
    }
    $offset=$utf8.GetByteCount($json.Substring(0,$at))+
            $utf8.GetByteCount('ATOMIX:')
    $original=$utf8.GetBytes($json)
    $mutated=[byte[]]::new($original.Length)
    [Array]::Copy($original,$mutated,$original.Length)
    if($mutated[$offset] -ne 0x41){throw 'ATOMIX|HOLD|invalid_A_reference'}
    $mutated[$offset]=[byte]($mutated[$offset] -bxor 1)
    $differences=0
    for($i=0;$i -lt $original.Length;$i++){
      if($original[$i] -ne $mutated[$i]){
        if($i -ne $offset -or ($original[$i] -bxor $mutated[$i]) -ne 1){
          throw 'ATOMIX|HOLD|mutation_exceeds_one_bit'
        }
        $differences++
      }
    }
    if($differences -ne 1 -or
       ($mutated[$offset] -bxor 1) -ne $original[$offset]){
      throw 'ATOMIX|HOLD|mutation_not_exactly_reversible'
    }
    $before=ConvertFrom-Json -InputObject $utf8.GetString($original) -Depth 9
    $after=ConvertFrom-Json -InputObject $utf8.GetString($mutated) -Depth 9
    if($before.version -cne '2.1.0' -or
       $after.version -cne '2.1.0' -or
       @($after.runs).Count -ne 1 -or
       @($after.runs[0].results).Count -ne 1 -or
       $after.runs[0].results[0].ruleId -cne 'ATOMIX/UNICODE_PIPE' -or
       $before.runs[0].results[0].message.text -cne $message -or
       $after.runs[0].results[0].message.text -cne
         $message.Replace($needle,'ATOMIX:@:')){
      throw ('ATOMIX|HOLD|SARIF_semantic_boundary|'+$phase.name)
    }
    $record=[ordered]@{
      protocol=$protocol
      phase=$phase.name
      source='pentagonal-xor-cpp20:PR34:shared/toolong_q62_core.hpp'
      expected_q62=$phase.expected
      original_sha256=(Get-Digest $original)
      xor_probe_sha256=(Get-Digest $mutated)
      utf8_byte_offset=$offset
      xor_mask='0x01'
      changed_bits=1
      parsed_sarif_version='2.1.0'
      status='LOCAL_PASS'
      live_upload='FORBIDDEN'
      release='HOLD'
    }
    $records.Add([pscustomobject]$record)
    $path=Join-Path $evidence ($phase.name.ToLowerInvariant()+'.json')
    [IO.File]::WriteAllText($path,($record|ConvertTo-Json -Depth 9 -WarningAction Stop),
      [Text.UTF8Encoding]::new($false))
    Write-Host ('ATOMIX|'+$phase.name+'|LOCAL_PROBE_PASS|XOR_BITS=1'+
      '|BASE='+$record.original_sha256+'|PROBE='+$record.xor_probe_sha256)
  } finally {
    & pwsh -NoLogo -NoProfile -NonInteractive -File $gate -RepositoryRoot $root -EvidenceDirectory $gateEvidence -Operation $op -Phase 'TAIL'
    if($LASTEXITCODE -ne 0){throw ('ATOMIX|HOLD|triforce_TAIL_failed|'+$op)}
  }
}
$gateState=Join-Path $gateEvidence 'triforce-state.json'
if(-not [IO.File]::Exists($gateState)){throw 'ATOMIX|HOLD|missing_triforce_ledger'}
$state=Get-Content -LiteralPath $gateState -Raw | ConvertFrom-Json -AsHashtable
if($state.phase -cne 'TAIL' -or [int]$state.completed -ne 3 -or
   $records.Count -ne 3){throw 'ATOMIX|HOLD|incomplete_three_phase_gate'}
$audit=[ordered]@{
  schema='SWE-ATOMIX-HALF-PHIPING-PR34-v1'
  protocol=$protocol
  source_commit=[string]$env:GITHUB_SHA
  original_triforce='pentagonal-xor-cpp20 PR34'
  q62_state_sha256=(Get-FileHash -LiteralPath $gateState -Algorithm SHA256).Hash.ToLowerInvariant()
  stages=@($records.ToArray())
  actual_remote_SARIF_testing='NONE'
  circle_of_sanity='original_native_Q62_gate_is_authority'
  airgap='logical_only_not_physical'
  reviewer_approval='NOT_AUTHENTICATED'
  release='HOLD'
}
$auditPath=Join-Path $evidence 'atomix-audit.json'
[IO.File]::WriteAllText($auditPath,($audit|ConvertTo-Json -Depth 9 -WarningAction Stop),
  [Text.UTF8Encoding]::new($false))
$hash=(Get-FileHash -LiteralPath $auditPath -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('ATOMIX|TRIFORCE|HEAD_AIRGAP_TAIL|Q62_PASS'+
  '|AUDIT_SHA256='+$hash+'|LOCAL_ONLY|RELEASE=HOLD')
Write-Host 'ATOMIX|AUDIT_JSON_BEGIN'
Write-Host ([IO.File]::ReadAllText($auditPath,[Text.Encoding]::UTF8))
Write-Host 'ATOMIX|AUDIT_JSON_END'
