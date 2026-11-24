# SPDX-License-Identifier: 0BSD
# EXCITE/COREAO-Q-001: evidence envelopes, not undisclosed Q algorithms.
# QCHART -> QLAM -> QGRAPH -> exact inverse-square witnesses -> FRESH HAODA -> QCODE.
# Source files, original labels and trust decisions are never rewritten.
Set-StrictMode -Version 2.0

function Get-ExciteDigest([string]$value) {
  return [Convert]::ToHexString(
    [Security.Cryptography.SHA256]::HashData(
      [Text.Encoding]::UTF8.GetBytes($value))).ToLowerInvariant()
}
function ConvertTo-ExciteJson([object]$value) {
  return (ConvertTo-Json -InputObject $value -Compress -Depth 9 -WarningAction Stop)
}
function Initialize-ExciteEvidence([string]$directory) {
  $directory=[IO.Path]::GetFullPath($directory)
  if([IO.Directory]::Exists($directory)) {throw 'EXCITE|HOLD|existing_evidence'}
  [void][IO.Directory]::CreateDirectory($directory)
  foreach($name in @('qchart','qlam','qgraph','qcode')) {
    [IO.File]::WriteAllText((Join-Path $directory ($name+'.jsonl')),'',
      [Text.UTF8Encoding]::new($false))
  }
  return [pscustomobject]@{
    Directory=$directory
    Sequence=[long]0
    PreviousCodeSha=('0'*64)
    FileCount=[long]0
    SubprojectCount=[long]0
    RepositoryCount=[long]0
  }
}
function Add-ExciteVehicle {
  [CmdletBinding()]
  param(
    [Parameter(Mandatory=$true)][object]$Context,
    [Parameter(Mandatory=$true)][ValidateSet('FILE','SUBPROJECT','REPOSITORY')][string]$Kind,
    [Parameter(Mandatory=$true)][string]$Label,
    [Parameter(Mandatory=$true)][ValidatePattern('^[a-f0-9]{64}$')][string]$SourceSha,
    [Parameter(Mandatory=$true)][ValidateSet('PASS','HOLD')][string]$Status,
    [ValidateRange(0,9223372036854775807)][long]$ElapsedMs=0,
    [ValidateRange(0,9223372036854775807)][long]$Bytes=0,
    [string]$Detail=''
  )
  if($Context.Sequence -ge 1000000 -or $Label.Length -gt 4096 -or
     $Detail.Length -gt 4096 -or $Label -match '[\r\n\t]' -or
     $Detail -match '[\r\n\t]') {
    throw 'EXCITE|HOLD|unsafe_or_unbounded_vehicle'
  }
  $ordinal=[long]$Context.Sequence+1
  # Source identity stays byte-exact. NFKC affects display metadata ONLY.
  $labelSha=Get-ExciteDigest $Label
  $qchart=[ordered]@{
    schema='EXCITE.QCHART.1'
    ordinal=$ordinal
    kind=$Kind
    label=$Label
    label_sha256=$labelSha
    source_sha256=$SourceSha
    status=$Status
    bytes=$Bytes
    elapsed_ms=$ElapsedMs
    detail=$Detail
  }
  $chartJson=ConvertTo-ExciteJson $qchart
  $chartSha=Get-ExciteDigest $chartJson
  $qlam=[ordered]@{
    schema='EXCITE.QLAM.1'
    ordinal=$ordinal
    chart_sha256=$chartSha
    previous_qcode_sha256=$Context.PreviousCodeSha
    immutable_input_sha256=$SourceSha
  }
  $lamJson=ConvertTo-ExciteJson $qlam
  $lamSha=Get-ExciteDigest $lamJson
  $qgraph=[ordered]@{
    schema='EXCITE.QGRAPH.1'
    ordinal=$ordinal
    chart_sha256=$chartSha
    lam_sha256=$lamSha
    directed_edges=@('QCHART>QLAM','QLAM>QGRAPH')
    cycle_count=0
  }
  $graphJson=ConvertTo-ExciteJson $qgraph
  $graphSha=Get-ExciteDigest $graphJson
  # Exact, dimensionless witnesses at radius 1,2,3. NOT a radiation model.
  $radii=@(1,2,3)
  $inverse=@(foreach($r in $radii) {
    $d=[Numerics.BigInteger]::Multiply(
      [Numerics.BigInteger]::new($r),[Numerics.BigInteger]::new($r))
    [ordered]@{radius=[string]$r;numerator='1';denominator=$d.ToString()}
  })
  $fresh=[ordered]@{
    schema='EXCITE.FRESH_HAODA.1'
    policy='NFKC_DISPLAY_ONLY'
    normalized_display_label=$Label.Normalize([Text.NormalizationForm]::FormKC)
    canonical_label_sha256=$labelSha
    canonical_source_sha256=$SourceSha
    source_mutated=$false
  }
  $qcode=[ordered]@{
    schema='EXCITE.QCODE.1'
    ordinal=$ordinal
    kind=$Kind
    status=$Status
    graph_sha256=$graphSha
    inverse_square_exact=$inverse
    inverse_square_scope='unitless_witness_not_physical_attenuation'
    fresh_haoda=$fresh
    previous_qcode_sha256=$Context.PreviousCodeSha
    release='HOLD'
  }
  $codeJson=ConvertTo-ExciteJson $qcode
  $codeSha=Get-ExciteDigest $codeJson
  $utf8=[Text.UTF8Encoding]::new($false)
  # QCODE is appended last; a partial write is detected by the final audit.
  foreach($pair in @(
    @('qchart',$chartJson),@('qlam',$lamJson),
    @('qgraph',$graphJson),@('qcode',$codeJson))) {
    [IO.File]::AppendAllText(
      (Join-Path $Context.Directory ($pair[0]+'.jsonl')),
      ($pair[1]+[char]10),$utf8)
  }
  $Context.Sequence=$ordinal
  $Context.PreviousCodeSha=$codeSha
  switch($Kind) {
    'FILE' {$Context.FileCount++}
    'SUBPROJECT' {$Context.SubprojectCount++}
    'REPOSITORY' {$Context.RepositoryCount++}
  }
  Write-Host ('EXCITE|COREAO|'+$Kind+'|'+$ordinal+'|'+$Status+
              '|QCODE_SHA256='+$codeSha+'|RELEASE=HOLD')
  return $codeSha
}
function Assert-ExciteEvidence([object]$Context) {
  $tables=@{}
  foreach($name in @('qchart','qlam','qgraph','qcode')){
    $tables[$name]=[IO.File]::ReadAllLines(
      (Join-Path $Context.Directory ($name+'.jsonl')),
      [Text.UTF8Encoding]::new($false,$true))
    if($tables[$name].Count -ne $Context.Sequence){
      throw ('EXCITE|HOLD|stream_count|'+$name)
    }
  }
  $previous='0'*64
  for([int]$i=0;$i -lt $Context.Sequence;$i++){
    $chart=ConvertFrom-Json -InputObject $tables['qchart'][$i] -AsHashtable -Depth 9
    $lam=ConvertFrom-Json -InputObject $tables['qlam'][$i] -AsHashtable -Depth 9
    $graph=ConvertFrom-Json -InputObject $tables['qgraph'][$i] -AsHashtable -Depth 9
    $code=ConvertFrom-Json -InputObject $tables['qcode'][$i] -AsHashtable -Depth 9
    $number=$i+1
    if($chart.schema -cne 'EXCITE.QCHART.1' -or
       $lam.schema -cne 'EXCITE.QLAM.1' -or
       $graph.schema -cne 'EXCITE.QGRAPH.1' -or
       $code.schema -cne 'EXCITE.QCODE.1' -or
       $chart.ordinal -ne $number -or $lam.ordinal -ne $number -or
       $graph.ordinal -ne $number -or $code.ordinal -ne $number -or
       $lam.chart_sha256 -cne (Get-ExciteDigest $tables['qchart'][$i]) -or
       $graph.chart_sha256 -cne $lam.chart_sha256 -or
       $graph.lam_sha256 -cne (Get-ExciteDigest $tables['qlam'][$i]) -or
       $code.graph_sha256 -cne (Get-ExciteDigest $tables['qgraph'][$i]) -or
       $lam.previous_qcode_sha256 -cne $previous -or
       $code.previous_qcode_sha256 -cne $previous -or
       $chart.source_sha256 -cne $lam.immutable_input_sha256 -or
       $chart.label_sha256 -cne (Get-ExciteDigest $chart.label) -or
       $chart.label_sha256 -cne $code.fresh_haoda.canonical_label_sha256 -or
       $chart.source_sha256 -cne $code.fresh_haoda.canonical_source_sha256 -or
       $code.fresh_haoda.source_mutated -ne $false -or
       $code.fresh_haoda.policy -cne 'NFKC_DISPLAY_ONLY' -or
       $code.fresh_haoda.normalized_display_label -cne
         $chart.label.Normalize([Text.NormalizationForm]::FormKC) -or
       $graph.cycle_count -ne 0 -or
       @($graph.directed_edges).Count -ne 2 -or
       $graph.directed_edges[0] -cne 'QCHART>QLAM' -or
       $graph.directed_edges[1] -cne 'QLAM>QGRAPH' -or
       $code.kind -cne $chart.kind -or $code.status -cne $chart.status -or
       $code.release -cne 'HOLD') {
      throw ('EXCITE|HOLD|integrity|ordinal='+$number)
    }
    if(@($code.inverse_square_exact).Count -ne 3){
      throw ('EXCITE|HOLD|inverse_square_count|ordinal='+$number)
    }
    for([int]$r=1;$r -le 3;$r++){
      $w=$code.inverse_square_exact[$r-1]
      if($w.radius -cne [string]$r -or $w.numerator -cne '1' -or
         $w.denominator -cne [string]($r*$r)){
        throw ('EXCITE|HOLD|inverse_square|ordinal='+$number)
      }
    }
    $previous=Get-ExciteDigest $tables['qcode'][$i]
  }
  if($previous -cne $Context.PreviousCodeSha -or
     $Context.Sequence -ne ($Context.FileCount+$Context.SubprojectCount+
                             $Context.RepositoryCount)){
    throw 'EXCITE|HOLD|terminal_chain_or_counts'
  }
  Write-Host ('EXCITE|CHAIN_VERIFIED|VEHICLES='+$Context.Sequence+
              '|FINAL_QCODE_SHA256='+$previous+'|RELEASE=HOLD')
  return $previous
}
