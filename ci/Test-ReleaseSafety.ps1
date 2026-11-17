# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Release pre-flight: reject active debugging traps and paused experimental code.
# PowerShell 7; no downloads, no writes, no build artifacts created.

[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$extensions = @('.cpp', '.c', '.h', '.hpp', '.ps1', '.java')

# const, constexpr, static_assert and debug-symbol flags are intentionally
# allowed: none is a runtime debugger stop or execution pause.
$forbidden = [ordered]@{
    'runtime debugger stop' = '(?i)\b(?:__debugbreak|DebugBreak|__builtin_debugtrap|__builtin_trap|__builtin_breakpoint|SIGTRAP|SIGSTOP)\b|\bint\s*3\b'
    'explicit pause'       = '(?i)\b(?:Read-Host|Pause-Process|Stop-Process)\b|\b(?:sleep|usleep)\s*\('
    'disabled source block'= '(?m)^\s*#\s*if\s+0\s*(?://.*)?$'
    'debug-only branch'    = '(?m)^\s*#\s*(?:ifdef\s+_DEBUG|if\s+defined\s*\(\s*_DEBUG\s*\))'
    'retired source marker'= '(?i)\b(?:frozen|freeze)\b'
}


# Preserve three exact Windows compile/expected-negative witnesses, NOT
# executable delays. All nonmatching calls and all other rules still fail.
$approvedCompileTimeWitness = @{
    'benchmarks/windows_ligaze_user_header_experiment.hpp' =
        '^\s*inline\s+void\s+Sleep\s*\(\s*\.\.\.\s*\)\s*\{\s*\}\s*$'
    'benchmarks/windows_ligaze_user_header_v2_experiment.hpp' =
        '^\s*inline\s+void\s+Sleep\s*\(\s*\.\.\.\s*\)\s*\{\s*\}\s*$'
    'tests/pentagonal_platform_v2_contract.cpp' =
        '^\s*concept\s+ForgedSleepCall\s*=\s*requires\s*\(\s*T\s+value\s*\)\s*\{\s*::Sleep\s*\(\s*value\s*,\s*value\s*\)\s*;\s*\}\s*;\s*$'
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
    $rel = [IO.Path]::GetRelativePath($root, $file.FullName).Replace([char]92, '/')
    foreach ($rule in $forbidden.GetEnumerator()) {
        $matches = [regex]::Matches($content, $rule.Value)
        foreach ($match in $matches) {
            $lf = [char]10
            $start = $content.LastIndexOf($lf, [Math]::Max(0,$match.Index-1)) + 1
            $end = $content.IndexOf($lf, $match.Index)
            if ($end -lt 0) { $end = $content.Length }
            $sourceLine = $content.Substring($start,$end-$start).TrimEnd([char]13)
            # Full-line exact witnesses only; no file or directory exemptions.
            if ($rule.Key -eq 'explicit pause' -and
                $approvedCompileTimeWitness.ContainsKey($rel) -and
                $sourceLine -cmatch $approvedCompileTimeWitness[$rel]) {
                continue
            }
            $line = 1 + ($content.Substring(0, $match.Index).Split($lf).Length - 1)
            $failures.Add(('{0}:{1}: {2}: {3}' -f $rel,$line,$rule.Key,$match.Value))
        }
    }
}

if ($failures.Count -gt 0) {
    foreach ($issue in $failures) { Write-Error $issue -ErrorAction Continue }
    throw ('Release pre-flight rejected {0} pause/debug artifact(s).' -f $failures.Count)
}

Write-Host ('Release pre-flight PASS: inspected {0} source/script files; no active breakpoints or paused branches.' -f $files.Count)
