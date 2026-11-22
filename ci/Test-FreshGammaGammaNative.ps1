# SPDX-License-Identifier: 0BSD
# Verify the native transcript against exact historical shell bytes without
# invoking a shell, evaluating archived code, or treating exit 42 as a failure.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$nativePath = Join-Path $root 'ci/FRESH-GammaGamma.ps1'
$archivePath = Join-Path $root 'Archive/Retired/ci/FRESH-GammaGamma.sigmapi-v8.sh.txt'
$utf8 = [Text.UTF8Encoding]::new($false,$true)

# This immutable Git object identifies v8, independently of the current registry.
$historicalBytes = [IO.File]::ReadAllBytes($archivePath)
$header = [Text.Encoding]::ASCII.GetBytes('blob ' + $historicalBytes.Length + [char]0)
$gitBytes = [byte[]]::new($header.Length + $historicalBytes.Length)
[Array]::Copy($header,0,$gitBytes,0,$header.Length)
[Array]::Copy($historicalBytes,0,$gitBytes,$header.Length,$historicalBytes.Length)
$blob = [Convert]::ToHexString([Security.Cryptography.SHA1]::HashData($gitBytes)).ToLowerInvariant()
if ($blob -cne '0a9cea633ca0b3fa2cf2b4b7a98776a7765d6289') {
    throw 'FRESH|NATIVE|HOLD|historical_v8_blob_drift'
}
$historical = $utf8.GetString($historicalBytes)
$records = [Collections.Generic.List[string]]::new()
$exited = $false
foreach ($line in ($historical -split "`n")) {
    if ($line.Length -eq 0 -or $line.StartsWith('#',[StringComparison]::Ordinal)) { continue }
    if ($exited) { throw 'FRESH|NATIVE|HOLD|historical_content_after_exit' }
    if ($line -cmatch "^echo '([^'\r\n]*)'$") { $records.Add($Matches[1]); continue }
    if ($line -ceq 'exit 42') { $exited = $true; continue }
    throw 'FRESH|NATIVE|HOLD|historical_nonliteral_statement'
}
if (-not $exited -or $records.Count -ne 482 -or
    $records[$records.Count - 1] -cne 'ItWasOnlyOnceILookedInTheirEye') {
    throw 'FRESH|NATIVE|HOLD|historical_transcript_contract'
}
$expected = ($records -join "`n") + "`n"
$expectedBytes = $utf8.GetBytes($expected)

# Only one static Console.Write of one single-quoted string and exit 42 are
# permitted. This rejects variables, external commands, expansion, callbacks,
# parameters, redirection and extra script blocks before any process is started.
$native = $utf8.GetString([IO.File]::ReadAllBytes($nativePath))
if ($native.Contains("`r")) { throw 'FRESH|NATIVE|HOLD|native_source_requires_LF' }
$tokens = $null
$parseErrors = $null
$ast = [Management.Automation.Language.Parser]::ParseInput($native,[ref]$tokens,[ref]$parseErrors)
if ($parseErrors.Count -ne 0 -or $null -ne $ast.ParamBlock -or
    $null -ne $ast.BeginBlock -or $null -ne $ast.ProcessBlock -or
    $null -ne $ast.CleanBlock -or $null -ne $ast.DynamicParamBlock -or
    $ast.UsingStatements.Count -ne 0 -or $ast.ScriptRequirements -or
    $null -eq $ast.EndBlock -or $null -ne $ast.EndBlock.Traps -or
    $ast.EndBlock.Statements.Count -ne 2) {
    throw 'FRESH|NATIVE|HOLD|native_script_shape'
}
$write = $ast.EndBlock.Statements[0]
if ($write -isnot [Management.Automation.Language.PipelineAst] -or
    $write.Background -or $write.PipelineElements.Count -ne 1 -or
    $write.PipelineElements[0] -isnot [Management.Automation.Language.CommandExpressionAst] -or
    $write.PipelineElements[0].Redirections.Count -ne 0) {
    throw 'FRESH|NATIVE|HOLD|native_output_pipeline'
}
$call = $write.PipelineElements[0].Expression
if ($call -isnot [Management.Automation.Language.InvokeMemberExpressionAst] -or
    -not $call.Static -or $call.Expression -isnot [Management.Automation.Language.TypeExpressionAst] -or
    $call.Expression.TypeName.FullName -cne 'Console' -or
    $call.Member -isnot [Management.Automation.Language.StringConstantExpressionAst] -or
    $call.Member.Value -cne 'Write' -or $call.Arguments.Count -ne 1 -or
    $call.Arguments[0] -isnot [Management.Automation.Language.StringConstantExpressionAst] -or
    $call.Arguments[0].StringConstantType -ne [Management.Automation.Language.StringConstantType]::SingleQuoted -or
    -not [StringComparer]::Ordinal.Equals($call.Arguments[0].Value,$expected)) {
    throw 'FRESH|NATIVE|HOLD|native_literal_drift'
}
$exit = $ast.EndBlock.Statements[1]
if ($exit -isnot [Management.Automation.Language.ExitStatementAst] -or
    $null -eq $exit.Pipeline -or $exit.Pipeline.PipelineElements.Count -ne 1 -or
    $exit.Pipeline.PipelineElements[0] -isnot [Management.Automation.Language.CommandExpressionAst] -or
    $exit.Pipeline.PipelineElements[0].Redirections.Count -ne 0 -or
    $exit.Pipeline.PipelineElements[0].Expression -isnot [Management.Automation.Language.ConstantExpressionAst] -or
    $exit.Pipeline.PipelineElements[0].Expression.Value -isnot [int] -or
    $exit.Pipeline.PipelineElements[0].Expression.Value -ne 42) {
    throw 'FRESH|NATIVE|HOLD|native_exit_contract'
}

