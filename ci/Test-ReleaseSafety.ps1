# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Release pre-flight: reject active debugging traps and paused experimental code.
# PowerShell 7; no downloads, no writes, no build artifacts created.

[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$extensions = @('.cpp', '.c', '.h', '.hpp', '.ps1')

# const, constexpr, static_assert and debug-symbol flags are intentionally
# allowed: none is a runtime debugger stop or execution pause.
$forbidden = [ordered]@{
    'runtime debugger stop' = '(?i)\b(?:__debugbreak|DebugBreak|__builtin_debugtrap|__builtin_trap|__builtin_breakpoint|SIGTRAP|SIGSTOP)\b|\bint\s*3\b'
    'explicit pause'       = '(?i)\b(?:Read-Host|Pause-Process|Stop-Process)\b|\b(?:sleep|usleep)\s*\('
    'disabled source block'= '(?m)^\s*#\s*if\s+0\s*(?://.*)?$'
    'debug-only branch'    = '(?m)^\s*#\s*(?:ifdef\s+_DEBUG|if\s+defined\s*\(\s*_DEBUG\s*\))'
    'retired source marker'= '(?i)\b(?:frozen|freeze)\b'
}

$failures = [System.Collections.Generic.List[string]]::new()
$files = @(Get-ChildItem -LiteralPath $root -File -Recurse |
    Where-Object {
        $_.Extension -in $extensions -and
        $_.FullName -ne $PSCommandPath -and
        $_.FullName -notmatch '[/\\](?:\.git|build|build-shared|stage|dist|evidence)[/\\]'
    })

foreach ($file in $files) {
    $content = Get-Content -LiteralPath $file.FullName -Raw
    if ($null -eq $content) { continue }
    $rel = [IO.Path]::GetRelativePath($root, $file.FullName)
    foreach ($rule in $forbidden.GetEnumerator()) {
        $matches = [regex]::Matches($content, $rule.Value)
        foreach ($match in $matches) {
            $line = 1 + ($content.Substring(0, $match.Index).Split("`n").Length - 1)
            $failures.Add(('{0}:{1}: {2}: {3}' -f $rel,$line,$rule.Key,$match.Value))
        }
    }
}

if ($failures.Count -gt 0) {
    foreach ($issue in $failures) { Write-Error $issue -ErrorAction Continue }
    throw ('Release pre-flight rejected {0} pause/debug artifact(s).' -f $failures.Count)
}

Write-Host ('Release pre-flight PASS: inspected {0} source/script files; no active breakpoints or paused branches.' -f $files.Count)
