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

# DOVIA railway autopilot: centrally frozen stage order; each vehicle's
# witnesses independently earn a GREEN (PASS input) or AMBER (HOLD input).
# RED means a stage-rule violation, never a silently repaired vehicle.
function Stop-DoviaSignal {
  param([string]$Rule,[string]$Stage,[long]$Ordinal,[string]$Expected)
  # Diagnostic "tiny robot": explain the violated rule with no user payload,
  # external side effects or authority over the originating evidence.
  Write-Host ('DOVIA|SIGNAL_BOT|RED|VEHICLE='+$Ordinal+
    '|STAGE='+$Stage+'|RULE='+$Rule+'|EXPECTED='+$Expected+
    '|ACTION=HOLD_REVIEW_NO_AUTOFIX|RELEASE=HOLD')
  throw ('EXCITE|HOLD|dovia_signal|ordinal='+$Ordinal+
    '|stage='+$Stage+'|rule='+$Rule)
}
# DOVIA.VIEW.1: one logical observer; no physical sensor or invented
# scene geometry. Total centered aperture 30 degrees forward (+/-15)
# and 12 degrees rearward (+/-6). Decimal, never silently rounded.
# QCHAIN remains the authority; signal.jsonl is derived evidence.
function Test-DoviaFieldOfView {
  [CmdletBinding()]
  param(
    [Parameter(Mandatory=$true)][ValidateSet('FRONT','REAR')][string]$Direction,
    [Parameter(Mandatory=$true)][decimal]$OffsetDegrees,
    [int]$CameraCount=1,
    [int]$FrontTotalDegrees=30,
    [int]$RearTotalDegrees=12
  )
  if($CameraCount -ne 1 -or $FrontTotalDegrees -ne 30 -or
     $RearTotalDegrees -ne 12){return $false}
  $half=if($Direction -ceq 'FRONT'){[decimal]15}else{[decimal]6}
  return ($OffsetDegrees -ge (-$half) -and $OffsetDegrees -le $half)
}

