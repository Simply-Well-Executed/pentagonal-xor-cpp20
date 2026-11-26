# Cross-Merge-Sort: PR #95 and PR #316

Status: EXPLORATORY / RELEASE=HOLD. This is an offline, read-only reconciliation protocol, NOT a merge authorization.

## Inputs and custody

Source repository: Simply-Well-Executed/pentagonal-xor-cpp20.

| PR | Captured head | Base branch | Recorded state |
| --- | --- | --- | --- |
| #95 | `0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f` | `feat/jstar-msvc-process-provenance-20261121` | open draft |
| #316 | `d75aff4e987492a6e098ec18b3e786aa8d9e77da` | `feat/jstar-msvc-process-provenance-20261121` | open draft |

Verified common ancestor on 2026-11-25: `2e53fddcd007c740ebb21c8c4748c131e6834842`. Git compare said divergent: #95 146 commits behind #316's head comparison, #316 237 commits ahead relative to common ancestor. These counts belong to the captured refs only. Re-query before relying on them.

### Algorithm

1. Resolve exact, immutable Git commit IDs for A and B, verify that both objects exist. Never treat PR number, display path, file name or static SHA in this document as authority for current head.
2. Compute `git merge-base A B`. If no unique ancestor, stop for review. If more than one best ancestor, do not choose one silently: HOLD.
3. Compute `git diff --name-only --no-ext-diff BASE A` and likewise for B, with Git's path quoting enabled. Sort path records ordinally and classify A-only, B-only, and shared-path. This is a deterministic *review order*, NOT a safe method of applying patches.
4. For shared paths, inspect `git diff BASE A -- path`, `git diff BASE B -- path`, rename history, and semantics. Equal final content does not establish equal provenance or complete functional parity.
5. Run a separate in-memory / temporary worktree three-way integration trial if explicitly authorized. Git's `merge-tree --write-tree` is an optional next stage: it does not move branch refs or change the working tree, but can write temporary/unreferenced objects to the Git object database. Its zero status alone is not a test pass.
6. Execute native C++20 optimized tests, CI policy gates, and all applicable parity checks. Conflict-free is necessary but never sufficient. Record exact input SHAs, commands, compiler, outcomes, dissent and provenance.
7. Only after review, choose a single integration branch and perform an explicitly approved merge or cherry-pick. Do not rebase, force-push, remove either source branch, squash away history, alter original PRs, or enable auto-merge by default.

### Observed overlap

The source PR file-list intersection included `.github/workflows/LADDERDI.yml`. It needs line-level and behavioral review. A file-list intersection says nothing about whether the lines conflict.

### What 'Subversion' means here

CROSS-MERGE-SORT is a project-defined Git reconciliation workflow with SVN/CVS-inspired vocabulary, not an SVN remote-procedure-call feature. Git, CVS, and Subversion have different version-control architectures; `THAILONG(CVS,SVN)` is project terminology rather than a claim that Git contains or inherits CVS/SVN internals.

### Invariants

- A and B are distinct until proven equivalent under a specified test; neither source's custody is overwritten.
- Sort only presentation records, not Git's ordered commit DAG.
- Merge-base and ancestry remain Git authority; ledger is derived.
- No silent conflict resolution or semantic deduplication; all unresolved overlaps HOLD.
- Source branch history retained; MOVE != EDIT; RETIRE != ERASE.
- Git porcelain and plumbing are local operations, not remote procedure calls. Fetching from a remote is separate, explicit, and disallowed in offline runs.
- Original PRs are not automatically modified, merged, or retargeted.

### Reproducible inspection (PowerShell, no network)

```powershell
$A = '0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f'
$B = 'd75aff4e987492a6e098ec18b3e786aa8d9e77da'
git cat-file -e "$A^{commit}"; if ($LASTEXITCODE -ne 0) { throw 'A unavailable locally' }
git cat-file -e "$B^{commit}"; if ($LASTEXITCODE -ne 0) { throw 'B unavailable locally' }
$Bases = @(git merge-base --all $A $B)
if ($LASTEXITCODE -ne 0 -or $Bases.Count -ne 1) { throw 'HOLD: missing/ambiguous merge base' }
$Base = $Bases[0]
$APaths = @(git -c core.quotepath=true diff --no-ext-diff --name-only $Base $A)
$BPaths = @(git -c core.quotepath=true diff --no-ext-diff --name-only $Base $B)
if ($LASTEXITCODE -ne 0) { throw 'HOLD: diff failed' }
$MapA = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$MapB = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach ($p in $APaths) { [void]$MapA.Add($p) }
foreach ($p in $BPaths) { [void]$MapB.Add($p) }
$Both = @($MapA | Where-Object { $MapB.Contains($_) } | Sort-Object -CaseSensitive)
[pscustomobject]@{ A=$A; B=$B; Base=$Base; ACount=$MapA.Count; BCount=$MapB.Count; SharedPaths=$Both; Decision='HOLD' } | ConvertTo-Json -Depth 5
```

The PowerShell example does not fetch objects, run merge-tree, or modify files. If source commit objects are missing, it fails closed. A production implementation should use NUL-delimited Git paths to correctly handle arbitrary newline-containing file names and verify both diff exit codes individually. No current merge execution, CI pass, or test qualification is claimed.
