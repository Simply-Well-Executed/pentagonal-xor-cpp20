# COREAO-365 Cross-Merge-Sort

**Status:** EXPLORATORY; release HOLD. The four angular views are names for audit lenses, not physical rotation or independent Git algorithms. `365 = 180 YIN + 180 YANG + 5 COREAO` is the project-specific coverage metaphor.

## Grounded source state (captured 2026-11-25)

- YIN: PR #95, captured `0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f`.
- YANG: PR #316, captured `d75aff4e987492a6e098ec18b3e786aa8d9e77da`.
- Shared Git merge-base: `2e53fddcd007c740ebb21c8c4748c131e6834842`.
- The two histories diverge. The common changed path `.github/workflows/LADDERDI.yml` needs substantive review: #316 adds native DYNAMO tests and requires their success in its final gate, while #95 adds its own workflow definition. Preserve the distinction.
- Head identifiers here are evidence snapshots, not authoritative living refs. Re-resolve and recapture before the next experiment. Do not claim the 146/237 divergence counts are permanent.

## Executable inspection

`ci/Invoke-CrossMergeSort.ps1` accepts two existing *local commit objects*, computes all merge bases, rejects absent or ambiguous ancestry, reads NUL-delimited changed paths, sorts using ordinal comparison, and emits a UTF-8 JSON record. Its three sets are YIN-only, YANG-only, and shared. It does not invoke remote networking, `merge`, `rebase`, `cherry-pick`, `fetch`, or ref mutation. A shared path means *review required*, not necessarily a textual conflict.

```powershell
pwsh -NoLogo -NoProfile -NonInteractive -File ci/Invoke-CrossMergeSort.ps1 -Yin 0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f -Yang d75aff4e987492a6e098ec18b3e786aa8d9e77da -HoldOnOverlap
pwsh -NoLogo -NoProfile -NonInteractive -File ci/Test-CrossMergeSort.ps1
```

Both referenced commits must already exist in the local object database. The first command intentionally reports HOLD with a nonzero exit when both changed the same path. Neither command should be mistaken for an integration or successful qualification. The self-test constructs a disposable local repository with two divergent synthetic branches and confirms exactly one shared path and one unique path per side.

## Four lenses

1. **YIN 180:** enumerate exact YIN-only paths and YIN's ancestry.
2. **YANG 180:** enumerate exact YANG-only paths and YANG's ancestry.
3. **COREAO 5:** inspect the shared paths, line-level edits, CI semantic gates, file renames and equivalent-content/different-history cases. Stop on ambiguity.
4. **Full 365:** review source custody, branch relationships, dependencies, mandatory C++20 /O2 qualifications, and retained dissent before a separate approved integration attempt.

## Safety invariants and limitations

- Git's object graph and merge-base are the authority. JSON is a derived index, not a rewrite of history.
- `diff.renames=false` intentionally makes rename-like operations visible as delete/add. This means a cross-path rename collision is NOT detected by the initial shared-path classification. Human review or a separate rename-aware phase remains mandatory before merging.
- Textual equality is neither semantic equivalence nor proof of equal history. No auto-deduplication or automatic conflict resolution.
- Arbitrary byte sequences that cannot be decoded as strict UTF-8 fail closed; filenames with tabs/newlines remain protected by NUL framing.
- No network or remote procedure calls. Subversion/CVS are conceptual analogies only. Git does not embed SVN/CVS as a THAILONG implementation.
- The tool emits a HOLD even with zero overlap because it does **not** execute a three-way merge, tests or policy checks. `-HoldOnOverlap` upgrades overlap to a nonzero exit status.
- PR #319 was observed closed and its branch unavailable during the feature's development. It is not silently reopened or recreated. This branch is a distinct implementation branch.
- Test code is committed, not claimed to have run. Evidence of a PASS requires execution and a recorded result.
