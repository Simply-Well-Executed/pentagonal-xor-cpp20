# SPDX-License-Identifier: 0BSD
[CmdletBinding()]
param([Parameter(Mandatory)][string]$RepositoryRoot)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$utf8 = [Text.UTF8Encoding]::new($false,$true)

function Read-CustodyBytes([string]$RelativePath) {
    $path = Join-Path $root $RelativePath
    if (-not [IO.File]::Exists($path)) { throw ('DISCARDED|HOLD|missing_file|' + $RelativePath) }
    $file = Get-Item -LiteralPath $path -Force
    if (($file.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw ('DISCARDED|HOLD|reparse_file|' + $RelativePath)
    }
    $item = $file.Directory
    while ($null -ne $item) {
        if (($item.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
            throw ('DISCARDED|HOLD|reparse_ancestor|' + $RelativePath)
        }
        $item = $item.Parent
    }
    return ,([IO.File]::ReadAllBytes($path))
}

function Get-GitBlobId([byte[]]$Bytes) {
    $prefix = [Text.Encoding]::ASCII.GetBytes(('blob ' + $Bytes.Length) + [char]0)
    $blob = [byte[]]::new($prefix.Length + $Bytes.Length)
    [Array]::Copy($prefix,0,$blob,0,$prefix.Length)
    [Array]::Copy($Bytes,0,$blob,$prefix.Length,$Bytes.Length)
    return [Convert]::ToHexString([Security.Cryptography.SHA1]::HashData($blob)).ToLowerInvariant()
}

$manifestBytes = Read-CustodyBytes 'Archive/Retired/ISSUE_29_LEGACY_SHELLS.tsv'
if ((Get-GitBlobId $manifestBytes) -cne 'd26de014326bca9eeec3aa901f9df541f992b7d7') {
    throw 'DISCARDED|HOLD|original_manifest_blob_drift'
}
$manifest = @($utf8.GetString($manifestBytes).TrimEnd([char]10).Split([char]10))
$manifestHeader = @('original_path','archive_path','original_git_blob_sha1','original_size_bytes','custody_state') -join [char]9
if ($manifest.Count -ne 18 -or $manifest[0] -cne $manifestHeader) {
    throw 'DISCARDED|HOLD|original_manifest_schema'
}
$registry = @($utf8.GetString((Read-CustodyBytes 'discardedfiles.tsv')).TrimEnd([char]10).Split([char]10))
$registryHeader = @('original_path','archive_path','original_git_blob_sha1','original_size_bytes',
    'wrapper_git_blob_sha1','wrapper_size_bytes','retired_at_commit','custody_state','reason') -join [char]9
if ($registry.Count -ne 18 -or $registry[0] -cne $registryHeader) {
    throw 'DISCARDED|HOLD|registry_schema'
}
$seen = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
for ($index = 1; $index -le 17; $index++) {
    $original = $manifest[$index].Split([char]9)
    $row = $registry[$index].Split([char]9)
    if ($original.Length -ne 5 -or $row.Length -ne 9) { throw 'DISCARDED|HOLD|registry_columns' }
    $path,$archive,$blob,$size,$wrapperBlob,$wrapperSize,$snapshot,$state,$reason = $row
    if ($path -cnotmatch '^ci/(?:docker/)?[A-Za-z0-9.-]+[.]sh$' -or -not $seen.Add($path) -or
        $archive -cne ('Archive/Retired/' + $path + '.txt') -or
        $blob -cnotmatch '^[0-9a-f]{40}$' -or $size -cnotmatch '^[0-9]+$' -or
        $wrapperBlob -cnotmatch '^[0-9a-f]{40}$' -or $wrapperSize -cnotmatch '^[0-9]+$' -or
        $snapshot -cne '0ee22aefa344b9ed4def61e3db606f19a3502d65' -or
        $state -cne 'REMOVED_HISTORY_ONLY' -or $reason -cne 'RETIRED_ENTRYPOINT_NO_EXECUTION' -or
        $original[4] -cne 'RETIRED_HISTORY_ONLY' -or
        (($row[0..3] -join [char]9) -cne ($original[0..3] -join [char]9))) {
        throw ('DISCARDED|HOLD|registry_record|' + $path)
    }
    $bytes = Read-CustodyBytes $archive
    if ($bytes.Length -ne [long]$size -or (Get-GitBlobId $bytes) -cne $blob) {
        throw ('DISCARDED|HOLD|original_blob_drift|' + $path)
    }
    if (Test-Path -LiteralPath (Join-Path $root $path)) {
        throw ('DISCARDED|HOLD|retired_entrypoint_reintroduced|' + $path)
    }
    # Reconstruct only the retired wrapper's exact nine-line text to verify its
    # recorded Git identity. This text is never written as a script or executed.
    $wrapperLines = @(
        '#!/bin/sh'
        '# SPDX-License-Identifier: 0BSD'
        '# ISSUE-29: retired legacy entrypoint; original exact bytes archived in Git.'
        ('# Archive: Archive/Retired/' + $path + '.txt')
        '# FRESH GammaGamma fixed control transfer; no original release operation.'
        '# REQUIRES repository root as working directory; experimental only.'
        ("echo 'FRESH|ENTRY|" + $path + "|RETIRED|HOLD'")
        'exec sh ./ci/FRESH-GammaGamma.sh'
        'exit 42'
    )
    $wrapperBytes = $utf8.GetBytes(($wrapperLines -join [char]10) + [char]10)
    if ($wrapperBytes.Length -ne [long]$wrapperSize -or (Get-GitBlobId $wrapperBytes) -cne $wrapperBlob) {
        throw ('DISCARDED|HOLD|wrapper_blob_drift|' + $path)
    }
}
$null = Read-CustodyBytes 'discardedfiles.md'
$shells = @(Get-ChildItem -LiteralPath $root -Filter '*.sh' -File -Recurse -Force)
if ($shells.Count -ne 0) { throw ('POLICY|HOLD|active_posix_scripts=' + $shells.Count) }

$dockerPath = 'docker/MagicComb-Oracle27-DotNet11-PowerShell77.Probe.Dockerfile'
if (Test-Path -LiteralPath (Join-Path $root $dockerPath)) { throw 'POLICY|HOLD|unpublished_gnu_probe_reintroduced' }
$dockerBytes = Read-CustodyBytes ('Archive/Retired/' + $dockerPath + '.txt')
if ((Get-GitBlobId $dockerBytes) -cne '8c0fb80d23e4c68a6022bfe7d90943cf605cd560') {
    throw 'POLICY|HOLD|unpublished_gnu_probe_archive_drift'
}

$disabledWorkflows = [ordered]@{
    'cloud-dockerhub-repush.yml' = 'b78a7650bc5f954aac283fd1afb8924a749cf779'
    'cloud-ghcr-android-no-python.yml' = '351eec2a30ee1dda509f231b8d96520e569c8ffa'
    'dockerhub-pythonfree-candidate-deployment.yml' = '66cb3e6c6256872c541681334c572f33db7d3368'
    'github-cloud-docker-gamp4.yml' = 'e5b7723e3e4092a92f183f813932b0575a667d30'
}
foreach ($name in $disabledWorkflows.Keys) {
    $body = $utf8.GetString((Read-CustodyBytes ('.github/workflows/' + $name)))
    # Pin complete source as well as each known job condition, so a newly added
    # YAML job, alias or differently spelled key cannot bypass the condition scan.
    $normalizedBytes = $utf8.GetBytes($body.Replace("`r`n","`n"))
    if ((Get-GitBlobId $normalizedBytes) -cne $disabledWorkflows[$name]) {
        throw ('POLICY|HOLD|disabled_workflow_source_drift|' + $name)
    }
    $jobsBlock = [regex]::Match($body,'(?ms)^jobs:\r?\n(?<entries>.*)$')
    if (-not $jobsBlock.Success) { throw ('POLICY|HOLD|missing_jobs|' + $name) }
    $jobs = [regex]::Matches($jobsBlock.Groups['entries'].Value,'(?ms)^  ([a-z0-9_-]+):\s*\r?\n(.*?)(?=^  [a-z0-9_-]+:\s*$|\z)')
    if ($jobs.Count -eq 0) { throw ('POLICY|HOLD|missing_jobs|' + $name) }
    foreach ($job in $jobs) {
        $jobIf = [regex]::Matches($job.Groups[2].Value,'(?m)^    if:\s*(.*?)\r?$')
        if ($jobIf.Count -ne 1 -or $jobIf[0].Groups[1].Value -cnotmatch '^\$\{\{ false \}\}(?:\s+#.*)?$') {
            throw ('POLICY|HOLD|legacy_workflow_executable|' + $name + '|' + $job.Groups[1].Value)
        }
    }
    Write-Host ('POLICY|WORKFLOW|DISABLED|' + $name + '|jobs=' + $jobs.Count)
}
Write-Host 'DISCARDED|PASS|original_blobs=17|wrapper_blobs=17|active_wrappers=0|RELEASE=HOLD'
Write-Host 'POLICY|SOURCE_CHECK|PASS|active_posix_scripts=0|gnu_probe=ARCHIVED_TEXT_ONLY|RELEASE=HOLD'
Write-Host 'POLICY|LIMIT|source_scan_only|third_party_and_organization_inventory_still_required'
