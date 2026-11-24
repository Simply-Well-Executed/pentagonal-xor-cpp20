# SPDX-License-Identifier: 0BSD
# OCTADI-G-001: offline, strictly separate Python 2.7.18 and 3.x processes.
# The same self-contained class G performs both binary orders and all 3! triples.
# No runtime fetch, package installation, py launcher, imports or remote RPC.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$code=Join-Path $root 'octadi/G.py'
$fixture=Join-Path $root 'pentadi/PentaDiC11BranchRecords.reference.tsv'
$utf8=[Text.UTF8Encoding]::new($false,$true)
if(-not [IO.File]::Exists($code) -or -not [IO.File]::Exists($fixture) -or
   [IO.Directory]::Exists($out) -or $out -ceq $root -or
   $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'OCTADI|HOLD|source_absent_or_nonfresh_external_evidence_required'
}
[void][IO.Directory]::CreateDirectory($out)
$before=@((Get-FileHash -LiteralPath $code -Algorithm SHA256).Hash,
          (Get-FileHash -LiteralPath $fixture -Algorithm SHA256).Hash)
$rows=[Collections.Generic.List[string]]::new()
$rows.Add('interpreter'+[char]9+'version'+[char]9+'executable_sha256'+[char]9+
 'stdout_sha256'+[char]9+'exit_code'+[char]9+'negative_canary'+[char]9+'status')
