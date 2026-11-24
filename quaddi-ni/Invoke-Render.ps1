# SPDX-License-Identifier: 0BSD
# QUADDI-NI: offline, scriptless XHTML5 from qualified TSV witness.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$WitnessFile,
 [Parameter(Mandatory=$true)][string]$Destination,
 [ValidateRange(1e-15,1e300)][double]$YHalfRange=1e12)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$c=[Globalization.CultureInfo]::InvariantCulture
$t=[char]9
function Esc([string]$v){return [Security.SecurityElement]::Escape($v)}
function Num([double]$v){return $v.ToString('G17',$c)}
$raw=[IO.File]::ReadAllLines($WitnessFile,[Text.Encoding]::UTF8)
if($raw.Count -lt 3){throw 'QUADDI|HOLD|empty_witness'}
$meta=$raw[0].Split($t)
if($meta.Count -ne 7 -or $meta[0] -cne 'META'){throw 'QUADDI|HOLD|bad_header'}
$power=[int]$meta[6];$pages=[int]$meta[5]
if($power -lt 0 -or $pages -lt 7){throw 'QUADDI|HOLD|bad_header_values'}
$presets=@(2.0,80.0,5000.0,300000.0,20000000.0,1000000000.0,60000000000.0)
$bound=if($PSBoundParameters.ContainsKey('YHalfRange')){$YHalfRange}elseif($power -le 6){$presets[$power]}else{1e12}
$stateNames=@('SEED','NEGATIVE_I','PRIME_ROOT','POSITIVE_PI','POSITIVE_I','NEGATIVE_PI')
$inscriptions=@('_X_R_2Y_i_I_x','__X_R_2Y_i_I_x','___X_R_2Y_i_I_x','____X_R_2Y_i_I_x','_____X_R_2Y_i_I_x','_______X_R_2Y_i_I_x')
$data=[Collections.Generic.List[object]]::new()
$notes=[Collections.Generic.List[object]]::new()
foreach($record in $raw[1..($raw.Count-2)]){
 $f=$record.Split($t)
 if($f[0] -ceq 'NOTE'){
  if($f.Count -ne 3){throw 'QUADDI|HOLD|bad_NOTE'}
  $notes.Add([pscustomobject]@{Page=[int]$f[1];Name=$f[2]})
 }elseif($f[0] -ceq 'ROW'){
  if($f.Count -ne 13){throw 'QUADDI|HOLD|bad_ROW'}
  $data.Add([pscustomobject]@{
   Page=[int]$f[1];Ordinal=[int]$f[2];Name=$f[3];State=[int]$f[5];
   X=[double]::Parse($f[6],$c);Re=[double]::Parse($f[7],$c);
   Im=[double]::Parse($f[8],$c);Centrix=$f[9];
   Gamma=[double]::Parse($f[10],$c);Closure=[double]::Parse($f[11],$c);
   Degree=[int64]$f[12]
  })
 }else{throw 'QUADDI|HOLD|unknown_record'}
}
if($raw[-1].Split($t)[0] -cne 'END'){throw 'QUADDI|HOLD|missing_END'}
$out=[Text.StringBuilder]::new()
function Add([string]$s){$null=$out.AppendLine($s)}
Add '<?xml version="1.0" encoding="UTF-8"?>'
Add '<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">'
Add '<head><meta charset="UTF-8" /><title>QuadDi NI | qualified witness</title></head><body>'
Add '<h1>QuadDi NI | Oracle Java / native C++20</h1>'
Add ('<p>RULER REGISTER COUNT: #1 | PAGE COUNT: '+$pages+' | SEED POWER: '+$power+' | RELEASE=HOLD</p>')
Add '<p>Unrestricted mathematical domains; finite screen view disclosed per page. One encoded ruler does not collapse real and imaginary data into the same measurement.</p>'
for($page=1;$page -le $pages;$page++){
 $pageRows=@($data|Where-Object{$_.Page -eq $page})
 $misc=@($notes|Where-Object{$_.Page -eq $page})
 $label=if($page -le 6){$stateNames[$page-1]+' | '+$inscriptions[$page-1]}elseif($misc.Count){'MISC '+$misc[0].Name}else{'SOURCE PAGE'}
 Add ('<details><summary>PAGE '+$page+' / '+$pages+' | '+(Esc $label)+' | samples '+$pageRows.Count+'</summary>')
 Add ('<p>RULER COUNT: #1 | X=-2..+2, tick=1 | Real Y=±'+(Num $bound)+', tick='+(Num ($bound/2))+' | Imaginary coordinate separately disclosed as raw text.</p>')
 if($misc.Count){
  Add '<p>COREAO / GammGamma is an experimental ordered complex offset witness, not physical radiation.</p>'
  Add '<table border="1"><tr><th>Source</th><th>State</th><th>Gamma-step</th><th>Closure</th></tr>'
  foreach($r in @($data|Where-Object{$_.Page -le 6 -and $_.X -eq 0})){
   Add ('<tr><td>'+(Esc $r.Name)+'</td><td>'+$r.State+'</td><td>'+(Num $r.Gamma)+'</td><td>'+(Num $r.Closure)+'</td></tr>')
  }
  Add '</table>'
 }
 foreach($group in @($pageRows|Group-Object Ordinal)){
  $items=@($group.Group|Sort-Object X)
  $one=$items[0]
  $index=$one.Ordinal
  $colors=@('#0891b2','#d97706','#9333ea','#15803d','#be123c')
  $color=$colors[($index-1)%$colors.Count]
  $clip='quaddi-clip-'+$page+'-'+$index
  Add ('<details><summary>SOURCE #'+$index+' '+(Esc $one.Name)+' | Show/hide curve</summary>')
  Add ('<p>Original line centrix: ('+(Esc $one.Centrix)+',0) | Imaginary coefficient '+(Num $one.Im)+' | GammGamma '+(Num $one.Gamma)+' | Closure '+(Num $one.Closure)+'</p>')
  Add '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 960 355" role="img" aria-label="Graph including numeric X and Y rulers and Cartesian origin">'
  Add ('<defs><clipPath id="'+$clip+'"><rect x="120" y="40" width="720" height="260"/></clipPath></defs>')
  Add '<rect x="120" y="40" width="720" height="260" fill="none" stroke="currentColor"/>'
  foreach($x in @(-2,-1,0,1,2)){
   $px=120+($x+2)*180
   Add ('<line x1="'+$px+'" x2="'+$px+'" y1="40" y2="300" stroke="currentColor" stroke-opacity=".18"/>')
   Add ('<text x="'+$px+'" y="318" text-anchor="middle" font-size="12" fill="currentColor">'+$x+'</text>')
  }
  foreach($f in @(-1,-0.5,0,0.5,1)){
   $py=170-130*$f
   Add ('<line x1="120" x2="840" y1="'+(Num $py)+'" y2="'+(Num $py)+'" stroke="currentColor" stroke-opacity=".18"/>')
   Add ('<text x="112" y="'+(Num ($py+4))+'" text-anchor="end" font-size="11" fill="currentColor">'+(Num ($f*$bound))+'</text>')
  }
  $samples=[Collections.Generic.List[string]]::new()
  foreach($r in $items){
   if($r.X -lt -2 -or $r.X -gt 2)continue
   $px=120+($r.X+2)*180;$py=170-130*$r.Re/$bound
   if(-not [double]::IsFinite($px) -or -not [double]::IsFinite($py)){throw 'QUADDI|HOLD|nonfinite_render'}
   $samples.Add((Num $px)+','+(Num $py))
  }
  Add ('<g clip-path="url(#'+$clip+')"><polyline fill="none" stroke="'+$color+'" stroke-width="3" points="'+($samples -join ' ')+'"/></g>')
  Add '<circle cx="480" cy="170" r="5" fill="currentColor"/>'
  Add '<text x="490" y="192" font-size="12" fill="currentColor">(0, 0)</text>'
  Add ('<text x="840" y="349" text-anchor="end" font-size="11" fill="currentColor">RULER COUNT: #1 | X tick 1 | Y tick '+(Num ($bound/2))+' | points '+$samples.Count+'</text>')
  Add '</svg>'
  Add '<details><summary>Full numeric witness (including offscreen X)</summary><table border="1"><tr><th>X</th><th>Real Y</th><th>Imag Y</th><th>Gamma</th></tr>'
  foreach($r in $items){
   Add ('<tr><td>'+(Num $r.X)+'</td><td>'+(Num $r.Re)+'</td><td>'+(Num $r.Im)+'</td><td>'+(Num $r.Gamma)+'</td></tr>')
  }
  Add '</table></details></details>'
 }
 Add '</details>'
}
Add ('<p>WITNESS SHA256: '+(Get-FileHash -LiteralPath $WitnessFile -Algorithm SHA256).Hash.ToLowerInvariant()+'</p>')
Add '</body></html>'
[IO.File]::WriteAllText([IO.Path]::GetFullPath($Destination),$out.ToString(),[Text.UTF8Encoding]::new($false))
$settings=[Xml.XmlReaderSettings]::new()
$settings.DtdProcessing=[Xml.DtdProcessing]::Prohibit
$settings.XmlResolver=$null
$reader=[Xml.XmlReader]::Create([IO.Path]::GetFullPath($Destination),$settings)
try{while($reader.Read()){}}
finally{$reader.Dispose()}
Write-Host ('QUADDI|XHTML5=PASS|POWER='+$power+'|PAGES='+$pages+'|RELEASE=HOLD')
