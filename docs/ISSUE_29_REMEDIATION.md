# Issue #29 remediation and validation ledger

**CURRENT | November 20, 2026 | OPEN / RELEASE HOLD.** The dedicated branch `29-exc-gnu-linux-001-prefer-mit-oracle-lgpl-21-forbid-lgpl-3-gpl-eupl-epl-release-hold` has a historical name that does not convey current approvals.

## Rules
- `0BSD` is the **only** approved BSD-family license. All BSD-2/3 and other BSD variants are forbidden.
- Separately permitted exact identifiers: `MIT`, `MPL-2.0`, `CDDL-1.0`. No other version or similarly named license is inferred.
- No GNU components, including LGPL-2.1 or GNU build-only dependencies, and no authored POSIX shell execution are approved. POSIX is an interface standard, not a license.
- Oracle Java approval is vendor/product-specific; verify applicable terms and third-party notices. Source references alone are evidence, not component authorization.

## Containment changes in this branch
1. Six historical GNU/POSIX-dependent jobs across four legacy GitHub workflow files have explicit false job guards. The original code is retained and remains in Git history; these **branch-version** jobs are skipped, not certified compliant.
2. The active no-Python static guard now runs via native Windows PowerShell and retrieves the event SHA archive using GitHub's API. It scans source paths and references without executing authored POSIX code, Python, or Node.
3. The PowerShell legal-reference custody workflow runs on main, pull requests and this branch, verifies legacy jobs remain blocked, checks license HEAD/TAIL, and reports HOLD for remaining `.sh` and known GNU build inputs.
4. The current license/security ledger was corrected without erasing its explicitly archival earlier policy snapshots.

## Remaining release blockers
- The remaining project-authored `.sh` source, Dockerfile toolchains, `/bin/sh` calls, inherited OS/GNU dependencies and corresponding other-branch workflows need independent remediation and verification.
- `LICENSES/COMPONENT_USAGE.tsv` is still unpopulated. No complete component-specific license inventory, SPDX AND/OR grant evidence, SBOM or immutable artifact attestation has been demonstrated.
- Windows runner, compiler, PowerShell, .NET and Oracle Java dependencies must be reviewed separately; Windows source scans cannot prove third-party binary compliance.
- Native C++20 `-O2` tests, relevant Java checks, supply-chain review, security scans and explicit release approval remain required.

**Status: Issue OPEN. Do not merge, publish to Docker Hub/GHCR, approve license exceptions or claim release PASS on these branch edits alone.**

## FRESH GammaGamma legacy-shell retirement (branch-only)

All **17 legacy shell programs** were first copied byte-for-byte into Archive/Retired/ci/*.sh.txt and their original Git blob IDs and sizes were written to Archive/Retired/ISSUE_29_LEGACY_SHELLS.tsv. Archive/Retired is historical custody, not execution permission.

Every original ci/*.sh and ci/docker/*.sh entrypoint is now a variable-free and percent-free nine-line literal wrapper. It passes control to ci/FRESH-GammaGamma.sh from the repository root and never performs its former build, test, audit, publish, or cross-compile operation. The historical behavior is recoverable from its exact archive or Git history. The wrappers and central harness terminate with **nonzero exit 79 / HOLD**.

The central vanilla shell trace has two unrolled four-Di Gamma passes, BITTDANCING's internal character witnesses, BiDi YIN/YANG forward and reverse, TriDi YIN/YANG/AIRGAP cyclic perspectives, and SigmaDi butterfly q1/q3, q1/q2, q4/q2, q4/q3 with two polarity twirls. The **last output** is the exact terminal witness ItWasOnlyOnceILookedInTheirEye, followed only by the HOLD exit. This is an exploratory trace, not the historic DOS DOOM implementation or a functioning replacement for the old shell operations.

The archived files contain historically sourced variables as inert records. The active replacement shell files contain no variable expansions, dollar signs, percent-format specifiers or recursive calls. Their literal shell execution remains forbidden by current policy, even though the experiment is written in vanilla POSIX sh.

Outstanding: independent source and binary SPDX/SBOM review, removal or native replacement of actual GNU/POSIX-dependent build inputs, third-party and vendor notices, C++20 -O2 tests, Java and security qualification, provenance and explicit release consent. A source-only check does not clear any of these.
