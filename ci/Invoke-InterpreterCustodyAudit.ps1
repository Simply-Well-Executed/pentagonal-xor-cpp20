# SPDX-License-Identifier: 0BSD
# HEADSTONE-AUDIT-001: read-only, offline static audit for interpreted sources.
# No installation, includes, package manager, HTTP, IPC/RPC or dynamic imports.
# Does not execute inspected scripts; static scanning cannot prove network isolation.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [switch]$FailOnHold
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
$utf8=[Text.UTF8Encoding]::new($false,$true)
if(-not [IO.Directory]::Exists($root) -or [IO.Directory]::Exists($out) -or
   $out -ceq $root -or
   $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'HEADSTONE|HOLD|nonfresh_or_unsafe_output'
}
$catalog=Join-Path $root 'ci/INTERPRETER_HEADSTONES.tsv'
if(-not [IO.File]::Exists($catalog)){throw 'HEADSTONE|HOLD|source_catalog_missing'}
$lines=[IO.File]::ReadAllLines($catalog,$utf8)
$expected='family'+[char]9+'runtime'+[char]9+'publisher_or_author'+[char]9+'version'+[char]9+'official_source'+[char]9+'download_reference'+[char]9+'license_expression'+[char]9+'package_sha256'+[char]9+'provisioning_status'
if($lines.Count -lt 3 -or $lines[0] -cne $expected){
 throw 'HEADSTONE|HOLD|catalog_schema'
}
$vendors=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($line in $lines[1..($lines.Count-1)]){
 $columns=$line.Split([char]9)
 if($columns.Count -ne 9 -or $columns[0] -cnotmatch '^[a-z][a-z0-9_]*$' -or
    -not $vendors.TryAdd($columns[0],$columns)){throw 'HEADSTONE|HOLD|catalog_shape_or_duplicate'}
 foreach($column in $columns){if($column -match '[\r\n|]'){throw 'HEADSTONE|HOLD|catalog_field'}}
}
$exts=@{
 '.ps1'='powershell';'.psm1'='powershell';'.sh'='shell';'.bash'='shell';'.ksh'='shell'
 '.lua'='lua3';'.py'='python_auto';'.py2'='python2';'.py3'='python3'
 '.pl'='perl';'.r'='r';'.js'='node';'.mjs'='node'
 '.php'='php';'.rb'='ruby';'.tcl'='tcl'
}
$all=@(Get-ChildItem -LiteralPath $root -File -Recurse -Force|Sort-Object FullName)
[void][IO.Directory]::CreateDirectory($out)
$ledger=[Collections.Generic.List[string]]::new()
$ledger.Add('path'+[char]9+'script_sha256'+[char]9+'git_history_authority'+[char]9+
 'runtime_family'+[char]9+'runtime_publisher'+[char]9+'runtime_version'+[char]9+
 'official_source'+[char]9+'download_reference'+[char]9+'license_expression'+[char]9+
 'package_sha256_or_pending'+[char]9+'license_header'+[char]9+'hazards'+[char]9+'status')
$summary=[ordered]@{sources=0;pass=0;hold=0;excluded_retired=0;remote_invocations=0;dynamic_includes=0;missing_notices=0}
# Static candidate strings are NOT proof of a syscall or an external connection.
$badCalls=@('Invoke-WebRequest','Invoke-RestMethod','Start-BitsTransfer',
 'Invoke-Command','New-PSSession','Enter-PSSession','New-CimSession',
 'Import-Module','Install-Module','Install-Package','Install-PSResource',
 'Invoke-Expression','curl','curl.exe','wget','wget.exe','npm','npx')
$networkNames=@('Invoke-WebRequest','Invoke-RestMethod','Start-BitsTransfer',
 'Invoke-Command','New-PSSession','Enter-PSSession','New-CimSession',
 'curl','curl.exe','wget','wget.exe','npm','npx')
