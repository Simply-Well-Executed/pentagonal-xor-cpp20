# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Exercise Docker publish gating with a local mock CLI; no registry access.
[CmdletBinding()]
param()
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

if ($IsWindows) {
    throw 'Docker publish policy test requires a Unix runner.'
}

$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$publishScript = Join-Path $PSScriptRoot 'Run-DockerValidation.ps1'
$testRoot = Join-Path ([IO.Path]::GetTempPath()) (
    'swe-docker-publish-policy-' + [Guid]::NewGuid().ToString('N'))
$dockerPath = Join-Path $testRoot 'docker'
$environmentNames = @(
    'PATH',
    'GITHUB_EVENT_NAME',
    'GITHUB_REF',
    'GITHUB_SHA',
    'DOCKER_CONFIG',
    'DOCKERHUB_USERNAME',
    'DOCKERHUB_TOKEN',
    'RUNNER_TEMP',
    'DOCKER_CALL_LOG',
    'OS_LICENSE_REVIEW_APPROVED',
    'OS_PACKAGE_LICENSE_MANIFEST_SHA256'
)
$originalEnvironment = @{}
foreach ($name in $environmentNames) {
    $originalEnvironment[$name] = [Environment]::GetEnvironmentVariable($name, 'Process')
}

try {
    [void][IO.Directory]::CreateDirectory($testRoot)
    $dockerStub = @'
#!/bin/sh
set -eu
printf '%s\n' "$*" >> "$DOCKER_CALL_LOG"
if [ "$1" = "image" ] && [ "$2" = "inspect" ]; then
    printf '%s\n' 'sha256:fixture'
elif [ "$1" = "run" ]; then
    case " $* " in
        *" --entrypoint rpm "*)
            printf '%s\n' 'fixture-package|1.0|MIT'
            ;;
        *)
            printf '%s\n' 'MOCK RUN SUCCEEDED'
            ;;
    esac
