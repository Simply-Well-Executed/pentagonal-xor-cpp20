# ARIRANG: Three COREAO Gauges, then AIRGAP

Status: **EXPLORATORY**. Both source PRs remain authoritative as independent Git histories. `CLEAR` is only a narrowly defined, independently verified *ancestry-in-main* snapshot; it never means test qualification, source equivalence, no hidden semantic conflicts, or release authorization.

## Frozen inputs captured 2026-11-25

| Source | PR | Captured head | State when inspected |
| --- | --- | --- | --- |
| YIN | #95 | `0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f` | open, unmerged |
| YANG | #316 | `d75aff4e987492a6e098ec18b3e786aa8d9e77da` | open, unmerged |

Common ancestor: `2e53fddcd007c740ebb21c8c4748c131e6834842`. Both heads were **not** ancestors of `main` at inspection. PR #322 was merged into the separate feature base branch, not `main`.

## Gauges and evidence

1. **COREAO provenance gauge:** refuse invalid commit IDs, identical source heads, or ambiguous merge bases. Resolve target via its actual Git ref rather than a hardcoded main SHA.
2. **Distrust YIN:** independently recompute YIN's delta against the common ancestor using Git's NUL-delimited filenames. Demand that YIN's captured head is a real ancestor of the target commit, regardless of PR prose or claims.
3. **Distrust YANG:** recompute YANG's delta and ancestry independently. No YIN result can substitute for evidence about YANG.
4. **ARIRANG AIRGAP:** require both `git merge-base --is-ancestor` checks to succeed. Report shared changed paths, each source-head-to-target residual, and the exact target commit. Fail closed when either head is absent.

Only after the AIRGAP passes does `CLEAR_ANCESTRY_ONLY` become available. This is the residual snapshot's precise status label, **not** a generic `CLEAR` claim. The script always reports `release=HOLD`, `semantics_qualified=false`, `tests_qualified=false`.

## Offline invocation

```powershell
pwsh -NoLogo -NoProfile -NonInteractive -File ci/Invoke-ArirangCoreaoClear.ps1 -RepositoryRoot . -Target refs/heads/main -Yin 0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f -Yang d75aff4e987492a6e098ec18b3e786aa8d9e77da -SnapshotPath ./ARIRANG_CLEAR_ANCESTRY.json
```

The snapshot path must be inside an already existing directory. It is written via `FileMode.CreateNew`: an existing file is not silently overwritten. The script writes no file when either source is not an ancestor. The path is only an example; do not commit a generated snapshot before checking the captured main SHA and source-branch status. No remote fetch is performed; local Git objects and the local `refs/heads/main` must be current and verified independently.

The snapshot's `yin_to_target_residual` and `yang_to_target_residual` are **separate path-diff lists**, not an assertion that those remaining changes were introduced by the other PR. Existing main changes and later commits may contribute. NUL parsing and ordinal sorting preserve unusual valid UTF-8 filenames. Strict UTF-8 fails closed on non-decodable path byte sequences. Rename detection is disabled and a separate rename-aware semantic review is required.

## Offline test

```powershell
pwsh -NoLogo -NoProfile -NonInteractive -File ci/Test-ArirangCoreaoClear.ps1
```

Creates disposable local branches: (1) no merges = HOLD; (2) only YIN merged = HOLD; (3) both Git histories merged = CLEAR_ANCESTRY_ONLY; (4) snapshot create-new = success; (5) overwrite attempt = failure. Execution has **not yet been verified** as of this specification.

## Explicit limits and next review

- Squash merges, cherry-picks and rebases can produce equivalent trees without retaining the exact head as an ancestor. In those cases the gauge stays HOLD: equivalence would require independent patch/semantic provenance, not auto-assumption.
- Both histories existing in main does not prove overlapping changes were preserved. The `.github/workflows/LADDERDI.yml` DYNAMO gate deserves line-level review and native /O2 test evidence. Other rename and policy conflicts remain unqualified.
- YIN or YANG being called 'lying' is an adversarial verification hypothesis, not a factual accusation.
- No changes to DOGMA, source PRs, main branch, CI or historical objects are made by this feature.
- Snapshot is **not automatically scheduled or triggered**. Run after both source histories reach the locally verified main ref.
