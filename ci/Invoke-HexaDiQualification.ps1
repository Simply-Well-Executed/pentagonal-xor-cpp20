# SPDX-License-Identifier: 0BSD
# HEXADI-001: sixth lane, pinned PowerShell 7.7.0 + in-process embedded C#.
# First five lanes remain independent; speed never waives correctness.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$utf8=[Text.UTF8Encoding]::new($false,$true)
if(-not $IsWindows -or
   $PSVersionTable.PSVersion.ToString() -cne '7.7.0' -or
   [Runtime.InteropServices.RuntimeInformation]::ProcessArchitecture.ToString() -cne 'X64'){
 throw 'HEXADI|HOLD|pinned_PowerShell_7_7_0_Windows_x64_required'
}
if(-not [IO.Directory]::Exists($root) -or [IO.Directory]::Exists($out) -or
   $out -ceq $root -or
   $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'HEXADI|HOLD|unsafe_or_nonfresh_evidence'
}
$required=@(
 'pentadi/PentaDiC11BranchRecords.reference.tsv',
 'pentadi/Invoke-PentaDiQualification.ps1',
 'ci/Invoke-HexaDiQualification.ps1'
)
$source=[Collections.Generic.List[object]]::new()
foreach($relative in $required){
 $path=Join-Path $root $relative
 if(-not [IO.File]::Exists($path)){throw ('HEXADI|HOLD|missing_fifth_sixth_lane_source|'+$relative)}
 $source.Add([pscustomobject]@{Path=$path;Sha=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()})
}
$records=@([IO.File]::ReadAllLines((Join-Path $root $required[0]),$utf8))
if($records.Count -ne 9){throw 'HEXADI|HOLD|nine_canonical_PentaDi_records_required'}
foreach($record in $records){
 if($record -cnotmatch '^[0-9]+\|[0-9]+\|[0-9]+\|[A-Z0-9_]+\|(SHORT|LONG)\|[0-9]+\|[0-9]+\|[0-9]+$'){
  throw 'HEXADI|HOLD|invalid_PentaDi_reference_record'
 }
}
[void][IO.Directory]::CreateDirectory($out)
# Embedded C# is compiled in-process by Add-Type. No external package, Node,
# downloaded source snippet, Python, or emitted distributable executable.
$code=@'
using System;
public static class HexaDiCSharp001 {
  public static byte[] XorShell(byte[] input) {
    if(input == null) throw new ArgumentNullException(nameof(input));
    var result = new byte[input.Length];
    for(int i=0;i<input.Length;i++)
      result[i] = (byte)(0x96 ^ (0x69 ^ input[i]));
    return result;
  }
  public static byte[] OrEnvelope(byte[] input) {
    if(input == null) throw new ArgumentNullException(nameof(input));
    var result = XorShell(input);
    for(int i=0;i<result.Length;i++)
      result[i] = (byte)(0x24 | result[i]);
    return result;
  }
  public static byte[] Recover(byte[] shell) {
    if(shell == null) throw new ArgumentNullException(nameof(shell));
    var result = new byte[shell.Length];
    for(int i=0;i<shell.Length;i++)
      result[i] = (byte)(0x69 ^ (0x96 ^ shell[i]));
    return result;
  }
}
'@
$codeHash=[Convert]::ToHexString(
 [Security.Cryptography.SHA256]::HashData($utf8.GetBytes($code))).ToLowerInvariant()
Add-Type -TypeDefinition $code -ErrorAction Stop
# OR masks this 0x04 mutation while XOR keeps it observable.
$original=[byte]0xdb
$mutated=[byte]($original -bxor 0x04)
if([HexaDiCSharp001]::XorShell([byte[]]@($original))[0] -eq
   [HexaDiCSharp001]::XorShell([byte[]]@($mutated))[0] -or
   [HexaDiCSharp001]::OrEnvelope([byte[]]@($original))[0] -ne
   [HexaDiCSharp001]::OrEnvelope([byte[]]@($mutated))[0]){
 throw 'HEXADI|HOLD|OR_masking_negative_canary_failed'
}
$rows=[Collections.Generic.List[string]]::new()
$rows.Add('lap'+[char]9+'record'+[char]9+'input_sha256'+[char]9+
 'xor_sha256'+[char]9+'envelope_sha256'+[char]9+'observed_ticks'+[char]9+'result')
