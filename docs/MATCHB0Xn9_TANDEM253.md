# MATCHB0Xn9 TANDEM253 | second festival historical-wave model

## Why this is not another nine merge PRs

The live Git graph has 36 branches (35 besides main) and exactly nine
PRs with currently unmerged source changes: #34, #95, #97–#103. All
nine are already in review batch 001. The remaining historical inventory
contains 25 branches whose commits are already behind/contained in main,
and one diverged branch with no net tree delta. Creating PRs for them
would falsely imply there is fresh content to merge. Do not invent PRs.

## 253 synthetic waves and nine bounded workers

A second festival mode instead rotates the 26 historical refs across
**253 model-only waves of exactly nine branch witnesses each**. Nine
independent native PowerShell worker processes prepare individual wave
manifests in parallel, while ONE shared literal `singlelane.lock`
with `FileShare.None` serializes entry to the shared convergence
lane. Once the global lease is held, the existing MATCHB0Xn9 BIDI driver
runs FORWARD and REVERSE with its own exclusive inner lock. A global
lease ledger records the exact sequence 1–253; a final independent audit
rejects gaps, duplicate waves, missing receipts, moved Git refs, and
incomplete counts. The fixed nine limit is a literal for every wave.

The 253 waves are **253 scheduler/locking stress trials**, not 253
new Git branches, not 253 independent native builds, and not 253
successful 255+1 qualification campaigns. The synthetic manifest has
a `MODEL_ONLY_DO_NOT_ADMIT.tsv` name, distinct from the actual
qualified-circuit manifest. Receipt digests are recomputed output
witnesses, never static authority. Nine workers may prepare in tandem,
but they cannot occupy the single shared lane simultaneously.

## Source of truth and safety

GitHub's live branch ref and commit graph are consulted to discover
every historical branch at run start. The commit-to-file path event
history can be queried for historical witnesses; neither a PR's text
nor a hardcoded SHA is trusted as the current source. Final live
branch ref and main SHA checks reject drift. The frozen human-maintained
roster contains only branch names and PR numbers.

The real batch-001 255-model-plus-native workflow remains separate.
Its latest run was unsuccessful, including pending JSTAR provenance
questions, incomplete historical native suites, and PR #34/#95 base
SHA drift. Those failure records remain decisive for true merge
admission; synthetic successes never override them.

The tandem controller and workflow are
`ci/Invoke-Matchb0xn9Tandem253.ps1` and
`.github/workflows/MATCHB0Xn9-Tandem253.yml`.
Read-only GitHub permissions. No force-update, no branch deletion,
no PR creation, no real BIDI promotion, no GitHub merge endpoint call,
no unattended release. `RELEASE=HOLD` remains fixed.
