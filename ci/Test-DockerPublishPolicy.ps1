[CmdletBinding()]
param()
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
if ($IsWindows) { throw 'POSIX Docker mock negative controls require Unix host' }

$legacy = Join-Path $PSScriptRoot 'Run-DockerValidation.ps1'
$new = Join-Path $PSScriptRoot 'Publish-AndroidSourceStudy.ps1'
$work = Join-Path ([IO.Path]::GetTempPath()) ('swe-docker-negative-'+[Guid]::NewGuid().ToString('N'))
$shim = Join-Path $work 'docker'
$log = Join-Path $work 'docker-commands.log'
$vars = @('PATH','DOCKER_CALL_LOG','DOCKER_HOST','DOCKERHUB_USERNAME',
          'DOCKER_MOCK_SOURCE_DIGEST','DOCKER_MOCK_MEDIA_TYPE',
          'GITHUB_EVENT_NAME','GITHUB_REF','DOCKERHUB_TOKEN')
$original = @{}
foreach ($v in $vars) {
  $original[$v] = [Environment]::GetEnvironmentVariable($v,'Process')
}
try {
  [void][IO.Directory]::CreateDirectory($work)
  $shell = @'
#!/bin/sh
set -eu
printf '%s\n' "$*" >> "$DOCKER_CALL_LOG"
if [ "$1" = context ] && [ "$2" = inspect ]; then
  printf '%s\n' 'unix:///var/run/docker.sock'
elif [ "$1" = info ]; then
  exit 0
elif [ "$1" = buildx ] && [ "$2" = imagetools ] && [ "$3" = inspect ]; then
  case "$4" in
    *:sha-07eb2a4561f0)
      printf '{"digest":"%s","mediaType":"%s"}\n' "$DOCKER_MOCK_SOURCE_DIGEST" "$DOCKER_MOCK_MEDIA_TYPE"
      ;;
    *:latest)
      printf '%s\n' '{"digest":"sha256:0000000000000000000000000000000000000000000000000000000000000001","mediaType":"application/vnd.docker.distribution.manifest.v2+json"}'
      ;;
    *) exit 50 ;;
  esac
else
  exit 60
fi
'@
  [IO.File]::WriteAllText($shim,$shell.Replace([char]13+[string][char]10,[string][char]10),[Text.UTF8Encoding]::new($false))
  & chmod +x $shim
  if ($LASTEXITCODE -ne 0) { throw 'Could not enable mock executable' }
  [Environment]::SetEnvironmentVariable('PATH',$work+[IO.Path]::PathSeparator+$original['PATH'],'Process')
  [Environment]::SetEnvironmentVariable('DOCKER_CALL_LOG',$log,'Process')
  [Environment]::SetEnvironmentVariable('DOCKER_HOST','unix:///var/run/docker.sock','Process')
  [Environment]::SetEnvironmentVariable('DOCKERHUB_USERNAME','paulcheek','Process')
  [Environment]::SetEnvironmentVariable('GITHUB_EVENT_NAME','push','Process')
  [Environment]::SetEnvironmentVariable('GITHUB_REF','refs/heads/main','Process')
  [Environment]::SetEnvironmentVariable('DOCKERHUB_TOKEN','mock','Process')
  [Environment]::SetEnvironmentVariable(
    'DOCKER_MOCK_SOURCE_DIGEST',
    'sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30',
    'Process'
  )
  [Environment]::SetEnvironmentVariable(
    'DOCKER_MOCK_MEDIA_TYPE',
    'application/vnd.docker.distribution.manifest.v2+json',
    'Process'
  )

  $blocked = $false
  try { & $legacy -Mode Publish -EvidenceDir $work | Out-Null }
  catch { $blocked = $_.Exception.Message -match 'Legacy environment-variable-controlled Docker publication is retired' }
  if (-not $blocked) { throw 'Old publish entrypoint was not retired' }
  if (Test-Path -LiteralPath $log) { throw 'Legacy script attempted Docker before rejection' }

  & $new | Out-Null
  if ($LASTEXITCODE -and $LASTEXITCODE -ne 0) { throw 'New publisher dry-run failed unexpectedly' }
  $calls = @(Get-Content -LiteralPath $log)
  if (@($calls | Where-Object { $_ -match '^(?:push|tag|login)\s' }).Count -ne 0) {
    throw 'Dry-run invoked a mutation or Docker login'
  }
  if (@($calls | Where-Object { $_ -match '^buildx imagetools inspect' }).Count -ne 2) {
    throw 'Dry-run failed to inspect immutable and latest registry manifests'
  }

  $blocked = $false
  try {
    & $new -Publish -AcknowledgeLatestReplacement (
      'paulcheek/android-source-study-v1:latest@sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30') | Out-Null
  } catch { $blocked = $_.Exception.Message -match 'required' }
  if (-not $blocked) { throw 'Missing approval and validation evidence failed open' }
  $calls = @(Get-Content -LiteralPath $log)
  if (@($calls | Where-Object { $_ -match '^(?:push|tag|login)\s' }).Count -ne 0) {
    throw 'Publisher mutated a tag without approval evidence'
  }

  [Environment]::SetEnvironmentVariable('DOCKER_MOCK_SOURCE_DIGEST',('f' * 64).Insert(0,'sha256:'),'Process')
  $blocked = $false
  try { & $new | Out-Null }
  catch { $blocked = $_.Exception.Message -match 'operator-supplied SHA256' }
  if (-not $blocked) { throw 'Digest mismatch did not block publisher' }

  [Environment]::SetEnvironmentVariable(
    'DOCKER_MOCK_SOURCE_DIGEST',
    'sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30',
    'Process'
  )
  [Environment]::SetEnvironmentVariable('DOCKER_MOCK_MEDIA_TYPE','application/vnd.oci.image.index.v1+json','Process')
  $blocked = $false
  try { & $new | Out-Null }
  catch { $blocked = $_.Exception.Message -match 'image index' }
  if (-not $blocked) { throw 'Multi-platform index was incorrectly accepted for docker push' }
  $calls = @(Get-Content -LiteralPath $log)
  if (@($calls | Where-Object { $_ -match '^(?:push|tag|login)\s' }).Count -ne 0) {
    throw 'A negative fixture caused a Docker mutation'
  }
  Write-Output 'PASS Docker publishing negative controls: legacy blocked; dry-run, missing approval, digest mismatch and multi-platform index fail closed.'
}
finally {
  foreach ($v in $vars) {
    [Environment]::SetEnvironmentVariable($v,$original[$v],'Process')
  }
  if (Test-Path -LiteralPath $work) { Remove-Item -LiteralPath $work -Recurse -Force }
}
