# MATCHB0Xn9 FESTIVAL | Batch 001 (nine or fewer)

## Provenance authority

Git commit graphs and the current ref-to-commit mapping are authoritative.
The human-maintained roster `ci/MATCHB0Xn9_FESTIVAL_CUSTODY.tsv`
contains ONLY branch name, PR ID and batch ID. It never contains
literal source SHAs, allowed release statuses or a trust flag.

At every run, the native Windows PowerShell controller queries GitHub:
- `GET /repos/Simply-Well-Executed/pentagonal-xor-cpp20/branches`
  to resolve live branch names to commit objects.
- `GET /repos/Simply-Well-Executed/pentagonal-xor-cpp20/compare/{main_commit}...{head_commit}`
  to determine Git ancestry and effective source-tree changes.
- `GET /repos/Simply-Well-Executed/pentagonal-xor-cpp20/commits?sha={head_commit}&path={file}&per_page=1`
  to locate the latest touching commit for one representative changed path
  on each PR, as an **informational historical witness**.

The `commits?path` history is not a raw-file content checksum and
the returned selection is not a complete history export. To inspect
file identity, the controller retrieves the exact Git commit's source
archive and computes independent SHA-256 digests for every included
file before and after execution. The **source itself** and its Git
commit tree, rather than a copied hash in a document or PR description,
remain the source of truth. The run rejects a branch changing after the
source ref was resolved.

## Present branch roster (audit dated 2026-11-24)

At discovery: 36 branches including `main`, 35 other branches.
The live Git comparison classified 25 as completely in `main`,
one as diverged with no net source-tree changes, and nine as branches
with active file changes. The last nine are review candidates:

- PR #34, `feat/jstar-msvc-process-provenance-20261121`
- PR #95, `feat/quaddi-ni-oracle-cpp20-20261123`
- PR #97, `audit/minza-dual-rendition-2026-11-19`
- PR #98, `copilot/resolve-docker-validation-error`
- PR #99, `evidence/jstar-oracle27-20261121`
- PR #100, `experiment/oracle-jdk27-vcxproj-diagnostics-20261121`
- PR #101, `feat/exploratory-quantum-operand-lexicon-20261119`
- PR #102, `feat/web9x-niterex-coreao-crystal-20261120`
- PR #103, `feature/native-euler-factorial-20261121`

All are initially draft and unmerged. PR #95 already targets #34,
while the other seven new PRs target `main`. Batch membership is
a review grouping, NOT Git ancestry or automatic merge order.
The 25 already-contained branches remain in Git history and do not
need an artificial empty PR; the one zero-delta branch is explicitly
retained in registry without inventing a change to merge.

## Fixed circuit protocol, no configurable safety switches

The nine-participant ceiling is enforced as a source-code literal,
not as `MAX_CIRCUITS` config, input parameter or environment variable.
The MATCHB0Xn9 engine uses a fixed, exclusive file-share lock across
each entire direction of BIDI travel, not a configurable lane count.
These invariants cannot be overridden by the PR registry or workflow
inputs. Any future modification must be independently reviewed as
a code change; source-code literals are not literally tamper-proof.

Each of the nine PR candidates receives:
1. 255 **deterministic, offline HULAO fault-model laps** with a
   per-lap ledger and recovery checks, using the same current
   festival harness. These are **NOT 255 native PR source tests**.
2. One separately invoked **native whole-circuit qualification** of
   that PR's own live-Git-pinned source, with original source
   immutability checked before and after, status evidence preserved,
   and no promotion of failed native stages.
3. A 256th witness which is only earned if the actual native suite
   succeeds. The existing HULAO `256_sanctuary` file remains explicitly
   fictional/model-only and is not counted as a native result.
4. After all nine independent qualifications and live Git ref
   rechecks agree, the fixed exclusive single-lane head/tail BIDI
   race can be attempted using their audited receipts.

The controller may continue independent diagnosis after earlier PR
failures, but any failed PR, broken Git lineage or changed main head
blocks all-real admission. A synthetic nine-circuit race is not
certification of real PRs.

## Merge and release boundary

Passing 255+1 alone does NOT mean a branch can safely bypass
required license checks, unresolved JSTAR provenance, GitHub branch
protections, integration conflicts, code review, or approval.
Every branch is reviewed separately before any merge into `main`.
This workflow has read-only permissions and **does not call**
GitHub's merge endpoint, force-update refs, delete historical branches
or deploy a release. No automatic merge, no automatic release.
`RELEASE=HOLD` stays a fixed output.

The first festival orchestration is
`.github/workflows/MATCHB0Xn9-Festival.yml`.
Its script is `ci/Invoke-Matchb0xn9Festival256.ps1`.
The roster must be explicitly updated if new branches appear or if
a different reviewed batch of at most nine participants is formed.
