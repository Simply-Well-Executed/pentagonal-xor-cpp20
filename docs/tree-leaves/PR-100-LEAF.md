# TREE DIFF leaf | PR #100

Status: **CURRENT branch witness**, recorded 2026-11-24. This leaf is derived provenance, not the algorithm, code source, a substitute for Git history, or release approval. The snapshot below predates this leaf commit and remains reachable by its immutable SHA.

## Custody: preserve instead of prune

- PR: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/100
- Branch: `experiment/oracle-jdk27-vcxproj-diagnostics-20261121`
- Observed branch HEAD (immutable source snapshot): `0ab36b948dc96d1a3645dd941066474d0aa0de07`
- Trunk `main` observed at `6823f5365bfd75d4ad61cd51e225bdf52737cf4e`
- Base branch at observation: `main`
- Scope/title: Oracle JDK27 VCXPROJ diagnostics
- PR diff metadata: 94 changed files; 10106 additions; 57 deletions (snapshot metadata, not fitness ranking).
- Immutable source copy: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/tree/0ab36b948dc96d1a3645dd941066474d0aa0de07
- Source commit audit: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/0ab36b948dc96d1a3645dd941066474d0aa0de07

## External evidence and workflow receipts

- LadderDi NUXPARLX checked run: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45444032055 (reported FAILURE; inspect per-rung rather than claiming all failed).
- BRITISHISH-FAIR/2 YANG, CORE, YIN checked run: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45444032126 (reported SUCCESS; synthetic witness, not release certification).
- Native EXCITE/EXCITED and other jobs remain independent. Their successes cannot override a failed gate.
- GitHub Actions uploaded artifact ZIPs are **not claimed copied into this repository**. The original workflow run/log links are retained as pointers; external artifacts can expire. On the inspected PR #103 LadderDi and PR-PARITY runs, no uploaded artifacts were returned. An immutable Git source snapshot is not the same thing as an uploaded Actions artifact.

## Branch topology

- Trunk = `main`; preserved roots are Git commits, not deleted files.
- PR #98 (`copilot/resolve-docker-validation-error`) is **merged** into main, historical PR: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/98.
- PR #102 (`feat/web9x-niterex-coreao-crystal-20261120`) is **merged** into main, historical PR: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/102.
- PR #95 is stacked on PR #34, which must not be flattened into a direct-main PR by assumption.
- Seven open tracked PRs at observation: #34, #95, #97, #99, #100, #101, #103.

## Decision and audit

- No deletion, forced rebase, squash, cross-branch overwrite, automatic merge, or permission change authorized.
- Historical code stays accessible through Git SHA. No branch's later revision is described as identical to this snapshot.
- Failed gates and missing evidence are retained as findings, not policies or false PASS.
- **Custody:** preserve this branch for later scoped evaluation; no automatic promotion.
- `RELEASE=HOLD`; chosen, never assumed.
