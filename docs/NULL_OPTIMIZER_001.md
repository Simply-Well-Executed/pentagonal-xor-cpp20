# NULL Optimizer / 001: HEAD → BiDi → NULL → XOR → TAIL

SPDX-License-Identifier: 0BSD

Status: **EXPLORATORY / contract-only; release HOLD**. This is not a compiler optimization, proof of secrecy, or a qualification of uninstalled runtimes. Source baseline: \`main\`, read at 6823f5365bfd75d4ad61cd51e225bdf52737cf4e; live Git histories, not the snapshot SHA in this document, remain authoritative.

## Intent

Preserve the original quote as a single LTR isolate with U+2066 LRI and U+2069 PDI. No explicit newline may occur between the pair. Do not infer human identity, behavior, assent, or threat from its words. Test the original text's exact bytes after unwrapping and reject unbalanced or newline-crossing controls. A renderer's visual reversal is not a changed canonical stream.

The symbol U+2205 (\`∅\`) is the user's display alias for a null payload **within this research protocol**. Do not conflate (1) missing field, (2) present-and-null field, (3) empty string, (4) unknown \`?\`, (5) numerical zero, (6) the abstract empty set, and (7) an encrypted or concealed payload. Identity/no-op bitwise operations are not access control.

## 8 × 8 research matrix

Eight specifically requested lanes:

| Lane | Frozen provenance / policy | Current NULL-variant status |
|---|---|---|
| HeptaDi | Lua 3.2.2, project-local variables, no declared globals; historical toolchain | HOLD: interpreter qualification independent |
| OctaDi | Python 2.7.18 AND separately provisioned, measured Python 3; never substitute versions | HOLD: both required; OctaDi G-table remains separate |
| NonaDi | Node/npm | SELF_EXCLUDE: do not execute |
| EnneaDi | Rust native rustc, O2, no Cargo remote source | HOLD: compiler/version not established here |
| DecaDi | Google official Go archive, independent hash, no remote modules | HOLD: provisioning is a separate pre-lab action |
| UnDecaDi | Microsoft x64 MASM; C++20 /O2 witness where required | HOLD: exact local build needed |
| DoDecaDi | Perl local source, no external package | HOLD: interpreter/license provenance separately verified |
| TriDecaDi | OCaml native compilation | HOLD: native compile needed |

The existing **HenDecaDi R*** baseline is deliberately not removed or reclassified; the user did not place it in the new eight-lane operator set. The existing LadderDi and policy checks remain in force.

Each requested lane is paired with the eight literal, **unevaluated labels** \`Di\`, \`Pi\`, \`PiPi\`, \`Sigma\`, \`SigmaSigma\`, \`SigmaSigmaSigma\`, \`SigmaSigmaSigmanull\`, and \`PiSigmaPiSigma\`. This gives **64 distinct matrix identities**, not 64 proven numerical functions. Semantics for NULL, Pi and Sigma combinations are **UNDEFINED** until the author specifies typed domains, operand order, null propagation, and error policy. Labels do not auto-execute. The ledger is generated, not a source of mathematical truth.

## Independent byte-level contract

\`ci/Invoke-NullOptimizer.ps1\` uses PowerShell 7+ only; it:
1. Requires a fresh evidence directory outside the source checkout; preserves the original source digest.
2. Distinguishes absence, present NULL, empty string, unknown \`?\`, zero, and the U+2205 display marker.
3. Validates balanced BiDi isolates, their exact round-trip, explicit-newline termination, and missing/extra-PDI negative controls.
4. Exhaustively checks 256 unsigned 8-bit values for neutral AND/OR/XOR identities.
5. Replays 255 COREAO STANDARDIZER/1 8-bit canaries, checking inner XOR integrity before an OR envelope masks the toggled bit.
6. Emits a 64-cell UTF-8 TSV and independent SHA-256 head/tail custody receipt; source is hashed again after execution.
7. States **CONTRACT_PASS** only for checks actually run, **LANGUAGES=HOLD**, and **RELEASE=HOLD**. Never automatically merges, releases, replaces code or grants a license exception.

The source-level contract cannot prove that runtime versions are present, that builds work, or that airgapping was achieved. GitHub CI's exact-SHA source bootstrap is online **before** the local execution stage and must not be described as physically airgapped.

## PR parity and branch inheritance

Apply the same two files (\`ci/Invoke-NullOptimizer.ps1\` and \`.github/workflows/NULL-OPTIMIZER.yml\`) to the independently open PR heads only if absent. Existing PR-specific differences must remain intact. In particular PR #95 is based on PR #34, so independent commits can appear on both branches without duplicating claims of independent tests. Reconcile their Git ancestry before any eventual integration. Maintain JSTAR, TRIFORCE HEAD/TAIL, PR-PARITY, LadderDi, OctaDi, other current workflows, and all prior HOLDs.

No PR is to be auto-merged, discarded, force-pushed, rebased, or presumed validated by documentation alone. Failures remain evidence; **failure never becomes policy**.
