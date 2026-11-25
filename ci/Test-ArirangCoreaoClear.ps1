# SPDX-License-Identifier: 0BSD
# Offline synthetic verification: neither merged -> one merged -> both merged.
[CmdletBinding()]
param()
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=Join-Path ([IO.Path]::GetTempPath()) ('arirang-test-'+[guid]::NewGuid().ToString('N'))
$script=Join-Path $PSScriptRoot 'Invoke-ArirangCoreaoClear.ps1'
function Git([string[]]$ArgsForGit) {
 $out=@(& git -C $root @ArgsForGit 2>&1)
 if($LASTEXITCODE -ne 0){throw ('ARIRANG_TEST|GIT_FAILED|'+($out -join ' '))}
 return $out
}
function File([string]$name,[string]$value) {
 [IO.File]::WriteAllText((Join-Path $root $name),$value,[Text.UTF8Encoding]::new($false))
}
function Inspect([string]$yin,[string]$yang,[bool]$expectClear) {
 $output=@(& pwsh -NoLogo -NoProfile -NonInteractive -File $script -RepositoryRoot $root -Yin $yin -Yang $yang -Target refs/heads/main 2>&1)
 $exit=$LASTEXITCODE
 $json=@($output | Where-Object {$_ -is [string]}) -join [Environment]::NewLine
 # On a HOLD the PowerShell error stream follows the JSON; split after the closing JSON line.
 $lines=$json -split '\r?\n'
 $depth=0; $last=-1; $began=$false
 for($i=0;$i -lt $lines.Length;$i++){
  if($lines[$i] -ceq '{'){$began=$true}
  if($began -and $lines[$i] -ceq '}'){$last=$i;break}
 }
 if($last -lt 0){throw 'ARIRANG_TEST|MISSING_JSON'}
 $data=($lines[0..$last] -join [Environment]::NewLine) | ConvertFrom-Json
 if($expectClear){
  if($exit -ne 0 -or $data.decision -cne 'CLEAR_ANCESTRY_ONLY' -or
     -not $data.airgap.both_heads_ancestors -or $data.release -cne 'HOLD'){
   throw 'ARIRANG_TEST|CLEAR_NOT_VERIFIED'
  }
 } else {
  if($exit -eq 0 -or $data.decision -cne 'HOLD'){
   throw 'ARIRANG_TEST|FAIL_OPEN'
  }
 }
}
try {
 [void][IO.Directory]::CreateDirectory($root)
 [void](Git @('init','-q','-b','main'))
 [void](Git @('config','user.name','ARIRANG test'))
 [void](Git @('config','user.email','arirang@example.invalid'))
 File 'origin.txt' 'ancestor'
 [void](Git @('add','.'));[void](Git @('commit','-qm','root'))
 $base=([string](Git @('rev-parse','HEAD'))).Trim()
 [void](Git @('switch','-qc','yin'))
 File 'a.txt' 'YIN'
 [void](Git @('add','.'));[void](Git @('commit','-qm','yin'))
 $yin=([string](Git @('rev-parse','HEAD'))).Trim()
 [void](Git @('switch','-q','main'))
 [void](Git @('switch','-qc','yang'))
 File 'b.txt' 'YANG'
 [void](Git @('add','.'));[void](Git @('commit','-qm','yang'))
 $yang=([string](Git @('rev-parse','HEAD'))).Trim()
 [void](Git @('switch','-q','main'))
 Inspect $yin $yang $false
 [void](Git @('merge','--no-ff','-qm','merge yin','yin'))
 Inspect $yin $yang $false
 [void](Git @('merge','--no-ff','-qm','merge yang','yang'))
 Inspect $yin $yang $true
 $snapshot=Join-Path $root 'clear.json'
 [void](& pwsh -NoLogo -NoProfile -NonInteractive -File $script -RepositoryRoot $root -Yin $yin -Yang $yang -Target refs/heads/main -SnapshotPath $snapshot)
 if($LASTEXITCODE -ne 0 -or -not [IO.File]::Exists($snapshot)){throw 'ARIRANG_TEST|SNAPSHOT_FAILURE'}
 $record=Get-Content -LiteralPath $snapshot -Raw | ConvertFrom-Json
 if($record.decision -cne 'CLEAR_ANCESTRY_ONLY'){throw 'ARIRANG_TEST|SNAPSHOT_LABEL'}
 $again=@(& pwsh -NoLogo -NoProfile -NonInteractive -File $script -RepositoryRoot $root -Yin $yin -Yang $yang -Target refs/heads/main -SnapshotPath $snapshot 2>&1)
 if($LASTEXITCODE -eq 0){throw 'ARIRANG_TEST|SNAPSHOT_OVERWRITTEN'}
 Write-Output 'ARIRANG|THREE_GAUGES=PASS|AIRGAP=PASS|SNAPSHOT_CREATE_NEW=PASS|RELEASE=HOLD'
}finally{
 if([IO.Directory]::Exists($root)){[IO.Directory]::Delete($root,$true)}
}
