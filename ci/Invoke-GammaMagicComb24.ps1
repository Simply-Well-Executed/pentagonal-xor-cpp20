# SPDX-License-Identifier: 0BSD
# Read-only GammaMAGICCOMB -> MAGICCOMBammaG, 24 explicit files.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [string]$ManifestPath='ci/PR95_GAMMA24_PATHS.txt'
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot).TrimEnd([IO.Path]::DirectorySeparatorChar)
$out=[IO.Path]::GetFullPath($EvidenceDirectory).TrimEnd([IO.Path]::DirectorySeparatorChar)
$utf8=[Text.UTF8Encoding]::new($false,$true)
if(-not [IO.Directory]::Exists($root) -or [IO.Directory]::Exists($out) -or
   $out -ceq $root -or $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'GAMMA24|HOLD|invalid_root_or_existing_or_in_tree_output'
}
$manifest=Join-Path $root $ManifestPath
if(-not [IO.File]::Exists($manifest)){throw 'GAMMA24|HOLD|manifest_missing'}
$names=@([IO.File]::ReadAllLines($manifest,$utf8))
if($names.Count -ne 24){throw ('GAMMA24|HOLD|expected_24_paths|found='+$names.Count)}
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach($name in $names){
 if([string]::IsNullOrWhiteSpace($name) -or $name -cnotmatch '^[A-Za-z0-9_.-]+(/[A-Za-z0-9_.-]+)+$' -or
    $name.Split('/') -contains '..' -or -not $seen.Add($name) -or
    $name.StartsWith('Archive/',[StringComparison]::Ordinal)){
  throw 'GAMMA24|HOLD|ambiguous_duplicate_or_retired_path'
 }
}
function Sha([byte[]]$bytes){
 return [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
}
function Read-Scan([string]$relative){
 $file=[IO.Path]::GetFullPath((Join-Path $root $relative))
 if(-not $file.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase) -or
   -not [IO.File]::Exists($file)){throw ('GAMMA24|HOLD|missing_or_escaping_path|'+$relative)}
 $item=Get-Item -LiteralPath $file -Force
 if(($item.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0){
  throw ('GAMMA24|HOLD|symlink_needs_separate_review|'+$relative)
 }
 $bytes=[IO.File]::ReadAllBytes($file)
 $sha=Sha $bytes
 try{$s=$utf8.GetString($bytes)}catch [Text.DecoderFallbackException]{
  return [pscustomobject]@{Path=$relative;Sha=$sha;Bytes=$bytes.Length;Utf8='INVALID';Nfkc='UNKNOWN';Bidi=0;ZeroWidth=0;Nul=0;Tabs=0;CrLf=0}
 }
 $bidi=[regex]::Matches($s,'[\u202A-\u202E\u2066-\u2069\u200E\u200F]').Count
 $zw=[regex]::Matches($s,'[\u200B-\u200D\uFEFF]').Count
 $nul=[regex]::Matches($s,[string][char]0).Count
 $tabs=[regex]::Matches($s,[string][char]9).Count
 $crlf=[regex]::Matches($s,'\r\n').Count
 $nfkc=if($s -ceq $s.Normalize([Text.NormalizationForm]::FormKC)){'SAME'}else{'DIFFERENT'}
 return [pscustomobject]@{Path=$relative;Sha=$sha;Bytes=$bytes.Length;Utf8='VALID';Nfkc=$nfkc;Bidi=$bidi;ZeroWidth=$zw;Nul=$nul;Tabs=$tabs;CrLf=$crlf}
}
$forward=@{}
foreach($p in $names){$forward.Add($p,(Read-Scan $p))}
$reverse=@{}
for($i=$names.Count-1;$i -ge 0;$i--){$p=$names[$i];$reverse.Add($p,(Read-Scan $p))}
$columns=@('Path','Sha','Bytes','Utf8','Nfkc','Bidi','ZeroWidth','Nul','Tabs','CrLf')
$rows=[Collections.Generic.List[string]]::new()
$rows.Add(('index'+[char]9+'path'+[char]9+'sha256'+[char]9+'bytes'+[char]9+'utf8'+[char]9+'nfkc_preview'+[char]9+'bidi_marks'+[char]9+'zero_width'+[char]9+'nul'+[char]9+'tabs'+[char]9+'crlf'+[char]9+'order_parity'))
$holds=0
for($i=0;$i -lt $names.Count;$i++){
 $p=$names[$i];$a=$forward[$p];$b=$reverse[$p]
 $same=$true
 foreach($column in $columns){if([string]$a.$column -cne [string]$b.$column){$same=$false}}
 $parity=if($same){'MATCH'}else{'DRIFT'}
 if(-not $same -or $a.Utf8 -cne 'VALID' -or $a.Nul -gt 0){$holds++}
 $rows.Add((@(($i+1),$p,$a.Sha,$a.Bytes,$a.Utf8,$a.Nfkc,$a.Bidi,$a.ZeroWidth,$a.Nul,$a.Tabs,$a.CrLf,$parity) -join [char]9))
}
[void][IO.Directory]::CreateDirectory($out)
$path=Join-Path $out 'banana.gamma24.tsv'
[IO.File]::WriteAllLines($path,$rows,$utf8)
$receiptSha=Sha $utf8.GetBytes(($rows -join [char]10)+[char]10)
Write-Host ('GAMMA24|HEAD_TAIL|FILES=24|HOLD='+$holds+'|RECEIPT_SHA256='+$receiptSha+'|SOURCE_NOT_MUTATED|RELEASE=HOLD')
Write-Host ('GAMMA24|BANANA|'+$path)
if($holds -ne 0){throw 'GAMMA24|HOLD|unverified_file_or_direction_drift'}