function Assert-DoviaRailTransition {
  param(
    [Parameter(Mandatory=$true)][string]$From,
    [Parameter(Mandatory=$true)][string]$To,
    [Parameter(Mandatory=$true)][long]$Ordinal,
    [decimal]$FrontOffsetDegrees=0,
    [decimal]$RearOffsetDegrees=0
  )
  $next=switch -CaseSensitive ($From) {
    'HEAD' {'QCHART'}
    'QCHART' {'QLAM'}
    'QLAM' {'QGRAPH'}
    'QGRAPH' {'INVERSE_SQUARE'}
    'INVERSE_SQUARE' {'FRESH_HAODA'}
    'FRESH_HAODA' {'QCODE'}
    default {''}
  }
  if($next -cne $To){
    Stop-DoviaSignal -Rule 'STAGE_ORDER' -Stage $To -Ordinal $Ordinal -Expected $next
  }
  if(-not (Test-DoviaFieldOfView -Direction FRONT -OffsetDegrees $FrontOffsetDegrees)){
    Stop-DoviaSignal -Rule 'FRONT_FOV_30_TOTAL' -Stage $To -Ordinal $Ordinal -Expected 'OFFSET_WITHIN_PLUS_MINUS_15_DEGREES'
  }
  if($From -cne 'HEAD' -and
     -not (Test-DoviaFieldOfView -Direction REAR -OffsetDegrees $RearOffsetDegrees)){
    Stop-DoviaSignal -Rule 'REAR_FOV_12_TOTAL' -Stage $From -Ordinal $Ordinal -Expected 'OFFSET_WITHIN_PLUS_MINUS_6_DEGREES'
  }
}
function New-DoviaVehicleSignals {
  [CmdletBinding()]
  param(
    [Parameter(Mandatory=$true)][object]$Chart,
    [Parameter(Mandatory=$true)][object]$Lam,
    [Parameter(Mandatory=$true)][object]$Graph,
    [Parameter(Mandatory=$true)][object]$Code,
    [Parameter(Mandatory=$true)][string]$ChartJson,
    [Parameter(Mandatory=$true)][string]$LamJson,
    [Parameter(Mandatory=$true)][string]$GraphJson,
    [Parameter(Mandatory=$true)][string]$CodeJson
  )
  $ordinal=[long]$Chart.ordinal
  $aspects=if($Chart.status -ceq 'PASS'){'GREEN'}elseif($Chart.status -ceq 'HOLD'){'AMBER'}else{'RED'}
  if($aspects -ceq 'RED') {
    Stop-DoviaSignal -Rule 'INPUT_STATUS' -Stage 'QCHART' -Ordinal $ordinal -Expected 'PASS_OR_HOLD'
  }
  $chartSha=Get-ExciteDigest $ChartJson
  $lamSha=Get-ExciteDigest $LamJson
  $graphSha=Get-ExciteDigest $GraphJson
  $inverseJson=ConvertTo-ExciteJson $Code.inverse_square_exact
  $freshJson=ConvertTo-ExciteJson $Code.fresh_haoda
  $codeSha=Get-ExciteDigest $CodeJson
  $radii=@($Code.inverse_square_exact)
  $inverseOK=($radii.Count -eq 3)
  if($inverseOK){
    for([int]$r=1;$r -le 3;$r++){
      $w=$radii[$r-1]
      if($w.radius -cne [string]$r -or $w.numerator -cne '1' -or
         $w.denominator -cne [string]($r*$r)){$inverseOK=$false;break}
    }
  }
  $tests=@(
    [pscustomobject]@{
      stage='QCHART';rule='IMMUTABLE_SOURCE_AND_STATUS';proof=$chartSha
      valid=($Chart.source_sha256 -cmatch '^[a-f0-9]{64}$' -and
        $Chart.label_sha256 -ceq (Get-ExciteDigest $Chart.label) -and
        $Chart.kind -cin @('FILE','SUBPROJECT','REPOSITORY'))
    },
    [pscustomobject]@{
      stage='QLAM';rule='CANONICAL_HASH_AND_PREDECESSOR';proof=$lamSha
      valid=($Lam.chart_sha256 -ceq $chartSha -and
        $Lam.immutable_input_sha256 -ceq $Chart.source_sha256 -and
        $Lam.previous_qcode_sha256 -cmatch '^[a-f0-9]{64}$')
    },
    [pscustomobject]@{
      stage='QGRAPH';rule='ACYCLIC_DIRECTED_WITNESS';proof=$graphSha
      valid=($Graph.chart_sha256 -ceq $chartSha -and
        $Graph.lam_sha256 -ceq $lamSha -and
        $Graph.cycle_count -eq 0 -and
        @($Graph.directed_edges).Count -eq 2 -and
        $Graph.directed_edges[0] -ceq 'QCHART>QLAM' -and
        $Graph.directed_edges[1] -ceq 'QLAM>QGRAPH')
    },
    [pscustomobject]@{
      stage='INVERSE_SQUARE';rule='EXACT_RATIONAL_1_4_9';proof=(Get-ExciteDigest $inverseJson)
      valid=$inverseOK
    },
    [pscustomobject]@{
      stage='FRESH_HAODA';rule='NFKC_DISPLAY_ONLY';proof=(Get-ExciteDigest $freshJson)
      valid=($Code.fresh_haoda.policy -ceq 'NFKC_DISPLAY_ONLY' -and
        $Code.fresh_haoda.canonical_label_sha256 -ceq $Chart.label_sha256 -and
        $Code.fresh_haoda.canonical_source_sha256 -ceq $Chart.source_sha256 -and
        $Code.fresh_haoda.source_mutated -eq $false -and
        $Code.fresh_haoda.normalized_display_label -ceq
          $Chart.label.Normalize([Text.NormalizationForm]::FormKC))
    },
    [pscustomobject]@{
      stage='QCODE';rule='HEAD_TAIL_SOURCE_STATUS_AND_RELEASE';proof=$codeSha
      valid=($Code.graph_sha256 -ceq $graphSha -and
        $Code.previous_qcode_sha256 -ceq $Lam.previous_qcode_sha256 -and
        $Code.kind -ceq $Chart.kind -and $Code.status -ceq $Chart.status -and
        $Code.ordinal -eq $ordinal -and $Code.release -ceq 'HOLD')
    }
  )
  $before='HEAD'
  $signalSha=$Lam.previous_qcode_sha256
  $signals=[Collections.Generic.List[object]]::new()
  foreach($checkpoint in $tests){
    Assert-DoviaRailTransition -From $before -To $checkpoint.stage -Ordinal $ordinal
    if(-not $checkpoint.valid){
      Stop-DoviaSignal -Rule $checkpoint.rule -Stage $checkpoint.stage -Ordinal $ordinal -Expected 'VALID_WITNESS'
    }
    $signalSha=Get-ExciteDigest ('DOVIA.SIGNAL.2|SINGLE_VIEW|30_FRONT|12_REAR|'+$ordinal+'|'+
      $Chart.kind+'|'+$aspects+'|'+$before+'>'+ $checkpoint.stage+'|'+
      $checkpoint.proof+'|'+$signalSha)
    $signals.Add([ordered]@{
      stage=$checkpoint.stage
      aspect=$aspects
      rule=$checkpoint.rule
      proof_sha256=$checkpoint.proof
      signal_sha256=$signalSha
    })
    $before=$checkpoint.stage
  }
  return [ordered]@{
    schema='DOVIA.SIGNAL.2'
    view_schema='DOVIA.VIEW.1'
    observer_count=1
    front_total_degrees=30
    rear_total_degrees=12
    front_axis_degrees=0
    rear_axis_degrees=180
    aperture_boundary='INCLUSIVE_CENTERED'
    view_policy='DERIVED_FROM_ONE_CANONICAL_INPUT_NO_SIDE_VISIBILITY'
    ordinal=$ordinal
    kind=$Chart.kind
    input_status=$Chart.status
    aspect=$aspects
    input_sha256=$Chart.source_sha256
    previous_qcode_sha256=$Lam.previous_qcode_sha256
    stages=@($signals.ToArray())
    stage_count=$signals.Count
    head_sha256=$signalSha
    decision=if($aspects -ceq 'GREEN'){'EVIDENCE_READY'}else{'UPSTREAM_HOLD_NOT_PROMOTED'}
    release='HOLD'
  }
}

