# SPDX-License-Identifier: 0BSD
# A safe issue-open diagnosis; never execute issue body, title or code snippets.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$RepositoryRoot,
  [Parameter(Mandatory=$true)][string]$EvidenceDirectory
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$repo='Simply-Well-Executed/pentagonal-xor-cpp20'
if(-not $IsWindows -or $env:GITHUB_REPOSITORY -cne $repo -or
   $env:GITHUB_EVENT_NAME -cne 'issues' -or
   [string]::IsNullOrWhiteSpace($env:GH_TOKEN) -or
   -not [IO.File]::Exists($env:GITHUB_EVENT_PATH)){
  throw 'ISSUE_CIRCUIT|HOLD|runner_or_event_untrusted'
}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$out=[IO.Path]::GetFullPath($EvidenceDirectory)
& pwsh -NoLogo -NoProfile -NonInteractive -File (Join-Path $root 'ci/Assert-ExternalEvidencePath.ps1') -RepositoryRoot $root -EvidencePath $out
if($LASTEXITCODE -ne 0 -or [IO.Directory]::Exists($out)){
  throw 'ISSUE_CIRCUIT|HOLD|evidence_path_not_fresh_or_safe'
}
[void][IO.Directory]::CreateDirectory($out)
$f=Get-Item -LiteralPath $env:GITHUB_EVENT_PATH
if($f.Length -lt 10 -or $f.Length -gt 2097152){throw 'ISSUE_CIRCUIT|HOLD|event_size'}
$eventHash=(Get-FileHash -LiteralPath $f.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
$doc=[Text.Json.JsonDocument]::Parse([IO.File]::ReadAllText($f.FullName))
try {
  $ev=$doc.RootElement
  $action=$ev.GetProperty('action').GetString()
  $issue=$ev.GetProperty('issue')
  $number=$issue.GetProperty('number').GetInt32()
  $author=$issue.GetProperty('user').GetProperty('login').GetString()
  $title=$issue.GetProperty('title').GetString()
  $body=$issue.GetProperty('body').GetString()
} finally {$doc.Dispose()}
if($null -eq $body){$body=''}
if($number -lt 1 -or $action -cnotin @('opened','reopened') -or
   $title.Length -gt 1000 -or $body.Length -gt 1000000){
  throw 'ISSUE_CIRCUIT|HOLD|issue_metadata'
}
$headers=@{
  Authorization='Bearer '+$env:GH_TOKEN
  Accept='application/vnd.github+json'
  'X-GitHub-Api-Version'='2022-11-28'
}
$class='GENERAL'
$status='NEEDS_HUMAN_REVIEW'
$headSha=''
$notes=[Collections.Generic.List[string]]::new()
$match=[regex]::Match($title,'^INCEPTION-GATE PR #(?<pr>[1-9][0-9]{0,8}) (?<rule>json-depth|serialization-warning) [a-f0-9]{64}$')
if($match.Success -and $author -ceq 'github-actions[bot]'){
  $class='INCEPTION_RECEIPT'
  $prNumber=[int]::Parse($match.Groups['pr'].Value)
  $rule=$match.Groups['rule'].Value
  $headMatch=[regex]::Match($body,'(?m)^Head: `(?<sha>[a-f0-9]{40})`$')
  $coordinates=[regex]::Matches($body,'(?m)^- `(?<path>ci/[A-Za-z0-9_.-]+[.]ps1):(?<line>[1-9][0-9]*)`$')
  if(-not $headMatch.Success -or $coordinates.Count -eq 0 -or $coordinates.Count -gt 100){
    $status='MALFORMED_RECEIPT_HOLD'
  }else{
    $headSha=$headMatch.Groups['sha'].Value
    $pr=Invoke-RestMethod -Uri ('https://api.github.com/repos/'+$repo+'/pulls/'+$prNumber) -Headers $headers
    if($pr.head.sha -cne $headSha){
      $status='SUPERSEDED_REVISION'
    }else{
      $status='SOURCE_COMPLIANT'
      $paths=@($coordinates|ForEach-Object{$_.Groups['path'].Value}|Sort-Object -Unique)
      foreach($path in $paths){
        $entry=Invoke-RestMethod -Uri ('https://api.github.com/repos/'+$repo+'/contents/'+$path+'?ref='+$headSha) -Headers $headers
        if($entry.type -cne 'file' -or $entry.encoding -cne 'base64'){
          throw 'ISSUE_CIRCUIT|HOLD|source_response_invalid'
        }
        $src=[Text.UTF8Encoding]::new($false,$true).GetString([Convert]::FromBase64String($entry.content))
        $tokens=$null;$parseErrors=$null
        $ast=[Management.Automation.Language.Parser]::ParseInput($src,[ref]$tokens,[ref]$parseErrors)
        if(@($parseErrors).Count -gt 0){
          $status='SOURCE_FINDINGS_PRESENT'
          $notes.Add($path+':syntax')
          continue
        }
        $commands=@($ast.FindAll({param($node) $node -is [Management.Automation.Language.CommandAst]},$true))
        foreach($coord in $coordinates){
          if($coord.Groups['path'].Value -cne $path){continue}
          $line=[int]::Parse($coord.Groups['line'].Value)
          $found=@($commands|Where-Object{
            $_.Extent.StartLineNumber -eq $line -and
            $_.GetCommandName() -in @('ConvertTo-Json','ConvertFrom-Json')
          })
          if($found.Count -ne 1){
            $status='SOURCE_FINDINGS_PRESENT';$notes.Add($path+':'+$line+':command_mismatch')
            continue
          }
          $command=$found[0]
          if($rule -ceq 'json-depth' -and $command.Extent.Text -cnotmatch '(?i)-Depth\s+9(?:\s|[)]|$)'){
            $status='SOURCE_FINDINGS_PRESENT';$notes.Add($path+':'+$line+':depth')
          }
          if($rule -ceq 'serialization-warning' -and
             ($command.GetCommandName() -cne 'ConvertTo-Json' -or
              $command.Extent.Text -cnotmatch '(?i)-WarningAction\s+Stop(?:\s|[)]|$)')){
            $status='SOURCE_FINDINGS_PRESENT';$notes.Add($path+':'+$line+':warning')
          }
        }
      }
      $notes.Add('Checked '+$coordinates.Count+' coordinates at the recorded PR head.')
    }
  }
}else{
  $notes.Add('No allowlisted deterministic repair is defined. Review before modifying code.')
}
$receipt=[ordered]@{
  protocol='ISSUE-CIRCUIT-001'
  issue=$number
  action=$action
  workflow_sha=$env:GITHUB_SHA
  issue_event_sha256=$eventHash
  issue_reference_sha=$headSha
  classification=$class
  status=$status
  checks=@($notes.ToArray())
  source_executed=$false
  code_modified=$false
  approval='NOT_ASSUMED'
  release='HOLD'
}
$path=Join-Path $out 'issue-receipt.json'
[IO.File]::WriteAllText($path,(ConvertTo-Json -InputObject $receipt -Depth 9 -WarningAction Stop),[Text.UTF8Encoding]::new($false))
$digest=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
Add-Content -LiteralPath $env:GITHUB_STEP_SUMMARY -Value (
  'Issue #'+$number+': '+$status+' | Evidence SHA-256 '+$digest+' | RELEASE=HOLD')
# One comment describes the outcome; it never pretends that the issue is closed.
$msg='ISSUE-CIRCUIT-001 diagnosis: **'+$status+'**. Source revision: `'+$headSha+'`. Receipt SHA-256: `'+$digest+'`. No code mutation, automatic issue closure, INCEPTION acknowledgment, merge or release. A green diagnostic job means triage ran, not that the issue was fixed.'
$payload=ConvertTo-Json -InputObject @{body=$msg} -Depth 9 -WarningAction Stop
Invoke-RestMethod -Method Post -Uri ('https://api.github.com/repos/'+$repo+'/issues/'+$number+'/comments') -Headers $headers -ContentType 'application/json' -Body $payload | Out-Null
Write-Host ('ISSUE_CIRCUIT|ISSUE='+$number+'|'+$status+'|SHA256='+$digest+'|RELEASE=HOLD')
