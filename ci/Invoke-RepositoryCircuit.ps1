# SPDX-License-Identifier: 0BSD
# EXCITE serial file/subproject/whole-repo circuit with COREAO evidence after each lap.
# File custody checks do not mean an individual file compiles.
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
# Trusted repository-owned function file. No user-controlled code or workflow downloads.
. (Join-Path $root 'ci/Excite-Coreao.ps1')
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Test-ExciteFlow.ps1') -RepositoryRoot $root -EvidenceDirectory (Join-Path $out 'excite-selftest')
if($LASTEXITCODE -ne 0){throw 'EXCITE|HOLD|selftest_failed'}
$excite=Initialize-ExciteEvidence (Join-Path $out 'excite')
# Fresh isolated scratch-pad-point0, with historical runs left untouched.
if($excite.Sequence -ne 0 -or $excite.PreviousCodeSha -cne ('0'*64)){
  throw 'EXCITE|HOLD|scratch_pad_point0_not_clean'
}
Write-Host ('EXCITE|SCRATCH_PAD_POINT0|SEQUENCE=0|QCODE_GENESIS='+$excite.PreviousCodeSha+
            '|VIEW=ONE|FRONT_TOTAL=30|REAR_TOTAL=12|RELEASE=HOLD')
$manifest=[Collections.Generic.List[object]]::new()
$stages=[Collections.Generic.List[object]]::new()
$failedSubprojects=[Collections.Generic.List[string]]::new()
$failedFiles=[Collections.Generic.List[string]]::new()
$exciteAudit='HOLD'
$exciteHead=''
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
  $sha=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant()
  $vehicleKind=if($name -ceq 'KENTUCKY_DERBY'){'REPOSITORY'}else{'SUBPROJECT'}
  $entry.qcode_sha256=Add-ExciteVehicle -Context $excite -Kind $vehicleKind `
    -Label $name -SourceSha $sha `
    -Status $(if($exitCode -eq 0){'PASS'}else{'HOLD'}) `
    -ElapsedMs $timer.ElapsedMilliseconds -Bytes ([IO.FileInfo]$source).Length `
    -Detail ('native_exit_code='+$exitCode)
  $stages.Add($entry)
  Write-Host ('NASCAR|FINISH|'+$name+'|EXIT='+$exitCode+'|MS='+$timer.ElapsedMilliseconds)
  Assert-SourceStable $name
  if($exitCode -ne 0){
    $failedSubprojects.Add($name)
    Write-Host ('NASCAR|SUBPROJECT_HOLD|'+$name+'|CONTINUING_INDEPENDENT_LAPS')
  }
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
    $fileTimer=[Diagnostics.Stopwatch]::StartNew()
    $digest=(Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    $cold=$relative.StartsWith('Archive/Retired/',[StringComparison]::OrdinalIgnoreCase)
    $kind=if($cold){'RETIRED_CUSTODY_ONLY'}else{'FINGERPRINT_ONLY'}
    $lapStatus='PASS'
    if(-not $cold -and $relative -match '[.](ps1|psm1|psd1)$'){
      $tokens=$null;$parseErrors=$null
      $null=[Management.Automation.Language.Parser]::ParseFile(
        $file.FullName,[ref]$tokens,[ref]$parseErrors)
      if(@($parseErrors).Count -gt 0){
        $kind='POWERSHELL_SYNTAX_HOLD'
        $lapStatus='HOLD'
      }else{$kind='POWERSHELL_SYNTAX_PASS'}
    }elseif(-not $cold -and $relative -match '[.](cpp|hpp|h|c|java|pas|dpr|asm|yml|yaml|xhtml|xml|svg|tsv|md|txt)$'){
      try {
        [void]$utf8.GetString([IO.File]::ReadAllBytes($file.FullName))
        $kind='UTF8_CUSTODY_PASS_NOT_COMPILE'
      }catch [Text.DecoderFallbackException]{
        $kind='INVALID_UTF8_HOLD'
        $lapStatus='HOLD'
      }
    }
    $fileTimer.Stop()
    # Each source vehicle enters COREAO immediately, regardless of PASS/HOLD.
    $qcodeSha=Add-ExciteVehicle -Context $excite -Kind FILE `
      -Label $relative -SourceSha $digest -Status $lapStatus `
      -ElapsedMs $fileTimer.ElapsedMilliseconds -Bytes $file.Length -Detail $kind
    $manifest.Add([ordered]@{path=$relative;sha256=$digest;bytes=$file.Length;validation=$kind;status=$lapStatus;qcode_sha256=$qcodeSha})
    if($lapStatus -ceq 'HOLD'){$failedFiles.Add($relative)}
    Write-Host ('FILE_LAP|'+$relative+'|'+$kind+'|SHA256='+$digest+'|QCODE='+$qcodeSha)
  }
  Assert-SourceStable 'FILE_LAPS'
  $null=Assert-ExciteEvidence $excite
  Write-Host ('DOVIA|RAIL_AUTOPILOT|CHECKPOINT=FILE_LAPS|VEHICLES='+$excite.Sequence+'|RULEBOOK=FROZEN|NO_OVERRIDES')
  $stages.Add([ordered]@{
    lane='FILE_LAPS'
    status=if($failedFiles.Count -eq 0){'PASS'}else{'HOLD'}
    files=$manifest.Count
    failures=$failedFiles.Count
  })
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
  $null=Assert-ExciteEvidence $excite
  if($failedFiles.Count -gt 0 -or $failedSubprojects.Count -gt 0){
    throw ('EXCITE|HOLD|file_failures='+$failedFiles.Count+
           '|subproject_failures='+($failedSubprojects -join ','))
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
  try {
    $exciteHead=Assert-ExciteEvidence $excite
    $exciteAudit='PASS'
  }catch{
    $exciteAudit='HOLD'
    $status='HOLD'
    $failure += '|excite_audit='+[string]$_.Exception.Message
    Write-Host ('EXCITE|HOLD|AUDIT|'+[string]$_.Exception.Message)
  }
  $receipt=[ordered]@{
    protocol='NASCAR-KENTUCKY-001'
    vehicle_signal_protocol='DOVIA.SIGNAL.2'
    vehicle_view_protocol='DOVIA.VIEW.1'
    vehicle_front_total_degrees=30
    vehicle_rear_total_degrees=12
    vehicle_observer_count=1
    vehicle_signal_scope='PER_VEHICLE_NOT_CIRCUIT_SCHEDULER'
    workflow_sha=$env:GITHUB_SHA
    status=$status
    failed_phase=if($status -ceq 'PASS'){''}else{$phase}
    failure=$failure
    elapsed_ms=$stopwatch.ElapsedMilliseconds
    file_count=$manifest.Count
    stages=@($stages.ToArray())
    subproject_failures=@($failedSubprojects.ToArray())
    file_failures=@($failedFiles.ToArray())
    excite_vehicles=$excite.Sequence
    excite_qcode_head=$exciteHead
    excite_chain_audit=$exciteAudit
    release='HOLD'
  }
  $manifestPath=Join-Path $out 'file-laps.json'
  $receiptPath=Join-Path $out 'race-receipt.json'
  [IO.File]::WriteAllText($manifestPath,(ConvertTo-Json -InputObject @($manifest.ToArray()) -Depth 9 -WarningAction Stop),[Text.UTF8Encoding]::new($false))
  [IO.File]::WriteAllText($receiptPath,(ConvertTo-Json -InputObject $receipt -Depth 9 -WarningAction Stop),[Text.UTF8Encoding]::new($false))
  $mh=(Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash.ToLowerInvariant()
  $rh=(Get-FileHash -LiteralPath $receiptPath -Algorithm SHA256).Hash.ToLowerInvariant()
  Write-Host ('NASCAR|MANIFEST_SHA256='+$mh+'|COUNT='+$manifest.Count)
  Write-Host ('KENTUCKY_DERBY|RECEIPT_SHA256='+$rh+'|RESULT='+$status+
              '|EXCITE_QCODE_SHA256='+$exciteHead+'|RELEASE=HOLD')
  if(-not [string]::IsNullOrEmpty($env:GITHUB_STEP_SUMMARY)){
    Add-Content -LiteralPath $env:GITHUB_STEP_SUMMARY -Value (
      'NASCAR/Kentucky: '+$status+'; file count '+$manifest.Count+
      '; receipt SHA-256 '+$rh+'; EXCITE QCODE SHA-256 '+$exciteHead+
      '; release HOLD.')
  }
  if($exciteAudit -cne 'PASS'){throw 'EXCITE|HOLD|final_chain_verification'}
}
