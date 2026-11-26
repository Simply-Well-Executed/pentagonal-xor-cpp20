# SPDX-License-Identifier: 0BSD
# SLIDINGTOOLONGA read-only XHTML5 exact-origin + disclosed approximate
# inverse-polynomial window. No CSS/JS, no automatic fitting or normalization.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$WitnessFile,
      [Parameter(Mandatory=$true)][string]$Destination,
      [string]$MiscFile='')
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$c=[Globalization.CultureInfo]::InvariantCulture
$sep=[char]9
function Escape([string]$text){return [Security.SecurityElement]::Escape($text)}
function Num([double]$x){return $x.ToString('G17',$c)}
$all=[IO.File]::ReadAllLines($WitnessFile,[Text.Encoding]::UTF8)
if($all.Count -lt 3){throw 'SLIDINGTOOLONGA|HOLD|empty_witness'}
$head=$all[0].Split($sep)
if($head.Count -ne 5 -or $head[0] -cne 'META' -or $head[1] -cne 'SLIDINGTOOLONGA-001' -or $head[3] -cne 'X=0'){
 throw 'SLIDINGTOOLONGA|HOLD|bad_head'
}
$powerText=$head[2].Substring('POWER='.Length)
if($powerText -cnotmatch '^[0-9]+$'){throw 'SLIDINGTOOLONGA|HOLD|bad_power'}
$power=[Numerics.BigInteger]::Parse($powerText,$c)
$records=@($all[1..($all.Count-2)])
$ending=$all[-1].Split($sep)
if($ending.Count -ne 3 -or $ending[0] -cne 'END' -or $ending[2] -cne 'RELEASE=HOLD' -or
   $records.Count -ne [int]$ending[1]){throw 'SLIDINGTOOLONGA|HOLD|bad_count_or_tail'}
