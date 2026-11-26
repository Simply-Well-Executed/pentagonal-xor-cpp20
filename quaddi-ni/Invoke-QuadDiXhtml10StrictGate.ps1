# SPDX-License-Identifier: 0BSD
# QUADDI-4LANE-XHTML10-STRICT-001: separate scriptless XHTML 1.0 Strict
# profile report. Validates exact DOCTYPE + namespace + well-formed XML
# + locally permitted element set. NOT a full W3C DTD validator; honest HOLD.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [Parameter(Mandatory=$true)][string]$OracleJdkHome
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$ev=[IO.Path]::GetFullPath($EvidenceDirectory)
if($ev -ceq $root -or $ev.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'QUADDI_4LANE|HOLD|evidence_inside_source'
}
if([IO.Directory]::Exists($ev) -and @(Get-ChildItem -LiteralPath $ev -Force).Count -gt 0){
 throw 'QUADDI_4LANE|HOLD|output_directory_not_empty'
}
[IO.Directory]::CreateDirectory($ev)|Out-Null
$jdk=[IO.Path]::GetFullPath($OracleJdkHome)
$manifest=Join-Path $jdk 'release'
$java=Join-Path $jdk 'bin/java.exe'
$javac=Join-Path $jdk 'bin/javac.exe'
if(-not [IO.File]::Exists($manifest) -or -not [IO.File]::Exists($java) -or
   -not [IO.File]::Exists($javac)){throw 'QUADDI_4LANE|HOLD|oracle_jdk_missing'}
$meta=[IO.File]::ReadAllText($manifest)
if($meta -cnotmatch '(?m)^JAVA_VERSION="27\.0\.1\.1"\r?$' -or
   $meta -cnotmatch '(?m)^IMPLEMENTOR="Oracle Corporation"\r?$'){
 throw 'QUADDI_4LANE|HOLD|oracle_latest_pin_mismatch'
}
$sources=@(
 (Join-Path $root 'quaddi-ni/Invoke-NativeQualification.ps1'),
 (Join-Path $root 'quaddi-ni/Invoke-OracleNativeParity.ps1'),
 (Join-Path $root 'quaddi-ni/Invoke-SlipSlidingToolongQualification.ps1'),
 (Join-Path $root 'quaddi-ni/Invoke-BitflyingCInteropMatrix.ps1'),
 (Join-Path $root 'shared/bitflying_c_abi_001.h'),
 $PSCommandPath
)
foreach($source in $sources){
 if(-not [IO.File]::Exists($source)){throw ('QUADDI_4LANE|HOLD|source_missing|'+$source)}
}
$before=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
$report=Join-Path $ev 'quaddi-four-lane-xhtml1-strict.xhtml'
if([IO.File]::Exists($report)){throw 'QUADDI_4LANE|HOLD|output_exists'}
# This bounded report deliberately excludes XHTML5 SVG, scripting, ARIA,
# HTML5 semantics and external entities. It is a separate presentation lane.
$document=@'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>QuadDi Four-Lane Engineering Qualification</title>
<meta http-equiv="Content-Type" content="application/xhtml+xml; charset=UTF-8" />
</head>
<body>
<h1>QuadDi Four-Lane Engineering Qualification</h1>
<p>Distinct language and document standards with source-preserving audit. RELEASE=HOLD.</p>
<table summary="QuadDi independent implementation and format lanes">
<caption>Four distinct validation lanes</caption>
<thead><tr><th scope="col">Lane</th><th scope="col">Mode</th><th scope="col">Validation basis</th></tr></thead>
<tbody>
<tr><td>C++20</td><td>MSVC native /O2</td><td>Separate native qualification step must succeed</td></tr>
<tr><td>Oracle Java</td><td>JDK 27.0.1.1</td><td>Vendor, version, SHA256 and Oracle parity steps must succeed</td></tr>
<tr><td>XHTML 1.0 Strict</td><td>XML 1.0, XHTML Strict DOCTYPE</td><td>Separate XML and bounded profile gate; full DTD validation not asserted</td></tr>
<tr><td>C11</td><td>MSVC /TC /std:c11 /O2</td><td>Shared ABI positive and C-specific negative/positive controls</td></tr>
</tbody>
</table>
<p>Both C and C++ can share an ABI without becoming identical languages. No auto-merge or speed claim.</p>
</body>
</html>
'@
[IO.File]::WriteAllText($report,$document,[Text.UTF8Encoding]::new($false))
$raw=[IO.File]::ReadAllText($report,[Text.Encoding]::UTF8)
$expectedDoctype='<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">'
if(-not $raw.StartsWith('<?xml version="1.0" encoding="UTF-8"?>',[StringComparison]::Ordinal) -or
   -not $raw.Contains($expectedDoctype,[StringComparison]::Ordinal) -or
   $raw.Contains('<svg',[StringComparison]::OrdinalIgnoreCase) -or
   $raw.Contains('<script',[StringComparison]::OrdinalIgnoreCase)){
 throw 'QUADDI_4LANE|HOLD|wrong_xhtml_1_strict_profile'
}
$settings=[Xml.XmlReaderSettings]::new()
$settings.DtdProcessing=[Xml.DtdProcessing]::Parse
$settings.XmlResolver=$null
$settings.MaxCharactersFromEntities=0
$settings.MaxCharactersInDocument=500000
$reader=[Xml.XmlReader]::Create($report,$settings)
$doc=[Xml.XmlDocument]::new()
$doc.XmlResolver=$null
try{$doc.Load($reader)}finally{$reader.Dispose()}
if($doc.DocumentType.Name -cne 'html' -or
   $doc.DocumentType.PublicId -cne '-//W3C//DTD XHTML 1.0 Strict//EN' -or
   $doc.DocumentType.SystemId -cne 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd' -or
   $doc.DocumentElement.LocalName -cne 'html' -or
   $doc.DocumentElement.NamespaceURI -cne 'http://www.w3.org/1999/xhtml'){
 throw 'QUADDI_4LANE|HOLD|xml_doctype_or_namespace'
}
$expected=@('html','head','title','meta','body','h1','p','table','caption',
 'thead','tbody','tr','th','td')
$elements=@($doc.SelectNodes('//*'))
foreach($node in $elements){
 if($node.NamespaceURI -cne 'http://www.w3.org/1999/xhtml' -or
    $node.LocalName -cnotin $expected){
  throw ('QUADDI_4LANE|HOLD|non_strict_profile_element|'+$node.LocalName)
 }
}
if(@($doc.SelectNodes('//*[local-name()="tr"]')).Count -ne 5){
 throw 'QUADDI_4LANE|HOLD|four_lane_table_missing'
}
$after=@($sources|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
for($i=0;$i -lt $sources.Count;$i++){
 if($before[$i] -cne $after[$i]){throw 'QUADDI_4LANE|HOLD|source_mutated'}
}
$hash=(Get-FileHash -LiteralPath $report -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('QUADDI_XHTML10|XML_WELLFORMED=PASS|STRICT_DOCTYPE=PASS|NAMESPACE=PASS|FOUR_LANE_REPORT=PASS|DTD_VALIDATION=NOT_PROVEN|SHA256='+$hash+'|RELEASE=HOLD')
Write-Host 'QUADDI_4LANE|CXX20=PRIOR_GATE|ORACLE_27_0_1_1=PRIOR_GATE|XHTML_1_0_STRICT_PROFILE=PASS|C11=PRIOR_GATE|RELEASE=HOLD'