$foreignRegex='(?i)\b(import|from|include|require|source|eval|exec|loadfile|dofile|library|install[.]packages|download[.]file|system2?|socketConnection|dyn[.]load)\s*(\(|[ \t]+)'
$remoteRegex='(?i)\b(curl|wget|npm|npx)\b|https?://|(?i:\b(fetch|XMLHttpRequest|WebSocket|socketConnection)\s*\()'
foreach($file in $all){
 $rel=[IO.Path]::GetRelativePath($root,$file.FullName).Replace('\','/')
 if($rel.StartsWith('Archive/Retired/',[StringComparison]::OrdinalIgnoreCase)){
  $summary.excluded_retired++;continue
 }
 $extension=[IO.Path]::GetExtension($rel).ToLowerInvariant()
 if(-not $exts.ContainsKey($extension)){continue}
 $family=$exts[$extension]
 $summary.sources++
 $hazards=[Collections.Generic.SortedSet[string]]::new([StringComparer]::Ordinal)
 $bytes=[IO.File]::ReadAllBytes($file.FullName)
 $sha=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
 try{$body=$utf8.GetString($bytes)}
 catch{$body='';[void]$hazards.Add('INVALID_UTF8')}
 $header=$body.Substring(0,[Math]::Min($body.Length,240))
 $spdx='UNVERIFIED'
 if($header -cmatch '(?m)^\s*#\s*SPDX-License-Identifier:\s*([A-Za-z0-9.+\-]+)\s*$'){
  $spdx=$Matches[1]
 }else{[void]$hazards.Add('MISSING_SPDX_NOTICE');$summary.missing_notices++}
 if($spdx -cne '0BSD' -and $spdx -cne 'UNVERIFIED'){
  [void]$hazards.Add('THIRD_PARTY_LICENSE_REVIEW')
 }
 if(-not $vendors.ContainsKey($family)){
  [void]$hazards.Add('UNKNOWN_RUNTIME_CREDIT')
  $vendor=@($family,'NEEDS_REVIEW','UNKNOWN','UNKNOWN','UNKNOWN','UNKNOWN','UNKNOWN','UNVERIFIED','NEEDS_REVIEW')
 }else{$vendor=$vendors[$family]}
 if($family -eq 'node'){[void]$hazards.Add('SELF_EXCLUDED_NODE')}
 if($family -eq 'shell'){[void]$hazards.Add('SHELL_POLICY_REVIEW')}
 if($family -eq 'python_auto'){[void]$hazards.Add('UNVERSIONED_PYTHON')}
 if($family -eq 'powershell'){
  $tokens=$null;$parseErrors=$null
  $ast=[Management.Automation.Language.Parser]::ParseInput($body,[ref]$tokens,[ref]$parseErrors)
  if(@($parseErrors).Count -gt 0){[void]$hazards.Add('INVALID_POWERSHELL_SYNTAX')}
  foreach($call in @($ast.FindAll({param($a) $a -is [Management.Automation.Language.CommandAst]},$true))){
   $cmd=$call.GetCommandName()
   if($null -ne $cmd -and $badCalls -icontains $cmd){
    $reason=if($networkNames -icontains $cmd){'REMOTE_COMMAND_CANDIDATE'}else{'DYNAMIC_INCLUDE_OR_EXEC'}
    [void]$hazards.Add($reason)
   }
   if($call.InvocationOperator -eq [Management.Automation.Language.TokenKind]::Dot){
    [void]$hazards.Add('DYNAMIC_INCLUDE_OR_EXEC')
   }
  }
  foreach($expr in @($ast.FindAll({param($a)
    $a -is [Management.Automation.Language.InvokeMemberExpressionAst]},$true))){
   $member=[string]$expr.Member.Extent.Text
   if($member -match '(?i)\b(DownloadString|DownloadFile|GetAsync|PostAsync|SendAsync|ConnectAsync)\b'){
    [void]$hazards.Add('REMOTE_API_CANDIDATE')
   }
  }
 }else{
  # Exclude comments for non-PowerShell language syntaxes; interpret conservatively.
  $executable=@($body -split "\r?\n"|Where-Object {
   $_ -cnotmatch '^\s*(#|--|//|;)' -and -not [string]::IsNullOrWhiteSpace($_)
  })
  foreach($line in $executable){
   if($line -match $foreignRegex){[void]$hazards.Add('DYNAMIC_INCLUDE_OR_EXEC')}
   if($line -match $remoteRegex){[void]$hazards.Add('REMOTE_CODE_OR_NETWORK_CANDIDATE')}
  }
 }
 if($vendor[8] -cne 'LOCALLY_VERIFIED' -and $family -cne 'powershell'){
  [void]$hazards.Add('EXTERNAL_RUNTIME_NOT_LAB_VERIFIED')
 }
 $status=if($hazards.Count -eq 0){'PASS_STATIC_ONLY'}else{'HOLD'}
 if($status -ceq 'HOLD'){$summary.hold++}else{$summary.pass++}
 if($hazards.Contains('REMOTE_COMMAND_CANDIDATE') -or $hazards.Contains('REMOTE_API_CANDIDATE') -or
    $hazards.Contains('REMOTE_CODE_OR_NETWORK_CANDIDATE')){$summary.remote_invocations++}
 if($hazards.Contains('DYNAMIC_INCLUDE_OR_EXEC')){$summary.dynamic_includes++}
 $joined=[string]::Join(';',$hazards)
 $safeFields=@($rel,$sha,'GIT_PATH_HISTORY_NOT_LEDGER',$family,$vendor[2],
  $vendor[3],$vendor[4],$vendor[5],$vendor[6],$vendor[7],$spdx,$joined,$status)
 foreach($field in $safeFields){if($field -match '[\t\r\n]'){throw 'HEADSTONE|HOLD|invalid_ledger_field'}}
 $ledger.Add([string]::Join([string][char]9,$safeFields))
}
$report=Join-Path $out 'interpreter-script-headstones.tsv'
[IO.File]::WriteAllLines($report,$ledger,$utf8)
$receipt=Join-Path $out 'interpreter-custody-summary.json'
$summary.ledger_sha256=(Get-FileHash -LiteralPath $report -Algorithm SHA256).Hash.ToLowerInvariant()
$summary.release='HOLD'
$summary.static_proof_only=$true
[IO.File]::WriteAllText($receipt,(ConvertTo-Json -InputObject $summary -Depth 4),$utf8)
Write-Host ('HEADSTONE|INVENTORY|FILES='+$summary.sources+'|PASS_STATIC_ONLY='+$summary.pass+
 '|HOLD='+$summary.hold+'|REMOTE_CANDIDATES='+$summary.remote_invocations+
 '|INCLUDES='+$summary.dynamic_includes+'|MISSING_NOTICES='+$summary.missing_notices+
 '|SHA256='+$summary.ledger_sha256+'|NO_NETWORK_EXECUTION_PROVEN=NO|RELEASE=HOLD')
if($FailOnHold -and $summary.hold -gt 0){throw 'HEADSTONE|HOLD|legacy_interpreted_sources_require_review'}
