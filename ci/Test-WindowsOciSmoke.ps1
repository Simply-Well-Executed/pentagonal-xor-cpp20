# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Experimental Windows container smoke, driven by PowerShell 7.
# The host compiler is authoritative. Microsoft Windows base stays local.
[CmdletBinding()]
param([string]$EvidenceDir='evidence-windows-oci')
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
if(!$IsWindows){throw 'Windows container execution requires a Windows host.'}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence=[IO.Path]::GetFullPath((Join-Path $root $EvidenceDir))
[void][IO.Directory]::CreateDirectory($evidence)
$sourceBin=Join-Path $root 'build-root/Release/pentagonal_xor.exe'
$guardBin=Join-Path $root 'build-shared/Release/swe_guard.exe'
foreach($file in @($sourceBin,$guardBin)){
 if(!(Test-Path -LiteralPath $file -PathType Leaf)){
   throw "Missing native Windows binary required by container smoke: $file"
 }
}
$dockerOS=((& docker info --format '{{.OSType}}' 2>$null) | Select-Object -First 1)
if($LASTEXITCODE -ne 0 -or $dockerOS -cne 'windows'){
 throw 'No compatible Windows container engine was available.'
}
$short=if($env:GITHUB_SHA.Length -ge 12){$env:GITHUB_SHA.Substring(0,12)}else{'local'}
$tag='swe-guard-windows-smoke:'+$short
$dockerfile=Join-Path $root 'docker/windows-cpp-smoke.Dockerfile'
& docker build --file $dockerfile --tag $tag $root *>&1 |
 Tee-Object -FilePath (Join-Path $evidence 'image-build.log')
if($LASTEXITCODE -ne 0){throw 'Windows container construction failed.'}
& docker run --rm --network none --isolation process $tag 'C:\guard\swe_guard.exe' --self-test *>&1 |
 Tee-Object -FilePath (Join-Path $evidence 'guard-run.log')
if($LASTEXITCODE -ne 0){throw 'Windows container 32-state parity smoke failed.'}
& docker run --rm --network none --isolation process $tag 'C:\guard\pentagonal_xor.exe' 1 1 1 0 0 *>&1 |
 Tee-Object -FilePath (Join-Path $evidence 'parity-run.log')
if($LASTEXITCODE -ne 0){throw 'Windows container runtime parity smoke failed.'}
$image=((& docker image inspect --format '{{.Id}}' $tag) | Select-Object -First 1)
[ordered]@{
 schema='SWE-Windows-OCI-Exploratory-v1'
 native_host=$env:RUNNER_OS
 source_head=((& git rev-parse HEAD) | Select-Object -First 1)
 image_id=$image
 vendor_os_base='mcr.microsoft.com/windows/servercore:ltsc2022'
 vendor_os_redistributed=$false
 code_compiled_on_host=$true
 native_windows_container_executed=$true
 runtime_tests_passed=$true
 source_exe_sha256=(Get-FileHash $sourceBin -Algorithm SHA256).Hash.ToLowerInvariant()
 guard_exe_sha256=(Get-FileHash $guardBin -Algorithm SHA256).Hash.ToLowerInvariant()
}|ConvertTo-Json|Set-Content -LiteralPath (Join-Path $evidence 'manifest.json') -Encoding utf8
Write-Host 'PASS: experimental Windows Server Core binary execution smoke.'
