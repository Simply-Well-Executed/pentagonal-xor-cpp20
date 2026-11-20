# IRON / T-CROSS: native call-boundary and CI audit

Status: **EXPLORATORY / NOT CERTIFIED**. This record reports inspections and a controlled proposed change. It does not assert that any build is completely network-free, that all function calls have been removed, or that a later workflow has passed.

## Evidence snapshot and authority

- Repository: `Simply-Well-Executed/pentagonal-xor-cpp20`; PR [#19](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/19), draft and not merged when inspected.
- Measured baseline PR head: `66d68005b0cdf89acf9b2993dd7bdea1422c2893`.
- Baseline [native run 44655939533](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44655939533): successful job with all **34/34 CTest cases passed** and Magic Comb `Verify|scanned=163|changes=0`. Source is the actual job log, not the README.
- Targeted offline version-gate correction: `6c87eeb03a2debafbe59c90e253958fd0701d6b9`, changing only `.github/workflows/native-core.yml` at that commit. The baseline run predates this change.
- Main reference: [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#main); scripting reference: [r/PowerShell beginners](https://www.reddit.com/r/PowerShell/wiki/beginners/). References are guidance, not proof of conformance.

## T-CROSS: side-by-side baseline and proposed correction

| Gate | Baseline head 66d68005 (observed) | Version-gate correction 6c87eeb (inspection / simulation) | T-CROSS outcome |
| --- | --- | --- | --- |
| C++20 build and CTest | 34/34 pass on hosted Windows | C++ source and build steps unchanged; **not rerun** at corrected SHA | Functionality unchanged in diff; new run required |
| Magic Comb | 163 tracked text files canonical, zero changes | Comb unchanged; **not rerun** at corrected SHA | New run required |
| Active CI workflows | One `native-core.yml`, no third-party `uses:` entries | One `native-core.yml`, no third-party `uses:` entries | No regression in workflow count |
| Live REST release lookup | `Invoke-RestMethod` calls GitHub PowerShell release API | Live lookup removed; reviewed offline floor `7.7.0` | Improvement: one CI HTTP lookup eliminated |
| Runner PowerShell 7.6.9 when 7.7.0 required | Warning, job still marked success | `throw` for a version below 7.7.0 | Improvement: fail-closed by control-flow inspection; runtime check pending |
| Hosted checkout network transport | `git fetch` from GitHub remote | Still `git fetch` from GitHub remote | **Not network-isolated** |
| Zero function-to-function calls | Violated by production code and shared header | Identical production code | **Not zero-call compliant** |
| Java / Docker / Android manual sources | Present but no active workflow invocation | Present but no active workflow invocation | Residual manual surface remains |
| Latest stable version guarantee | Live version comparison was not enforced | Pinned reviewed minimum, not perpetual latest | A separately maintained version policy is needed |

Do **not** reinterpret the pre-correction successful run as proof of the corrected CI policy. A future green result also would not prove a total absence of local call edges or all network communications.

## Source call-edge observations

Inspected `pentagonal_xor_cpp20.cpp`, `shared/pentagonal_xor_std_free.h`, `ci/Invoke-MagicComb.ps1` and the native workflow.

- `main` calls `pentagon::validate` and `pentagon::parity`; `parity` calls `swe_pxor_xor_parity_arr`.
- The shared `swe_pxor_scores5` calls `swe_pxor_fifth_root`, which has a recursive call for negative input; these are tracked even though the production CLI's primary XOR path uses the array parity helper.
- The CLI also uses C++ standard-library routines, including `std::stod`, `std::isfinite`, `std::to_string` and stream operations. Further compiler-generated and C-runtime edges remain unmeasured.
- PowerShell calls `Assert-MagicCombSelfTest` and `Invoke-TextComb`, and invokes .NET file/process/encoding operations plus Git. Thus the comb is not zero-call.
- A local call edge is recorded as a call edge. An actual remote/network edge is recorded separately based on transport behavior, not inferred from a function name.

A true zero-call acceptance rule would require an independently scoped redesign and a full functional equivalence test; it must **not** be inferred from a lack of RPC product names.

## External integrations and residual surface

- Ten legacy GitHub Actions workflow YAML files and the active Dependabot configuration were removed on the PR branch. The previous workflow definitions remain in Git history. `main` is not yet changed.
- One explicit active CI HTTP/API lookup was found and removed by commit `6c87eeb`. One necessary Git source fetch remains in hosted CI, which is network activity.
- Manual-use source under `ci/`, `docker/`, `android/` and `java/` remains tracked. It is not executed by the one active workflow merely because it exists, but it is not deleted or proven harmless.
- Public C/C++ headers that expose programming interfaces are not evidence of a network client. No explicit MCP client/server transport was identified in the inspected production path; this is **not a complete whole-repository absence proof**.
- The complete call graph, binary import table, DNS/HTTP behavior, transitive system-library dependencies, signing provenance, and runtime network capture have **not** been independently measured.

## Required gates before IRON may be claimed

1. Run the corrected PR head CI. Reconcile actual CTest, Magic Comb, PowerShell version, and build logs with the committed SHA.
2. Decide and record how the project obtains its source while disallowing external transport. Hosted GitHub Actions cannot fetch a remote repository with zero network access; an offline local/trusted runner is needed for a genuinely isolated verification phase.
3. Create a complete C++ and PowerShell call-edge inventory: caller, callee, module, external interface, library/import, possible transport, and whether reachable from supported commands.
4. Evaluate remaining manual Docker/Java/Android paths with source and dependency review. Retire only after demonstrating replacement capability or documenting intentional removal; retain original Git provenance.
5. Check actual C++ core-guideline findings (ownership, errors, lifetime, input validation, interfaces, and portability) and PowerShell parameter validation, error propagation, exit codes, and StrictMode 2.0.
6. Carry out side-by-side fixed-input and negative-control comparison: parity outputs and error contracts, all 34 CTests, public installed-header consumer tests, memory behavior, import table, and runtime network observation. Preserve compiler `/O2` / `-O2` and C++20 build contract. Label unavailable results **NOT TESTED**.
7. Resolve the documentation distinction between a manually reviewed minimum PowerShell version and a dynamically verified latest release; never claim an offline pin automatically tracks the latest stable version.
8. Keep PR #19 draft and `main` unchanged until an explicit promotion decision. An unverified result is a blocker, not permission for policy relaxation.

## Issue sweep

At inspection, GitHub's open-issues endpoint returned **only PR #19** (the GitHub Issues API includes pull requests). There were **zero standalone open issues** to disposition. Future issues must be reviewed at the then-current snapshot; do not infer closure or absence indefinitely.

## Provenance rule

Algorithm and exact source records are authoritative. This audit is derived evidence, not a replacement for code or run logs. Preserve the baseline SHA, changed SHA, observed versus simulated labels, dissent and unresolved gates. Failure never becomes policy.

## Native-only transition: retire all GitHub Actions runner images (2026-11-19)

**Superseding status for execution:** the earlier workflow-based snapshot above is retained for provenance; it is not the current execution design after this change.

**DOLPHIN subject:** hosted GitHub Actions runner image. Source: the prior `.github/workflows/native-core.yml` contained `runs-on: windows-2025`. Observed GitHub job 44661958832 resolved to `windows-2025-vs2026` with PowerShell 7.6.9. The operator-required latest stable was 7.7.0. The earlier version gate blocked as intended, after 34/34 CTests passed and 164 source files passed Magic Comb.

**Disposition:** remove the last workflow definition from the PR branch rather than changing it to `runs-on: self-hosted`, which would still require GitHub's remote runner service. The locally controlled entrypoint is `ci/Invoke-NativeCore.ps1`. It does not request an image, launch a container, download source, query a REST service or install dependencies. It uses only the already-installed Windows PowerShell, local Git history, local Visual Studio MSVC, CMake and CTest.

**Version assertion contract:** the operator must independently verify the latest stable release and supply both `-LatestStableVersion` and its corresponding official release URL. The entrypoint compares the local PowerShell version exactly before invoking project source. It fails closed on mismatch and does not automatically fetch release metadata. The operator-supplied URL is an auditable assertion **not** a cryptographically checked or live-verified release fact. At authoring time the official latest release was 7.7.0; no version is hard-coded into the script.

### T-CROSS side-by-side

| Gate | Old hosted workflow (observed) | New local-native process (static inspection) | Disposition |
| --- | --- | --- | --- |
| Runner image | GitHub Windows VM image used | No GitHub Actions workflow and no VM-image request | Removed from PR branch |
| Networked source checkout | GitHub `git fetch` | Local `git rev-parse` and `git diff`, no fetch | Remote source fetch removed from execution code |
| PowerShell selection | 7.6.9 inherited from hosted image | Locally installed executable must exactly match operator-declared latest | Intended fail-closed; local runtime untested |
| Mandatory source verifier | Magic Comb SelfTest + Verify, 164 files checked in last hosted run | Same Magic Comb source and modes invoked | Retained; new local run NOT TESTED |
| C++20 /O2 compile and CTest | 34/34 passed on hosted Windows | Same CMake options, compile and CTest commands | Retained; new local run NOT TESTED |
| Java, Docker, Node and Linux runtime steps | None in last workflow | None in new entrypoint | No new dependencies in entrypoint |
| Historical manual scripts and Dockerfiles | Tracked but outside active CI | Still tracked, not invoked by new entrypoint | Residual surface, separate review required |
| Proof of zero network and zero calls | Not demonstrated | Not demonstrated | **IRON NOT CERTIFIED** |

**Important limitations:** no independent local Windows execution, complete import-table review, call-graph closure, offline firewall enforcement, or source/toolchain signing inspection has yet occurred. Merely removing an Actions workflow is not proof that CMake, a compiler, third-party tool, or manually invoked historical scripts cannot access a network. No production runtime code or mathematical source was changed by this transition. The PR stays draft and unmerged; `main` still has its historical workflows until the branch is promoted.

**Evidence custody:** The old runner definition, job IDs, exit conditions and reason for removal remain in Git history and this audit. The new local entrypoint records its own JSON evidence in `.iron-native-evidence.json`; this generated record is not committed automatically. Its status must not be confused with hosted CI status.


## T-CROSS +&-DIFF: verified Git source delta

**Status: VERIFIED SOURCE DIFF / LOCAL RUNTIME NOT TESTED.** This section is the canonical plus/minus source-delta ledger for the native-only transition. It does not revise the original algorithm or supersede actual test evidence. Delta labels refer to Git lines, not measurements of performance or inferred security outcomes.

- **CORE (before):** `3570a7512bc0cd66b39df0fa26133a584b7a35fe` (one hosted workflow, warning previously corrected to fail closed).
- **Candidate (after):** `79c3cab7f094b53002be76292f278fcb1fd39b62` (no active workflows, local-native entrypoint).
- **Authority:** [GitHub commit comparison](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/compare/3570a7512bc0cd66b39df0fa26133a584b7a35fe...79c3cab7f094b53002be76292f278fcb1fd39b62), seven commits ahead, zero behind, seven changed files, **+198 / -104 lines**, **net +94**. This section itself is added *after* the compared candidate, so future commits must use a new comparison range instead of treating these frozen numbers as live.
- **Readout convention:** `-` means removed from active branch, `+` means added to active branch, `=` means equivalent intent or unchanged interface, `?` means evidence not obtained. Removing a file from current Git state preserves its prior committed history.

### Exact +/- source ledger

| Path | Git status | + additions | - deletions | net lines |
| --- | --- | ---: | ---: | ---: |
| `.github/pull_request_template.md` | modified | +18 | -8 | +10 |
| `.github/workflows/native-core.yml` | removed | +0 | -77 | -77 |
| `.gitignore` | modified | +3 | -0 | +3 |
| `CONTRIBUTING.md` | modified | +19 | -6 | +13 |
| `README.md` | modified | +24 | -13 | +11 |
| `ci/Invoke-NativeCore.ps1` | added | +107 | -0 | +107 |
| `docs/IRON_T_CROSS_REVIEW.md` | modified | +27 | -0 | +27 |
| **TOTAL** | **7 files** | **+198** | **-104** | **+94** |

### T-CROSS behavior and evidence, side by side

| Subject | - BEFORE: hosted workflow | + AFTER: local native entrypoint | Status and implication |
| --- | --- | --- | --- |
| Execution authority | `runs-on: windows-2025` on a GitHub-managed image | `ci/Invoke-NativeCore.ps1` executes locally on Windows | **+** Image scheduling removed; local trust still requires review |
| Remote source transport | Workflow `git remote add origin` and `git fetch` | Local `git rev-parse` and `git diff --quiet`, no fetch in entrypoint | **+** Automated GitHub checkout transport removed from the defined execution path |
| PowerShell version | Hosted 7.6.9 was below reviewed 7.7.0 minimum; late workflow gate failed | Operator supplies a verified latest-version assertion; script requires **exact** equality before source checks | **+** Early fail-closed check in code; **?** no local execution or live release proof |
| Magic Comb | SelfTest + Verify, nesting depth 9 | SelfTest + Verify, nesting depth 9 | **=** Same source-verification intent; **?** local run not observed |
| C++ toolchain | Windows MSVC, CMake, C++20 extensions ON, `/O2` | Locally installed MSVC, CMake, C++20 extensions ON, `/O2` | **=** Declared flags retained; **?** local toolchain not exercised |
| Tests | Historical hosted CTest **34/34 passed** on run `44661958832` | Local entrypoint invokes CTest after compilation | **?** Local outcome not measured, do not inherit old green status |
| Documentation | Workflow links and obsolete release automation statements | README, CONTRIBUTING and PR template describe native review and evidence | **+** Change process documented, but older historical sections may still need targeted review |
| Native/API/MCP isolation | No proof of complete absence of call edges or network paths | No explicit CI API/downloader in local entrypoint; manual-use historical scripts retained | **?** Whole-repository call graph, binary imports and network monitoring remain unverified |

### Cross-file +/- excerpt (faithful individual lines, not a contiguous unified hunk)

```diff
-    runs-on: windows-2025
-          & git remote add origin "https://github.com/$env:GITHUB_REPOSITORY.git"
-          & git -c advice.detachedHead=false fetch --no-tags --depth 1 origin $env:GITHUB_SHA
-      - name: 'Fail closed on reviewed PowerShell Core version (offline check)'
+    [Parameter(Mandatory = $true)][version] $LatestStableVersion,
+    [Parameter(Mandatory = $true)][string] $ReleaseEvidenceUri
+    if (-not $IsWindows) { throw 'Native verification requires an actual Windows host.' }
+    if ($actual -ne $LatestStableVersion) {
+        throw "PowerShell $actual differs from operator-declared latest stable $LatestStableVersion. No build attempted."
+    }
+    & git -C $root diff --quiet --exit-code HEAD --
+        & cmake --build build --config Release --parallel
+        & ctest --test-dir build -C Release --output-on-failure
```

The `-` lines are extracted from the deleted `.github/workflows/native-core.yml`; the `+` lines are extracted from the newly added `ci/Invoke-NativeCore.ps1`. These are meaningful source examples, **not** one literal patch hunk. Use the linked GitHub compare for full exact file patches.

### Acceptance and negative controls

- **Source delta:** PASS for the stated frozen Git comparison only. No active workflows remain in the candidate tree at that SHA.
- **Compatibility:** historical hosted 34/34 CTest passed; native Windows run of new entrypoint **NOT TESTED**. Run the same fixed-input and invalid-input tests locally before claiming parity.
- **Version negative control:** a local `pwsh` version different from the operator-declared latest must terminate before Magic Comb or compilation; this branch was inspected, **NOT EXECUTED**.
- **Network negative control:** no active workflow checkout/fetch exists after migration. Offline operation, network activity of compilers and tools, and manual scripts still require direct observation.
- **C++ call graph:** no changes to the production C++ core in this comparison; strict zero-function-call criterion remains **UNMET / NOT CERTIFIED**.
- **Publication:** PR #19 stays DRAFT and unmerged. No automatic releases, test verdicts or main-branch mutations are inferred.

**Decision:** Retire runner-image execution on the PR branch (**SOURCE DIFF ACCEPTED**). Keep the whole-project IRON seal **HOLD** until local execution, toolchain provenance, negative controls, call-path inventory and explicit maintainer promotion.

## ISO C++20 + POSIX T-CROSS YAML continuation

Status: **SOURCE CHANGE COMMITTED / NEW LOCAL RUNTIME NOT TESTED**. This continuation does not rewrite the frozen historical `+198/-104` diff snapshot above. A new snapshot must be derived from Git before/current SHAs and preserved as a distinct record.

### CMake language-level correction

- `CMAKE_CXX_STANDARD 20` is the numeric language level; setting it to `TRUE` is not a valid way to request C++20.
- `CMAKE_CXX_STANDARD_REQUIRED ON` is the boolean that prevents silent fallback to older C++ modes.
- `CMAKE_CXX_EXTENSIONS OFF` requests ISO C++20 rather than compiler-specific dialect extensions. All 11 prior target-level `CXX_EXTENSIONS YES` overrides were changed to `NO`, and the dedicated parity-core object explicitly uses standard C++20.
- A subsequent whole-repository review also found `CXX_EXTENSIONS ON` in `shared/CMakeLists.txt` and `tests/consumer/CMakeLists.txt`. Both independent CMake projects now set global and explicit target extensions `OFF`, avoiding downstream dialect inconsistency.
- Optimization remains MSVC `/O2` or GCC/Clang `-O2`. Compiler-extension selection is not a performance optimization setting. This source change still needs full CMake configure/build and regression testing.

### Derived audit YAML and execution source

- [Taskfile.yml](../Taskfile.yml) contains the nine ordered T-CROSS gates and `emit-diff-yaml` task in YAML v3 format, **not** in `.github/workflows`. Running `task` is optional and does not install third-party tools.
- [ci/t-cross-posix.sh](../ci/t-cross-posix.sh) is the primary POSIX `sh` implementation. `sh ci/t-cross-posix.sh` automatically derives `.iron-t-cross-diff.yml` from the locally available frozen baseline and committed `HEAD` before attempting gates.
- `sh ci/t-cross-posix.sh emit-diff-yaml > .iron-t-cross-diff.yml` produces the audit without attempting build gates. The output is a derived Git ledger (`before`, `after`, per-file added/removed, totals, binary-line caveat); no hand-authored numbers override Git.
- The example offline operator assertions in Taskfile are PowerShell 7.7.0 and CMake 3.30.0. A static assertion is **not proof** that these are forever the latest versions or that the local host has them. The script fails closed when versions differ or tools are unavailable.
- The default policy is deliberately strict: configured Git remotes, any workflow YAML, source REST/download candidates, and remaining `java/`, `docker/`, `android/` directories each have separate gates. Existence of historical source is factual, not automatically evidence of active execution. Retiring it requires separate provenance and acceptance review.

### Canonical parity zero-CALL strategy

- The production CLI remains unchanged: input parsing (`std::stod`), validation and formatting still require routines outside the pure mathematical kernel. No claim is made that a functioning CLI has zero function calls.
- [tests/zero_call_core_probe.cpp](../tests/zero_call_core_probe.cpp) builds a **separate object file** from the canonical inline five-factor parity function in [shared/pentagonal_xor_std_free.h](../shared/pentagonal_xor_std_free.h), at the existing O2 optimization level. This avoids adding a second algorithm definition.
- The POSIX disassembly gate fails if this probe object is absent, ambiguous, cannot be disassembled, lacks its expected symbol, or contains any detected CALL instructions. It scopes evidence to the optimized parity object, not the CLI, CTest, static/shared library ecosystem or runtime transport.
- The historical fifth-root routine has been changed from recursive sign-handling to an iterative sign/magnitude result without changing the finite-input formula. Its signed outputs still require regression comparison.
- Direct CLI import scanning for known network libraries is a narrower gate than runtime network isolation. Dynamic loading, operating-system interactions and transitive imports still need investigation.

### T-CROSS source-before/candidate summary

| Subject | - Prior | + Candidate | Observed status |
| --- | --- | --- | --- |
| Language mode | C++20, vendor extensions ON | `CXX_STANDARD 20`, required ON, extensions OFF | Source diff verified; runtime NOT TESTED |
| Optimization | O2 (MSVC/GCC/Clang) | O2 unchanged | Source diff verified; runtime NOT TESTED |
| Source recursion | Fifth-root called itself for negative finite input | Local sign flag, same magnitude iterations | Source diff verified; behavioral test NOT TESTED |
| Zero-CALL scope | Whole executable assumption not credible with CLI I/O | Isolated canonical parity object plus disassembly gate | Test implemented; machine-code outcome NOT TESTED |
| Audit format | Hand-authored Markdown historical table | Git-derived `+/-DIFF` YAML on demand / default | Exporter committed; runtime NOT TESTED |
| Residual Java/Docker/Android directories | Present | Still present, strict gate reports a blocker | Source existence confirmed; retirement NOT DONE |

**Current IRON verdict:** no runner images in the draft branch; ISO C++20 policy and POSIX T-CROSS source changes committed. New local tests and disassembly are unobserved, residual-surface and REST reviews remain open, and PR #19 remains draft/unmerged. This section is a methodology/source-change record, **not** certification of passing gates.
