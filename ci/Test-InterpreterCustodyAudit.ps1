# SPDX-License-Identifier: 0BSD
# HEADSTONE-SELFTEST-001: offline positive/negative static-policy canaries.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$RepositoryRoot,
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
if([IO.Directory]::Exists($out) -or $out -ceq $root -or
   $out.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'HEADSTONE_TEST|HOLD|unsafe_or_nonfresh'
}
$utf8=[Text.UTF8Encoding]::new($false,$true)
[void][IO.Directory]::CreateDirectory($out)
$fixture=Join-Path $out 'fixture'
$ci=Join-Path $fixture 'ci'
[void][IO.Directory]::CreateDirectory($ci)
[IO.File]::Copy((Join-Path $root 'ci/INTERPRETER_HEADSTONES.tsv'),
 (Join-Path $ci 'INTERPRETER_HEADSTONES.tsv'))
$lf=[string][char]10
$cases=[ordered]@{
 'good.ps1'="# SPDX-License-Identifier: 0BSD"+$lf+"Write-Output 'local only'"+$lf
 'remote.ps1'="# SPDX-License-Identifier: 0BSD"+$lf+"Invoke-WebRequest 'https://example.invalid/'"+$lf
 'include.ps1'="# SPDX-License-Identifier: 0BSD"+$lf+"Import-Module NamedForeignModule"+$lf
 'unlicensed.ps1'="Write-Output 'no authority'"+$lf
 'node.js'="// SPDX-License-Identifier: 0BSD"+$lf+"console.log('banned')"+$lf
 'python_auto.py'="# SPDX-License-Identifier: 0BSD"+$lf+"print('needs version pin')"+$lf
 'python2.py2'="# SPDX-License-Identifier: 0BSD"+$lf+"print 'legacy'"+$lf
 'python3.py3'="# SPDX-License-Identifier: 0BSD"+$lf+"print('modern')"+$lf
}
foreach($key in $cases.Keys){
 [IO.File]::WriteAllText((Join-Path $ci $key),$cases[$key],$utf8)
}
$auditor=Join-Path $root 'ci/Invoke-InterpreterCustodyAudit.ps1'
if(-not [IO.File]::Exists($auditor)){throw 'HEADSTONE_TEST|HOLD|auditor_missing'}
$report=Join-Path $out 'report'
& pwsh -NoLogo -NoProfile -NonInteractive -File $auditor -RepositoryRoot $fixture -EvidenceDirectory $report
if($LASTEXITCODE -ne 0){throw 'HEADSTONE_TEST|HOLD|non_strict_inventory_failed'}
$rows=@(Import-Csv -LiteralPath (Join-Path $report 'interpreter-script-headstones.tsv') -Delimiter ([char]9))
if($rows.Count -ne $cases.Count){throw 'HEADSTONE_TEST|HOLD|sample_count'}
$byPath=@{}
foreach($item in $rows){$byPath[$item.path]=$item}
function Expect([string]$name,[string]$status,[string]$hazard){
 $p='ci/'+$name
 if(-not $byPath.ContainsKey($p) -or $byPath[$p].status -cne $status){
  throw ('HEADSTONE_TEST|HOLD|wrong_status|'+$name)
 }
 if($hazard -and -not $byPath[$p].hazards.Contains($hazard,[StringComparison]::Ordinal)){
  throw ('HEADSTONE_TEST|HOLD|missing_hazard|'+$name+'|'+$hazard)
 }
}
Expect 'good.ps1' 'PASS_STATIC_ONLY' ''
Expect 'remote.ps1' 'HOLD' 'REMOTE_COMMAND_CANDIDATE'
Expect 'include.ps1' 'HOLD' 'DYNAMIC_INCLUDE_OR_EXEC'
Expect 'unlicensed.ps1' 'HOLD' 'MISSING_SPDX_NOTICE'
Expect 'node.js' 'HOLD' 'SELF_EXCLUDED_NODE'
Expect 'python_auto.py' 'HOLD' 'UNVERSIONED_PYTHON'
Expect 'python2.py2' 'HOLD' 'EXTERNAL_RUNTIME_NOT_LAB_VERIFIED'
Expect 'python3.py3' 'HOLD' 'EXTERNAL_RUNTIME_NOT_LAB_VERIFIED'
$strictOut=Join-Path $out 'strict-negative'
& pwsh -NoLogo -NoProfile -NonInteractive -File $auditor -RepositoryRoot $fixture -EvidenceDirectory $strictOut -FailOnHold 2>&1 | Out-Null
if($LASTEXITCODE -eq 0){throw 'HEADSTONE_TEST|HOLD|false_PASS_on_strict_negative'}
Write-Host 'HEADSTONE_TEST|PASS|CONTROL=1|NEGATIVE_CANARIES=7|STRICT_FAIL_CLOSED=YES|NO_SCRIPT_EXECUTED|RELEASE=HOLD'
