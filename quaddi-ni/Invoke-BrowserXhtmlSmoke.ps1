# SPDX-License-Identifier: 0BSD
# Headless browser smoke: actual scriptless Oracle-derived XHTML in Edge.
# Browser consumes the output, never hosts or executes the JDK.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$Report)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$report=[IO.Path]::GetFullPath($Report)
if(-not [IO.File]::Exists($report)){throw 'QUADDI|BROWSER=HOLD|report_not_found'}
$paths=@(
 (Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft/Edge/Application/msedge.exe'),
 (Join-Path ([Environment]::GetFolderPath('ProgramFiles')) 'Microsoft/Edge/Application/msedge.exe')
)
$browsers=@($paths|Where-Object{[IO.File]::Exists($_)})
if($browsers.Count -ne 1){throw 'QUADDI|BROWSER=HOLD|Edge_browser_not_unambiguous'}
$edge=$browsers[0]
$profile=Join-Path ([IO.Path]::GetDirectoryName($report)) 'browser-smoke-userdata'
if([IO.Directory]::Exists($profile)){throw 'QUADDI|BROWSER=HOLD|existing_profile'}
[IO.Directory]::CreateDirectory($profile)|Out-Null
$uri=[Uri]::new($report).AbsoluteUri
$dom=@(& $edge '--headless=new' '--disable-gpu' '--disable-background-networking' '--disable-extensions' '--disable-sync' '--no-first-run' '--no-default-browser-check' ('--user-data-dir='+$profile) '--dump-dom' $uri 2>$null)
if($LASTEXITCODE -ne 0){throw ('QUADDI|BROWSER=HOLD|Edge_exit='+$LASTEXITCODE)}
$view=[string]::Join([Environment]::NewLine,[string[]]$dom)
foreach($needle in @(
 'SLIDINGTOOLONGA',
 'RULER REGISTER COUNT: #1',
 'PAGE COUNT: 10',
 'POWER: 647',
 'Children',
 'Conventional_e',
 'Your_Jig',
 '(0,0)',
 '<svg'
)){
 if(-not $view.Contains($needle,[StringComparison]::Ordinal)){
  throw ('QUADDI|BROWSER=HOLD|missing_rendered_field|'+$needle)
 }
}
Write-Host 'QUADDI|BROWSER=PASS|EDGE_HEADLESS=PASS|ORACLE_XHTML=PASS|POWER=647|PAGES=10'
