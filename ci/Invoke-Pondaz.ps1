# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# PONDAZ: validate source and event input before the GitHub job queue.
# PowerShell Core (latest stable version), .NET NFKC; no PHP/Node/Python/ICU.
[CmdletBinding()]
param(
    [string] $RepositoryRoot = '.',
    [string] $EventPath = '',
    [string] $EvidenceDir = 'pondaz-evidence',
    [switch] $SelfTest
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $false
$utf8 = [Text.UTF8Encoding]::new($false, $true)
$invisible = [regex]::new('[\u200B-\u200D\uFEFF]', 'Compiled')
$illegalControl = [regex]::new('[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]', 'Compiled')

function Test-PondazBytes {
    param([byte[]] $Bytes, [string] $Path)
    $issues = [Collections.Generic.List[string]]::new()
    $text = ''
    try {
        $text = $utf8.GetString($Bytes)
    } catch [Text.DecoderFallbackException] {
        $issues.Add('INVALID_UTF8')
        return [pscustomobject]@{ path=$Path; valid=$false; issues=@($issues); nfkc_changed=$false; shadow_sha256=$null }
    }
    try {
        $normalized = $text.Normalize([Text.NormalizationForm]::FormKC)
    } catch {
        $issues.Add('NFKC_FAILURE')
        return [pscustomobject]@{ path=$Path; valid=$false; issues=@($issues); nfkc_changed=$false; shadow_sha256=$null }
    }
    # A source is immutable. The sanitized view is evidence only, never written
    # back to the checkout or silently used as an identity/algorithm substitute.
    $shadow = $invisible.Replace($normalized, '')
    $nfkcChanged = -not [string]::Equals($text, $normalized, [StringComparison]::Ordinal)
    if ($invisible.IsMatch($text)) { $issues.Add('INVISIBLE_U200B_U200D_OR_FEFF') }
    if ($illegalControl.IsMatch($text)) { $issues.Add('CONTROL_CHARACTER') }
    # Identity-critical job definitions cannot be silently compatibility-folded.
    if ($nfkcChanged -and ($Path -match '(^|/)\.github/workflows/.*\.(yml|yaml)$')) {
        $issues.Add('NONCANONICAL_WORKFLOW_NFKC')
    }
    $hash = [Security.Cryptography.SHA256]::HashData($utf8.GetBytes($shadow))
    $sha = [Convert]::ToHexString($hash).ToLowerInvariant()
    return [pscustomobject]@{ path=$Path; valid=($issues.Count -eq 0); issues=@($issues); nfkc_changed=$nfkcChanged; shadow_sha256=$sha }
}

function Invoke-PondazSelfTest {
    $cases = @(
        @{ name='ascii'; text='ABC 123'; outcome=$true; delta=$false },
        @{ name='greek'; text='π'; outcome=$true; delta=$false },
        @{ name='fullwidth'; text=[string][char]0xFF21; outcome=$true; delta=$true },
        @{ name='zero-width'; text=('A'+[char]0x200B+'B'); outcome=$false; delta=$false },
        @{ name='zwnj'; text=('A'+[char]0x200C+'B'); outcome=$false; delta=$false },
        @{ name='zwj'; text=('A'+[char]0x200D+'B'); outcome=$false; delta=$false },
        @{ name='bom'; text=([string][char]0xFEFF+'ABC'); outcome=$false; delta=$false },
        @{ name='control'; text=('A'+[char]0x0001+'B'); outcome=$false; delta=$false },
        @{ name='rtl-isolate'; text=([string][char]0x2067+'אבג'+[char]0x2069); outcome=$true; delta=$false }
    )
    foreach($case in $cases) {
        $bytes=$utf8.GetBytes([string]$case.text)
        $result=Test-PondazBytes -Bytes $bytes -Path ('fixtures/'+$case.name+'.md')
        if($result.valid -ne $case.outcome -or $result.nfkc_changed -ne $case.delta) {
            throw ('PONDAZ self-test failed: '+$case.name+' '+($result|ConvertTo-Json -Compress))
        }
    }
    $invalid=Test-PondazBytes -Bytes ([byte[]]@(0xC3,0x28)) -Path 'fixtures/invalid.md'
    if($invalid.valid -or $invalid.issues -notcontains 'INVALID_UTF8') {
        throw 'PONDAZ failed invalid UTF-8 negative control'
    }
    $workflow=Test-PondazBytes -Bytes ($utf8.GetBytes([string][char]0xFF21)) -Path '.github/workflows/negative.yml'
    if($workflow.valid -or $workflow.issues -notcontains 'NONCANONICAL_WORKFLOW_NFKC') {
        throw 'PONDAZ failed NFKC workflow identity negative control'
    }
    Write-Host 'PONDAZ|SELF_TEST|PASS|11 cases'
}

if($SelfTest) {
    Invoke-PondazSelfTest
    return
}

$root=[IO.Path]::GetFullPath($RepositoryRoot)
$directory=[IO.Path]::GetFullPath((Join-Path $root $EvidenceDir))
[IO.Directory]::CreateDirectory($directory) | Out-Null

# Git's NUL-delimited path stream avoids line parsing and quoted-name ambiguity.
$start=[Diagnostics.ProcessStartInfo]::new()
$start.FileName='git'
$start.WorkingDirectory=$root
$start.RedirectStandardOutput=$true
$start.RedirectStandardError=$true
foreach($arg in @('ls-files','-z','--cached')) { [void]$start.ArgumentList.Add($arg) }
$process=[Diagnostics.Process]::Start($start)
if($null -eq $process) { throw 'PONDAZ cannot start git ls-files' }
$memory=[IO.MemoryStream]::new()
try {
    $process.StandardOutput.BaseStream.CopyTo($memory)
    $errorText=$process.StandardError.ReadToEnd()
    $process.WaitForExit()
    if($process.ExitCode -ne 0) { throw ('PONDAZ git ls-files failed: '+$errorText) }
    $bytes=$memory.ToArray()
} finally {
    $memory.Dispose()
    $process.Dispose()
}
$paths=$utf8.GetString($bytes).Split([char]0)
$results=[Collections.Generic.List[object]]::new()
$audited=0
foreach($path in $paths) {
    if([string]::IsNullOrEmpty($path)) { continue }
    $path=$path.Replace('\','/')
    $name=[IO.Path]::GetFileName($path)
    $isText=($path -match '\.(md|mdx|rst|txt|yml|yaml|json|xml|xhtml|html|svg|ps1|psm1|psd1|cpp|hpp|h|c|cc|cxx|java|properties|cmake|sh|css|js|ts|toml|ini|cfg|in|gradle)$' -or
        $name -in @('CMakeLists.txt','Dockerfile','.gitignore','.gitattributes','.editorconfig','LICENSE','LICENSE.txt'))
    if(-not $isText) { continue }
    $abs=[IO.Path]::GetFullPath((Join-Path $root $path))
    if(-not $abs.StartsWith($root.TrimEnd([IO.Path]::DirectorySeparatorChar)+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)) {
        throw 'PONDAZ rejected path outside checkout'
    }
    $entry=Get-Item -LiteralPath $abs -Force
    if(($entry.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
        $results.Add([pscustomobject]@{path=$path;valid=$false;issues=@('TRACKED_SYMLINK');nfkc_changed=$false;shadow_sha256=$null})
        continue
    }
    $record=Test-PondazBytes -Bytes ([IO.File]::ReadAllBytes($abs)) -Path $path
    $results.Add($record)
    $audited++
}

if(-not [string]::IsNullOrWhiteSpace($EventPath)) {
    # GitHub event text is external input as well, including JSON-escaped strings.
    $eventRecord=Test-PondazBytes -Bytes ([IO.File]::ReadAllBytes($EventPath)) -Path 'github.event.raw'
    $results.Add($eventRecord)
    if($eventRecord.valid) {
        $eventText=$utf8.GetString([IO.File]::ReadAllBytes($EventPath))
        $eventObject=ConvertFrom-Json -InputObject $eventText -AsHashtable -Depth 100
        $stack=[Collections.Generic.Stack[object]]::new()
        $stack.Push($eventObject)
        $valueCount=0
        while($stack.Count -gt 0) {
            $node=$stack.Pop()
            if($null -eq $node) { continue }
            if($node -is [string]) {
                $valueCount++
                if($valueCount -gt 200000) { throw 'PONDAZ event value count exceeded' }
                $results.Add((Test-PondazBytes -Bytes ($utf8.GetBytes($node)) -Path ('github.event.value['+$valueCount+']')))
            } elseif($node -is [Collections.IDictionary]) {
                foreach($key in $node.Keys) {
                    $stack.Push([string]$key)
                    $stack.Push($node[$key])
                }
            } elseif($node -is [Collections.IEnumerable]) {
                foreach($item in $node) { $stack.Push($item) }
            }
        }
    }
}

$failures=@($results | Where-Object { -not $_.valid })
$compat=@($results | Where-Object { $_.nfkc_changed })
$version=[string]$PSVersionTable.PSVersion
$sha=$env:GITHUB_SHA
$evidence=[ordered]@{
    schema='SWE-PONDAZ-UNICODE-PREFLIGHT-v1'
    original_source_commit=$sha
    input_files=$audited
    entries_checked=$results.Count
    nfkc_differences=$compat.Count
    blocked=$failures.Count
    unicode_policy='strict UTF-8; NFKC shadow; reject zero-width U+200B..U+200D, FEFF and invalid C0 controls; preserve canonical inputs'
    normalization_does_not_mutate_source=$true
    powershell_core_version=$version
    findings=@($results | Where-Object { -not $_.valid -or $_.nfkc_changed } | Select-Object path,valid,issues,nfkc_changed,shadow_sha256)
}
$file=Join-Path $directory 'pondaz.json'
$evidence|ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $file -Encoding utf8
Write-Host ('PONDAZ|SCAN|files='+$audited+'|entries='+$results.Count+'|compat='+$compat.Count+'|blocked='+$failures.Count+'|source='+$sha)
foreach($issue in $failures | Select-Object -First 25) {
    Write-Host ('PONDAZ|BLOCK|'+$issue.path+'|'+($issue.issues -join ','))
}
if($failures.Count -gt 0) { throw ('PONDAZ rejected '+$failures.Count+' input record(s); see '+$file) }
Write-Host 'PONDAZ|PASS|Input validation completed without rewriting source'
