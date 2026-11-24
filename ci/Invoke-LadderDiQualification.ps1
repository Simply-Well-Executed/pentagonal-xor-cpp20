# SPDX-License-Identifier: 0BSD
# LADDERDI-001: read exact local sources and run each available interpreter/compiler.
# No installs, remote includes, dependency downloads, RPC, or auto-merges.
# Unavailable/policy-invalid lanes are HOLD, never fabricated PASS.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot,
      [Parameter(Mandatory=$true)][string]$EvidenceDirectory)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$utf8=[Text.UTF8Encoding]::new($false,$true)
if(-not [IO.Directory]::Exists($root) -or [IO.Directory]::Exists($out) -or
   $out -ceq $root -or $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'LADDERDI|HOLD|source_missing_or_output_not_fresh_external'
}
[void][IO.Directory]::CreateDirectory($out)
$items=[Collections.Generic.List[string]]::new()
$items.Add('lane'+[char]9+'status'+[char]9+'toolchain_version'+[char]9+
 'source_sha256'+[char]9+'binary_sha256_or_missing'+[char]9+'note')
$failure=0
function Record([string]$lane,[string]$status,[string]$version,[string]$source,[string]$binary,[string]$note){
 $script:items.Add(@($lane,$status,$version,$source,$binary,$note)-join ([char]9))
 Write-Host ('LADDERDI|'+$lane+'|'+$status+'|VERSION='+$version+'|'+$note+'|RELEASE=HOLD')
 if($status -cne 'PASS'){$script:failure++}
}
function Hash([string]$path){return (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()}
function Run([string]$lane,[string]$tool,[string[]]$arguments){
 $stdout=Join-Path $out ($lane+'.stdout.txt')
 $stderr=Join-Path $out ($lane+'.stderr.txt')
 # Arguments are passed as an array directly; no shell or expression evaluation.
 $output=@(& $tool @arguments 2>&1)
 $status=$LASTEXITCODE
 [IO.File]::WriteAllText($stdout,([string]::Join([string][char]10,$output)+[char]10),$utf8)
 [IO.File]::WriteAllText($stderr,('exit='+$status+[char]10),$utf8)
 return [pscustomobject]@{Exit=$status;Text=([string]::Join([string][char]10,$output))}
}
function Find([string]$environment,[string]$executable){
 $hint=[Environment]::GetEnvironmentVariable($environment)
 if(-not [string]::IsNullOrWhiteSpace($hint)){
  $full=[IO.Path]::GetFullPath($hint)
  if([IO.File]::Exists($full)){return $full}
  return $null
 }
 $candidates=@(Get-Command $executable -CommandType Application -ErrorAction SilentlyContinue)
 if($candidates.Count -eq 0){return $null}
 # Pin the single first resolved executable, never concatenate PATH matches.
 return [string]$candidates[0].Source
}
$definitions=[ordered]@{
 HeptaDi='heptadi/HeptaDiLua3L.lua'
 EnneaDi='enneadi/EnneaDi.rs'
 DecaDi='decadi/DecaDi.go'
 UnDecaDi='undecadi/UnDecaDi.asm'
 DoDecaDi='dodecadi/DoDecaDi.pl'
 TriDecaDi='tridecadi/TriDecaDi.ml'
 NonaDi='nonadi/Invoke-NonaDiSelfExclude.ps1'
 OctaDi='octadi/G.py'
}
foreach($key in $definitions.Keys){
 $p=Join-Path $root $definitions[$key]
 if(-not [IO.File]::Exists($p)){throw ('LADDERDI|HOLD|missing_canonical_source|'+$key)}
}
# This is not a claim of formal Lua3 lexical verification, nor a runtime PASS.
$lua=Find 'HEPTADI_LUA_EXE' 'lua.exe'
$source=Join-Path $root $definitions.HeptaDi
if($null -eq $lua){Record 'HeptaDi' 'HOLD' 'LUA_3.2.2_UNAVAILABLE' (Hash $source) 'NONE' 'not_installed'}
else{
 $version=Run 'hepta-version' $lua @('-v')
 if($version.Exit -ne 0 -or $version.Text -notmatch 'Lua 3[.]2[.]2'){
  Record 'HeptaDi' 'HOLD' 'WRONG_LUA_VERSION' (Hash $source) (Hash $lua) 'must_be_3.2.2'
 }else{
  $result=Run 'hepta-test' $lua @($source)
  Record 'HeptaDi' $(if($result.Exit -eq 0){'PASS'}else{'HOLD'}) '3.2.2' (Hash $source) (Hash $lua) ('exit='+$result.Exit)
 }
}
$non=Join-Path $root $definitions.NonaDi
$nonOut=Join-Path $out 'nona-evidence'
$nonRun=Run 'nona-self-exclude' (Get-Command pwsh).Source @('-NoLogo','-NoProfile','-NonInteractive','-File',$non,'-RepositoryRoot',$root,'-EvidenceDirectory',$nonOut)
Record 'NonaDi' $(if($nonRun.Exit -eq 0){'PASS'}else{'HOLD'}) 'SELF_EXCLUDE' (Hash $non) 'NOT_EXECUTED' ('active_node_candidate_exit='+$nonRun.Exit)
$rust=Find 'ENNEADI_RUSTC_EXE' 'rustc.exe'
$source=Join-Path $root $definitions.EnneaDi
if($null -eq $rust){Record 'EnneaDi' 'HOLD' 'RUSTC_UNAVAILABLE' (Hash $source) 'NONE' 'native_only'}
else{
 $ver=Run 'ennea-version' $rust @('--version')
 if($ver.Exit -ne 0 -or $ver.Text -notmatch '^rustc 1[.]100[.]0\b'){
  Record 'EnneaDi' 'HOLD' $ver.Text.Trim() (Hash $source) (Hash $rust) 'requires_1.100.0'
 }else{
  $exe=Join-Path $out 'ennea-native.exe'
  $build=Run 'ennea-build' $rust @('-C','opt-level=2','-C','debuginfo=0',$source,'-o',$exe)
  if($build.Exit -eq 0 -and [IO.File]::Exists($exe)){
   $test=Run 'ennea-test' $exe @()
   Record 'EnneaDi' $(if($test.Exit -eq 0 -and $test.Text.Contains('ENNEADI|PASS')){'PASS'}else{'HOLD'}) '1.100.0' (Hash $source) (Hash $exe) ('native_exit='+$test.Exit)
  }else{Record 'EnneaDi' 'HOLD' '1.100.0' (Hash $source) (Hash $rust) 'native_compilation_failed'}
 }
}
$go=Find 'DECADI_GO_EXE' 'go.exe'
$source=Join-Path $root $definitions.DecaDi
$official='178ab98950ac69ab25069307dc6ade79f3816e414cce95b4da166a115cde1e7e'
$archive=[Environment]::GetEnvironmentVariable('DECADI_GO_OFFICIAL_ARCHIVE')
if($null -eq $go -or [string]::IsNullOrWhiteSpace($archive) -or -not [IO.File]::Exists($archive)){
 Record 'DecaDi' 'HOLD' 'GO_1.27.4' (Hash $source) 'NONE' 'requires_offline_official_google_archive_with_SHA256'
}elseif((Hash $archive) -cne $official){
 Record 'DecaDi' 'HOLD' 'GO_1.27.4' (Hash $source) (Hash $archive) 'not_google_official_sha256'
}else{
 $ver=Run 'deca-version' $go @('version')
 if($ver.Exit -ne 0 -or $ver.Text -notmatch '^go version go1[.]27[.]4 windows/amd64'){
  Record 'DecaDi' 'HOLD' $ver.Text.Trim() (Hash $source) (Hash $go) 'toolchain_mismatch'
 }else{
  $prior=@{}
  foreach($key in @('GOTOOLCHAIN','GOPROXY','GOSUMDB','GO111MODULE','CGO_ENABLED','GOCACHE')){
   $prior[$key]=[Environment]::GetEnvironmentVariable($key)
  }
  try{
   $env:GOTOOLCHAIN='local';$env:GOPROXY='off';$env:GOSUMDB='off'
   $env:GO111MODULE='off';$env:CGO_ENABLED='0'
   $env:GOCACHE=Join-Path $out 'go-cache'
   $exe=Join-Path $out 'deca-native.exe'
   $build=Run 'deca-build' $go @('build','-o',$exe,$source)
   if($build.Exit -eq 0 -and [IO.File]::Exists($exe)){
    $test=Run 'deca-test' $exe @()
    Record 'DecaDi' $(if($test.Exit -eq 0){'PASS'}else{'HOLD'}) '1.27.4' (Hash $source) (Hash $exe) ('native_exit='+$test.Exit+'|archive_verified_google')
   }else{Record 'DecaDi' 'HOLD' '1.27.4' (Hash $source) (Hash $go) 'compile_failed'}
  }finally{foreach($key in $prior.Keys){[Environment]::SetEnvironmentVariable($key,$prior[$key])}}
 }
}
$asm=Join-Path $root $definitions.UnDecaDi
$cpp=Join-Path $root 'undecadi/UnDecaDiTest.cpp'
$masm=Find 'UNDECADI_ML64_EXE' 'ml64.exe'
$cl=Find 'UNDECADI_CL_EXE' 'cl.exe'
if($null -eq $masm -or $null -eq $cl -or -not [IO.File]::Exists($cpp)){
 Record 'UnDecaDi' 'HOLD' 'MSVC_MASM_NOT_PROVISIONED' (Hash $asm) 'NONE' 'must_compile_MASM_x64_and_ISO_CXX20_O2'
}else{
 $obj=Join-Path $out 'undeca.obj'
 $exe=Join-Path $out 'undeca.exe'
 $a=Run 'undeca-asm' $masm @('/nologo','/c',('/Fo'+$obj),$asm)
 if($a.Exit -eq 0 -and [IO.File]::Exists($obj)){
  $b=Run 'undeca-link' $cl @('/nologo','/O2','/std:c++20',$cpp,$obj,('/Fe:'+ $exe))
  if($b.Exit -eq 0 -and [IO.File]::Exists($exe)){
   $q=Run 'undeca-test' $exe @()
   Record 'UnDecaDi' $(if($q.Exit -eq 0){'PASS'}else{'HOLD'}) 'MSVC_MASM_LOCALLY_PROVISIONED' (Hash $asm) (Hash $exe) ('native_exit='+$q.Exit)
  }else{Record 'UnDecaDi' 'HOLD' 'MSVC_MASM' (Hash $asm) 'NONE' 'CXX20_O2_link_failed'}
 }else{Record 'UnDecaDi' 'HOLD' 'MSVC_MASM' (Hash $asm) 'NONE' 'ml64_compile_failed'}
}
$perl=Find 'DODECADI_PERL_EXE' 'perl.exe'
$source=Join-Path $root $definitions.DoDecaDi
if($null -eq $perl){Record 'DoDecaDi' 'HOLD' 'PERL_5.44.0_UNAVAILABLE' (Hash $source) 'NONE' 'offline_runtime_required'}
else{
 $version=Run 'dodeca-version' $perl @('-v')
 if($version.Exit -ne 0 -or $version.Text -notmatch 'v5[.]44[.]0\b'){
  Record 'DoDecaDi' 'HOLD' 'PERL_VERSION_MISMATCH' (Hash $source) (Hash $perl) 'requires_5.44.0'
 }else{
  $result=Run 'dodeca-test' $perl @('-T',$source)
  Record 'DoDecaDi' $(if($result.Exit -eq 0 -and $result.Text.Contains('DODECADI|PASS')){'PASS'}else{'HOLD'}) '5.44.0' (Hash $source) (Hash $perl) ('exit='+$result.Exit)
 }
}
$ocaml=Find 'TRIDECADI_OCAMLOPT_EXE' 'ocamlopt.exe'
$source=Join-Path $root $definitions.TriDecaDi
if($null -eq $ocaml){Record 'TriDecaDi' 'HOLD' 'OCAML_5.5.2_UNAVAILABLE' (Hash $source) 'NONE' 'native_5.5.2_required'}
else{
 $ver=Run 'trideca-version' $ocaml @('-version')
 if($ver.Exit -ne 0 -or $ver.Text.Trim() -cne '5.5.2'){
  Record 'TriDecaDi' 'HOLD' $ver.Text.Trim() (Hash $source) (Hash $ocaml) 'requires_5.5.2'
 }else{
  $exe=Join-Path $out 'trideca-native.exe'
  $build=Run 'trideca-build' $ocaml @('-O2','-o',$exe,$source)
  if($build.Exit -eq 0 -and [IO.File]::Exists($exe)){
   $test=Run 'trideca-test' $exe @()
   Record 'TriDecaDi' $(if($test.Exit -eq 0 -and $test.Text.Contains('TRIDECADI|PASS')){'PASS'}else{'HOLD'}) '5.5.2' (Hash $source) (Hash $exe) ('native_exit='+$test.Exit)
  }else{Record 'TriDecaDi' 'HOLD' '5.5.2' (Hash $source) (Hash $ocaml) 'native_build_failed'}
 }
}
# OCTADI remains separately tested: two runtimes must be locally available.
$oct=Join-Path $root 'octadi/Invoke-OctaDiGTable.ps1'
$octResult=Run 'octadi-subgate' (Get-Command pwsh).Source @('-NoLogo','-NoProfile','-NonInteractive','-File',$oct,
 '-RepositoryRoot',$root,'-EvidenceDirectory',(Join-Path $out 'octadi-evidence'))
Record 'OctaDi' $(if($octResult.Exit -eq 0){'PASS'}else{'HOLD'}) 'PY2_2.7.18_PLUS_PY3' (Hash $oct) 'NOT_BUNDLED' ('exit='+$octResult.Exit)
$receipt=Join-Path $out 'ladderdi-qualification.tsv'
[IO.File]::WriteAllLines($receipt,$items,$utf8)
Write-Host ('LADDERDI|SUMMARY|PASS='+($items.Count-1-$failure)+'|HOLD='+$failure+
 '|RECEIPT_SHA256='+(Hash $receipt)+'|NO_AUTO_MERGE|RELEASE=HOLD')
if($failure -gt 0){throw 'LADDERDI|HOLD|not_all_lanes_qualified'}
