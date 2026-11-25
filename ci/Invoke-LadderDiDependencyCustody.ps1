# SPDX-License-Identifier: 0BSD
# LADDERDI transport ONLY: never installs, expands, compiles or executes an archive.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][ValidateSet('HeptaDi','OctaDi','DoDecaDi','TriDecaDi')][string]$Lane,
 [Parameter(Mandatory=$true)][string]$Manifest,
 [Parameter(Mandatory=$true)][string]$DestinationRoot
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$utf8=[Text.UTF8Encoding]::new($false,$true)
$expectedHeader=('lane'+[char]9+'version'+[char]9+'url'+[char]9+'sha256'+[char]9+'license'+[char]9+'approval')
if(-not [IO.File]::Exists($Manifest)){throw 'DEPENDENCY|HOLD|manifest_missing'}
$lines=[IO.File]::ReadAllLines([IO.Path]::GetFullPath($Manifest),$utf8)
if($lines.Length -lt 2 -or $lines[0] -cne $expectedHeader){throw 'DEPENDENCY|HOLD|manifest_schema'}
$records=@{}
foreach($line in $lines[1..($lines.Length-1)]){
 $c=$line.Split([char]9)
 if($c.Length -ne 6 -or $c[0] -cnotmatch '^(HeptaDi|OctaDi|DoDecaDi|TriDecaDi)$' -or $records.ContainsKey($c[0])){
  throw 'DEPENDENCY|HOLD|manifest_duplicate_or_invalid_row'
 }
 $records.Add($c[0],$c)
}
if(-not $records.ContainsKey($Lane)){throw 'DEPENDENCY|HOLD|lane_unlisted'}
$row=$records[$Lane]
# A change to this source-controlled manifest must be reviewed separately.
# No approval is inferred from a flag, environment variable or successful network request.
if($row[5] -cne 'APPROVED'){throw ('DEPENDENCY|HOLD|license_review_required|lane='+$Lane)}
if($row[4] -cnotin @('0BSD','MIT','MPL-2.0','CDDL-1.0')){
 throw 'DEPENDENCY|HOLD|license_not_in_repo_allowlist'
}
if($row[1] -cnotmatch '^[0-9][0-9A-Za-z._-]{0,40}$' -or
   $row[3] -cnotmatch '^[0-9a-f]{64}$'){
 throw 'DEPENDENCY|HOLD|invalid_version_or_sha256_pin'
}
$uri=$null
if(-not [Uri]::TryCreate($row[2],[UriKind]::Absolute,[ref]$uri) -or
   $uri.Scheme -cne 'https' -or
   $uri.UserInfo.Length -ne 0 -or
   $uri.Port -ne 443 -or
   $uri.Host -cnotin @('www.lua.org','ocaml.org','github.com')){
 throw 'DEPENDENCY|HOLD|nonapproved_source_origin'
}
$dest=[IO.Path]::GetFullPath($DestinationRoot)
$manifestDir=[IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($Manifest))
if($dest -ceq $manifestDir -or $dest.StartsWith($manifestDir+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'DEPENDENCY|HOLD|destination_must_be_outside_manifest_tree'
}
$folder=Join-Path $dest ($Lane+'-'+$row[1])
if([IO.Directory]::Exists($folder)){throw 'DEPENDENCY|HOLD|destination_not_fresh'}
[void][IO.Directory]::CreateDirectory($folder)
$part=Join-Path $folder 'archive.part'
$final=Join-Path $folder 'archive.verified'
try {
 # Disable redirects; cross-origin redirects need separate provenance and approval.
 Invoke-WebRequest -Uri $uri.AbsoluteUri -OutFile $part -MaximumRedirection 0 -ErrorAction Stop
 if(-not [IO.File]::Exists($part)){throw 'DEPENDENCY|HOLD|download_missing'}
 $actual=(Get-FileHash -LiteralPath $part -Algorithm SHA256).Hash.ToLowerInvariant()
 if($actual -cne $row[3]){throw 'DEPENDENCY|HOLD|sha256_mismatch'}
 [IO.File]::Move($part,$final)
 $receipt=Join-Path $folder 'custody.tsv'
 $record=@('lane'+[char]9+'version'+[char]9+'sha256'+[char]9+'license'+[char]9+'source',''+$Lane+[char]9+$row[1]+[char]9+$actual+[char]9+$row[4]+[char]9+$uri.AbsoluteUri)
 [IO.File]::WriteAllLines($receipt,$record,$utf8)
 Write-Host ('DEPENDENCY|STAGED_ONLY|LANE='+$Lane+'|SHA256='+$actual+'|NO_EXECUTION|NO_INSTALL|RELEASE=HOLD')
} catch {
 if([IO.File]::Exists($part)){[IO.File]::Delete($part)}
 throw
}