fi
'@
    [IO.File]::WriteAllText(
        $dockerPath,
        $dockerStub.Replace("`r`n", "`n"),
        [Text.UTF8Encoding]::new($false)
    )
    & chmod +x $dockerPath
    if ($LASTEXITCODE -ne 0) {
        throw 'Could not make mock Docker CLI executable.'
    }

    $mockPath = $testRoot + [IO.Path]::PathSeparator + $originalEnvironment['PATH']
    [Environment]::SetEnvironmentVariable('PATH', $mockPath, 'Process')
    [Environment]::SetEnvironmentVariable('GITHUB_EVENT_NAME', 'push', 'Process')
    [Environment]::SetEnvironmentVariable('GITHUB_REF', 'refs/heads/main', 'Process')
    [Environment]::SetEnvironmentVariable(
        'GITHUB_SHA',
        '0123456789abcdef0123456789abcdef01234567',
        'Process'
    )
    [Environment]::SetEnvironmentVariable('DOCKERHUB_USERNAME', 'paulcheek', 'Process')
    [Environment]::SetEnvironmentVariable('DOCKERHUB_TOKEN', 'mock', 'Process')
    [Environment]::SetEnvironmentVariable('RUNNER_TEMP', $testRoot, 'Process')
    [Environment]::SetEnvironmentVariable('OS_LICENSE_REVIEW_APPROVED', $null, 'Process')
    [Environment]::SetEnvironmentVariable(
        'OS_PACKAGE_LICENSE_MANIFEST_SHA256',
        $null,
        'Process'
    )

    $resolvedDocker = Get-Command docker -CommandType Application |
        Select-Object -First 1
    if ([IO.Path]::GetFullPath($resolvedDocker.Source) -cne [IO.Path]::GetFullPath($dockerPath)) {
        throw 'Mock Docker CLI did not take precedence on PATH.'
    }

    $unapprovedLog = Join-Path $testRoot 'unapproved-docker.log'
    [Environment]::SetEnvironmentVariable('DOCKER_CALL_LOG', $unapprovedLog, 'Process')
    $unapprovedEvidence = Join-Path $testRoot 'unapproved-evidence'
    $unapprovedEvidenceRelative = [IO.Path]::GetRelativePath($root, $unapprovedEvidence)
    & $publishScript -Mode Publish -EvidenceDir $unapprovedEvidenceRelative | Out-Null

    $unapprovedCalls = @(Get-Content -LiteralPath $unapprovedLog)
    $androidPushes = @($unapprovedCalls | Where-Object {
        $_ -match '^push paulcheek/android-source-study-v1:'
    })
    if ($androidPushes.Count -ne 2 -or
        $unapprovedCalls -notcontains 'push paulcheek/android-source-study-v1:sha-0123456789ab' -or
        $unapprovedCalls -notcontains 'push paulcheek/android-source-study-v1:latest') {
        throw 'Android immutable and latest tags were not both pushed without OpenSUSE approval.'
    }
    if ($unapprovedCalls -match 'opensuse-leap-hardened|opensuse-leap-hardened\.Dockerfile') {
        throw 'OpenSUSE Docker operations occurred without review approval.'
    }
    if (@($unapprovedCalls | Where-Object { $_ -match '^run ' }).Count -ne 1) {
        throw 'Unexpected container validation occurred without OpenSUSE review approval.'
    }

    $unapprovedPublish = Get-Content -LiteralPath (
        Join-Path $unapprovedEvidence 'publish.json'
    ) -Raw | ConvertFrom-Json
    if ($unapprovedPublish.android_published -ne $true -or
        $unapprovedPublish.opensuse_published -ne $false -or
        $unapprovedPublish.opensuse_skip_reason -notmatch 'OS_LICENSE_REVIEW_APPROVED') {
        throw 'Publication evidence did not record the independent Android publish and OpenSUSE skip.'
    }

    $fixtureLicenseFile = Join-Path $testRoot 'approved-license-fixture.tsv'
    'fixture-package|1.0|MIT' |
        Set-Content -LiteralPath $fixtureLicenseFile -Encoding utf8
    $approvedManifestSha = (Get-FileHash -LiteralPath $fixtureLicenseFile -Algorithm SHA256).Hash.ToLowerInvariant()
    $approvedLog = Join-Path $testRoot 'approved-docker.log'
    [Environment]::SetEnvironmentVariable('DOCKER_CALL_LOG', $approvedLog, 'Process')
    [Environment]::SetEnvironmentVariable('OS_LICENSE_REVIEW_APPROVED', 'true', 'Process')
    [Environment]::SetEnvironmentVariable(
        'OS_PACKAGE_LICENSE_MANIFEST_SHA256',
        $approvedManifestSha,
        'Process'
    )
    $approvedEvidence = Join-Path $testRoot 'approved-evidence'
    $approvedEvidenceRelative = [IO.Path]::GetRelativePath($root, $approvedEvidence)
    & $publishScript -Mode Publish -EvidenceDir $approvedEvidenceRelative | Out-Null

    $approvedCalls = @(Get-Content -LiteralPath $approvedLog)
    $approvedPushes = @($approvedCalls | Where-Object { $_ -match '^push ' })
    $approvedAndroidPushes = @($approvedCalls | Where-Object {
        $_ -match '^push paulcheek/android-source-study-v1:'
    })
    $opensuseImmutablePushes = @($approvedCalls | Where-Object {
        $_ -match '^push paulcheek/opensuse-leap-hardened:sha-'
    })
    $opensuseLatestPushes = @($approvedCalls | Where-Object {
        $_ -match '^push paulcheek/opensuse-leap-hardened:latest$'
    })
    if ($approvedPushes.Count -ne 4 -or
        $approvedAndroidPushes.Count -ne 2 -or
        $opensuseImmutablePushes.Count -ne 1 -or
        $opensuseLatestPushes.Count -ne 1) {
        throw 'A matching OpenSUSE review manifest did not permit both images to publish.'
    }
    $approvedPublish = Get-Content -LiteralPath (
        Join-Path $approvedEvidence 'publish.json'
    ) -Raw | ConvertFrom-Json
    if ($approvedPublish.opensuse_published -ne $true -or
        $approvedPublish.opensuse_license_manifest_sha256 -cne $approvedManifestSha) {
        throw 'Publication evidence did not record the approved OpenSUSE image and manifest.'
    }

    $mismatchLog = Join-Path $testRoot 'mismatch-docker.log'
    [Environment]::SetEnvironmentVariable('DOCKER_CALL_LOG', $mismatchLog, 'Process')
    [Environment]::SetEnvironmentVariable(
        'OS_PACKAGE_LICENSE_MANIFEST_SHA256',
        ('0' * 64),
        'Process'
    )
    $mismatchEvidence = Join-Path $testRoot 'mismatch-evidence'
    $mismatchEvidenceRelative = [IO.Path]::GetRelativePath($root, $mismatchEvidence)
    & $publishScript -Mode Publish -EvidenceDir $mismatchEvidenceRelative | Out-Null
    $mismatchCalls = @(Get-Content -LiteralPath $mismatchLog)
    $mismatchAndroidPushes = @($mismatchCalls | Where-Object {
        $_ -match '^push paulcheek/android-source-study-v1:'
    })
    $mismatchOpenSUSEPushes = @($mismatchCalls | Where-Object {
        $_ -match '^push paulcheek/opensuse-leap-hardened:'
    })
    if ($mismatchAndroidPushes.Count -ne 2 -or $mismatchOpenSUSEPushes.Count -ne 0) {
        throw 'A mismatched OpenSUSE license inventory did not permit Android-only publication.'
    }
    $mismatchManifestSha = (Get-FileHash -LiteralPath (
        Join-Path $mismatchEvidence 'opensuse-os-licenses.tsv'
    ) -Algorithm SHA256).Hash.ToLowerInvariant()
    $mismatchPublish = Get-Content -LiteralPath (
        Join-Path $mismatchEvidence 'publish.json'
    ) -Raw | ConvertFrom-Json
    if ($mismatchPublish.android_published -ne $true -or
        $mismatchPublish.opensuse_published -ne $false -or
        $mismatchPublish.opensuse_skip_reason -notmatch 'manifest changed' -or
        $mismatchPublish.opensuse_license_manifest_sha256 -cne $mismatchManifestSha) {
        throw 'Publication evidence did not record the changed OpenSUSE manifest and Android-only publish.'
    }
    if (!(Test-Path -LiteralPath (Join-Path $mismatchEvidence 'opensuse-os-licenses.tsv'))) {
        throw 'The OpenSUSE license inventory was not captured for the mismatch test.'
    }

    Write-Output 'PASS Docker publish policy: Android is independent; OpenSUSE remains review/hash gated.'
} finally {
    if (Test-Path -LiteralPath $testRoot) {
        Remove-Item -LiteralPath $testRoot -Recurse -Force
    }
    foreach ($name in $environmentNames) {
        [Environment]::SetEnvironmentVariable($name, $originalEnvironment[$name], 'Process')
    }
}
