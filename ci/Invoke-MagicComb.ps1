# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Magic Comb: repository text at rest, before build evaluation.
# Requires latest stable PowerShell Core; no Python, Node, Linux container or PHP.
[CmdletBinding()]
param(
    [ValidateSet('Apply','Verify','SelfTest')][string] $Mode = 'Verify',
    [string] $RepositoryRoot = '.',
    [ValidateRange(1,9)][int] $NestingDepth = 9,
    [string] $AuditPath = '.magic-comb-audit.json'
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $false
$utf8 = [Text.UTF8Encoding]::new($false, $true)
$combPattern = '[\u200B-\u200D\uFEFF]'
function Invoke-TextComb {
    param([string] $Original)
    return [regex]::Replace($Original.Normalize([Text.NormalizationForm]::FormKC), $combPattern, '')
}
function Assert-MagicCombSelfTest {
    $inputText = ([string][char]0x213C) + ([string][char]0x200C) + ([string][char]0xFF21) + ([string][char]0xFEFF)
    if ((Invoke-TextComb $inputText) -cne ([string][char]0x03C0+'A')) {
        throw 'Magic Comb NFKC/zero-width fixture failed'
    }
    if ((Invoke-TextComb '中文 עברית π') -cne '中文 עברית π') {
        throw 'Magic Comb multilingual preservation fixture failed'
    }
    try {
        [void]$utf8.GetString([byte[]]@(0xC3,0x28))
        throw 'Magic Comb invalid UTF-8 fixture unexpectedly decoded'
    } catch [Text.DecoderFallbackException] { }
    Write-Host 'MAGIC_COMB|SELF_TEST|PASS'
}
if ($Mode -eq 'SelfTest') {
    Assert-MagicCombSelfTest
    return
}
$root = [IO.Path]::GetFullPath($RepositoryRoot)
# Externally issued legal texts are canonical bytes, not MagicComb input.
# Verify the local source-bound registry first; never run NFKC on a legal term.
$registryGate = Join-Path $root 'ci/Assert-LocalLicenseRegistry.ps1'
if (-not [IO.File]::Exists($registryGate)) { throw 'MagicComb local license registry checker missing' }
& $registryGate -RepositoryRoot $root

$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = 'git'
$start.WorkingDirectory = $root
$start.UseShellExecute = $false
$start.RedirectStandardOutput = $true
$start.RedirectStandardError = $true
foreach ($a in @('ls-files','-z','--cached')) { [void]$start.ArgumentList.Add($a) }
$proc = [Diagnostics.Process]::Start($start)
if ($null -eq $proc) { throw 'Cannot enumerate tracked repository inputs' }
$stream = [IO.MemoryStream]::new()
try {
    $proc.StandardOutput.BaseStream.CopyTo($stream)
    $stderr = $proc.StandardError.ReadToEnd()
    $proc.WaitForExit()
    if ($proc.ExitCode -ne 0) { throw "git ls-files failed: $stderr" }
    $pathStream = $utf8.GetString($stream.ToArray())
} finally {
    $stream.Dispose()
    $proc.Dispose()
}
$changed = [Collections.Generic.List[object]]::new()
$scanned = 0
$paths = $pathStream.Split([char]0)
foreach ($rel in $paths) {
    if ([string]::IsNullOrEmpty($rel)) { continue }
    if ($rel.IndexOf([char]0x5C) -ge 0) { throw "Tracked filename contains ambiguous literal backslash: $rel" }
    $path = $rel
    $file = [IO.Path]::GetFullPath((Join-Path $root $path))
    if (-not $file.StartsWith($root.TrimEnd([IO.Path]::DirectorySeparatorChar)+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)) {
        throw "Tracked path escapes repository: $path"
    }
    $item = Get-Item -LiteralPath $file -Force
    if (($item.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw "Tracked symbolic link requires separate source review: $path"
    }
    # The current repository consists of text records. This rejects invalid
    # UTF-8 instead of silently treating unknown binary content as source.
    $bytes = [IO.File]::ReadAllBytes($file)
    try {
        $original = $utf8.GetString($bytes)
    } catch [Text.DecoderFallbackException] {
        throw "Invalid UTF-8 in tracked file: $path"
    }
    if ($original.Contains([char]0)) {
        throw "NUL byte / binary candidate requires explicit review: $path"
    }
    $scanned++
    # Byte-exact external license copies are separately hash-pinned.
    # Applying Unicode normalization here would corrupt legal evidence.
    if ($path -ceq 'LICENSE' -or $path.StartsWith('LICENSES/text/',[StringComparison]::Ordinal)) { continue }
    $result = Invoke-TextComb $original
    if ($original -cne $result) {
        $oldSha = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
        $newBytes = $utf8.GetBytes($result)
        $newSha = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($newBytes)).ToLowerInvariant()
        $changed.Add([pscustomobject]@{ path=$path; original_sha256=$oldSha; cleaned_sha256=$newSha })
        if ($Mode -eq 'Apply') {
            [IO.File]::WriteAllBytes($file,$newBytes)
        }
    }
}
$audit = [ordered]@{
    schema='SWE-MAGIC-COMB-AT-REST-v1'
    mode=$Mode
    source_commit=(& git -C $root rev-parse HEAD).Trim()
    powershell_core_version=[string]$PSVersionTable.PSVersion
    nesting_depth=$NestingDepth
    transform='strict UTF-8 decode -> Unicode NFKC -> remove U+200B..U+200D and U+FEFF -> UTF-8 encode'
    legal_texts='LICENSE and LICENSES/text/** separately verified by standard 0BSD derivation / local Git-blob lock; never NFKC-transformed'
    tracked_files_scanned=$scanned
    files_changed=$changed.Count
    changes=@($changed.ToArray())
}
$target = [IO.Path]::GetFullPath((Join-Path $root $AuditPath))
$audit | ConvertTo-Json -Depth $NestingDepth | Set-Content -LiteralPath $target -Encoding utf8
Write-Host "MAGIC_COMB|$Mode|scanned=$scanned|changes=$($changed.Count)|audit=$target"
if ($Mode -eq 'Verify' -and $changed.Count -ne 0) {
    foreach ($finding in $changed | Select-Object -First 30) { Write-Host "MAGIC_COMB|NONCANONICAL|$($finding.path)" }
    throw 'Repository text is not NFKC-cleaned at rest. Run the approved PowerShell Core Apply mode and review its diff.'
}
