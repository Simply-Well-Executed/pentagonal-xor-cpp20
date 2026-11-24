# NASCAR / Kentucky Derby stacked circuit (NASCAR-KENTUCKY-001)

Status: experimental, review-required. No automatic merge, source mutation, release authorization or issue closure.

## GitHub stack
- PR #34: `main` -> `feat/jstar-msvc-process-provenance-20261121`.
- PR #95: `feat/jstar-msvc-process-provenance-20261121` -> `feat/quaddi-ni-oracle-cpp20-20261123`.
- PR #95 is explicitly joined to new PR #34 commits through an ordinary two-parent merge commit, not a destructive rebase. Run evidence is SHA-bound.

## NASCAR file laps
Every source-tree file is walked one at a time, sorted by path; size and SHA-256 are recorded, UTF-8 verified on recognized textual types, and active PowerShell source is parsed without execution. Archived `Archive/Retired` code is fingerprinted for provenance, not activated. A header, Java, Pascal, assembly or XHTML text fingerprint is **not** a claim of independent compilation or browser conformance. Each file is checked again for mutation after each downstream subproject.

## Six serial subproject lanes
1. TRIFORCE/JSTAR native security and exact C++ target.
2. DIAERY parity and geometric witnesses.
3. QUADDI-NI native power, ledger and XHTML fixture.
4. MINZA topology and 54-byte pipeline tests.
5. PENTADI C11/assembly witness; proprietary Delphi may remain HOLD.
6. PENTADI deterministic HULAO chaos fixture.

These are existing reviewable test scripts; no source is evaluated from an issue or external message. External Oracle parity, proprietary Delphi compilation, Android physical-device tests and intentionally retired code remain separate qualifications, not assumed by this race.

## Kentucky Derby
Only after all six lanes pass, run the existing full-source autostart suite: root/shared/consumer CMake + CTest, install, native program startup and project security/output contracts. Source files are verified again. All builds remain serial (/O2, C++20, no extensions); source bootstrap is the only remote source acquisition in this pipeline.

## Issue solver
`.github/workflows/issue-solver-jig.yml` uses `issues: opened, reopened` to inspect the issue as **untrusted data**. It recognizes only scoped INCEPTION receipts and reads exact source revisions. It records a digest and diagnostic comment, without source writes, automatic acknowledgment or closure. GitHub only activates the issue trigger once this workflow is present on the default branch.

At the audited November 23, 2026 snapshot, 60 open issues (#35–#94) were generated INCEPTION receipts on successive PR #34 revisions. #93/#94 concerned the then-current head; older receipts must remain accessible as provenance. A clean source correction is not a substitute for CI, historical issue deletion, or human acknowledgment.

## Interpretation
`FILE_LAPS=PASS` means custody/parse success, not that all translations compiled. `SUBPROJECT_LAPS=PASS` means the six named validators executed successfully. `KENTUCKY_DERBY=PASS` means the full-source *native scope* passed. No outcome certifies unsupported platforms or grants release. Any failed lap retains HOLD.
