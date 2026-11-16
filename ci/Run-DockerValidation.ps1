# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Docker guard. CI jobs call this script directly with PowerShell 7.
# "Publish" may only run on a trusted main/tag push with Docker Hub secrets.
[CmdletBinding()]
param(
    [ValidateSet('Linux','Android','OpenSUSE','Publish')][Parameter(Mandatory)][string]$Mode,
    [string]$EvidenceDir = 'evidence-docker'
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$evidence = [IO.Path]::GetFullPath((Join-Path $root $EvidenceDir))
[void][IO.Directory]::CreateDirectory($evidence)
$dockerfile = Join-Path $root 'docker/android-source-study-v1.Dockerfile'
$imageRepo = 'paulcheek/android-source-study-v1'
$leapRepo = 'paulcheek/opensuse-leap-hardened'
$leapDockerfile = Join-Path $root 'docker/opensuse-leap-hardened.Dockerfile'
$sourceSha = [string]$env:GITHUB_SHA
$shortSha = if ($sourceSha.Length -ge 12) { $sourceSha.Substring(0,12) } else { 'local' }
$localImage = 'swe-source-study:' + $Mode.ToLowerInvariant() + '-' + $shortSha

function Invoke-CheckedDocker([string[]]$Arguments) {
    & docker @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Docker invocation failed ($LASTEXITCODE): $($Arguments[0])"
    }
}

if ($Mode -eq 'Publish') {
    if ($env:GITHUB_EVENT_NAME -cne 'push' -or
        ($env:GITHUB_REF -cne 'refs/heads/main' -and
         $env:GITHUB_REF -notlike 'refs/tags/v*')) {
        throw 'Docker Hub publishing is forbidden outside trusted main/version-tag pushes.'
    }
    if ($env:DOCKERHUB_USERNAME -cne 'paulcheek' -or
        [string]::IsNullOrWhiteSpace($env:DOCKERHUB_TOKEN)) {
        throw 'Docker Hub credentials are missing or do not match the paulcheek namespace.'
    }

    $env:DOCKER_CONFIG = Join-Path $env:RUNNER_TEMP 'swe-docker-credentials'
    [void][IO.Directory]::CreateDirectory($env:DOCKER_CONFIG)
    try {
        $immutableTag = "$imageRepo" + ':sha-' + $shortSha
        $latestTag = 'paulcheek/android-source-study-v1:latest'
        Invoke-CheckedDocker -Arguments @('build','--pull','--file',$dockerfile,'--target',
            'source','--tag',$immutableTag,'--tag',$latestTag,$root)

        # Explicitly verify the public image contains neither Android SDK
        # nor a source-space keystore, and runs as an unprivileged user.
        Invoke-CheckedDocker -Arguments @('run','--rm','--network','none','--read-only',
            '--cap-drop','ALL','--security-opt','no-new-privileges',
            $latestTag,'-Command',
            'if ((Test-Path /opt/android-sdk) -or (Test-Path /study/android/app/build)) { exit 51 }; if ([Environment]::UserName -eq "root") { exit 52 }; "PASS SDK-free public image"')

        if ($env:OS_LICENSE_REVIEW_APPROVED -cne 'true') {
            throw 'openSUSE container publication requires third-party license review clearance.'
        }
        $leapImmutable = "$leapRepo" + ':sha-' + $shortSha
        $leapLatest = 'paulcheek/opensuse-leap-hardened:latest'
        Invoke-CheckedDocker -Arguments @('build','--file',$leapDockerfile,
            '--target','source','--tag',$leapImmutable,'--tag',$leapLatest,$root)
        Invoke-CheckedDocker -Arguments @('run','--rm','--network','none','--read-only',
            '--cap-drop','ALL','--security-opt','no-new-privileges',
            $leapLatest,'cmake','--version')
        $licenseArgs = @('run','--rm','--network','none','--read-only','--cap-drop',
            'ALL','--security-opt','no-new-privileges','--entrypoint','rpm',$leapLatest,
            '-qa','--qf','%{NAME}|%{VERSION}-%{RELEASE}|%{LICENSE}\n')
        $licenseRows = @(& docker @licenseArgs)
        if ($LASTEXITCODE -ne 0 -or $licenseRows.Count -lt 1) {
            throw 'OS package license inventory missing: no publication.'
        }
        $licenseRows | Sort-Object |
            Set-Content -LiteralPath (Join-Path $evidence 'opensuse-os-licenses.tsv') -Encoding utf8
        # Never expose the Docker Hub access token in argv or output.
        $env:DOCKERHUB_TOKEN | & docker login --username $env:DOCKERHUB_USERNAME --password-stdin
        if ($LASTEXITCODE -ne 0) { throw 'Docker Hub login failed.' }
        Invoke-CheckedDocker -Arguments @('push',$immutableTag)
        # Intentional exact user-requested publication target:
        Invoke-CheckedDocker -Arguments @('push','paulcheek/android-source-study-v1:latest')
        Invoke-CheckedDocker -Arguments @('push',$leapImmutable)
        Invoke-CheckedDocker -Arguments @('push','paulcheek/opensuse-leap-hardened:latest')
        $localDigest = (& docker image inspect --format '{{.Id}}' $latestTag |
            Select-Object -First 1)
        [ordered]@{
            schema = 'SWE-GAMP4-style-container-publication-v1'
            image = $latestTag
            immutable_tag = $immutableTag
            opensuse_image = $leapLatest
            opensuse_immutable_tag = $leapImmutable
            opensuse_licenses = 'opensuse-os-licenses.tsv'
            third_party_licenses_present = $true
            github_sha = $sourceSha
            local_image_id = $localDigest
            sdk_embedded = $false
            published_after_gates = $true
            created_utc = (Get-Date).ToUniversalTime().ToString('o')
        } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $evidence 'publish.json')
        Write-Host "PASS: docker push paulcheek/android-source-study-v1:latest"
        Write-Host "PASS: docker push paulcheek/opensuse-leap-hardened:latest"
    } finally {
        & docker logout 2>$null | Out-Null
        Remove-Item -LiteralPath $env:DOCKER_CONFIG -Recurse -Force -ErrorAction SilentlyContinue
    }
    return
}

