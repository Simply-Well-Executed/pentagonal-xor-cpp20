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
if ($lines.Length -lt 2 -or $lines[0] -cne $head) { throw 'LOCAL_LICENSE|HOLD|lock_header_invalid' }
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
    if ($id -cne [IO.Path]::GetFileNameWithoutExtension($relative)) { throw "LOCAL_LICENSE|HOLD|name_mismatch|$relative" }
    if ($sourceCommit -cnotmatch '^[0-9a-f]{40}$' -or $expectedSha -cnotmatch '^[0-9a-f]{40}$') { throw "LOCAL_LICENSE|HOLD|invalid_revision|$relative" }
    if ($source -eq 'spdx/license-list-data') {
        if ($sourceCommit -cne '31ba1a50e5397e00a304dbadc76531740e89ee48' -or $upstream -cne "text/$id.txt") {
            throw "LOCAL_LICENSE|HOLD|source_pin_drift|$relative"
        }
        $want = if ($id -cin $preferred) { 'PREFERRED' } elseif ($id -cin $review) { 'REVIEW' } elseif ($id -cin $forbidden) { 'FORBIDDEN' } else { throw "LOCAL_LICENSE|HOLD|unreviewed_id|$id" }
        if ($policy -cne $want) { throw "LOCAL_LICENSE|HOLD|policy_drift|$id" }
    } elseif ($source -eq 'oracle/tribuo-site') {
        if ($id -cne 'Oracle-NFTC-reference' -or $policy -cne 'PREFERRED_VENDOR_REVIEW' -or
            $sourceCommit -cne '4548f7e2c326c4284adbbd57bfbeca65897fbbc4' -or
            $upstream -cne 'learn/4.0/javadoc/legal/LICENSE') { throw 'LOCAL_LICENSE|HOLD|Oracle_identity_drift' }
    } elseif ($source -eq 'Simply-Well-Executed/pentagonal-xor-cpp20') {
        if ($id -cne 'PROJECT-0BSD' -or $policy -cne 'PROJECT_LICENSE' -or
            $sourceCommit -cne 'fa9bec5a7efaef9b02b70969cff40c9542b7534c' -or
            $upstream -cne 'LICENSE') { throw 'LOCAL_LICENSE|HOLD|project_identity_drift' }
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
Write-Output "LOCAL_LICENSE|PASS|verified=$($seen.Count)|offline=true|release_approval=false"
