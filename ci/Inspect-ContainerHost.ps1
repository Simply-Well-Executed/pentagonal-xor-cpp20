# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Honest engine capability inventory; no claim of native Darwin OCI builds.
[CmdletBinding()]
param([string]$EvidenceDir='evidence-oci-host')
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence=[IO.Path]::GetFullPath((Join-Path $root $EvidenceDir))
[void][IO.Directory]::CreateDirectory($evidence)
# This job inventories a HOST CAPABILITY; it does not require a Docker daemon.
# A nonzero docker-info exit can be normal on hosted Windows runners. Capture
# it as evidence, then return a successful *inventory* exit status; never
# describe an unavailable daemon as an executed container validation.
$PSNativeCommandUseErrorActionPreference=$false
$docker=Get-Command docker -ErrorAction SilentlyContinue | Select-Object -First 1
$engine='unavailable'
$probeExitCode=$null
$probeStatus=if($docker){'UNKNOWN'}else{'CLI_ABSENT'}
$unexpectedResponse=$false
if($docker) {
 try {
   $reply=@(& docker info --format '{{.OSType}}' 2>$null)
   if($null -ne $LASTEXITCODE) { $probeExitCode=[int]$LASTEXITCODE }
   if($probeExitCode -eq 0) {
     if($reply.Count -eq 1 -and [string]$reply[0] -in @('windows','linux')) {
       $engine=[string]$reply[0]
       $probeStatus='ENGINE_RESPONDING'
     }else{
       $probeStatus='UNRECOGNIZED_ENGINE_RESPONSE'
       $unexpectedResponse=$true
     }
   }else{
     $probeStatus='ENGINE_UNAVAILABLE'
   }
 } catch {
   # No assertion that the Docker daemon can execute containers.
   $probeStatus='ENGINE_PROBE_EXCEPTION'
   if($null -ne $LASTEXITCODE){$probeExitCode=[int]$LASTEXITCODE}
 }
}
$nativeOS=if($IsMacOS){'macOS'}elseif($IsWindows){'Windows'}else{'Linux'}
$claim=if($IsMacOS) {
 'Docker, when available, runs a Linux VM; it is not native Darwin container validation.'
}elseif($IsWindows) {
 'Native Windows containers require a compatible Windows daemon and licensed matching base image.'
}else{'Linux OCI container validation is available in the dedicated Docker lanes.'}
$record=[ordered]@{
 schema='SWE-Native-OCI-Capability-v2'
 host=$nativeOS
 platform_arch=[Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString()
 docker_cli_present=[bool]$docker
 docker_info_exit_code=$probeExitCode
 probe_status=$probeStatus
 engine_platform=$engine
 native_vm_tests_are_authority=$true
 host_container_build_executed=$false
 container_execution_verified=$false
 note=$claim
 }
$record|ConvertTo-Json -Depth 8|Set-Content -LiteralPath (Join-Path $evidence 'capability.json') -Encoding utf8
Write-Host "OCI capability on $($nativeOS): $engine; probe_status=$probeStatus; docker_info_exit_code=$probeExitCode. $claim"
# An exit-zero reply with an invalid/missing OSType is a broken probe contract,
# not a documented unsupported host. Preserve capability.json and fail closed.
if($unexpectedResponse) {
 throw 'OCI capability probe returned exit 0 without one recognized engine OSType'
}
# An unsupported OCI host is an EXPECTED feasibility finding, not a build fault.
# PowerShell may otherwise propagate docker info's nonzero LASTEXITCODE through
# the GitHub step wrapper, causing a misleading red job after writing evidence.
$global:LASTEXITCODE=0
exit 0
