# SPDX-License-Identifier: 0BSD
# OPENBSD 8.0 fixed release source custody, not the mutable -stable patch tree.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string] $RepositoryRoot,
    [string] $ArchiveDirectory,
    [string] $ReportPath
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$utf8 = [Text.UTF8Encoding]::new($false,$true)
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$lock = Join-Path $root 'ci/OPENBSD_80_SOURCE_LOCK.tsv'
if (-not [IO.File]::Exists($lock)) { throw 'OPENBSD|HOLD|missing_lock' }
if (((Get-Item -LiteralPath $lock -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
    throw 'OPENBSD|HOLD|lock_symlink'
}
$bytes = [IO.File]::ReadAllBytes($lock)
$rows = [IO.File]::ReadAllLines($lock,$utf8)
$header = @('asset','sha256','area','custody_state') -join [char]9
if ($rows.Length -ne 6 -or -not [StringComparer]::Ordinal.Equals($rows[0],$header)) {
    throw 'OPENBSD|HOLD|invalid_lock_schema'
}
$names = @('src.tar.gz','sys.tar.gz','ports.tar.gz','xenocara.tar.gz','openbsd-80-base.pub')
$areas = @('base-userland','base-kernel-and-drivers','third-party-build-recipes',
           'x-window-sources','signed-manifest-public-key')
$expectedStatuses = @('SOURCE_ONLY_REVIEW','SOURCE_ONLY_REVIEW',
                      'SOURCE_ONLY_THIRD_PARTY_REVIEW','SOURCE_ONLY_THIRD_PARTY_REVIEW',
                      'SOURCE_ONLY_TRUST_ANCHOR_REVIEW')
$parsed = [Collections.Generic.List[object]]::new()
for ($i=0; $i -lt $names.Count; $i++) {
    $f = $rows[$i+1].Split([char]9)
    if ($f.Length -ne 4 -or
        -not [StringComparer]::Ordinal.Equals($f[0],$names[$i]) -or
        -not [StringComparer]::Ordinal.Equals($f[2],$areas[$i]) -or
        -not [StringComparer]::Ordinal.Equals($f[3],$expectedStatuses[$i]) -or
        $f[1] -cnotmatch '^[0-9a-f]{64}$') {
        throw ('OPENBSD|HOLD|invalid_lock_row|' + $names[$i])
    }
    $parsed.Add($f)
}
$lockSHA = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
$mode = 'LOCK_METADATA_ONLY'
if (-not [string]::IsNullOrWhiteSpace($ArchiveDirectory)) {
    $archive = [IO.Path]::GetFullPath($ArchiveDirectory)
    foreach ($row in $parsed) {
        $path = Join-Path $archive $row[0]
        if (-not [IO.File]::Exists($path)) { throw ('OPENBSD|HOLD|missing_asset|' + $row[0]) }
        if (((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
            throw ('OPENBSD|HOLD|asset_symlink|' + $row[0])
        }
        $actual = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
        if (-not [StringComparer]::Ordinal.Equals($actual,$row[1])) {
            throw ('OPENBSD|HOLD|sha256_mismatch|' + $row[0])
        }
    }
    $signature = Join-Path $archive 'SHA256.sig'
    if (-not [IO.File]::Exists($signature)) { throw 'OPENBSD|HOLD|missing_signature' }
    $signify = Get-Command signify -CommandType Application -ErrorAction SilentlyContinue
    if ($null -eq $signify) { throw 'OPENBSD|HOLD|signify_unavailable' }
    Push-Location -LiteralPath $archive
    try {
        $filesToCheck = @('src.tar.gz','sys.tar.gz','ports.tar.gz','xenocara.tar.gz')
        & $signify.Source -C -p (Join-Path $archive 'openbsd-80-base.pub') -x $signature @filesToCheck
        if ($LASTEXITCODE -ne 0) { throw 'OPENBSD|HOLD|signify_verification_failed' }
    } finally {
        Pop-Location
    }
    $mode = 'SIGNED_RELEASE_ARCHIVES_VERIFIED'
}
$record = 'OPENBSD|RELEASE=8.0|TAG=OPENBSD_8_0_BASE|STATE=' + $mode +
          '|ASSETS=5|LOCK_SHA256=' + $lockSHA +
          '|STABLE_PATCH_SNAPSHOT=UNPINNED|SOURCE_BUILD=UNVERIFIED|RELEASE_HOLD'
if (-not [string]::IsNullOrWhiteSpace($ReportPath)) {
    [IO.File]::WriteAllText([IO.Path]::GetFullPath($ReportPath),$record+[char]10,$utf8)
}
Write-Host $record