# Device visualization is finite. Approximate inverse uses positive double p
# only when representable; otherwise the exact expression is retained without a plot.
$pDouble=0.0
if($power -le [Numerics.BigInteger]::Parse('1000000000000000')){
 $pDouble=[double]::Parse($power.ToString($c),$c)
}
$rows=[Collections.Generic.List[object]]::new()
foreach($line in $records){
 $f=$line.Split($sep)
 if($f.Count -ne 9 -or $f[0] -cne 'SOURCE'){throw 'SLIDINGTOOLONGA|HOLD|invalid_source'}
 if($f[8] -cnotmatch '^[0-9]+$' -or $f[7] -cnotmatch '^[0-9]+$'){throw 'SLIDINGTOOLONGA|HOLD|invalid_mantissa'}
 if($f[8].Length -ne [int]$f[7]){throw 'SLIDINGTOOLONGA|HOLD|length_mismatch'}
 $rows.Add([pscustomobject]@{
  Ordinal=[int]$f[1]; Name=$f[2]; Slope=$f[3]; Intercept=$f[4];
  Sign=[int]$f[5]; Scale=$f[6]; Digits=$f[7]; Mantissa=$f[8]
 })
}
if(-not $MiscFile){$MiscFile=Join-Path $PSScriptRoot 'default-misc.tsv'}
if(-not [IO.File]::Exists($MiscFile)){throw 'SLIDINGTOOLONGA|HOLD|misc_page_file_missing'}
$miscPages=@([IO.File]::ReadAllLines($MiscFile,[Text.Encoding]::ASCII)|Where-Object{$_ -and -not $_.StartsWith('#')})
foreach($m in $miscPages){if($m -cnotmatch '^[A-Za-z0-9_.-]+$'){throw 'SLIDINGTOOLONGA|HOLD|unsafe_misc_page_name'}}
$b=[Text.StringBuilder]::new()
function Add([string]$s){$null=$b.AppendLine($s)}
Add '<?xml version="1.0" encoding="UTF-8"?>'
Add '<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">'
Add '<head><meta charset="UTF-8" /><title>SLIDINGTOOLONGA | exact origin witness</title></head><body>'
Add '<h1>SLIDINGTOOLONGA | QuadDi-NI exact origin</h1>'
Add ('<p>SEED POWER: '+(Escape $powerText)+' | EXACT SEED: TRUE | ORIGIN: (0,0) | RULER REGISTER COUNT: #1 | RELEASE=HOLD</p>')
Add ('<p>PAGE COUNT: '+(6+$miscPages.Count+$rows.Count)+' | SOURCE COUNT: '+$rows.Count+' | MISC PAGES: '+$miscPages.Count+' | six ordered states remain separate</p>')
Add '<p>Source seed is exactly (m*x+b)^p at x=0, using integer mantissa and decimal scale. No floating-point overflow. For large p, nearest-prime root is UNVERIFIED, not declared valid.</p>'
$names=@('SEED','NEGATIVE_IMAGINARY','PRIME_ROOT','POSITIVE_PI','POSITIVE_IMAGINARY','NEGATIVE_PI')
$expr=@('S','S - i','S - i + R','S - i + R + pi','S + R + pi','S + R')
$inscriptions=@('_X_R_2Y_i_I_x','__X_R_2Y_i_I_x','___X_R_2Y_i_I_x','____X_R_2Y_i_I_x','_____X_R_2Y_i_I_x','_______X_R_2Y_i_I_x')
for($page=1;$page -le 6;$page++){
 Add ('<details><summary>PAGE '+$page+' | '+$names[$page-1]+' | '+(Escape $inscriptions[$page-1])+'</summary>')
 Add ('<p>Symbolic ordered state: '+(Escape $expr[$page-1])+'. R is the nearest-prime root of the exact origin seed and has not been verified for large values.</p>')
 Add ('<p>State arithmetic status: '+$(if($page -le 2){'EXACT_SYMBOLIC'}else{'PRIME_ROOT_UNRESOLVED'})+'. No implied gamma radiation or physical radioactivity.</p>')
 Add '</details>'
}
for($j=0;$j -lt $miscPages.Count;$j++){
 $miscPage=7+$j
 Add ('<details><summary>PAGE '+$miscPage+' | MISC '+(Escape $miscPages[$j])+'</summary>')
 Add '<p>Miscellaneous page retained without mutating any source line, state or ruler. GammGamma prime-root verification remains HOLD if not proved.</p>'
 Add '</details>'
}
foreach($r in $rows){
 $page=6+$miscPages.Count+$r.Ordinal
 Add ('<details><summary>PAGE '+$page+' | SOURCE #'+$r.Ordinal+' '+(Escape $r.Name)+' | EXACT SEED TRUE</summary>')
 Add ('<p>SOURCE IDENTITY #'+$r.Ordinal+' | slope='+(Escape $r.Slope)+' | intercept='+(Escape $r.Intercept)+'</p>')
 Add ('<p>Original centrix: x = -('+(Escape $r.Intercept)+')/('+(Escape $r.Slope)+') when slope is nonzero. Cartesian origin: (0,0).</p>')
 Add ('<p>Exact origin witness: sign='+$r.Sign+'; mantissa digits='+$r.Digits+'; scale (decimal places)='+(Escape $r.Scale)+'.</p>')
 Add '<details><summary>Unfold the complete exact origin mantissa</summary>'
 Add ('<pre>'+(Escape $r.Mantissa)+'</pre></details>')
 Add '<p>Fixed, equal-unit display: X from -2 to +2; Y from -2 to +2. One numerical unit = 80 SVG units on both axes. No fit, normalization or moving ruler.</p>'
 Add '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 800 560" role="img" aria-label="Finite X/Y polynomial window with both rulers, the (0,0) origin and exact source provenance">'
 $clip='exact-window-'+$r.Ordinal
 Add ('<defs><clipPath id="'+$clip+'"><rect x="190" y="185" width="320" height="320"/></clipPath></defs>')
 Add '<rect x="190" y="185" width="320" height="320" fill="none" stroke="currentColor"/>'
 foreach($i in @(-2,-1,0,1,2)){
  $xpx=350+$i*80;$ypy=345-$i*80
  Add ('<line x1="'+$xpx+'" y1="185" x2="'+$xpx+'" y2="505" stroke="currentColor" stroke-opacity=".2"/>')
  Add ('<line x1="190" y1="'+$ypy+'" x2="510" y2="'+$ypy+'" stroke="currentColor" stroke-opacity=".2"/>')
  Add ('<text x="'+$xpx+'" y="526" font-size="13" text-anchor="middle" fill="currentColor">'+$i+'</text>')
  Add ('<text x="180" y="'+($ypy+5)+'" font-size="13" text-anchor="end" fill="currentColor">'+$i+'</text>')
 }
 Add '<circle cx="350" cy="345" r="5" fill="currentColor"/>'
 Add '<text x="360" y="364" font-size="13" fill="currentColor">(0,0)</text>'
 $slope=0.0;$intercept=0.0;$canPlot=$pDouble -ge 0
 try {
  $slope=[double]::Parse($r.Slope,$c);$intercept=[double]::Parse($r.Intercept,$c)
  if(-not [double]::IsFinite($slope) -or -not [double]::IsFinite($intercept)){ $canPlot=$false }
 }catch{$canPlot=$false}
 if($canPlot){
  $paths=[Collections.Generic.List[string]]::new()
  if($power.IsZero){
   $paths.Add('M190,265 L510,265')
  }elseif($slope -eq 0){
   $v=[Math]::Pow($intercept,$pDouble)
   if([double]::IsFinite($v) -and $v -ge -2 -and $v -le 2){
    $py=345-80*$v;$paths.Add(('M190,'+(Num $py)+' L510,'+(Num $py)))
   }
  }else{
   $branches=if($power.IsEven){@(-1.0,1.0)}else{@(1.0)}
   foreach($branch in $branches){
    $points=[Collections.Generic.List[string]]::new()
    for($i=0;$i -le 200;$i++){
     $y=if($power.IsEven){$i/100.0}else{($i-100)/50.0}
     if($power.IsEven){
      $t=$branch*[Math]::Exp([Math]::Log([Math]::Max($y,1e-300))/$pDouble)
      if($y -eq 0){$t=0}
     }else{
      $t=if($y -eq 0){0}else{[Math]::Sign($y)*[Math]::Exp([Math]::Log([Math]::Abs($y))/$pDouble)}
     }
     $x=($t-$intercept)/$slope
     $px=350+80*$x;$py=345-80*$y
     if(-not [double]::IsFinite($px) -or -not [double]::IsFinite($py)){throw 'SLIDINGTOOLONGA|HOLD|invalid_display_coordinate'}
     $points.Add((Num $px)+','+(Num $py))
    }
    $paths.Add(('M'+($points -join ' L')))
   }
  }
  Add ('<g clip-path="url(#'+$clip+')">')
  foreach($d in $paths){
   Add ('<path d="'+$d+'" fill="none" stroke="#9333ea" stroke-width="3" pathLength="1" stroke-dasharray="1" stroke-dashoffset="0"><animate attributeName="stroke-dashoffset" from="1" to="0" dur="1.2s" fill="freeze"/></path>')
  }
  Add '</g>'
  Add '<text x="540" y="215" font-size="12" fill="currentColor">Displayed curve: inverse-power</text>'
  Add '<text x="540" y="232" font-size="12" fill="currentColor">double-precision reconstruction</text>'
  Add '<text x="540" y="250" font-size="12" fill="currentColor">Exact seed: separate witness</text>'
 }else{
  Add '<text x="540" y="215" font-size="12" fill="currentColor">Plot unavailable in finite renderer</text>'
  Add '<text x="540" y="235" font-size="12" fill="currentColor">Exact origin value still retained</text>'
 }
 Add '<text x="190" y="550" font-size="12" fill="currentColor">RULER COUNT: #1 | X/Y ticks 1 | no automatic fit | clipped outside window</text>'
 Add '</svg>'
 Add '<p>Visible curve is an approximate inverse-power reconstruction within the disclosed finite window. It is NOT an exact arbitrary-precision polynomial sample. The exact decimal seed at x=0 is authoritative.</p>'
 Add '</details>'
}
Add ('<p>Witness SHA-256: '+(Get-FileHash -LiteralPath $WitnessFile -Algorithm SHA256).Hash.ToLowerInvariant()+'</p>')
Add '</body></html>'
[IO.File]::WriteAllText([IO.Path]::GetFullPath($Destination),$b.ToString(),[Text.UTF8Encoding]::new($false))
$settings=[Xml.XmlReaderSettings]::new()
$settings.DtdProcessing=[Xml.DtdProcessing]::Prohibit
$settings.XmlResolver=$null
$reader=[Xml.XmlReader]::Create([IO.Path]::GetFullPath($Destination),$settings)
try{while($reader.Read()){}}
finally{$reader.Dispose()}
Write-Host ('SLIDINGTOOLONGA|XHTML=PASS|PAGES='+(6+$miscPages.Count+$rows.Count)+'|POWER='+$powerText)