# The openSUSE validation target compiles/tests at Docker build time.
# Each CTest RUN step has BuildKit network disabled.
if ($Mode -eq 'OpenSUSE') {
    $buildArgs = @('build','--progress','plain','--file',$leapDockerfile,
        '--target','validation','--tag',$localImage,$root)
    & docker @buildArgs *>&1 | Tee-Object -FilePath (Join-Path $evidence 'opensuse-validation.log')
    if ($LASTEXITCODE -ne 0) { throw 'Pinned openSUSE C++20/C11 acceptance suite rejected image.' }
    $imageId = (& docker image inspect --format '{{.Id}}' $localImage | Select-Object -First 1)
    if ($LASTEXITCODE -ne 0) { throw 'Cannot inspect openSUSE validation image.' }
    $licenseArgs = @('run','--rm','--network','none','--read-only','--cap-drop',
        'ALL','--security-opt','no-new-privileges','--entrypoint','rpm',$localImage,
        '-qa','--qf','%{NAME}|%{VERSION}-%{RELEASE}|%{LICENSE}\n')
    $licenseRows = @(& docker @licenseArgs)
    if ($LASTEXITCODE -ne 0 -or $licenseRows.Count -lt 1) {
        throw 'openSUSE package license inventory unavailable.'
    }
    $licenseRows | Sort-Object |
        Set-Content -LiteralPath (Join-Path $evidence 'opensuse-os-licenses.tsv') -Encoding utf8
    [ordered]@{
        schema='SWE-Pinned-Leap16-GAMP4-Style-v1'
        base_index_sha256='a4115e2f5aea9712a7c73974fd94000a034fc3051ef29b5f7722fbf04070dd9d'
        source_sha=$sourceSha
        validation_image_id=$imageId
        tests_passed=$true
        source_license='0BSD'
        os_packages_are_separately_licensed=$true
    } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $evidence 'opensuse-validation.json')
    $checksumRows = @(Get-ChildItem -LiteralPath $evidence -File | Sort-Object Name |
        ForEach-Object {
            '{0}  {1}' -f (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant(),$_.Name
        })
    $checksumRows | Set-Content -LiteralPath (Join-Path $evidence 'SHA256SUMS') -Encoding utf8
    Write-Host "PASS: pinned openSUSE Leap acceptance suite, image $imageId"
    return
}
$target = if ($Mode -eq 'Android') { 'android-sdk' } else { 'source' }
$args = @('build','--pull','--progress','plain','--file',$dockerfile,
    '--target',$target,'--tag',$localImage)
