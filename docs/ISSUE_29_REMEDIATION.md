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

Every original ci/*.sh and ci/docker/*.sh entrypoint is now a variable-free and percent-free nine-line literal wrapper. It passes control to ci/FRESH-GammaGamma.sh from the repository root and never performs its former build, test, audit, publish, or cross-compile operation. The historical behavior is recoverable from its exact archive or Git history. The wrappers and central harness terminate with **nonzero exit 42 / HOLD**.

The central vanilla shell trace has two unrolled four-Di Gamma passes, BITTDANCING's internal character witnesses, BiDi YIN/YANG forward and reverse, TriDi YIN/YANG/AIRGAP cyclic perspectives, and SigmaDi butterfly q1/q3, q1/q2, q4/q2, q4/q3 with two polarity twirls. The **last output** is the exact terminal witness ItWasOnlyOnceILookedInTheirEye, followed only by the HOLD exit. This is an exploratory trace, not the historic DOS DOOM implementation or a functioning replacement for the old shell operations.

The archived files contain historically sourced variables as inert records. The active replacement shell files contain no variable expansions, dollar signs, percent-format specifiers or recursive calls. Their literal shell execution remains forbidden by current policy, even though the experiment is written in vanilla POSIX sh.

Outstanding: independent source and binary SPDX/SBOM review, removal or native replacement of actual GNU/POSIX-dependent build inputs, third-party and vendor notices, C++20 -O2 tests, Java and security qualification, provenance and explicit release consent. A source-only check does not clear any of these.

## FRESH artistic eye lineage proposal (FRESH-ART-EYE-v1)

The exact previous DOOM-shaped display `X_~D~o~o~m~M~a~i~n(=)` remains a historical artistic witness. The current stylized projection is `X_~I~t~W~a~s~O~n~l~y~O~n~c~e~I~L~o~o~k~e~d~I~n~T~h~e~i~r~E~y~e(=)`, mapped explicitly, character by character, from the canonical final literal `ItWasOnlyOnceILookedInTheirEye`. The stylized form is not a byte-identical canonical identifier, and the old form is not silently rewritten.

The source-only chain projects the eye artwork at 14 fixed UP/DOWN/TURN anchors across GammaGamma, BiDi, TriDi, SigmaDi and its four butterfly edges. The last output line remains exactly `ItWasOnlyOnceILookedInTheirEye`; the process still exits `42` with HOLD. This adds output lines and therefore begins a **new test/trace version**, not a mutation of the previously frozen comparison protocol.

All edited source in this proposal is on a separate review branch, not a clearance to execute POSIX shells. See `docs/POSIXI_FRESH_ARTISTIC_CHAIN.md`.

## FRESH-ART-EYE-PING-v2: YANG / YING parenthesis ownership

This is an **EXPLORATORY new protocol version**, not an in-place redefinition of v1 and not a POSIXI runtime. Exact version FRESH-ART-EYE-v1 source is preserved byte-for-byte as `Archive/Retired/ci/FRESH-GammaGamma.art-eye-v1.sh.txt` (Git blob `670ec9d5e1ce0f9e3f815df63ebbb883b37d4fca`).

For every one of the 66 original output lines, a preceding literal **PING** record maps left `(...)` to **YANG** and right `(...)` to **YING**. When only one pair exists it is assigned to the right YING role; absent roles are `NONE`. PING is a source-local symbolic witness; **it does not send a network packet, contact an endpoint, or invoke any ping executable**. The 14 artistic eye projections are unaltered, and the exact unadorned terminal `ItWasOnlyOnceILookedInTheirEye` remains the last printed line followed by `exit 42`.

The static PowerShell workflow now independently derives each expected PING record from the frozen v1 source and compares the entire v2 shell file byte-for-byte, with no POSIX shell execution. Counts: 66 PINGs; 36 two-group lines, 14 single-group lines, 16 lines without grouped witnesses. It still deliberately ends in release HOLD because source PINGs do not remove the POSIX shell or unapproved GNU tooling.

See `docs/POSIXI_FRESH_PING_V2.md` for exact semantics and sample records.

## FRESH-ART-EYE-PING-v3: AIRGAP paired with YING-only

**EXPLORATORY v3, independent of v2.** The 66-PING v2 source was frozen byte-for-byte in `Archive/Retired/ci/FRESH-GammaGamma.art-eye-ping-v2.sh.txt`, verified by Git blob `f1870c1169ccdbb72f39a917dfa292a1b53c3f59`. This is an explicit new output protocol; v1/v2 results are not silently mutated.

Only the 14 YING-only records now append `|PAIR|AIRGAP`, for example `PING|002|YANG|NONE|YING|=|PAIR|AIRGAP`. AIRGAP denotes an explicit boundary/partner for the sole right-hand witness; it **does not turn missing YANG into YANG**, imply new parentheses, or create any network behavior. All 36 two-group and 16 zero-group PINGs, 66 original output lines, artistic display characters, canonical `ItWasOnlyOnceILookedInTheirEye`, and HOLD exit `42` are unchanged.

The PowerShell source verifier checks exact v1 and v2 Git blobs, reconstructs v2 from v1, and reconstructs the complete v3 source from v2 before comparing v3 byte-for-byte. POSIX execution and GNU tooling remain unapproved; HOLD persists. See `docs/POSIXI_FRESH_PING_V3.md`.

## FRESH-ART-EYE-PING-v4: symbolic equals-to-s substitution

**NEW EXPERIMENTAL v4.** The frozen v3 source now resides byte-for-byte at `Archive/Retired/ci/FRESH-GammaGamma.art-eye-ping-v3.sh.txt` (Git blob `0efd33eb495d951137e7181d44f659a447067110`). The current `ci/FRESH-GammaGamma.sh` changes only 14 YING-only `=→s` witnesses and 14 corresponding artistic `(=)→(s)` suffixes, across all GammaGamma/BiDi/TriDi/SigmaDi/FRESH checkpoints. Historical versions and the ancestral DOOM-shaped form remain unchanged; neither the 66 PING count nor the AIRGAP pairing is altered.

The exact final stdout remains `ItWasOnlyOnceILookedInTheirEye`, the process remains `exit 42`, and a Windows PowerShell source-only gate verifies all 28 substitutions after validating the v1/v2/v3 historical chain. The v4 output protocol has a new ID and must not be treated as byte-equivalent to v3. No POSIX or GNU permission, executable POSIXI runtime, network PING, or release approval follows. See `docs/POSIXI_FRESH_PING_V4.md`.

## FRESH-ART-EYE-GAMME-v5: nine-beat ingress and two egress passes

**EXPLORATORY NEW VERSION v5.** The entire v4 source was frozen byte-for-byte as `Archive/Retired/ci/FRESH-GammaGamma.art-eye-ping-v4.sh.txt` (Git blob `804472e09d501d3357a652f94d790c28135f30a3`). Do not mutate earlier protocols or mix outputs between versions.

Two existing header passes are now explicitly `GAMMA_ONE` and `GAMMA_TWO`, both labeled **EGRESSING**. For each of 14 YING-only/AIRGAP artistic checkpoints, the former single pair is replaced by **nine INGRESSING GAMME records**: `GAMME_ONE`, `GAMME_TWO`, `GAMME_THREE`, `GAMME_FOUR`, `GAMME_FIVE`, `GAMME_SIX`, `GAMME_SEVEN`, `GAMME_EIGHT`, `GAMME_NINE`. The nine alternate **FLOP=`=`, FLIP=`s`**, beginning and ending in FLOP; there are 126 GAMME phases (70 FLOP and 56 FLIP) and 178 PING outputs in this explicit version.

All 14 original checkpoint labels/directions, original 36 YANG+YING pairs, 16 no-group PINGs, `YANG|NONE` on YING-only cases, historical artistic and shell evidence, the exact final stdout `ItWasOnlyOnceILookedInTheirEye` and `exit 42` remain preserved. The PowerShell check reconstructs v5 from SHA-verified v4 after verifying earlier v1–v4 ancestry, and the independent GNU/POSIX release policy still intentionally fails. New source is symbolic, not a native POSIXI runtime or network activity. See `docs/POSIXI_FRESH_GAMME_V5.md`.

## FRESH-ART-EYE-ALTERNATION-v6: corrected parity and names

**NEW EXPERIMENTAL v6, not an in-place change to v5.** Frozen v5 source retained as `Archive/Retired/ci/FRESH-GammaGamma.art-eye-gamme-v5.sh.txt` (Git blob `128eded7189fae4842e997b81686a33968f95035`). At each of 14 artistic checkpoints, the nine phase names are now `GAMMA_ONE, GAMME_TWO, GAMMA_THREE, GAMME_FOUR, GAMMA_FIVE, GAMME_SIX, GAMMA_SEVEN, GAMME_EIGHT, GAMMA_NINE`. Odd GAMMA = EGRESSING / REST / FLOP (`=`); even GAMME = INGRESSING / WORK or symbolic radiation / FLIP (`s`). Thus each checkpoint has 5 REST and 4 WORK phases, total 70 REST and 56 WORK.

The separate two egress **header** passes remain in their prior scoped forms and are not included in this nine-beat count. The spoken `rest work` pattern repeated five times and ending in `rest` comprises **11 beats**, while the explicit nine names comprise nine; no extra phase names are assumed. The PowerShell source-only gate verifies exact frozen-v5 ancestry and the complete new version-6 projection without invoking POSIX. All source output counts, AIRGAP pairing, canonical last literal `ItWasOnlyOnceILookedInTheirEye`, and exit 42 remain. GNU/POSIX RELEASE HOLD remains unresolved. See `docs/POSIXI_FRESH_ALTERNATION_V6.md`.