# Run the verified native source with an unrelated working directory. Capture
# raw bytes so Windows CRLF conversion, BOMs, host banners or extra output fail.
$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = (Get-Process -Id $PID).Path
$start.UseShellExecute = $false
$start.CreateNoWindow = $true
$start.RedirectStandardOutput = $true
$start.RedirectStandardError = $true
$start.WorkingDirectory = [IO.Path]::GetTempPath()
foreach ($arg in @('-NoLogo','-NoProfile','-NonInteractive','-File',$nativePath)) {
    $start.ArgumentList.Add($arg)
}
$process = [Diagnostics.Process]::new()
$process.StartInfo = $start
$stdout = [IO.MemoryStream]::new()
$started = $false
try {
    $started = $process.Start()
    if (-not $started) { throw 'FRESH|NATIVE|HOLD|process_start' }
    $copy = $process.StandardOutput.BaseStream.CopyToAsync($stdout)
    $stderrRead = $process.StandardError.ReadToEndAsync()
    if (-not $process.WaitForExit(30000)) {
        $process.Kill($true)
        throw 'FRESH|NATIVE|HOLD|process_timeout'
    }
    [void]$copy.GetAwaiter().GetResult()
    $stderr = $stderrRead.GetAwaiter().GetResult()
    if ($process.ExitCode -ne 42 -or $stderr.Length -ne 0) {
        throw ('FRESH|NATIVE|HOLD|exit=' + $process.ExitCode + '|stderr_bytes=' + $utf8.GetByteCount($stderr))
    }
    $actualBytes = $stdout.ToArray()
    if ($actualBytes.Length -ne $expectedBytes.Length) { throw 'FRESH|NATIVE|HOLD|stdout_length_drift' }
    for ($i = 0; $i -lt $expectedBytes.Length; $i++) {
        if ($actualBytes[$i] -ne $expectedBytes[$i]) { throw ('FRESH|NATIVE|HOLD|stdout_byte_drift|' + $i) }
    }
} finally {
    if ($started -and -not $process.HasExited) { $process.Kill($true) }
    $process.Dispose()
    $stdout.Dispose()
}
$digest = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($expectedBytes)).ToLowerInvariant()
Write-Host ('FRESH|NATIVE|PASS|literal_records=482|stdout_bytes=' + $expectedBytes.Length +
    '|stdout_sha256=' + $digest + '|exit=42|terminal=EXACT|RELEASE=HOLD')
