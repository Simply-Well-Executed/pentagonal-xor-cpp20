# SPDX-License-Identifier: 0BSD
# Synthetic offline Git DAG verification for Invoke-CrossMergeSort.ps1.
[CmdletBinding()]
param()
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=Join-Path ([IO.Path]::GetTempPath()) ('coreao-365-'+[guid]::NewGuid().ToString('N'))
$script=Join-Path $PSScriptRoot 'Invoke-CrossMergeSort.ps1'
function Git([string[]]$Arguments) {
  $output=@(& git -C $root @Arguments 2>&1)
  if($LASTEXITCODE -ne 0) { throw ('COREAO_TEST|GIT_FAILED|'+($output -join ' ')) }
  return $output
}
function Save([string]$Name,[string]$Value) {
  [IO.File]::WriteAllText((Join-Path $root $Name),$Value,[Text.UTF8Encoding]::new($false))
}
try {
  [void][IO.Directory]::CreateDirectory($root)
  [void](Git @('init','-q'))
  [void](Git @('config','user.name','COREAO synthetic'))
  [void](Git @('config','user.email','coreao@example.invalid'))
  Save 'seed.txt' 'origin'
  [void](Git @('add','--','seed.txt'))
  [void](Git @('commit','-qm','seed'))
  $base=([string](Git @('rev-parse','HEAD'))).Trim()
  [void](Git @('switch','-qc','yin'))
  Save 'overlap.txt' 'yin'
  Save 'yin only.txt' 'yin'
  [void](Git @('add','--','.'))
  [void](Git @('commit','-qm','yin'))
  $yin=([string](Git @('rev-parse','HEAD'))).Trim()
  [void](Git @('switch','-q','--detach',$base))
  [void](Git @('switch','-qc','yang'))
  Save 'overlap.txt' 'yang'
  Save 'yang only.txt' 'yang'
  [void](Git @('add','--','.'))
  [void](Git @('commit','-qm','yang'))
  $yang=([string](Git @('rev-parse','HEAD'))).Trim()
  $result=@(& pwsh -NoLogo -NoProfile -NonInteractive -File $script -RepositoryRoot $root -Yin $yin -Yang $yang)
  if($LASTEXITCODE -ne 0) { throw 'COREAO_TEST|INSPECTOR_EXIT' }
  $data=($result -join [Environment]::NewLine) | ConvertFrom-Json
  if($data.merge_base -cne $base -or
     $data.counts.shared -ne 1 -or
     $data.counts.yin_only -ne 1 -or
     $data.counts.yang_only -ne 1 -or
     $data.shared_paths[0] -cne 'overlap.txt' -or
     $data.yin_only[0] -cne 'yin only.txt' -or
     $data.yang_only[0] -cne 'yang only.txt' -or
     $data.decision -cne 'HOLD') { throw 'COREAO_TEST|CLASSIFICATION_MISMATCH' }
  $blocked=@(& pwsh -NoLogo -NoProfile -NonInteractive -File $script -RepositoryRoot $root -Yin $yin -Yang $yang -HoldOnOverlap 2>&1)
  if($LASTEXITCODE -eq 0) { throw 'COREAO_TEST|EXPECTED_HOLD_MISSING' }
  Write-Output 'COREAO-365|SYNTHETIC_DAG=PASS|OVERLAP_HOLD=PASS|RELEASE=HOLD'
} finally {
  if([IO.Directory]::Exists($root)) {
    [IO.Directory]::Delete($root,$true)
  }
}