$clock=[Diagnostics.Stopwatch]::Frequency
if($clock -le 0){throw 'HEXADI|HOLD|monotonic_clock_missing'}
for([int]$lap=1;$lap -le 255;$lap++){
 $index=($lap-1)%$records.Count
 $payload=$utf8.GetBytes($records[$index])
 $before=[Diagnostics.Stopwatch]::GetTimestamp()
 [byte[]]$shell=[HexaDiCSharp001]::XorShell($payload)
 [byte[]]$wrapped=[HexaDiCSharp001]::OrEnvelope($payload)
 [byte[]]$restored=[HexaDiCSharp001]::Recover($shell)
 $ticks=[Diagnostics.Stopwatch]::GetTimestamp()-$before
 if($shell.Count -ne $payload.Count -or $wrapped.Count -ne $payload.Count -or
    $restored.Count -ne $payload.Count){throw ('HEXADI|HOLD|length|lap='+$lap)}
 for($i=0;$i -lt $payload.Count;$i++){
  $p=[int]$payload[$i]
  $inner=(0x69 -bxor $p)
  $expectedXor=(0x96 -bxor $inner)
  $expectedOr=(0x24 -bor $expectedXor)
  if($shell[$i] -ne $expectedXor -or $wrapped[$i] -ne $expectedOr -or
     $restored[$i] -ne $payload[$i]){
   throw ('HEXADI|HOLD|CSharp_PowerShell_witness_disagree|lap='+$lap+'|byte='+$i)
  }
 }
 $inputHash=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($payload)).ToLowerInvariant()
 $xorHash=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($shell)).ToLowerInvariant()
 $orHash=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($wrapped)).ToLowerInvariant()
 $rows.Add(('{0:000}' -f $lap)+[char]9+($index+1)+[char]9+
  $inputHash+[char]9+$xorHash+[char]9+$orHash+[char]9+$ticks+[char]9+'PASS')
}
foreach($item in $source){
 if((Get-FileHash -LiteralPath $item.Path -Algorithm SHA256).Hash.ToLowerInvariant() -cne $item.Sha){
  throw 'HEXADI|HOLD|source_changed_during_qualification'
 }
}
$ledger=Join-Path $out 'hexadi-255-witness.tsv'
[IO.File]::WriteAllText($ledger,([string]::Join([string][char]10,$rows)+[char]10),$utf8)
$readback=@([IO.File]::ReadAllLines($ledger,$utf8))
if($readback.Count -ne 256){throw 'HEXADI|HOLD|ledger_count_mismatch'}
for($i=0;$i -lt $rows.Count;$i++){
 if($readback[$i] -cne $rows[$i]){throw ('HEXADI|HOLD|ledger_mutation|'+$i)}
}
$ledgerHash=(Get-FileHash -LiteralPath $ledger -Algorithm SHA256).Hash.ToLowerInvariant()
$receipt=@(
 'schema'+[char]9+'HEXADI-001',
 'powershell'+[char]9+$PSVersionTable.PSVersion.ToString(),
 'csharp_compilation'+[char]9+'Add-Type_in_process',
 'embedded_csharp_sha256'+[char]9+$codeHash,
 'pentadi_source_sha256'+[char]9+$source[0].Sha,
 'native_witness_laps'+[char]9+'255',
 'negative_or_masking_canary'+[char]9+'PASS',
 'logical_atmoseconds_first_nine'+[char]9+'45',
 'logical_atmoseconds_meaning'+[char]9+'triangular_counter_not_wall_time',
 'clock_frequency_hz'+[char]9+$clock,
 'ledger_sha256'+[char]9+$ledgerHash,
 'release'+[char]9+'HOLD'
)
[IO.File]::WriteAllText((Join-Path $out 'hexadi-receipt.tsv'),
 ([string]::Join([string][char]10,$receipt)+[char]10),$utf8)
Write-Host ('HEXADI|PASS|PWSH=7.7.0|EMBEDDED_CSHARP=PASS|XOR_INVERSE=PASS'+
 '|OR_MASK_NEGATIVE=PASS|LAPS=255|SOURCE_STABLE=YES|LEDGER_SHA256='+$ledgerHash+
 '|ATMOSECONDS=MODEL_NOT_ELAPSED_TIME|RELEASE=HOLD')
