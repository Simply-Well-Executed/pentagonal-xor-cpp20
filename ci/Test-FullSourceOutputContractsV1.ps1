# SPDX-License-Identifier: 0BSD
# Typed/negative output contracts V1, in the style of independently testable
# W3C QA assertions. Only SVG XML checks have W3C format scope. The CLI and
# JSON assertions use their own program and IETF data contracts.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$InstalledBinDirectory,
 [ValidateSet('Windows','Linux')][Parameter(Mandatory=$true)][string]$Platform
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$bin=[IO.Path]::GetFullPath($InstalledBinDirectory)
if(-not [IO.Directory]::Exists($bin)){throw 'OUTPUT_CONTRACT_V1|HOLD|missing_install_dir'}
function Run-Native([string]$File,[string[]]$Args,[int]$ExpectedExit,[string]$Contract){
 if(-not [IO.File]::Exists($File)){throw ('OUTPUT_CONTRACT_V1|HOLD|binary_missing|'+$Contract)}
 $info=[Diagnostics.ProcessStartInfo]::new($File)
 $info.UseShellExecute=$false
 $info.RedirectStandardOutput=$true
 $info.RedirectStandardError=$true
 $info.CreateNoWindow=$true
 $info.StandardOutputEncoding=[Text.UTF8Encoding]::new($false,$true)
 $info.StandardErrorEncoding=[Text.UTF8Encoding]::new($false,$true)
 foreach($arg in $Args){[void]$info.ArgumentList.Add($arg)}
 $p=[Diagnostics.Process]::new()
 $p.StartInfo=$info
 if(-not $p.Start()){throw ('OUTPUT_CONTRACT_V1|HOLD|start_failed|'+$Contract)}
 try{
  $stdout=$p.StandardOutput.ReadToEnd()
  $stderr=$p.StandardError.ReadToEnd()
  if(-not $p.WaitForExit(60000)){
   $p.Kill($true)
   throw ('OUTPUT_CONTRACT_V1|HOLD|execution_timeout|'+$Contract)
  }
  if($p.ExitCode -ne $ExpectedExit){
   throw ('OUTPUT_CONTRACT_V1|HOLD|exit_code|'+$Contract+'|ACTUAL='+$p.ExitCode+'|EXPECTED='+$ExpectedExit)
  }
  return [pscustomobject]@{Output=$stdout;Error=$stderr;ExitCode=$p.ExitCode}
 }finally{$p.Dispose()}
}
$extension=if($Platform -ceq 'Windows'){'.exe'}else{''}
$main=Join-Path $bin ('pentagonal_xor'+$extension)
$ok=Run-Native $main @('0.80','0.70','0.90','0.60','0.95') 0 'normal_parity'
$line=$ok.Output.TrimEnd([char[]]@([char]13,[char]10))
if($line -cne 'Pentagonal XOR: 51.72800000%' -or
   $line -cnotmatch '^Pentagonal XOR: (?:100|[0-9]{1,2})[.][0-9]{8}%$' -or
   -not [string]::IsNullOrEmpty($ok.Error)){
 throw 'OUTPUT_CONTRACT_V1|HOLD|parity_exact_ascii_decimal_contract'
}
$number=$line.Substring('Pentagonal XOR: '.Length).TrimEnd('%')
$score=[decimal]::Parse($number,[Globalization.CultureInfo]::InvariantCulture)
if($score -lt 0 -or $score -gt 100){throw 'OUTPUT_CONTRACT_V1|HOLD|parity_unit_range'}
foreach($case in @(
 @('0.8','0.7','1.2','0.6','0.95'),
 @('0.8','0.7','nan','0.6','0.95'),
 @('0.8','0.7','0.9','0.6')
)){
 $bad=Run-Native $main $case 2 'parity_negative'
 if($bad.Output.Trim().Length -ne 0 -or
    $bad.Error -cnotmatch 'Error:|Usage:'){
  throw 'OUTPUT_CONTRACT_V1|HOLD|negative_test_unexpected_output'
 }
}
$control=Join-Path $bin ('pentagonal_control'+$extension)
$json=Run-Native $control @('0x3fffffff','0x3fffffff','or') 0 'control_json'
if(-not [string]::IsNullOrEmpty($json.Error)){throw 'OUTPUT_CONTRACT_V1|HOLD|control_stderr'}
try{$object=ConvertFrom-Json -InputObject $json.Output -AsHashtable -Depth 30}catch{
 throw 'OUTPUT_CONTRACT_V1|HOLD|control_json_invalid'
}
if($object.schema -cne 'SWE-PENTAGONAL-CXX20-experimental' -or
   $object.source_mutated -isnot [bool] -or $object.source_mutated -ne $false -or
   $object.witness_supplied -isnot [bool] -or $object.witness_supplied -ne $true -or
   $object.logical_evaluations -isnot [ValueType] -or
   $object.loops_completed -isnot [ValueType] -or
   @($object.stages).Count -ne 6 -or @($object.blocks).Count -ne 6 -or
   $object.operand_hex -cnotmatch '^0x[a-fA-F0-9]{16}$'){
 throw 'OUTPUT_CONTRACT_V1|HOLD|control_json_schema_or_types'
}
$expectedNames=@('xor','or','and','nop','cor','flop')
for($i=0;$i -lt 6;$i++){
 if($object.stages[$i].name -cne $expectedNames[$i] -or
    $object.stages[$i].value -isnot [bool] -or
    $object.blocks[$i].xor -isnot [bool] -or
    $object.blocks[$i].or -isnot [bool] -or
    $object.blocks[$i].and -isnot [bool]){
  throw ('OUTPUT_CONTRACT_V1|HOLD|control_stage_type|'+$i)
 }
}
$svgFiles=@(Get-ChildItem -LiteralPath (Join-Path $root 'docs') -File -Filter '*.svg')
if($svgFiles.Count -lt 3){throw 'OUTPUT_CONTRACT_V1|HOLD|svg_reference_files_missing'}
foreach($file in $svgFiles){
 $settings=[Xml.XmlReaderSettings]::new()
 $settings.DtdProcessing=[Xml.DtdProcessing]::Prohibit
 $settings.XmlResolver=$null
 $settings.MaxCharactersInDocument=5000000
 $settings.MaxCharactersFromEntities=0
 try{
  $reader=[Xml.XmlReader]::Create($file.FullName,$settings)
  try{
   [void]$reader.MoveToContent()
   if($reader.LocalName -cne 'svg' -or
      $reader.NamespaceURI -cne 'http://www.w3.org/2000/svg'){
    throw 'svg_root_or_namespace_mismatch'
   }
   while($reader.Read()){}
  }finally{$reader.Dispose()}
 }catch{throw ('OUTPUT_CONTRACT_V1|HOLD|W3C_SVG_XML_WELL_FORMEDNESS|'+$file.Name+'|'+$_.Exception.Message)}
}
Write-Host ('OUTPUT_CONTRACT_V1|PASS|NORMAL_PARITY_DECIMAL_TYPE|3_NEGATIVE_CASES|CONTROL_JSON_TYPED_STAGES=6'+
 '|W3C_SVG_XML_WELL_FORMEDNESS_AND_ROOT_NAMESPACE='+$svgFiles.Count+
 '|PLATFORM='+$Platform+'|NO_BROWSER_RENDER_CERTIFICATION')
