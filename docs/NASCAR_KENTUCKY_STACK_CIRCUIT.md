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
All six subprojects are attempted in sequence even if an earlier one fails, preserving independent evidence; any HOLD blocks the final derby. Only after all six lanes pass, run the existing full-source autostart suite: root/shared/consumer CMake + CTest, install, native program startup and project security/output contracts. Source files are verified again. All builds remain serial (/O2, C++20, no extensions); source bootstrap is the only remote source acquisition in this pipeline.

## Issue solver
`.github/workflows/issue-solver-jig.yml` uses `issues: opened, reopened` to inspect the issue as **untrusted data**. It recognizes only scoped INCEPTION receipts and reads exact source revisions. It records a digest and diagnostic comment, without source writes, automatic acknowledgment or closure. GitHub only activates the issue trigger once this workflow is present on the default branch.

At the audited November 23, 2026 snapshot, 60 open issues (#35–#94) were generated INCEPTION receipts on successive PR #34 revisions. #93/#94 concerned the then-current head; older receipts must remain accessible as provenance. A clean source correction is not a substitute for CI, historical issue deletion, or human acknowledgment.

## Interpretation
`FILE_LAPS=PASS` means custody/parse success, not that all translations compiled. `SUBPROJECT_LAPS=PASS` means the six named validators executed successfully. `KENTUCKY_DERBY=PASS` means the full-source *native scope* passed. No outcome certifies unsupported platforms or grants release. Any failed lap retains HOLD.

## EXCITE / EXCITED successor architecture

The NASCAR / Kentucky Derby implementation is renamed to
`.github/workflows/EXCITE.yml`. The older workflow file is removed from
the current tree so two identical racing runs do not compete for resources.
It remains recoverable in Git history. The source-custody logic and
`RELEASE=HOLD` remain authoritative.

EXCITE requires every enumerated source file vehicle (542 in the previous
run, but not hardcoded as the target), including a failure witness, to enter
a deterministic COREAO pipeline **immediately after its own lap**:

`QCHART -> QLAM -> QGRAPH -> inverse-square rational witnesses -> FRESH HAODA -> QCODE`.

These are **new evidence-envelope definitions**, not claims about preexisting
QCHART/QLAM/QGRAPH/QCODE mathematics. QCHART carries source SHA-256, validation,
duration and byte count. QLAM references the chart and the previous QCODE digest.
QGRAPH records directed provenance edges with zero graph cycles. The
inverse-square witness lists the dimensionless, exact rational values
`1/1`, `1/4`, and `1/9` for radii `1`, `2`, and `3`. These do
not infer electromagnetic exposure or physical radiation. FRESH HAODA NFKC
normalizes **display labels only**; canonical source bytes, names, and file
identity are unchanged. QCODE wraps those records with a chained SHA-256
digest; the end-to-end verifier checks every predecessor and coordinate,
including deliberately corrupted synthetic test data.

All six independent native subprojects enter the **same** chain after their
own tests, PASS or HOLD. Passing subprojects do not erase failed ones.
Only after file and subproject integrity passes can the whole-repository
Kentucky Derby enter as the final vehicle. No job silently treats a fingerprint
as successful C++ translation or promotes a green result to release approval.

`.github/workflows/EXCITED.yml` is a **reusable workflow** reached only
through `needs: stack-circuit` in EXCITE. Thus on PRs, EXCITED starts
**only if EXCITE passed**, without waiting for a default-branch
`workflow_run` trigger. It executes a deterministic A* path over a small
fixed nonnegative graph using PowerShell on `windows-2025`, publishes
the generated `Sovieta Diao Xian ^.^` Markdown to the GitHub job summary,
and verifies SHA-256. There is no Python, Bash, Ubuntu, Node-based checkout,
or Node-based artifact upload. The Markdown also exists as a file in the
runner's external temporary evidence directory, which is ephemeral;
the job summary is its persistent, user-visible record. EXCITED does not
commit, publish, or merge code.

### Optional local read-only Markdown API

The native PowerShell application `ci/Serve-ExcitedMarkdown.ps1` is **not**
launched in GitHub CI. Once a Markdown file has been generated locally,
it can expose precisely that file on **127.0.0.1 only**, at
`GET /v1/markdown` (with `GET /healthz`), for up to 9 requests:

```powershell
pwsh -NoLogo -NoProfile -NonInteractive -File ci/Serve-ExcitedMarkdown.ps1 -MarkdownPath "C:\evidence\sovieta-diao-xian-excited.md" -Port 8179 -MaxRequests 9
```

No public bind, remote execution, filesystem browse endpoint, API writes,
or unreviewed release. The local API is an opt-in preview, not an external
network deployment. The developer supplies the Markdown file path manually.
The HTTP server merely serves its **unchanged bytes**; it neither retrieves
nor mutates repository source.

### EXCITED v2 native A* plus SPT Finland

EXCITED's Markdown generator is implemented in `ci/excited-native/astar_raw.cpp`
as a separately compiled, header-free, no-`std::` C++20 program. It uses
raw fixed arrays, explicitly bounded work, native Windows `WriteFile` to
standard output, and an MSVC `/O2` build with C++ extensions disabled.
PowerShell is the orchestration wrapper, not the A* implementation. No
Python, g++, Bash, Ubuntu runner, JavaScript/Node checkout, or artifact-upload
action is used. The generated file and TSV receipt live outside the checkout;
the Markdown is copied into the Actions run summary.

**SPT Finland** is the user-chosen label for an additional **Shortest Path
Tree**, computed with an independent deterministic Dijkstra pass over the
same fixed weighted graph. It is not the Ethernet **Spanning Tree Protocol
(STP)**, and no attribution of the protocol to Finland is asserted. The
SPT validator ensures all five nodes are reachable, every nonroot vertex
has a parent, every parent chain reaches the unique root without cycles,
and the Bellman inequality holds for every directed edge. Compile-time
assertions fix the expected distances `0,2,4,5,6` and parents
`-1,0,0,1,2` from `Overview` to `Summary`.

A* independently selects `Overview -> Security -> Summary` with cost
`6`. Both equal-cost routes are valid, but this bounded implementation's
stable tie-breaking selects the one through Security. The runtime validator
checks that every edge of the selected A* route exists and that its sum
equals the SPT goal distance. CTest separately checks the A* and SPT
Markdown witnesses. A failed cross-check aborts EXCITED; it does not change
EXCITE's earlier source evidence.

EXCITED v2's generated Markdown and native executable each receive SHA-256
receipts in `excited-receipt.tsv`. No JSON is used by this v2 builder or
its TSV validation step; the wider EXCITE evidence ledger still uses JSONL,
which is not silently changed or reclassified. These algorithms do not
prove radiation physics, switch-port loop prevention, or release eligibility.
