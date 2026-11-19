# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# PowerShell 7 regression checks for advisory OCI inventory semantics.
# No Docker daemon, container image, Python, Node or new system service needed.
[CmdletBinding()]
param()
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'

$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$inspector=(Join-Path $PSScriptRoot 'Inspect-ContainerHost.ps1').Replace("'","''")
$tempDir=Join-Path ([IO.Path]::GetTempPath()) ('swe-oci-probe-test-'+[guid]::NewGuid().ToString('N'))
[void][IO.Directory]::CreateDirectory($tempDir)
$cases=@(
  [ordered]@{
    name='daemon-unavailable';mock='function docker { $global:LASTEXITCODE=125 }'
    expect_status='ENGINE_UNAVAILABLE';expect_engine='unavailable';expect_exit=125
  },
  [ordered]@{
    name='engine-responding';mock='function docker { $global:LASTEXITCODE=0; Write-Output "windows" }'
    expect_status='ENGINE_RESPONDING';expect_engine='windows';expect_exit=0
  }
)
Push-Location -LiteralPath $root
try {
  foreach($case in $cases) {
    $evidenceDir='evidence-oci-regression-'+$case.name
    $driver=Join-Path $tempDir ($case.name+'.ps1')
    $source=$case.mock+[Environment]::NewLine+
      "& '"+$inspector+"' -EvidenceDir '"+$evidenceDir+"'"+[Environment]::NewLine+
      'if (Test-Path variable:LASTEXITCODE) { exit $LASTEXITCODE }'
    $source|Set-Content -LiteralPath $driver -Encoding utf8
    & pwsh -NoLogo -NoProfile -File $driver
    $exitCode=$LASTEXITCODE
    if($exitCode -ne 0) {
      throw "OCI regression $($case.name): expected advisory exit 0, got $exitCode"
    }
    $recordPath=Join-Path (Join-Path $root $evidenceDir) 'capability.json'
    if(!(Test-Path -LiteralPath $recordPath -PathType Leaf)) {
      throw "OCI regression $($case.name): evidence file missing"
    }
    $e=Get-Content -LiteralPath $recordPath -Raw|ConvertFrom-Json
    if($e.schema -cne 'SWE-Native-OCI-Capability-v2' -or
       $e.probe_status -cne $case.expect_status -or
       $e.engine_platform -cne $case.expect_engine -or
       $e.docker_info_exit_code -ne $case.expect_exit -or
       !$e.docker_cli_present -or
       $e.host_container_build_executed -or
       $e.container_execution_verified) {
      throw "OCI regression $($case.name): incorrect feasibility/evidence record"
    }
    Write-Host "PASS OCI mock: $($case.name); evidence persisted and no container claims"
  }
} finally {
  Pop-Location
  Remove-Item -LiteralPath $tempDir -Recurse -Force -ErrorAction SilentlyContinue
}
