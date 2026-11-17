# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Isolated fail-closed release-policy tests. Never changes tracked sources.
[CmdletBinding()]
param()
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$site=Join-Path ([IO.Path]::GetTempPath()) ('swe-release-gate-'+[guid]::NewGuid().ToString('N'))
$ci=Join-Path $site 'ci'; $bench=Join-Path $site 'benchmarks'; $tests=Join-Path $site 'tests'
foreach($p in @($ci,$bench,$tests)){[void][IO.Directory]::CreateDirectory($p)}
$probe=Join-Path $ci 'Test-ReleaseSafety.ps1'
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'Test-ReleaseSafety.ps1') -Destination $probe
$token='Sl'+'eep'
function Set-Fixture([string]$path,[string]$value) {
    [IO.File]::WriteAllText($path,$value,[Text.UTF8Encoding]::new($false))
}
function Check([bool]$allowed,[string]$label) {
    $output=Join-Path $site 'scan.log'
    & pwsh -NoLogo -NoProfile -File $probe *> $output
    $code=$LASTEXITCODE
    if (($code -eq 0) -ne $allowed) {
        throw ('Gate contract {0}: expected allow={1}, actual exit={2}: {3}' -f
            $label,$allowed,$code,(Get-Content -LiteralPath $output -Raw))
    }
    Write-Host ('PASS {0}: {1}' -f $label,$(if($allowed){'allowed'}else{'rejected'}))
}
try {
    foreach($name in @('windows_ligaze_user_header_experiment.hpp',
        'windows_ligaze_user_header_v2_experiment.hpp')) {
        Set-Fixture (Join-Path $bench $name) ('inline void '+$token+'(...) {}')
    }
    $contract=Join-Path $tests 'pentagonal_platform_v2_contract.cpp'
    Set-Fixture $contract ('template<typename T>'+[Environment]::NewLine+'concept ForgedSleepCall = requires(T value) { ::'+$token+'(value,value); };')
    Check $true 'exact negative/compile-time witnesses'

    $active=Join-Path $tests 'active.cpp'
    Set-Fixture $active ('int main() { '+$token+'(1); }')
    Check $false 'live native pause'
    Remove-Item -LiteralPath $active -Force

    $header=Join-Path $bench 'windows_ligaze_user_header_experiment.hpp'
    Add-Content -LiteralPath $header -Value ('inline void '+$token+'(int n) { (void)n; }')
    Check $false 'modified experimental declaration'
    Set-Fixture $header ('inline void '+$token+'(...) {}')

    Add-Content -LiteralPath $contract -Value ('void active() { ::'+$token+'(1); }')
    Check $false 'live call beside compile-time contract'
    Set-Fixture $contract ('template<typename T>'+[Environment]::NewLine+'concept ForgedSleepCall = requires(T value) { ::'+$token+'(value,value); };')

    Set-Fixture $active ('int main() { Debug'+'Break(); }')
    Check $false 'debugger trap'
    Remove-Item -LiteralPath $active -Force

    Set-Fixture $active ('#'+'if 0'+[Environment]::NewLine+'int main() { return 1; }'+[Environment]::NewLine+'#endif')
    Check $false 'disabled code'
    Remove-Item -LiteralPath $active -Force
    Check $true 'clean fixtures restored'
    Write-Host 'PASS: all intentional witnesses retained; active hazards rejected.'
} finally {
    Remove-Item -LiteralPath $site -Recurse -Force -ErrorAction SilentlyContinue
}