function Initialize-ExciteEvidence([string]$directory) {
  $directory=[IO.Path]::GetFullPath($directory)
  if([IO.Directory]::Exists($directory)) {throw 'EXCITE|HOLD|existing_evidence'}
  [void][IO.Directory]::CreateDirectory($directory)
  foreach($name in @('qchart','qlam','qgraph','qcode','signal')) {
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
  # Vehicle-local rail autopilot checks ALL six stages before writing evidence.
  $signals=New-DoviaVehicleSignals -Chart $qchart -Lam $qlam -Graph $qgraph -Code $qcode `
    -ChartJson $chartJson -LamJson $lamJson -GraphJson $graphJson -CodeJson $codeJson
  $signalJson=ConvertTo-ExciteJson $signals
  $utf8=[Text.UTF8Encoding]::new($false)
  # QCODE is appended last; a partial write is detected by the final audit.
  foreach($pair in @(
    @('qchart',$chartJson),@('qlam',$lamJson),
    @('qgraph',$graphJson),@('qcode',$codeJson),@('signal',$signalJson))) {
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
  Write-Host ('DOVIA|SINGLE_VIEW|VEHICLE='+$ordinal+'|OBSERVER=1|FRONT_TOTAL=30|REAR_TOTAL=12|SIDE=UNOBSERVED|SOURCE=CANONICAL_QCHAIN|RELEASE=HOLD')
  Write-Host ('DOVIA|RAIL_AUTOPILOT|VEHICLE='+$ordinal+'|ASPECT='+$signals.aspect+
    '|STAGES='+$signals.stage_count+'|HEAD='+$signals.head_sha256+'|RELEASE=HOLD')
  if($signals.aspect -ceq 'AMBER'){
    Write-Host ('DOVIA|SIGNAL_BOT|AMBER|VEHICLE='+$ordinal+
      '|STAGE=INPUT|RULE=UPSTREAM_HOLD|EXPECTED=INDEPENDENT_REVIEW|ACTION=KEEP_EVIDENCE_NO_PROMOTION')
  }
  Write-Host ('EXCITE|COREAO|'+$Kind+'|'+$ordinal+'|'+$Status+
              '|QCODE_SHA256='+$codeSha+'|RELEASE=HOLD')
  return $codeSha
}
function Assert-ExciteEvidence([object]$Context) {
  $tables=@{}
  foreach($name in @('qchart','qlam','qgraph','qcode','signal')){
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
    $signal=ConvertFrom-Json -InputObject $tables['signal'][$i] -AsHashtable -Depth 9
    $number=$i+1
    # Independent replay of the frozen central signal rules for this vehicle.
    $expectedSignal=New-DoviaVehicleSignals -Chart $chart -Lam $lam -Graph $graph -Code $code `
      -ChartJson $tables['qchart'][$i] -LamJson $tables['qlam'][$i] `
      -GraphJson $tables['qgraph'][$i] -CodeJson $tables['qcode'][$i]
    if((ConvertTo-ExciteJson $expectedSignal) -cne $tables['signal'][$i]){
      Stop-DoviaSignal -Rule 'SIGNAL_RECEIPT_OR_PROVENANCE_TAMPER' -Stage 'QCODE' `
        -Ordinal $number -Expected 'EXACT_RECOMPUTED_SIGNAL_STREAM'
    }
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
      Write-Host ('DOVIA|SIGNAL_BOT|RED|VEHICLE='+$number+
        '|STAGE=QCHAIN|RULE=CHAIN_INTEGRITY|ACTION=HOLD_REVIEW_NO_AUTOFIX')
      throw ('EXCITE|HOLD|integrity|ordinal='+$number)
    }
    if(@($code.inverse_square_exact).Count -ne 3){
      Write-Host ('DOVIA|SIGNAL_BOT|RED|VEHICLE='+$number+
        '|STAGE=INVERSE_SQUARE|RULE=WITNESS_COUNT|ACTION=HOLD_REVIEW_NO_AUTOFIX')
      throw ('EXCITE|HOLD|inverse_square_count|ordinal='+$number)
    }
    for([int]$r=1;$r -le 3;$r++){
      $w=$code.inverse_square_exact[$r-1]
      if($w.radius -cne [string]$r -or $w.numerator -cne '1' -or
         $w.denominator -cne [string]($r*$r)){
        Write-Host ('DOVIA|SIGNAL_BOT|RED|VEHICLE='+$number+
          '|STAGE=INVERSE_SQUARE|RULE=RATIONAL_WITNESS|ACTION=HOLD_REVIEW_NO_AUTOFIX')
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
