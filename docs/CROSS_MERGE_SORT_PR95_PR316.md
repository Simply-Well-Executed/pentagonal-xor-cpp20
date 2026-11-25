# CROSS-MERGE-SORT | PR #95 ↔ PR #316

Status: **EXPLORATORY / HOLD**, not permission to merge either PR.

## Ownership and terminology

- **THAILONG = hides** (user-defined name).
- **THAILONGA = proud to be open** (user-defined name).
- The adapter therefore uses THAILONGA's **open provenance**: all inputs, changed paths, original Git object IDs, merge-base, proposed tree, and Git's raw conflict diagnostics are disclosed.
- **Cross-merge-sort** is the user-defined feature name, not a built-in Git, CVS, or SVN command.
- Git, CVS and Subversion (SVN) are distinct version-control systems. This adapter uses Git's merge-base, diff, object IDs and merge-tree, without claiming one is an implementation of the others.

## Exactly observed source heads

- PR #95 QUADDI-NI: `0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f` (`feat/quaddi-ni-oracle-cpp20-20261123`)
- PR #316 APPLICATION OF Y=MX+B: `d75aff4e987492a6e098ec18b3e786aa8d9e77da` (`feat/application-of-ymxb-diaery-minting-20261124`)
- Common ancestor from GitHub's native compare: `2e53fddcd007c740ebb21c8c4748c131e6834842`.
- At audit time, the branches **diverged**: 146 commits ahead and 237 behind from PR #316 toward PR #95. The reverse comparison reports 237 ahead and 146 behind. These are directional history metrics, not file conflict counts.
- PR #95 holds `quaddi-ni/Invoke-NativeQualification.ps1`. The same path was **absent** on PR #316's head. EXCITE on #316 rejected this as `REPO_CIRCUIT|HOLD|missing_lap|quaddi-ni/Invoke-NativeQualification.ps1`.

**Do not infer** that copying only the missing script is sufficient: its C++ sources, TSV fixtures, other PowerShell functions, and source paths must stay dependency-consistent. PR #95's actual `quaddi-ni/` files must be considered as a unit during reconciliation.

## Local execution, no network or checkout mutations

Prepare a **local** Git worktree where both exact commit objects and their reachable trees are already present. Do not use a shallow clone missing either history, or silently pull from remote. Example:

```powershell
$repo = 'C:\\path\\to\\pentagonal-xor-cpp20'
$out = Join-Path $env:TEMP 'cross-merge-sort-pr95-pr316-review'
pwsh -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File (Join-Path $repo 'tools/Invoke-CrossMergeSort.ps1') `
  -RepositoryRoot $repo `
  -Pr95Commit 0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f `
  -Pr316Commit d75aff4e987492a6e098ec18b3e786aa8d9e77da `
  -EvidenceDirectory $out
```

`$out` must not already exist and must be outside the repository. The command writes three files: `cross-merge-sort.json`, `cross-merge-sort.tsv`, and `merge-tree-evidence.txt`.

**Outcomes**: exit 0 means Git produced a conflict-free *preview tree*; exit 2 means Git reported a conflict, with the tree and diagnostics preserved. Infrastructure, missing history, invalid inputs, and ambiguous merge bases fail closed with an error. A clean preview does not qualify Oracle/C++ parity, make a merge commit, update branches, infer a release, or prove all tests.

In the sorted ledger:
- `PR95_ONLY` and `PR316_ONLY` indicate a changed path on one side since the common ancestor.
- `BOTH_IDENTICAL` means both changed the same path to identical Git object IDs.
- `BOTH_DIFFERENT` means both changed the path but its object IDs differ. It is a **review candidate**, not necessarily a merge conflict. Git's `merge-tree` result is authoritative.
- `ABSENT` is explicitly different from empty content.

The preview tree object is created in the local Git object database; it does **not** touch the working tree or update a Git ref. Reject or resolve conflicts explicitly on a separately reviewed integration branch, and run the existing QUADDI, EXCITE, DIAERY, PR-PARITY and REDNECKENGINEERED checks there. Do not turn HOLD into PASS through filename sorting or process exits.

## Offline regression test

```powershell
pwsh -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File (Join-Path $repo 'tests/Test-CrossMergeSort.ps1')
```

The test creates two **disposable** local Git histories: one with independent changes and identical overlap, one with a genuine same-line conflict. It asserts ordinal path sorting, preserved source pointers, two-parent merge-tree behavior, fail-closed conflict reporting, and unchanged refs. No Python, Node, REST, GitHub token, or network required.

## Custody

Source records, commit ancestry, Git object IDs, and raw merge diagnostics outrank a generated report. Move does not mean edit; archive does not erase; historical failures remain evidence. The resulting review is **THAILONGA** because everything needed to evaluate the proposed merge is openly accounted for.
