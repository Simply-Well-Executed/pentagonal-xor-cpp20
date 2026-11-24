# SPDX-License-Identifier: 0BSD
# Single-runner serial circuit. File checks do not imply every file compiles alone.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'REPO_CIRCUIT|HOLD|Windows_MSVC_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $out
if($LASTEXITCODE -ne 0 -or [IO.Directory]::Exists($out)){
  throw 'REPO_CIRCUIT|HOLD|evidence_path_not_fresh_or_safe'
}
[void][IO.Directory]::CreateDirectory($out)
$manifest=[Collections.Generic.List[object]]::new()
$stages=[Collections.Generic.List[object]]::new()
$phase='FILE_LAPS'
$status='HOLD'
$failure=''
$stopwatch=[Diagnostics.Stopwatch]::StartNew()
$utf8=[Text.UTF8Encoding]::new($false,$true)
function Assert-SourceStable([string]$scope){
  foreach($entry in $manifest){
    $file=Join-Path $root $entry.path
    if(-not [IO.File]::Exists($file)){
      throw ('REPO_CIRCUIT|HOLD|source_deleted|'+$entry.path)
    }
    $now=(Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant()
    if($now -cne $entry.sha256){
      throw ('REPO_CIRCUIT|HOLD|source_mutated|'+$scope+'|'+$entry.path)
    }
  }
  Write-Host ('REPO_CIRCUIT|SOURCE_STABLE|'+$scope+'|FILES='+$manifest.Count)
}
function Invoke-Lap([string]$name,[string]$relative,[string[]]$arguments){
  $source=Join-Path $root $relative
  if(-not [IO.File]::Exists($source)){
    throw ('REPO_CIRCUIT|HOLD|missing_lap|'+$relative)
  }
  Write-Host ('NASCAR|START|'+$name+'|SEQUENTIAL=1')
  $timer=[Diagnostics.Stopwatch]::StartNew()
  & pwsh -NoLogo -NoProfile -NonInteractive -File $source @arguments
  $exitCode=$LASTEXITCODE
  $timer.Stop()
  $entry=[ordered]@{
    lane=$name
    source=$relative
    exit_code=$exitCode
    elapsed_ms=$timer.ElapsedMilliseconds
    status=if($exitCode -eq 0){'PASS'}else{'HOLD'}
  }
  $stages.Add($entry)
  Write-Host ('NASCAR|FINISH|'+$name+'|EXIT='+$exitCode+'|MS='+$timer.ElapsedMilliseconds)
  Assert-SourceStable $name
  if($exitCode -ne 0){throw ('REPO_CIRCUIT|HOLD|subproject_failed|'+$name)}
}
try{
  # Stage 1: enumerate EVERY file, including documentation and cold-retired
  # records, serially; execute no historical/retired code and no issue text.
  $files=@(Get-ChildItem -LiteralPath $root -File -Recurse -Force | Sort-Object FullName)
  if($files.Count -lt 340){throw ('REPO_CIRCUIT|HOLD|incomplete_source|'+$files.Count)}
  foreach($file in $files){
    $relative=[IO.Path]::GetRelativePath($root,$file.FullName).Replace('\','/')
    if($relative -match '[\r\n\t|]' -or $file.Length -gt 67108864){
      throw 'REPO_CIRCUIT|HOLD|unrepresentable_or_oversize_source'
    }
    $digest=(Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    $cold=$relative.StartsWith('Archive/Retired/',[StringComparison]::OrdinalIgnoreCase)
    $kind=if($cold){'RETIRED_CUSTODY_ONLY'}else{'FINGERPRINT_ONLY'}
    if(-not $cold -and $relative -match '[.](ps1|psm1|psd1)$'){
      $tokens=$null;$parseErrors=$null
      $null=[Management.Automation.Language.Parser]::ParseFile(
        $file.FullName,[ref]$tokens,[ref]$parseErrors)
      if(@($parseErrors).Count -gt 0){
        throw ('REPO_CIRCUIT|HOLD|powershell_parse|'+$relative)
      }
      $kind='POWERSHELL_SYNTAX_PASS'
    }elseif(-not $cold -and $relative -match '[.](cpp|hpp|h|c|java|pas|dpr|asm|yml|yaml|xhtml|xml|svg|tsv|md|txt)$'){
      [void]$utf8.GetString([IO.File]::ReadAllBytes($file.FullName))
      $kind='UTF8_CUSTODY_PASS_NOT_COMPILE'
    }
    $manifest.Add([ordered]@{path=$relative;sha256=$digest;bytes=$file.Length;validation=$kind})
    Write-Host ('FILE_LAP|'+$relative+'|'+$kind+'|SHA256='+$digest)
  }
  Assert-SourceStable 'FILE_LAPS'
  $stages.Add([ordered]@{lane='FILE_LAPS';status='PASS';files=$manifest.Count})
  # Stage 2: actual native validators for known subproject contracts.
  # Each writes unique evidence OUTSIDE the checkout and runs in isolation.
  $phase='SUBPROJECT_LAPS'
  $lanes=@(
    @{name='TRIFORCE';path='ci/Invoke-TriforcePipeline.ps1'},
    @{name='DIAERY';path='diaery/Invoke-ParityGate.ps1'},
    @{name='QUADDI';path='quaddi-ni/Invoke-NativeQualification.ps1'},
    @{name='MINZA_TOPOLOGY';path='ci/Invoke-MinzaTopologyQualification.ps1'},
    @{name='PENTADI';path='pentadi/Invoke-PentaDiQualification.ps1'},
    @{name='PENTADI_CHAOS';path='pentadi/Invoke-HulaoChaos255.ps1'}
  )
  foreach($lane in $lanes){
    $evidence=Join-Path $out ('module-'+$lane.name.ToLowerInvariant())
    Invoke-Lap $lane.name $lane.path @('-RepositoryRoot',$root,'-EvidenceDirectory',$evidence)
  }
  # Stage 3: entire build/install/CTest/normal app and consumer together.
  $phase='KENTUCKY_DERBY'
  Invoke-Lap 'KENTUCKY_DERBY' 'ci/Invoke-FullSourceAutostartNativeSuite.ps1' @(
    '-RepositoryRoot',$root,
    '-OutputDirectory',(Join-Path $out 'whole-repository'),
    '-Platform','Windows')
  Assert-SourceStable 'FINAL_POSTFLIGHT'
  $status='PASS'
  Write-Host 'KENTUCKY_DERBY|WHOLE_REPOSITORY=PASS|SUBPROJECTS=SERIAL|RELEASE=HOLD'
}catch{
  $failure=[string]$_.Exception.Message
  Write-Host ('REPO_CIRCUIT|HOLD|PHASE='+$phase+'|ERROR='+$failure)
  throw
}finally{
  $stopwatch.Stop()
  $receipt=[ordered]@{
    protocol='NASCAR-KENTUCKY-001'
    workflow_sha=$env:GITHUB_SHA
    status=$status
    failed_phase=if($status -ceq 'PASS'){''}else{$phase}
    failure=$failure
    elapsed_ms=$stopwatch.ElapsedMilliseconds
    file_count=$manifest.Count
    stages=@($stages.ToArray())
    release='HOLD'
  }
  $manifestPath=Join-Path $out 'file-laps.json'
  $receiptPath=Join-Path $out 'race-receipt.json'
  [IO.File]::WriteAllText($manifestPath,(ConvertTo-Json -InputObject @($manifest.ToArray()) -Depth 9 -WarningAction Stop),[Text.UTF8Encoding]::new($false))
  [IO.File]::WriteAllText($receiptPath,(ConvertTo-Json -InputObject $receipt -Depth 9 -WarningAction Stop),[Text.UTF8Encoding]::new($false))
  $mh=(Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash.ToLowerInvariant()
  $rh=(Get-FileHash -LiteralPath $receiptPath -Algorithm SHA256).Hash.ToLowerInvariant()
  Write-Host ('NASCAR|MANIFEST_SHA256='+$mh+'|COUNT='+$manifest.Count)
  Write-Host ('KENTUCKY_DERBY|RECEIPT_SHA256='+$rh+'|RESULT='+$status+'|RELEASE=HOLD')
  if(-not [string]::IsNullOrEmpty($env:GITHUB_STEP_SUMMARY)){
    Add-Content -LiteralPath $env:GITHUB_STEP_SUMMARY -Value (
      'NASCAR/Kentucky: '+$status+'; file count '+$manifest.Count+
      '; receipt SHA-256 '+$rh+'; release HOLD.')
  }
}
