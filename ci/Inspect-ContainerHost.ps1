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
$docker=Get-Command docker -ErrorAction SilentlyContinue | Select-Object -First 1
$engine='unavailable'
if($docker) {
 try {
   $reply=@(& docker info --format '{{.OSType}}' 2>$null)
   if($LASTEXITCODE -eq 0 -and $reply.Count -eq 1 -and
      $reply[0] -in @('windows','linux')) { $engine=[string]$reply[0] }
 } catch { $engine='unavailable' }
}
$nativeOS=if($IsMacOS){'macOS'}elseif($IsWindows){'Windows'}else{'Linux'}
$claim=if($IsMacOS) {
 'Docker, when available, runs a Linux VM; it is not native Darwin container validation.'
}elseif($IsWindows) {
 'Native Windows containers require a compatible Windows daemon and licensed matching base image.'
}else{'Linux OCI container validation is available in the dedicated Docker lanes.'}
$record=[ordered]@{
 schema='SWE-Native-OCI-Capability-v1'
 host=$nativeOS
 platform_arch=[Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString()
 engine_platform=$engine
 native_vm_tests_are_authority=$true
 host_container_build_executed=$false
 note=$claim
 }
$record|ConvertTo-Json|Set-Content -LiteralPath (Join-Path $evidence 'capability.json') -Encoding utf8
Write-Host "OCI capability on $nativeOS: $engine. $claim"
