# SPDX-License-Identifier: 0BSD
# JSTAR: observed Windows MSVC process provenance, never a malware verdict.
# Evidence is SHA-256 pinned. No source mutation or release authorization.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$RepositoryRoot,
    [Parameter(Mandatory=$true)][string]$BuildDirectory,
    [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
    [Parameter(Mandatory=$true)][ValidatePattern('^[a-zA-Z0-9_-]+$')]
    [string]$Operation,
    [string[]]$Targets=@()
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(-not $IsWindows){throw 'JSTAR|HOLD|Windows_MSVC_required'}
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$build=[IO.Path]::GetFullPath($BuildDirectory)
$evidence=[IO.Path]::GetFullPath($EvidenceDirectory)
if(-not [IO.File]::Exists((Join-Path $root 'CMakeLists.txt')) -or
   -not [IO.Directory]::Exists($build) -or
   $evidence.StartsWith($root+[IO.Path]::DirectorySeparatorChar,
                        [StringComparison]::OrdinalIgnoreCase)){
    throw 'JSTAR|HOLD|invalid_source_build_or_evidence_boundary'
}
[void][IO.Directory]::CreateDirectory($evidence)
# Never concatenate multiple Get-Command matches into a fake executable path.
# The GitHub Windows runner has both official CMake and Strawberry's shim.
# Prefer the canonical installed CMake binary; HOLD if it is absent.
$exe=[IO.Path]::GetFullPath((Join-Path $env:ProgramFiles 'CMake/bin/cmake.exe'))
if(-not [IO.File]::Exists($exe)){
    throw 'JSTAR|HOLD|canonical_ProgramFiles_CMake_missing'
}
$stdoutPath=Join-Path $evidence ($Operation+'-stdout.log')
$stderrPath=Join-Path $evidence ($Operation+'-stderr.log')
$auditPath=Join-Path $evidence ($Operation+'-process-audit.json')
if([IO.File]::Exists($stdoutPath) -or
   [IO.File]::Exists($stderrPath) -or
   [IO.File]::Exists($auditPath)){
    throw 'JSTAR|HOLD|evidence_must_be_new'
}
$argv=[Collections.Generic.List[string]]::new()
foreach($part in @('--build',$build,'--config','Release','--parallel','1')){
    $argv.Add([string]$part)
}
if($Targets.Count -gt 0){
    $argv.Add('--target')
    foreach($target in $Targets){
        if($target -cnotmatch '^[a-zA-Z0-9_-]+$'){throw 'JSTAR|HOLD|unsafe_target_name'}
        $argv.Add($target)
    }
}
foreach($part in @('--','/m:1','/nr:false')){$argv.Add($part)}
# Disallow MSBuild node reuse in both the command arguments and inherited environment.
$env:MSBUILDDISABLENODEREUSE='1'
$env:DOTNET_CLI_DO_NOT_USE_MSBUILD_SERVER='1'
$watch=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($n in @('cmake.exe','msbuild.exe','cl.exe','link.exe','cvtres.exe',
                'rc.exe','mt.exe','conhost.exe','vctip.exe','cmd.exe','mspdbsrv.exe','Tracker.exe')){
    [void]$watch.Add($n)
}
$critical=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($n in @('cmake.exe','msbuild.exe','cl.exe','link.exe','Tracker.exe')){
    [void]$critical.Add($n)
}
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$observed=[Collections.Generic.List[object]]::new()
$errors=[Collections.Generic.List[string]]::new()
$rootPid=-1
$rootBirthUtc=[DateTimeOffset]::MinValue
$exitCode=-999
$startedUtc=[DateTimeOffset]::UtcNow.ToString('o')
$psi=[Diagnostics.ProcessStartInfo]::new()
$psi.FileName=$exe
$psi.WorkingDirectory=$root
$psi.UseShellExecute=$false
$psi.CreateNoWindow=$true
$psi.RedirectStandardOutput=$true
$psi.RedirectStandardError=$true
foreach($arg in $argv){[void]$psi.ArgumentList.Add($arg)}
$proc=[Diagnostics.Process]::new()
$proc.StartInfo=$psi
$stdOutStream=[IO.File]::Open($stdoutPath,[IO.FileMode]::CreateNew,
                             [IO.FileAccess]::Write,[IO.FileShare]::Read)
$stdErrStream=[IO.File]::Open($stderrPath,[IO.FileMode]::CreateNew,
                             [IO.FileAccess]::Write,[IO.FileShare]::Read)
try {
    if(-not $proc.Start()){throw 'JSTAR|HOLD|cmake_process_not_started'}
    $rootPid=$proc.Id
    $rootBirthUtc=([DateTimeOffset]$proc.StartTime).ToUniversalTime()
    $stdoutTask=$proc.StandardOutput.BaseStream.CopyToAsync($stdOutStream)
    $stderrTask=$proc.StandardError.BaseStream.CopyToAsync($stdErrStream)
    do {
        # Enumerate system PIDs for relationship tests but RECORD only
        # sampled build descendants and selected named tools.
        $snapshot=@(Get-CimInstance -ClassName Win32_Process -ErrorAction Stop)
        $current=@{}
        foreach($p in $snapshot){$current[[int]$p.ProcessId]=$p}
        # Build ancestry from *this snapshot only*. A stale PID retained
        # across samples can otherwise turn unrelated services into descendants.
        $active=[Collections.Generic.HashSet[int]]::new()
        if($current.ContainsKey($rootPid) -and
           $null -ne $current[$rootPid].CreationDate){
            $rootObserved=([DateTimeOffset]$current[$rootPid].CreationDate).ToUniversalTime()
            if([Math]::Abs(($rootObserved-$rootBirthUtc).TotalSeconds) -le 5){
                [void]$active.Add($rootPid)
            }
        }
        $expanded=$true
        while($expanded){
            $expanded=$false
            foreach($p in $snapshot){
                $parentId=[int]$p.ParentProcessId
                if(-not $active.Contains($parentId) -or
                   -not $current.ContainsKey($parentId) -or
                   $null -eq $current[$parentId].CreationDate -or
                   $null -eq $p.CreationDate){continue}
                $parentBirth=([DateTimeOffset]$current[$parentId].CreationDate).ToUniversalTime()
                $childBirth=([DateTimeOffset]$p.CreationDate).ToUniversalTime()
                if($childBirth -ge $parentBirth -and $active.Add([int]$p.ProcessId)){
                    $expanded=$true
                }
            }
        }
        foreach($p in $snapshot){
            $pid=[int]$p.ProcessId
            if(-not $watch.Contains([string]$p.Name) -and
               -not $active.Contains($pid)){continue}
            $birth=if($null -ne $p.CreationDate){
                ([DateTimeOffset]$p.CreationDate).ToString('o')
            }else{'UNAVAILABLE'}
            $key=([string]$pid)+'|'+$birth+'|'+[string]$p.Name
            if(-not $seen.Add($key)){continue}
            if($seen.Count -gt 4096){throw 'JSTAR|HOLD|process_audit_record_limit'}
            $scope=if($pid -eq $rootPid -and $active.Contains($pid)){
                'build_root'
            }elseif($active.Contains($pid)){
                'build_descendant'
            }else{'host_sample'}
            $observedPath=[string]$p.ExecutablePath
            if($scope -ceq 'build_root' -and
               [string]::IsNullOrWhiteSpace($observedPath)){
                # Exact binary was started via ProcessStartInfo by this script.
                $observedPath=$exe
            }
            $observed.Add([pscustomobject]@{
                pid=$pid
                parent_pid=[int]$p.ParentProcessId
                name=[string]$p.Name
                created_utc=$birth
                executable_path=$observedPath
                command_line=[string]$p.CommandLine
                scope=$scope
            })
        }
    }while(-not $proc.WaitForExit(250))
    $proc.WaitForExit()
    $stdoutTask.GetAwaiter().GetResult()
    $stderrTask.GetAwaiter().GetResult()
    $exitCode=[int]$proc.ExitCode
} catch {
    $errors.Add(('capture_failed|'+$_.Exception.GetType().Name+'|'+$_.Exception.Message))
    if(-not $proc.HasExited){
        try{$proc.Kill($true)}catch{}
    }
} finally {
    $stdOutStream.Dispose()
    $stdErrStream.Dispose()
    $proc.Dispose()
}
# Hash every observed executable; inspect Authenticode separately from
# process names. Missing information is not proof of malicious behavior.
$identities=[Collections.Generic.List[object]]::new()
$paths=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
[void]$paths.Add($exe)
foreach($p in $observed){
    if(-not [string]::IsNullOrWhiteSpace($p.executable_path)){
        [void]$paths.Add($p.executable_path)
    } elseif($critical.Contains($p.name) -or $p.scope -ceq 'build_descendant'){ 
        $errors.Add(('unknown_critical_executable_path|'+$p.name+'|'+$p.pid))
    }
}
foreach($path in $paths){
    $name=[IO.Path]::GetFileName($path)
    $status='UNAVAILABLE'
    $subject=''
    $digest=''
    try {
        if(-not [IO.File]::Exists($path)){throw 'path_missing'}
        $digest=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
        $signature=Get-AuthenticodeSignature -LiteralPath $path
        $status=[string]$signature.Status
        if($null -ne $signature.SignerCertificate){
            $subject=[string]$signature.SignerCertificate.Subject
        }
        if($critical.Contains($name) -and
           ($status -cne 'Valid' -or $subject -notmatch '(?i)(Microsoft|Kitware)')){
            $errors.Add(('unapproved_critical_binary|'+$name+'|'+$status))
        }
    }catch{
        if($critical.Contains($name)){
            $errors.Add(('critical_identity_probe_failed|'+$name+'|'+$_.Exception.Message))
        }
    }
    $identities.Add([pscustomobject]@{
        path=$path
        name=$name
        sha256=$digest
        authenticode_status=$status
        signer_subject=$subject
    })
}
foreach($p in $observed){
    if($p.scope -ceq 'build_descendant' -and -not $watch.Contains($p.name)){
        $errors.Add(('unreviewed_build_descendant|'+$p.name+'|'+$p.pid))
    }
}
if($exitCode -eq -999){$errors.Add('build_exit_not_observed')}
if($observed.Count -eq 0){$errors.Add('process_sampling_empty')}
$report=[ordered]@{
    schema='SWE-JSTAR-PROCESS-PROVENANCE-v1'
    operation=$Operation
    source_commit=[string]$env:GITHUB_SHA
    root_cmake_pid=$rootPid
    started_utc=$startedUtc
    completed_utc=[DateTimeOffset]::UtcNow.ToString('o')
    executable=$exe
    command_arguments=@($argv.ToArray())
    msbuild_node_reuse='disabled_env_and_/nr:false'
    msbuild_max_workers=1
    sample_interval_ms=250
    scope='same_snapshot_parent_child_birth_checked_descendants_and_named_host_tools; not a complete kernel event trace'
    process_count=$observed.Count
    observed_processes=@($observed.ToArray())
    executable_identities=@($identities.ToArray())
    stdout_sha256=(Get-FileHash -LiteralPath $stdoutPath -Algorithm SHA256).Hash.ToLowerInvariant()
    stderr_sha256=(Get-FileHash -LiteralPath $stderrPath -Algorithm SHA256).Hash.ToLowerInvariant()
    process_integrity_errors=@($errors.ToArray())
    exit_code=$exitCode
    process_integrity_status=if($errors.Count -eq 0){'SAMPLED_PASS'}else{'HOLD'}
    release='HOLD'
}
[IO.File]::WriteAllText($auditPath,($report|ConvertTo-Json -Depth 12),
    [Text.UTF8Encoding]::new($false))
$auditHash=(Get-FileHash -LiteralPath $auditPath -Algorithm SHA256).Hash.ToLowerInvariant()
Write-Host ('JSTAR|PROCESS_AUDIT|OP='+$Operation+'|SAMPLED='+$observed.Count+
            '|EXIT='+$exitCode+'|INTEGRITY='+$report.process_integrity_status+
            '|SHA256='+$auditHash+'|RELEASE=HOLD')
# GitHub Actions logs retain the full JSON and two channel hashes even when
# the build fails; no Node-based artifact action and no repository mutation.
Write-Host 'JSTAR|PROCESS_AUDIT_JSON_BEGIN'
Write-Host ([IO.File]::ReadAllText($auditPath))
Write-Host 'JSTAR|PROCESS_AUDIT_JSON_END'
if($errors.Count -gt 0){
    throw ('JSTAR|HOLD|process_integrity|'+($errors -join ';'))
}
if($exitCode -ne 0){
    Write-Host 'JSTAR|BUILD_STDERR_TAIL_BEGIN'
    Get-Content -LiteralPath $stderrPath -Tail 80 | ForEach-Object { Write-Host $_ }
    Write-Host 'JSTAR|BUILD_STDERR_TAIL_END'
    Write-Host 'JSTAR|BUILD_STDOUT_TAIL_BEGIN'
    Get-Content -LiteralPath $stdoutPath -Tail 80 | ForEach-Object { Write-Host $_ }
    Write-Host 'JSTAR|BUILD_STDOUT_TAIL_END'
    throw ('JSTAR|HOLD|native_build_failed|'+$Operation+'|exit='+$exitCode)
}
Write-Host ('JSTAR|BUILD_PASS|'+$Operation+'|RELEASE=HOLD')