if ($Mode -eq 'Android') {
    $args += @('--build-arg','ACCEPT_ANDROID_SDK_TERMS=true')
}
$args += $root
& docker @args *>&1 | Tee-Object -FilePath (Join-Path $evidence 'image-build.log')
if ($LASTEXITCODE -ne 0) { throw "Docker $Mode image construction failed." }

$imageId = (& docker image inspect --format '{{.Id}}' $localImage |
    Select-Object -First 1)
if ($LASTEXITCODE -ne 0) { throw 'Cannot fingerprint local validation image.' }

$containerName = "swe-$($Mode.ToLowerInvariant())-$shortSha-$env:GITHUB_RUN_ID"
$scriptPath = if ($Mode -eq 'Android') {
    './ci/Invoke-AndroidDockerValidation.ps1'
} else {
    './ci/Invoke-LinuxDockerValidation.ps1'
}
$createArgs = @('create','--name',$containerName,'--network','none',
    '--cap-drop','ALL','--security-opt','no-new-privileges',
    '--pids-limit','512','--memory','4g','--entrypoint','pwsh',
    $localImage,'-NoLogo','-NoProfile','-File',$scriptPath)
$container = (& docker @createArgs | Select-Object -First 1)
if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($container)) {
    throw 'Unable to create unprivileged offline Docker validation container.'
}
$exitCode = 125
try {
    & docker start -a $container *>&1 |
        Tee-Object -FilePath (Join-Path $evidence 'container-run.log')
    if ($LASTEXITCODE -ne 0) {
        Write-Warning 'docker start/log attach returned an error; inspecting exit code.'
    }
    $exitLines = @(& docker inspect --format '{{.State.ExitCode}}' $container)
    if ($exitLines.Count -ne 1) {
        throw 'Container exit status unavailable.'
    }
    $exitCode = [int]$exitLines[0]
    $inside = if ($Mode -eq 'Android') { 'evidence-android' } else { 'evidence-linux' }
    & docker cp "$($container):/study/$inside/." $evidence *>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Warning 'No complete in-container evidence directory was available.'
    }
    if ($Mode -eq 'Android' -and $exitCode -eq 0) {
        $apk = Join-Path $evidence 'app-unsigned.apk'
        Invoke-CheckedDocker -Arguments @('cp',
            "$($container):/study/android/app/build/outputs/apk/release/app-unsigned.apk",
            $apk)
        $sha = (Get-FileHash -LiteralPath $apk -Algorithm SHA256).Hash.ToLowerInvariant()
        Write-Host "Validated Android APK SHA256: $sha"
    }
} finally {
    & docker rm -f $container *>$null
}
[ordered]@{
    schema = 'SWE-GAMP4-style-container-validation-v1'
    github_sha = $sourceSha
    target = $target
    container_validation_exit_code = $exitCode
    offline_build = $true
    no_new_privileges = $true
    all_linux_capabilities_dropped = $true
    image_id = $imageId
    created_utc = (Get-Date).ToUniversalTime().ToString('o')
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $evidence 'docker-validation.json')
if ($exitCode -ne 0) {
    throw "Docker $Mode validation failed with exit status $exitCode."
}
$lines = Get-ChildItem -LiteralPath $evidence -File -Recurse |
    Where-Object { $_.Name -ne 'SHA256SUMS' } |
    Sort-Object FullName | ForEach-Object {
        $relative = [IO.Path]::GetRelativePath($evidence,$_.FullName).Replace('\','/')
        '{0}  {1}' -f (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant(),$relative
    }
$lines | Set-Content -LiteralPath (Join-Path $evidence 'SHA256SUMS') -Encoding utf8
Write-Host "PASS Docker $($Mode): source commit $sourceSha, image $imageId"
