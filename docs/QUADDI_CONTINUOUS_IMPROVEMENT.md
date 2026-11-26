# QuadDi/1 | CI = Continuous Improvement

<!-- SPDX-License-Identifier: 0BSD -->

**Status:** active exploratory audit workflow on PR #34's branch. Not a replacement for required GitHub Continuous Integration / PR-PARITY gates. `RELEASE=HOLD`.

## Four steps, one evidence chain

| Stage | Quantitative witness | Qualitative interpretation |
| --- | --- | --- |
| **1. OBSERVE** | Exact `github.sha`, total tracked-source archive files, per-file SHA-256 | Establish the event-commit source before evaluating it. Git commit ancestry is authority; individual hashes are derived custody witnesses. |
| **2. MEASURE** | Required-path PASS/HOLD rows, PowerShell syntax rows for all `.ps1` files, CMake C++20 and O2 policy row | Disclose specifically what exists and parses without claiming successful builds or safety certification. |
| **3. PROPOSE** | One review recommendation for each HOLD | A proposal is never an automatic code edit, approval, dismissal, or merge. Historical files stay accessible. |
| **4. VERIFY** | Replay every observed file size and SHA-256 against the same source snapshot; PASS/HOLD totals | Confirm there was no source mutation; fail the workflow if any required measurement remains on HOLD. Do not average mandatory failures into PASS. |

The implementation is `.github/workflows/QUADDI-CONTINUOUS-IMPROVEMENT.yml`, triggered by pushes to `feat/jstar-msvc-process-provenance-20261121` and manual workflow dispatch on that branch. The workflow runs in **Windows PowerShell**, with read-only repository permissions and no Node actions, Python execution, container builds, forced branch update, or source writes. It bootstraps from the GitHub event commit using the repository GitHub API; this explicit online source transport is *not* an airgap claim. The four measurements after transport operate on that local snapshot.

### Expected first finding

At the preceding audit snapshot PR #34 contained `shared/slidingtoolong.hpp` and `diaery/Invoke-ParityGate.ps1`, while the dependent PR #95 carried `quaddi-ni/Invoke-NativeQualification.ps1`. That is a **review finding**, not permission to copy, flatten or merge the branches. At each new run the pipeline rechecks actual contents instead of assuming the historical snapshot still applies.

### Scope and custody limits

- A PASS for this workflow establishes *only* that its source inventory, PowerShell syntax, declared file dependencies, compiler-policy text check and integrity replay passed. It does not run C++ compilation, the actual four failing LadderDi lanes, JSTAR signatures, or the repository's full PR-PARITY gate.
- HOLD findings print into the GitHub Actions job logs and a human-readable step summary. Derived temporary TSV/Markdown files live under the GitHub runner's temp directory. They are **not** automatically committed or uploaded as durable Actions artifacts; log retention can expire.
- The source ZIP is pinned by the **event SHA**, but the Git tree/commit history remains authoritative. Do not promote static digest constants into repository truth.
- Recurring runs on future pushes are new witnesses. They do **not** count as repeated independent confirmations of an unchanged experiment unless the protocol and inputs are frozen.
- No historical file removal, silent branch rewrite, automatic release, quorum-based consent, or toolchain-policy bypass.

**Review target:** [PR #34](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/34). **Dependency:** [PR #95](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/95). **Decision authority:** human reviewers.
