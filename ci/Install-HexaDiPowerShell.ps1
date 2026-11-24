# SPDX-License-Identifier: 0BSD
# Official PowerShell GitHub release bootstrap. Freeze toolchain for comparisons.
# Authoritative digest is live PowerShell/PowerShell latest release metadata.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$DestinationDirectory)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$dest=[IO.Path]::GetFullPath($DestinationDirectory)
if([IO.Directory]::Exists($dest)){throw 'HEXADI_BOOT|HOLD|output_not_fresh'}
if(-not $IsWindows){throw 'HEXADI_BOOT|HOLD|Windows_required'}
$headers=@{
 Accept='application/vnd.github+json'
 'X-GitHub-Api-Version'='2022-11-28'
 'User-Agent'='swe-hexadi-official-pwsh'
}
if(-not [string]::IsNullOrWhiteSpace($env:GH_TOKEN)){
 $headers.Authorization='Bearer '+$env:GH_TOKEN
}
$release=Invoke-RestMethod -Uri 'https://api.github.com/repos/PowerShell/PowerShell/releases/latest' -Headers $headers
$expectedVersion='7.7.0'
$expectedDigest='7c74e03b20a75b59715847439d046a6214b1863ee79b187872980f139bb4a05f'
if($release.tag_name -cne ('v'+$expectedVersion) -or $release.draft -or $release.prerelease){
 throw 'HEXADI_BOOT|HOLD|upstream_latest_version_changed_reaudit_required'
}
$matches=@($release.assets|Where-Object {$_.name -ceq ('PowerShell-'+$expectedVersion+'-win-x64.zip')})
if($matches.Count -ne 1 -or
   $matches[0].digest -cne ('sha256:'+$expectedDigest) -or
   $matches[0].browser_download_url -cne
     ('https://github.com/PowerShell/PowerShell/releases/download/v'+$expectedVersion+
      '/PowerShell-'+$expectedVersion+'-win-x64.zip')){
 throw 'HEXADI_BOOT|HOLD|official_release_digest_or_asset_mismatch'
}
[void][IO.Directory]::CreateDirectory($dest)
$zip=Join-Path $dest 'official-powershell.zip'
# Public release binary; never forward GitHub token to redirected asset hosts.
Invoke-WebRequest -Uri $matches[0].browser_download_url -OutFile $zip -MaximumRedirection 5
$actual=(Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToLowerInvariant()
if($actual -cne $expectedDigest){
 throw 'HEXADI_BOOT|HOLD|download_sha256_not_equal_to_GitHub_release'
}
$install=Join-Path $dest 'extracted'
Expand-Archive -LiteralPath $zip -DestinationPath $install
$exe=Join-Path $install 'pwsh.exe'
if(-not [IO.File]::Exists($exe)){throw 'HEXADI_BOOT|HOLD|pwsh_missing'}
[string[]]$observed=@(& $exe -NoLogo -NoProfile -NonInteractive -Command '$PSVersionTable.PSVersion.ToString()')
if($LASTEXITCODE -ne 0 -or $observed.Count -ne 1 -or $observed[0] -cne $expectedVersion){
 throw 'HEXADI_BOOT|HOLD|actual_powershell_version_mismatch'
}
Write-Host ('HEXADI_BOOT|PASS|TAG=v'+$expectedVersion+
 '|OFFICIAL_RELEASE_SHA256='+$actual+'|PROCESS_VERSION='+$observed[0]+
 '|FROZEN=YES|RELEASE=HOLD')
