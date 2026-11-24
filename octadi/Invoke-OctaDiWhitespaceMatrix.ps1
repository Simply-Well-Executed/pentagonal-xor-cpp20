# SPDX-License-Identifier: 0BSD
# OCTADI-WHITESPACE-001: last-resort, separate fixtures; NEVER edit original.
# Python tabs expand to multiples of EIGHT columns, not five fixed spaces.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot,
      [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
      [switch]$DiagnoseIndentation)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $DiagnoseIndentation){throw 'OCTADI_WS|NOT_RUN|only_after_indent_diagnostic'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$src=Join-Path $root 'octadi/G.py'
$fixture=Join-Path $root 'pentadi/PentaDiC11BranchRecords.reference.tsv'
$utf8=[Text.UTF8Encoding]::new($false,$true)
if(-not [IO.File]::Exists($src) -or -not [IO.File]::Exists($fixture) -or
 [IO.Directory]::Exists($out) -or $out -ceq $root -or
 $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'OCTADI_WS|HOLD|bad_source_or_nonfresh_output'
}
[void][IO.Directory]::CreateDirectory($out)
$original=(Get-FileHash $src -Algorithm SHA256).Hash.ToLowerInvariant()
$fixtureSha=(Get-FileHash $fixture -Algorithm SHA256).Hash.ToLowerInvariant()
$lines=@([IO.File]::ReadAllLines($src,$utf8))
$variants=[ordered]@{
 'canonical-4spaces'='4s'
 'candidate-5spaces'='5s'
 'candidate-8spaces'='8s'
 'candidate-tabs'='tabs'
}
$ledger=[Collections.Generic.List[string]]::new()
$ledger.Add('variant'+[char]9+'code_sha256'+[char]9+'indent'+[char]9+
 'python2'+[char]9+'python3'+[char]9+'stdout_identical'+[char]9+'status')
$versioned=[ordered]@{
 Python2=$env:OCTADI_PY2_EXE
 Python3=$env:OCTADI_PY3_EXE
}
foreach($entry in $variants.GetEnumerator()){
 $dir=Join-Path $out $entry.Key
 $p=Join-Path $dir 'pentadi'
 $o=Join-Path $dir 'octadi'
 [void][IO.Directory]::CreateDirectory($p)
 [void][IO.Directory]::CreateDirectory($o)
 [IO.File]::Copy($fixture,(Join-Path $p 'PentaDiC11BranchRecords.reference.tsv'))
 $adjusted=[Collections.Generic.List[string]]::new()
 foreach($line in $lines){
  if($line -match '^( +)'){
   $spaces=$Matches[1].Length
   if($spaces%4 -ne 0){throw 'OCTADI_WS|HOLD|noncanonical_original_indent'}
   $level=[int]($spaces/4)
   $prefix=switch($entry.Value){
    '4s' {[string]::new(' ',4*$level)}
    '5s' {[string]::new(' ',5*$level)}
    '8s' {[string]::new(' ',8*$level)}
    'tabs' {[string]::new([char]9,$level)}
   }
   $adjusted.Add($prefix+$line.Substring($spaces))
  }else{$adjusted.Add($line)}
 }
 $candidate=Join-Path $o 'G.py'
 [IO.File]::WriteAllText($candidate,([string]::Join([string][char]10,$adjusted)+[char]10),$utf8)
 $results=@{}
 foreach($pair in $versioned.GetEnumerator()){
  if([string]::IsNullOrWhiteSpace($pair.Value) -or
     -not [IO.File]::Exists($pair.Value)){
   $results[$pair.Key]=[pscustomobject]@{Status='HOLD_RUNTIME_MISSING';Output=''}
   continue
  }
  $exe=[IO.Path]::GetFullPath($pair.Value)
  $stdout=Join-Path $dir ($pair.Key+'.stdout.txt')
  $stderr=Join-Path $dir ($pair.Key+'.stderr.txt')
  $process=Start-Process -FilePath $exe -ArgumentList @('-E','-S','"'+$candidate+'"') -WorkingDirectory $dir -PassThru -NoNewWindow -RedirectStandardOutput $stdout -RedirectStandardError $stderr
  if(-not $process.WaitForExit(30000)){
   $process.Kill($true);$process.WaitForExit()
   $results[$pair.Key]=[pscustomobject]@{Status='HOLD_TIMEOUT';Output=''}
  }else{
   $actual=[IO.File]::ReadAllText($stdout,$utf8)
   $status=if($process.ExitCode -eq 0 -and $actual.Contains('G|PASS|LAYOUTS=8|BINARY=2|TERNARY=6')){'PASS'}else{'HOLD'}
   $results[$pair.Key]=[pscustomobject]@{Status=$status;Output=$actual}
  }
  $process.Dispose()
 }
 $same=$results.Python2.Status -ceq 'PASS' -and
       $results.Python3.Status -ceq 'PASS' -and
       $results.Python2.Output -ceq $results.Python3.Output
 $all=if($same){'PASS'}else{'HOLD'}
 $digest=(Get-FileHash $candidate -Algorithm SHA256).Hash.ToLowerInvariant()
 $ledger.Add($entry.Key+[char]9+$digest+[char]9+$entry.Value+[char]9+
  $results.Python2.Status+[char]9+$results.Python3.Status+[char]9+$same+[char]9+$all)
 Write-Host ('OCTADI_WS|VARIANT='+$entry.Key+'|PY2='+$results.Python2.Status+
 '|PY3='+$results.Python3.Status+'|SAME='+$same+'|STATUS='+$all)
}
$file=Join-Path $out 'whitespace-variants.tsv'
[IO.File]::WriteAllLines($file,$ledger,$utf8)
if($original -cne (Get-FileHash $src -Algorithm SHA256).Hash.ToLowerInvariant() -or
   $fixtureSha -cne (Get-FileHash $fixture -Algorithm SHA256).Hash.ToLowerInvariant()){
 throw 'OCTADI_WS|HOLD|source_changed'
}
Write-Host ('OCTADI_WS|SOURCE_UNCHANGED=YES|VARIANTS=4|MISSING_INTERPRETER_IS_NOT_INDENT_FAILURE'+
 '|RECEIPT_SHA256='+(Get-FileHash $file -Algorithm SHA256).Hash.ToLowerInvariant()+
 '|RELEASE=HOLD')
if(@($ledger|Where-Object {$_ -match ([string][char]9+'PASS$')}).Count -eq 0){
 throw 'OCTADI_WS|HOLD|no_both_interpreters_agreed'
}
