# SPDX-License-Identifier: 0BSD
# Pure PowerShell Core / .NET. No network or mutation of legal text.
# LOCK records exact Git blob content, not speculative component rights.
[CmdletBinding()]
param(
    [string] $RepositoryRoot = (Join-Path $PSScriptRoot '..'),
    [string] $ReportPath = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$utf8 = [Text.UTF8Encoding]::new($false,$true)
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$textRoot = [IO.Path]::GetFullPath((Join-Path $root 'LICENSES/text'))
$lockPath = Join-Path $root 'LICENSES/LOCK.tsv'
if (-not [IO.Directory]::Exists($textRoot)) { throw 'LOCAL_LICENSE|HOLD|missing_text_directory' }
if (-not [IO.File]::Exists($lockPath)) { throw 'LOCAL_LICENSE|HOLD|missing_lock' }
if (((Get-Item -LiteralPath $textRoot -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
    throw 'LOCAL_LICENSE|HOLD|text_directory_is_link'
}
$lines = [IO.File]::ReadAllLines($lockPath,$utf8)
$head = @('local_path','license_id','policy_status','source_repository','source_commit','upstream_path','git_blob_sha1') -join [char]9
if ($lines.Length -lt 2 -or -not [StringComparer]::Ordinal.Equals($lines[0],$head)) { throw 'LOCAL_LICENSE|HOLD|lock_header_invalid' }
$seen = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$records = [Collections.Generic.List[string]]::new()
$preferred = @('0BSD','BSD-2-Clause','BSD-3-Clause','MIT','LGPL-2.1-only')
$review = @('BSD-3-Clause-Clear','BSD-3-Clause-Attribution','BSD-3-Clause-LBNL','MIT-0','LGPL-2.1-or-later')
$forbidden = @('LGPL-2.0-only','LGPL-3.0-only','GPL-1.0-only','GPL-2.0-only','GPL-3.0-only','GPL-3.0-or-later','AGPL-1.0-only','AGPL-3.0-only','AGPL-3.0-or-later','EPL-1.0','EPL-2.0','EUPL-1.1','EUPL-1.2','GFDL-1.3-only')
foreach ($line in $lines[1..($lines.Length-1)]) {
    $f = $line.Split([char]9)
    if ($f.Length -ne 7) { throw 'LOCAL_LICENSE|HOLD|lock_columns_invalid' }
    $relative,$id,$policy,$source,$sourceCommit,$upstream,$expectedSha = $f
    if ($relative -cnotmatch '^LICENSES/text/[A-Za-z0-9.-]+[.]txt$') { throw 'LOCAL_LICENSE|HOLD|invalid_relative_path' }
    if (-not $seen.Add($relative)) { throw "LOCAL_LICENSE|HOLD|duplicate|$relative" }
    if (-not [StringComparer]::Ordinal.Equals($id,[IO.Path]::GetFileNameWithoutExtension($relative))) { throw "LOCAL_LICENSE|HOLD|name_mismatch|$relative" }
    if ($sourceCommit -cnotmatch '^[0-9a-f]{40}$' -or $expectedSha -cnotmatch '^[0-9a-f]{40}$') { throw "LOCAL_LICENSE|HOLD|invalid_revision|$relative" }
    if ($source -eq 'spdx/license-list-data') {
        if (-not [StringComparer]::Ordinal.Equals($sourceCommit,'31ba1a50e5397e00a304dbadc76531740e89ee48') -or -not [StringComparer]::Ordinal.Equals($upstream,"text/$id.txt")) {
            throw "LOCAL_LICENSE|HOLD|source_pin_drift|$relative"
        }
        $want = if ($id -cin $preferred) { 'PREFERRED' } elseif ($id -cin $review) { 'REVIEW' } elseif ($id -cin $forbidden) { 'FORBIDDEN' } else { throw "LOCAL_LICENSE|HOLD|unreviewed_id|$id" }
        if (-not [StringComparer]::Ordinal.Equals($policy,$want)) { throw "LOCAL_LICENSE|HOLD|policy_drift|$id" }
    } elseif ($source -eq 'oracle/tribuo-site') {
        if (-not [StringComparer]::Ordinal.Equals($id,'Oracle-NFTC-reference') -or -not [StringComparer]::Ordinal.Equals($policy,'PREFERRED_VENDOR_REVIEW') -or
            -not [StringComparer]::Ordinal.Equals($sourceCommit,'4548f7e2c326c4284adbbd57bfbeca65897fbbc4') -or
            -not [StringComparer]::Ordinal.Equals($upstream,'learn/4.0/javadoc/legal/LICENSE')) { throw 'LOCAL_LICENSE|HOLD|Oracle_identity_drift' }
    } else { throw "LOCAL_LICENSE|HOLD|source_not_reviewed|$source" }
    $full = [IO.Path]::GetFullPath((Join-Path $root ($relative.Replace('/',[IO.Path]::DirectorySeparatorChar))))
    if (-not $full.StartsWith($textRoot + [IO.Path]::DirectorySeparatorChar,[StringComparison]::Ordinal)) {
        throw "LOCAL_LICENSE|HOLD|path_escape|$relative"
    }
    if (-not [IO.File]::Exists($full)) { throw "LOCAL_LICENSE|HOLD|missing|$relative" }
    if (((Get-Item -LiteralPath $full -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw "LOCAL_LICENSE|HOLD|file_link|$relative"
    }
    $bytes = [IO.File]::ReadAllBytes($full)
    [void]$utf8.GetString($bytes)
    $prefix = [Text.Encoding]::ASCII.GetBytes(('blob ' + $bytes.Length) + [char]0)
    $buffer = [byte[]]::new($prefix.Length + $bytes.Length)
    [Array]::Copy($prefix,0,$buffer,0,$prefix.Length)
    [Array]::Copy($bytes,0,$buffer,$prefix.Length,$bytes.Length)
    $actualSha = [Convert]::ToHexString([Security.Cryptography.SHA1]::HashData($buffer)).ToLowerInvariant()
    if ($actualSha -cne $expectedSha) { throw "LOCAL_LICENSE|HOLD|content_mismatch|$relative" }
    $sha256 = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
    $records.Add(([pscustomobject]@{ path=$relative; spdx_or_reference=$id; status=$policy; source_repository=$source; source_commit=$sourceCommit; git_blob_sha1=$actualSha; sha256=$sha256; length=$bytes.Length } | ConvertTo-Json -Compress -Depth 3))
    Write-Output "LOCAL_LICENSE|VERIFIED|$id|$policy|sha256=$sha256"
}
# The root LICENSE must equal the standard SPDX 0BSD boilerplate with only
# the owner/year/email template placeholders substituted. Never relicense,
# normalize, or add conditions to the canonical legal terms.
$canonicalPath = Join-Path $textRoot '0BSD.txt'
$projectPath = Join-Path $root 'LICENSE'
if (-not [IO.File]::Exists($projectPath)) { throw 'LOCAL_LICENSE|HOLD|missing_root_license' }
if (((Get-Item -LiteralPath $projectPath -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
    throw 'LOCAL_LICENSE|HOLD|root_license_is_link'
}
$standard = $utf8.GetString([IO.File]::ReadAllBytes($canonicalPath))
$placeholder = 'Copyright (C) YEAR by AUTHOR EMAIL'
$notice = 'Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824'
if (-not $standard.StartsWith($placeholder + "`n`n",[StringComparison]::Ordinal)) {
    throw 'LOCAL_LICENSE|HOLD|SPDX_0BSD_placeholder_changed'
}
$expectedProject = $standard.Replace($placeholder,$notice)
$projectBytes = [IO.File]::ReadAllBytes($projectPath)
$projectText = $utf8.GetString($projectBytes)
if (-not [StringComparer]::Ordinal.Equals($projectText,$expectedProject)) {
    throw 'LOCAL_LICENSE|HOLD|root_license_differs_from_standard_0BSD'
}
$projectSHA256 = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($projectBytes)).ToLowerInvariant()
$records.Add(([pscustomobject]@{
    path='LICENSE'; spdx_or_reference='0BSD'; status='STANDARD_TEXT_WITH_COPYRIGHT_FIELDS';
    source_repository='spdx/license-list-data';
    source_commit='31ba1a50e5397e00a304dbadc76531740e89ee48';
    git_blob_sha1='derived_from_0BSD_template'; sha256=$projectSHA256;
    length=$projectBytes.Length
} | ConvertTo-Json -Compress -Depth 3))
Write-Output "LOCAL_LICENSE|ROOT_0BSD|PASS|sha256=$projectSHA256"

# An internally consistent but incomplete lock is not a valid baseline.
# The fixed ID set is derived from CURRENT policy categories, not from LOCK.tsv,
# so deleting a reference and its lock row must fail closed.
$requiredIds = @($preferred) + @($review) + @($forbidden) + @('Oracle-NFTC-reference')
$requiredPaths = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach ($requiredId in $requiredIds) {
    if (-not $requiredPaths.Add("LICENSES/text/$requiredId.txt")) {
        throw "LOCAL_LICENSE|HOLD|duplicate_required_license|$requiredId"
    }
}
if (-not $seen.SetEquals($requiredPaths)) {
    $missing = @($requiredPaths | Where-Object { -not $seen.Contains($_) })
    $extra = @($seen | Where-Object { -not $requiredPaths.Contains($_) })
    throw "LOCAL_LICENSE|HOLD|required_reference_set_drift|missing=$($missing -join ',')|extra=$($extra -join ',')"
}

$inventory = @(Get-ChildItem -LiteralPath $textRoot -File -Recurse -Force)
if ($inventory.Count -ne $seen.Count) { throw "LOCAL_LICENSE|HOLD|unregistered_files|found=$($inventory.Count)|locked=$($seen.Count)" }
foreach ($item in $inventory) {
    $rel = 'LICENSES/text/' + $item.Name
    if (-not $seen.Contains($rel)) { throw "LOCAL_LICENSE|HOLD|unregistered|$rel" }
}
foreach ($required in @('LICENSES/UNKNOWN.md','LICENSES/MIXED.md','LICENSES/UNAPPROVED-GNU.md','LICENSES/COMPONENT_USAGE.tsv')) {
    if (-not [IO.File]::Exists((Join-Path $root $required))) { throw "LOCAL_LICENSE|HOLD|missing_policy_record|$required" }
}
if ($ReportPath) {
    $target = [IO.Path]::GetFullPath($ReportPath)
    [IO.File]::WriteAllLines($target,$records,$utf8)
    Write-Output "LOCAL_LICENSE|JSONL|$target"
}
Write-Output "LOCAL_LICENSE|PASS|references=$($seen.Count)|root_standard_0BSD=PASS|offline=true|release_approval=false"
