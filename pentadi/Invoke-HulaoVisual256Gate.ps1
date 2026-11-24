# SPDX-License-Identifier: 0BSD
# HULAO-VISUAL-256-001 | Validate markdown, XHTML 1.0 Strict profile,
# 256 addressable lap views and independent repository-native SVG.
# This is a bounded XML/profile check, NOT official W3C DTD validation.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$paths=@(
 (Join-Path $root 'docs/PentaPi_Hulao_256.xhtml'),
 (Join-Path $root 'docs/pentapi_hulao_gate.svg'),
 (Join-Path $root 'docs/PENTAPI_HULAO_GATE_255.md'),
 (Join-Path $root 'README.md'),
 $PSCommandPath
)
foreach($p in $paths){
 if(-not [IO.File]::Exists($p)){
  throw ('HULAO_VISUAL|FAIL|missing|'+$p)
 }
}
$hashes=@($paths|ForEach-Object{(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash})
$xhtml=[IO.File]::ReadAllText($paths[0],[Text.Encoding]::UTF8)
if(-not $xhtml.StartsWith('<?xml version="1.0" encoding="UTF-8"?>',
 [StringComparison]::Ordinal)){
 throw 'HULAO_VISUAL|FAIL|xml_declaration'
}
foreach($forbidden in @('<script','<iframe','<footer','<main','<nav','<section',
                            'javascript:','onload=','onclick=')){
 if($xhtml.Contains($forbidden,[StringComparison]::OrdinalIgnoreCase)){
  throw ('HULAO_VISUAL|FAIL|forbidden_xhtml|'+$forbidden)
 }
}
$settings=[Xml.XmlReaderSettings]::new()
$settings.DtdProcessing=[Xml.DtdProcessing]::Parse
$settings.XmlResolver=$null
$settings.MaxCharactersFromEntities=0
$settings.MaxCharactersInDocument=1048576
$reader=[Xml.XmlReader]::Create($paths[0],$settings)
$doc=[Xml.XmlDocument]::new()
$doc.XmlResolver=$null
try{$doc.Load($reader)}finally{$reader.Dispose()}
if($doc.DocumentType.Name -cne 'html' -or
   $doc.DocumentType.PublicId -cne '-//W3C//DTD XHTML 1.0 Strict//EN' -or
   $doc.DocumentType.SystemId -cne 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd' -or
   $doc.DocumentElement.LocalName -cne 'html' -or
   $doc.DocumentElement.NamespaceURI -cne 'http://www.w3.org/1999/xhtml'){
 throw 'HULAO_VISUAL|FAIL|strict_doctype_or_namespace'
}
$allowed=[string[]]@('html','head','title','meta','style','body','div',
 'span','h1','h2','p','img','ol','li','strong','a')
$elements=@($doc.SelectNodes('//*'))
foreach($n in $elements){
 if($n.NamespaceURI -cne 'http://www.w3.org/1999/xhtml' -or
    $n.LocalName -cnotin $allowed){
  throw ('HULAO_VISUAL|FAIL|unapproved_xhtml_element|'+$n.LocalName)
 }
}
$laps=@($doc.SelectNodes('//*[local-name()="div" and starts-with(@id,"lap-")]'))
if($laps.Count -ne 256){throw ('HULAO_VISUAL|FAIL|lap_count|'+$laps.Count)}
$seen=[System.Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach($lap in $laps){
 if(-not $seen.Add([string]$lap.GetAttribute('id'))){
  throw 'HULAO_VISUAL|FAIL|duplicate_lap_id'
 }
}
$hell=0
for($i=1;$i -le 256;$i++){
 $id='lap-'+('{0:000}' -f $i)
 if(-not $seen.Contains($id)){throw ('HULAO_VISUAL|FAIL|missing_lap|'+$id)}
 $node=$doc.SelectSingleNode('//*[@id="'+$id+'"]')
 $cls=[string]$node.GetAttribute('class')
 $next=if($i -eq 256){1}else{$i+1}
 $nextHref='#lap-'+('{0:000}' -f $next)
 $links=@($node.SelectNodes('.//*[local-name()="a"]')|
           ForEach-Object{$_.GetAttribute('href')})
 if($nextHref -cnotin $links){
  throw ('HULAO_VISUAL|FAIL|next_link_missing|'+$id)
 }
 if($i -lt 256){
  if($cls -cnotmatch '(^|\s)siege(\s|$)' -or
     -not $node.InnerText.Contains('INJECTED FAULT',[StringComparison]::Ordinal) -or
     -not $node.InnerText.Contains('RECOVER',[StringComparison]::Ordinal)){
   throw ('HULAO_VISUAL|FAIL|fault_or_recovery_missing|'+$id)
  }
  ++$hell
 }else{
  if($cls -cnotmatch '(^|\s)sanctuary(\s|$)' -or
     -not $node.InnerText.Contains('Fictional', [StringComparison]::OrdinalIgnoreCase)){
   throw 'HULAO_VISUAL|FAIL|fictional_final_not_scoped'
  }
 }
}
$svgReader=[Xml.XmlReader]::Create($paths[1],$settings)
$svg=[Xml.XmlDocument]::new()
$svg.XmlResolver=$null
try{$svg.Load($svgReader)}finally{$svgReader.Dispose()}
if($svg.DocumentElement.LocalName -cne 'svg' -or
   $svg.DocumentElement.NamespaceURI -cne 'http://www.w3.org/2000/svg' -or
   @($svg.SelectNodes('//*[local-name()="polygon"]')).Count -lt 2 -or
   @($svg.SelectNodes('//*[local-name()="script" or local-name()="foreignObject"]')).Count -gt 0){
 throw 'HULAO_VISUAL|FAIL|svg_scene_invalid'
}
$md=[IO.File]::ReadAllText($paths[2],[Text.Encoding]::UTF8)
$rootReadme=[IO.File]::ReadAllText($paths[3],[Text.Encoding]::UTF8)
if(([regex]::Matches($md,'\[Lap [0-9]{3} · ')).Count -ne 255 -or
   ([regex]::Matches($md,'<details>')).Count -ne 16 -or
   -not $md.Contains('](pentapi_hulao_gate.svg)',[StringComparison]::Ordinal) -or
   -not $md.Contains('PentaPi_Hulao_256.xhtml',[StringComparison]::Ordinal) -or
   -not $rootReadme.Contains('docs/PENTAPI_HULAO_GATE_255.md',
                               [StringComparison]::Ordinal) -or
   -not $rootReadme.Contains('docs/pentapi_hulao_gate.svg',
                               [StringComparison]::Ordinal)){
 throw 'HULAO_VISUAL|FAIL|markdown_or_readme_links'
}
for($i=0;$i -lt $paths.Count;$i++){
 if($hashes[$i] -cne (Get-FileHash -LiteralPath $paths[$i] -Algorithm SHA256).Hash){
  throw 'HULAO_VISUAL|FAIL|source_mutation'
 }
}
$hash=(Get-FileHash -LiteralPath $paths[0] -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('HULAO_VISUAL|XHTML_1_0_STRICT_DOCTYPE=PASS|XML_WELLFORMED=PASS'+
 '|PROFILE_ELEMENTS=PASS|HELL_LAPS='+$hell+
 '|FICTIONAL_FINAL=1|NEXT_LINKS=256|MARKDOWN_LAP_LINKS=255'+
 '|SVG=PASS|README_EMBED=PASS|DTD_VALIDATION=NOT_PROVEN'+
 '|SOURCE_UNCHANGED=PASS|XHTML_SHA256='+$hash+'|RELEASE=HOLD')