function Launch([string]$exe,[string[]]$arguments,[string]$directory,[string]$stem){
 $stdout=Join-Path $out ($stem+'.stdout.txt')
 $stderr=Join-Path $out ($stem+'.stderr.txt')
 # Arguments contain no shell interpolation or remote command evaluator.
 $quoted=@($arguments|ForEach-Object {
  if($_.Contains('"')){throw 'OCTADI|HOLD|untrusted_argument_quote'}
  '"'+$_+'"'
 })
 $process=Start-Process -FilePath $exe -ArgumentList $quoted -WorkingDirectory $directory -PassThru -NoNewWindow -RedirectStandardOutput $stdout -RedirectStandardError $stderr
 if(-not $process.WaitForExit(30000)){
  $process.Kill($true);$process.WaitForExit()
  throw ('OCTADI|HOLD|interpreter_timeout|'+$stem)
 }
 $exit=$process.ExitCode
 $process.Dispose()
 return [pscustomobject]@{
  ExitCode=$exit
  Out=[IO.File]::ReadAllText($stdout,$utf8)
  Err=[IO.File]::ReadAllText($stderr,$utf8)
  Sha=(Get-FileHash -LiteralPath $stdout -Algorithm SHA256).Hash.ToLowerInvariant()
 }
}
function Resolve-Python([int]$major){
 $hint=if($major -eq 2){$env:OCTADI_PY2_EXE}else{$env:OCTADI_PY3_EXE}
 if([string]::IsNullOrWhiteSpace($hint) -and $major -eq 3){
  $candidate=Get-Command python.exe -CommandType Application -ErrorAction SilentlyContinue
  if($null -ne $candidate){$hint=$candidate.Source}
 }
 # IMPORTANT: never use the auto-installing py.exe runtime manager.
 if([string]::IsNullOrWhiteSpace($hint)){return $null}
 $path=[IO.Path]::GetFullPath($hint)
 if(-not [IO.File]::Exists($path) -or
    $path.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
  return $null
 }
 return $path
}
$statuses=@{}
foreach($major in @(2,3)){
 $identity='Python'+$major
 $exe=Resolve-Python $major
 if($null -eq $exe){
  $statuses[$identity]='HOLD_RUNTIME_NOT_LOCALLY_INSTALLED'
  $rows.Add($identity+[char]9+'UNKNOWN'+[char]9+'UNAVAILABLE'+[char]9+
    'UNAVAILABLE'+[char]9+'NA'+[char]9+'NOT_RUN'+[char]9+$statuses[$identity])
  Write-Host ('OCTADI|'+$identity+'|HOLD|LOCAL_INTERPRETER_NOT_PROVISIONED|NO_DOWNLOAD')
  continue
 }
 $digest=(Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash.ToLowerInvariant()
 $versionCall=Launch $exe @('-E','-S','-V') $root ($identity+'-version')
 $versionText=($versionCall.Out+$versionCall.Err).Trim()
 $pattern=if($major -eq 2){'^Python 2[.]7[.]18$'}else{'^Python 3[.][0-9]+[.][0-9]+$'}
 if($versionCall.ExitCode -ne 0 -or $versionText -cnotmatch $pattern){
  $statuses[$identity]='HOLD_VERSION_MISMATCH'
  $rows.Add($identity+[char]9+$versionText+[char]9+$digest+[char]9+
    'UNAVAILABLE'+[char]9+$versionCall.ExitCode+[char]9+'NOT_RUN'+[char]9+$statuses[$identity])
  Write-Host ('OCTADI|'+$identity+'|HOLD|EXPECTED='+$pattern+'|OBSERVED='+$versionText)
  continue
 }
 $run=Launch $exe @('-E','-S',$code) $root ($identity+'-eight-layouts')
 $lines=@($run.Out.Trim().Split([char]10)|ForEach-Object{$_.Trim()})
 $valid=($run.ExitCode -eq 0 -and $lines.Count -eq 9 -and
         @($lines|Where-Object {$_ -notmatch '\|PASS$' -and $_ -notmatch '^G\|PASS\|'}).Count -eq 0 -and
         @($lines|Where-Object {$_ -cmatch '^G\|LAYOUT='}).Count -eq 8 -and
         $lines[-1] -cmatch '^G\|PASS\|LAYOUTS=8\|BINARY=2\|TERNARY=6\|')
 $canary='NOT_RUN'
 if($valid){
  $scratch=Join-Path $out ($identity+'-negative-fixture')
  $p=Join-Path $scratch 'pentadi'
  [void][IO.Directory]::CreateDirectory($p)
  $original=[IO.File]::ReadAllText($fixture,$utf8)
  # Preserve 9-line count but break the first canonical branch field.
  $linesOld=@($original -split "\r?\n"|Where-Object {$_ -ne ''})
  if($linesOld.Count -ne 9){throw 'OCTADI|HOLD|source_fixture_shape'}
  $linesOld[0]='MALFORMED|TRAP'
  [IO.File]::WriteAllText((Join-Path $p 'PentaDiC11BranchRecords.reference.tsv'),
    ([string]::Join([string][char]10,$linesOld)+[char]10),$utf8)
  $negative=Launch $exe @('-E','-S',$code) $scratch ($identity+'-negative')
  $canary=if($negative.ExitCode -ne 0 -and
     $negative.Err.Contains('G|HOLD|invalid_record_shape',[StringComparison]::Ordinal)){'REJECTED'}else{'FAILED'}
 }
 $statuses[$identity]=if($valid -and $canary -ceq 'REJECTED'){'PASS'}else{'HOLD'}
 $rows.Add($identity+[char]9+$versionText+[char]9+$digest+[char]9+
   $run.Sha+[char]9+$run.ExitCode+[char]9+$canary+[char]9+$statuses[$identity])
 Write-Host ('OCTADI|'+$identity+'|STATUS='+$statuses[$identity]+
  '|LAYOUTS='+$(if($valid){'8'}else{'INVALID'})+
  '|CANARY='+$canary+'|EXE_SHA256='+$digest+'|RELEASE=HOLD')
 $statuses[$identity+'Output']=$run.Out
}
$receipt=Join-Path $out 'G-table-receipt.tsv'
[IO.File]::WriteAllLines($receipt,$rows,$utf8)
if($before[0] -cne (Get-FileHash -LiteralPath $code -Algorithm SHA256).Hash -or
   $before[1] -cne (Get-FileHash -LiteralPath $fixture -Algorithm SHA256).Hash){
 throw 'OCTADI|HOLD|source_changed'
}
if($statuses['Python2'] -cne 'PASS' -or $statuses['Python3'] -cne 'PASS'){
 throw 'OCTADI|HOLD|both_interpreters_required_no_substitution'
}
if($statuses['Python2Output'] -cne $statuses['Python3Output']){
 throw 'OCTADI|HOLD|cross_version_stdout_mismatch'
}
Write-Host ('OCTADI|PASS|PY2=2.7.18|PY3=LOCALLY_PINNED|LAYOUTS=8'+
 '|TESTS=2_FACTORIAL_PLUS_3_FACTORIAL|SOURCE_STABLE=YES'+
 '|RECEIPT_SHA256='+(Get-FileHash -LiteralPath $receipt -Algorithm SHA256).Hash.ToLowerInvariant()+
 '|NO_REMOTE_RUNTIME|RELEASE=HOLD')
