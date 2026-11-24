# SPDX-License-Identifier: 0BSD
# NiteRex native C++20 / XHTML 1.0 Strict projection contract.
# Offline XML inspection only: no external DTD fetch or trust promotion.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$InstalledBinDirectory)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not [Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
 [Runtime.InteropServices.OSPlatform]::Windows)){
 throw 'NITEREX|HOLD|Win32_renderer_test_requires_Windows'
}
$bin=Join-Path ([IO.Path]::GetFullPath($InstalledBinDirectory)) 'niterex_web9x_xhtml.exe'
if(-not [IO.File]::Exists($bin)){throw 'NITEREX|HOLD|native_renderer_missing'}
$pinfo=[Diagnostics.ProcessStartInfo]::new($bin)
$pinfo.UseShellExecute=$false
$pinfo.RedirectStandardOutput=$true
$pinfo.RedirectStandardError=$true
$pinfo.CreateNoWindow=$true
$pinfo.StandardOutputEncoding=[Text.UTF8Encoding]::new($false,$true)
$pinfo.StandardErrorEncoding=[Text.UTF8Encoding]::new($false,$true)
$proc=[Diagnostics.Process]::new()
$proc.StartInfo=$pinfo
if(-not $proc.Start()){throw 'NITEREX|HOLD|failed_to_launch_native_renderer'}
try {
 $stdout=$proc.StandardOutput.ReadToEnd()
 $stderr=$proc.StandardError.ReadToEnd()
 if(-not $proc.WaitForExit(30000)){
  $proc.Kill($true)
  throw 'NITEREX|HOLD|renderer_timeout'
 }
 if($proc.ExitCode -ne 0 -or $stdout.Length -lt 1000 -or
    $stdout.Length -gt 65536 -or -not [string]::IsNullOrEmpty($stderr)){
  throw 'NITEREX|HOLD|process_or_output_contract'
 }
}finally {$proc.Dispose()}

if(-not $stdout.StartsWith('<?xml version="1.0" encoding="UTF-8"?>'+[char]10,
                             [StringComparison]::Ordinal) -or
 -not $stdout.Contains('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"',
                       [StringComparison]::Ordinal) -or
 $stdout.Contains('<script',[StringComparison]::OrdinalIgnoreCase)){
 throw 'NITEREX|HOLD|XML_declaration_doctype_or_script_contract'
}
$settings=[Xml.XmlReaderSettings]::new()
$settings.DtdProcessing=[Xml.DtdProcessing]::Ignore
$settings.XmlResolver=$null
$settings.MaxCharactersInDocument=65536
$settings.MaxCharactersFromEntities=0
$doc=[Xml.XmlDocument]::new()
$doc.XmlResolver=$null
$reader=[Xml.XmlReader]::Create([IO.StringReader]::new($stdout),$settings)
try{$doc.Load($reader)}finally{$reader.Dispose()}
if($doc.DocumentElement.LocalName -cne 'html' -or
 $doc.DocumentElement.NamespaceURI -cne 'http://www.w3.org/1999/xhtml'){
 throw 'NITEREX|HOLD|XML_root_namespace'
}
$ns=[Xml.XmlNamespaceManager]::new($doc.NameTable)
$ns.AddNamespace('x','http://www.w3.org/1999/xhtml')
$tables=@($doc.SelectNodes('//x:table',$ns))
if($tables.Count -ne 2){throw 'NITEREX|HOLD|table_count'}
$questRows=@($tables[0].SelectNodes('./x:tbody/x:tr',$ns))
$slotRows=@($tables[1].SelectNodes('./x:tbody/x:tr',$ns))
if($questRows.Count -ne 9 -or $slotRows.Count -ne 9){
 throw 'NITEREX|HOLD|nine_quest_and_nine_slot_counts'
}
$names=@('AND','OR','XOR','NOT','ADD','SUB','ROR','NOP','ZERO')
$expected=@(8,15,7,498,23,3,262,13,0)
$slots=@('CSS','HTML / XHTML','JavaScript','Java','CSV','SVN',
         'Root CA','Intermediate CA','Subject certificate')
for($i=0;$i -lt 9;$i++){
 $quest=@($questRows[$i].ChildNodes)
 $slot=@($slotRows[$i].ChildNodes)
 if($quest.Count -ne 3 -or $slot.Count -ne 3 -or
    $quest[0].InnerText -cne [string]($i+1) -or
    $quest[1].InnerText -cne $names[$i] -or
    $quest[2].InnerText -cne [string]$expected[$i] -or
    $slot[0].InnerText -cne [string]($i+1) -or
    $slot[1].InnerText -cne $slots[$i]){
  throw ('NITEREX|HOLD|row_drift|'+$i)
 }
}
if(-not $doc.DocumentElement.InnerText.Contains('Reference checksum: 829',
 [StringComparison]::Ordinal) -or
 -not $doc.DocumentElement.InnerText.Contains('no signing performed',
 [StringComparison]::Ordinal)){
 throw 'NITEREX|HOLD|checksum_or_signature_boundary'
}
$sha=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
 [Text.UTF8Encoding]::new($false).GetBytes($stdout))).ToLowerInvariant()
Write-Host ('NITEREX|WEB9X_XHTML_PASS|NATIVE_WINDOWS_CXX20|NINE_QUESTS'+
 '|NINE_DISTINCT_SLOTS|CHECKSUM=829|XML_WELL_FORMED|DTD_NOT_FETCHED'+
 '|UNSIGNED_EXPERIMENT|SHA256='+$sha)
