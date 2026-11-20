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

## STD-free high-throughput XOR branch continuation

**Decision: SOURCE-MODULE MIGRATION COMMITTED; RUNTIME VALIDATION PENDING; WHOLE-REPO GATE BLOCKED.** This is a new T-CROSS continuation after the prior frozen source-diff ledgers. Historical numeric +/- counts above are unchanged and must not be silently promoted to counts for this newer change set.

**Frozen source-before reference:** `45bd2c004427906ed8fb5ba9a2056ed904ab65d3`, on draft PR #19 before the STD-free conversion. Current after-revision must be taken from the actual Git HEAD at review time. `sh ci/t-cross-posix.sh emit-diff-yaml > .iron-t-cross-diff.yml` is the authority for current source added/removed counts; no invented ledger values.

| T-CROSS dimension | - Before | + Candidate | Evidence/status |
| --- | --- | --- | --- |
| Native memory aliases | `std::size_t`, `std::int64_t`, library headers | `unsigned char`, `int`, `long long`, `decltype(sizeof(0))`; compiler-width static assertions | Source inspected |
| Fixed buffers / borrowed views | `std::`-based pointer/array facades | `FixedBuffer`, `ByteSlice`, `ConstByteSlice`, `StringSlice` in `include/pxor_memory.hpp` | Source inspected |
| 8-byte XOR | `std::memcpy` payload/key load and store | C++20 constexpr `pack_u64_le`/`unpack_u64_le` and 0..7-byte tail | Source inspected; optimized machine code NOT TESTED |
| String obfuscation | `std::is_constant_evaluated` conditional cleanup | C++20 compile-time cipher; runtime-only `void` scoped callback with volatile buffer cleanup | Source inspected; cleanup not cryptographic |
| CLI storage | `std::vector`, `std::unique_ptr`, `std::chrono`, `std::printf` | 16 MiB fixed buffer, native Win32/POSIX page mapping/output/clock, bounded manual decimal parsing | Source inspected; Windows/Linux runtime NOT TESTED |
| Test dependencies | `std::array`, `std::vector`, `std::span` and traits | Built-in arrays, C++20 core language compile-time assertions, runtime regression return codes | Source inspected; CTest NOT TESTED |
| Downstream install | Throughput header only | Throughput header **plus** `pxor_memory.hpp` | CMake source inspected |
| Other repository modules | Previously used standard-library code | Left unchanged, explicit `gate-no-std-repository` failure until migrated | **WHOLE-REPOSITORY NOT CERTIFIED** |
| Runtime linkage | Not inspected | New `gate-no-std-binary` inspects direct C/C++ standard runtime imports | **NOT TESTED**; may fail because POSIX calls depend on libc |

**Gate semantics:** `gate-no-std-throughput` examines four named source files. `gate-no-std-repository` searches all tracked C/C++ source and header paths and fails when legacy `std::` references remain. `gate-no-std-binary` checks compiled throughput CLI runtime imports, including C runtime dependencies, but even a clean direct import table is insufficient to prove full absence of dynamically resolved or transitive dependencies. The default YAML Taskfile and POSIX executor include these gates; any unrun gate is **NOT TESTED**, not automatically green.

**Preserved:** The established odd-parity probability formula and CLI remain intact, historical Docker/Java source remains under review, no GitHub Actions workflow was added, and the PR remains **DRAFT / UNMERGED**. Removing standard-library APIs from a newly added independent throughput module does not establish machine-level zero-CALL, secure secrets, memory isolation or whole-repository freestanding compliance.

Canonical source-level contract: [STD-free native T-CROSS](STD_FREE_NATIVE_T_CROSS.md). The full [throughput engineering protocol](PENTAGONAL_XOR_THROUGHPUT.md) contains historical design provenance and a superseding pointer.

## Pentagonal Suite T-CROSS +&-DIFF frozen source snapshot

**Status: VERIFIED GIT DIFF, SOURCE INSPECTION COMPLETE, COMPILE AND RUNTIME NOT TESTED.** This is a new, scoped source ledger, not a revision of earlier historical T-CROSS counts. Changes after the candidate commit must generate a new record.

- **CORE (before):** `9f678c13d02e55eb736838c413d41e49dd864f6c`, draft PR #19 prior to Pentagonal Suite additions.
- **CANDIDATE (after):** `2aca633391d8fafc589f022f35f49316e49b709b`, includes three comment-free suite sources, CMake targets, POSIX/YAML gates and supporting documentation.
- **Authority:** [actual Git compare](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/compare/9f678c13d02e55eb736838c413d41e49dd864f6c...2aca633391d8fafc589f022f35f49316e49b709b). **10 files, 11 commits, +647 / -37, net +610 source lines**. These are Git line counts, not memory/performance or machine-code measurements.
- **Future reports:** `sh ci/t-cross-posix.sh emit-diff-yaml > .iron-t-cross-diff.yml` derives fresh `before`, `after`, per-file additions/deletions and totals from the actual local Git state; frozen figures below must not masquerade as a future head.

| Source path | Git status | + added | - removed |
| --- | --- | ---: | ---: |
| `.github/pull_request_template.md` | modified | +9 | -0 |
| `CMakeLists.txt` | modified | +21 | -0 |
| `CONTRIBUTING.md` | modified | +2 | -1 |
| `README.md` | modified | +8 | -0 |
| `Taskfile.yml` | modified | +10 | -4 |
| `ci/t-cross-posix.sh` | modified | +46 | -32 |
| `docs/PENTAGONAL_SUITE.md` | added | +57 | -0 |
| `include/pentagonal_suite.hpp` | added | +259 | -0 |
| `src/suite_main.cpp` | added | +78 | -0 |
| `tests/pentagonal_suite_test.cpp` | added | +157 | -0 |
| **TOTAL** | **10 files** | **+647** | **-37** |

### Nine-operation implementation and acceptance

| T-CROSS subject | - Baseline `9f678c13` | + Candidate `2aca6333` | Evidence |
| --- | --- | --- | --- |
| Bitwise operation family | Existing exact XOR and periodic LUT XOR only | Adds OR, AND, NAND, NOP, NIL as separate exact-seeded and periodic functions | Source inspected, CTest NOT RUN |
| Quiet-NaN payloads | No suite NaN boxing | `pentagonal_nan_bits`, `extract_nan_payload`, `is_pentagonal_nan`, complete 8-byte LE boxes and 0xFF tail | Source inspected, CTest NOT RUN |
| Stateful generator | No bounded COR | `PentagonalCor` uses remaining count and checked exhaustion, no `start+count` overflow | Source inspected, CTest NOT RUN |
| Lazy counted range | No suite VISTA | `PentagonalVista` clamps at the 64-bit index boundary and supports indexed/iterator/subvista access | Source inspected, CTest NOT RUN |
| Canonical XOR and keystream | Existing `pentagonal_xor.hpp` authority | Suite imports existing `pentagonal_xor.hpp`; no duplicate `PENTAGONAL_LUT` or `fast_xor_buffer` | Source comparison inspected |
| Suite source STD usage | Previous suite sources absent | All three new suite sources: zero `std::` names and no C/C++ standard-library includes | Static source check; native audit command NOT RUN |
| Source comments | Previous suite sources absent | All three new suite sources have zero `//` or `/*...*/` comments | Static source check; native audit command NOT RUN |
| Native CLI and CTest | Existing parity/throughput CLIs | Adds independent 16 MiB Suite CLI and `pentagonal_suite_operations` plus `pentagonal_suite_cli_smoke` CTests | Targets present, build/CTest NOT RUN |
| Whole-repository STD and binary imports | Historical C/C++ STD usage and POSIX/Windows system-library dependencies | Remain in place; strict global STD and binary gates remain blocking | **NOT CERTIFIED** |

**Special limitations:** `is_pentagonal_nan` detects a quiet-NaN header pattern, not cryptographic origin or mathematical inverse provenance. NIL zero-fill is not guaranteed secure memory erasure. The NOP mode performs no memory accesses and has O(1) semantics, not a guaranteed constant-duration full-buffer benchmark. CPU affinity and moderate priority are best-effort hints only. COR's 64-bit *index* bounds do not guarantee arbitrary-precision pentagonal *values*. Integer qNaN bytes are not interpreted as floating-point values in the implementation.

**Execution boundary:** No GitHub Actions workflow, hosted/self-hosted runner or container was added to this PR. Existing unrelated C++ headers and algorithms were not deleted. **PR remains DRAFT / UNMERGED. Release, performance, compiler portability, object disassembly, zero-runtime-import and whole-repository STD-free certification remain HOLD pending direct evidence.**

## Native OS matrix and repository-wide STD-free T-CROSS continuation

**Status: VERIFIED GIT SOURCE DIFF / OS BUILD NOT TESTED / WHOLE-REPOSITORY STD-FREE BLOCKED.** This snapshot is independent of previous frozen T-CROSS records. No original algorithms or historical evidence are retired by documenting it.

- **CORE baseline:** `51d12b15f7df9f3ec443f7d5a54da9278006e69b` before the native OS/ABI build bridge.
- **CANDIDATE:** `8e8da3dee92c9c9b37c41df3097296ab886e01ab` prior to this audit-only commit.
- **Authority:** https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/compare/51d12b15f7df9f3ec443f7d5a54da9278006e69b...8e8da3dee92c9c9b37c41df3097296ab886e01ab
- **Frozen totals:** 19 commits; 15 files; **+886 / -23 lines; net +863**. These are Git source counts, not compile/CTest/benchmark results.
- **Current baseline tree:** 184 tracked blobs, 73 C/C++ source/header paths, and zero `.github/workflows` paths at frozen candidate SHA. Subsequent audit changes must not silently revise those figures.

| File | Status | Lines added | Lines removed |
| --- | --- | ---: | ---: |
| `.github/pull_request_template.md` | modified | +12 | -1 |
| `.gitignore` | modified | +1 | -0 |
| `CMakeLists.txt` | modified | +60 | -2 |
| `CONTRIBUTING.md` | modified | +10 | -2 |
| `README.md` | modified | +14 | -4 |
| `Taskfile.yml` | modified | +65 | -1 |
| `ci/Build-PlatformMatrix.ps1` | added | +150 | -0 |
| `ci/Build-PlatformMatrix.sh` | added | +114 | -0 |
| `ci/Export-StdFreeInventory.sh` | added | +69 | -0 |
| `ci/native-platform-matrix.yml` | added | +113 | -0 |
| `ci/t-cross-posix.sh` | modified | +17 | -6 |
| `docs/NATIVE_PLATFORM_MATRIX.md` | added | +89 | -0 |
| `include/pentagonal_xor.hpp` | modified | +12 | -7 |
| `src/parity_std_free_main.cpp` | added | +110 | -0 |
| `tests/std_free_portable_probe.cpp` | added | +50 | -0 |
| **TOTAL** | **15 files** | **+886** | **-23** |

### Explicit T-CROSS source and platform gates

| Dimension | Before | Candidate | Acceptance status |
| --- | --- | --- | --- |
| macOS | No native macOS CLI backend or fixed matrix | arm64, x86_64 and dual-architecture universal core archives; native POSIX I/O and clock | Configured; native Xcode build and hardware execution NOT TESTED |
| macOS 32-bit Intel | No dedicated status | i386 explicitly UNSUPPORTED for modern macOS (10.15+) | Not advertised as working |
| Windows | x64 Windows verifier only | Local MSVC Win32, x64/AMD64 aliases, ARM64 archives; operator-verified exact PowerShell | Configured; local MSVC build NOT TESTED |
| Android | Historical Java/NDK scripts and container sources | Four installed-NDK ABI static archive profiles without an APK or runtime | Configured; cross compilation NOT TESTED |
| Linux | General project CMake | local x86_64, ARM64 and native 32-bit x86 archive profiles | Configured; build NOT TESTED |
| STD-free core build | Full legacy repository by default | Explicit SWE_STD_FREE_CORE_ONLY=ON early CMake archive target and compile-time assertions; default full build retained | Source inspected; compile NOT TESTED |
| Original parity | Existing std::stod / iostream canonical CLI | Preserved, plus alternative STD-free native parity CLI and six parity/invalid CTests | New CLI NOT TESTED, numeric grammar not asserted identical |
| Source STD audit | Legacy STD references present | Git-derived per-file YAML inventory and ten-source core-only gate | Inventory generator committed; entire repository still BLOCKED |
| Network/runner images | No workflows on prior PR branch | Zero hosted/self-hosted workflow YAML remains, no fetch or binary download in new entrypoints | Source inspected; dynamic isolation NOT TESTED |
| Binary dependencies | No complete linker/import audit | Static archive plus platform-specific output; CRT/libc imports not proven absent | NOT CERTIFIED |

### Frozen acceptance and custody

1. Core-only is a build scope, not a declaration that the 73 tracked C/C++ paths are STD-free. The original math and legacy files remain available under their existing paths. No archive, rename, deletion or wholesale bypass is used to fake repository compliance.
2. macOS i386 is unsupported on modern macOS. Windows AMD64 and x86-64 use one canonical x64 architecture. Android builds produce installed-NDK static archives, not APKs.
3. Missing SDKs, dirty tracked source and mismatched version assertions block the local scripts. Their YAML output records actual local source commit, host, stage and status. No profile is green merely because it exists in a manifest.
4. After this snapshot, run `sh ci/t-cross-posix.sh emit-diff-yaml` and `sh ci/Export-StdFreeInventory.sh` to derive current, rather than historical, counts.
5. Actual CMake configure/build, native CTest, toolchain identification, unsupported ABI negative controls and runtime import/zero-CALL checks remain **HOLD / NOT TESTED**.

Canonical references: [native platform protocol](NATIVE_PLATFORM_MATRIX.md), [platform YAML](../ci/native-platform-matrix.yml), [STD inventory exporter](../ci/Export-StdFreeInventory.sh), [POSIX executor](../ci/Build-PlatformMatrix.sh), [Windows executor](../ci/Build-PlatformMatrix.ps1).

## T-CROSS: operator-authorized post-GAMP4 Docker and native multi-OS release candidate

**State: source-only comparison verified; release HOLD.** New independent section, with all previous T-CROSS numerical history retained.

- Baseline: `cc0f789fee53a6c22e86c31d45b822f7716699f0` on main following merged PR #19.
- Candidate: `18e52a3739e27721ee790c645a3a47a451be83ff` on draft PR #20 before this audit append.
- Authority: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/compare/cc0f789fee53a6c22e86c31d45b822f7716699f0...18e52a3739e27721ee790c645a3a47a451be83ff
- Source delta: 22 commits, 21 files, **+1130 / -2 lines, net +1128**. These are Git source counts, not runtime or validation measurements.
- Subsequent commits must not be silently represented by this frozen candidate.

| Source path | Status | Added | Removed |
| --- | --- | ---: | ---: |
| `.github/pull_request_template.md` | modified | +16 | -0 |
| `CONTRIBUTING.md` | modified | +13 | -1 |
| `README.md` | modified | +8 | -0 |
| `Taskfile.yml` | modified | +46 | -0 |
| `ci/Assemble-Gamp4-Release.sh` | added | +97 | -0 |
| `ci/Build-Release-OciPayload.sh` | added | +40 | -0 |
| `ci/Build-Windows-ReleaseContainer.ps1` | added | +104 | -0 |
| `ci/Invoke-LocalDocker-Gamp4.sh` | added | +85 | -0 |
| `ci/Run-Native-Macos-Gamp4.sh` | added | +117 | -0 |
| `ci/Run-Native-Windows-Gamp4.ps1` | added | +147 | -0 |
| `ci/docker/Build-Android-NDK.sh` | added | +74 | -0 |
| `ci/docker/Run-Gamp4-Container.sh` | added | +104 | -0 |
| `ci/gamp4-docker-release.yml` | added | +75 | -0 |
| `ci/native-platform-matrix.yml` | modified | +6 | -1 |
| `docker/ReleasePayload.Dockerfile` | added | +7 | -0 |
| `docker/WindowsGamp4Smoke.Dockerfile` | added | +11 | -0 |
| `docker/compose.release.yml` | added | +55 | -0 |
| `docs/GAMP4_DOCKER_RELEASE.md` | added | +116 | -0 |
| `docs/GAMP4_VALIDATION.md` | modified | +3 | -0 |
| `docs/NATIVE_PLATFORM_MATRIX.md` | modified | +3 | -0 |
| `docs/RELEASE_PREFLIGHT.md` | modified | +3 | -0 |
| **TOTAL** | **21 files** | **+1130** | **-2** |

### T-CROSS gate mapping

| Gate | - Previous | + Candidate | Evidence |
| --- | --- | --- | --- |
| Container policy | Earlier absolute Docker ban | Explicit operator-requested local Docker release-only exception; no hosted runner | Policy diff inspected |
| Provenance | Earlier unpinned/fetch-enabled Docker experiments | New Compose: cached digest, pull never, local daemon, source RO, no network, non-root, caps dropped | Source inspected, Docker NOT RUN |
| Linux | Earlier native/experimental artifacts | Full CTest plus exactly URS-11..22 each for amd64, arm64, i386 | Scripts committed; no runtime evidence |
| Android | Historical APK and core paths | Four NDK archives require exact-SHA Linux source GAMP4, no Android OQ claim | Build code committed, NOT TESTED |
| macOS | Native source profiles | Native Apple Clang GAMP4 + CTest, arm64/x86_64 receipts prior to universal | Code committed, NOT TESTED |
| Windows | MSVC source profiles | Native x86/x64/ARM64 GAMP4 + CTest; optional cached Windows x64 container smoke | Code committed, NOT TESTED |
| Distribution | Historical previews | Thirteen SHA-bound package checksums + release notes; local scratch OCI payload transport only | Release not assembled or published |
| Runtime STD | Legacy standard headers remain | Opt-in core archive stays separate from full STD-backed GAMP4 test suite | Whole repo STD-free remains BLOCKED |

**Authority boundary:** Supplier-side GAMP4-style automated checks are not GMP certification, operational PQ, independent QA authorization or authentication of an untrusted evidence host. A Mac artifact inside OCI does not run a Mac kernel; Android archives are not APK or device tests. Windows images require matching hosts. macOS 32-bit i386 remains unsupported. Windows AMD64 and x86-64 are aliases of x64.

**Current observations:** No Docker daemon available in assistant environment, no approved preloaded builder image digests, no native Windows/macOS target run, no 12-URS runtime report for this candidate, no 13-profile release artifact, no registry push and no published GitHub release. This is a deliberate HOLD, not a passing release.

**Canonical protocol:** [GAMP4 Docker release custody](GAMP4_DOCKER_RELEASE.md), [13-profile YAML](../ci/gamp4-docker-release.yml), [local Docker Compose](../docker/compose.release.yml), [release assembler](../ci/Assemble-Gamp4-Release.sh).

## T-CROSS follow-up: sanitized local Docker source isolation

**FROZEN SECURITY SUB-DELTA, SOURCE ONLY.** This record is separate from the preceding +1130/-2 Docker implementation snapshot. It does not erase or merge its provenance.

- Baseline Git SHA: `18e52a3739e27721ee790c645a3a47a451be83ff`.
- Candidate Git SHA: `23553ce0065bec6cdd34f29f6407c13ecf06553f`.
- Authoritative comparison: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/compare/18e52a3739e27721ee790c645a3a47a451be83ff...23553ce0065bec6cdd34f29f6407c13ecf06553f
- **16 commits, 11 files, +131 / -25 lines, net +106**. Figures represent actual source lines only; no test, Docker run or release outcome implied.

| Changed path | Added | Removed |
| --- | ---: | ---: |
| `README.md` | +1 | -1 |
| `ci/Assemble-Gamp4-Release.sh` | +1 | -1 |
| `ci/Invoke-LocalDocker-Gamp4.sh` | +44 | -3 |
| `ci/Run-Native-Macos-Gamp4.sh` | +2 | -2 |
| `ci/Run-Native-Windows-Gamp4.ps1` | +4 | -0 |
| `ci/docker/Build-Android-NDK.sh` | +4 | -6 |
| `ci/docker/Run-Gamp4-Container.sh` | +5 | -7 |
| `ci/gamp4-docker-release.yml` | +4 | -1 |
| `docker/compose.release.yml` | +2 | -2 |
| `docs/GAMP4_DOCKER_RELEASE.md` | +10 | -2 |
| `docs/IRON_T_CROSS_REVIEW.md` | +54 | -0 |
| **TOTAL** | **+131** | **-25** |

**Security correction:** the initial Compose proposal bound the entire checkout read-only. The candidate instead exports committed source using local `git archive HEAD`, excludes `.git` and untracked source, records `.swe-source-commit`, performs host SHA256 inventory verification and symlink/extra-file checks, and mounts that sanitized snapshot read-only. Docker's writable bind mount is restricted to ignored `build/gamp4-release/`, *separate* from the host snapshot under `build/gamp4-source-snapshots/`; consequently, a container cannot simply mutate its read-only source via the writable output alias. The local builder image must already be digest-pinned and cached; neither downloading an image nor remotely publishing a release is authorized.

**Additional gate refinements:** native macOS and Windows GAMP4 scripts now require a **single terminal** twelve-URS PASS summary. The Android validation stage reads only the sanitized source marker. The release packager uses portable `find` rather than `-maxdepth` where possible.

**Residual risks:** operator workstation, local Docker daemon and builder image remain trusted; a compromised daemon can defeat bind-mount protections. Git source snapshots and unsigned local YAML receipts are *not* independently authenticated supply-chain attestations. Actual full native CTest/URS, Docker builds, NDK device OQ, native hardware testing, third-party license review and human QA remain NOT TESTED / HOLD. Any new changes after the frozen candidate must generate a new diff.

## T-CROSS: Docker Hub historic source-study immutable-digest promotion target

**State: GIT SOURCE TARGET RECORDED / DOCKER HUB MANIFEST NOT VERIFIED / PUBLICATION HOLD.** This independent +/-DIFF does not alter earlier frozen T-CROSS records.

- Baseline: `c7647832fa076891a4f877619b922b6c2a4182d4`, PR #20 immediately before new Docker Hub target request.
- Candidate: `d3fbc942e015de5a2ef2b031abb6f933b20aa1ae`, before this documentation-only audit append.
- Authority: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/compare/c7647832fa076891a4f877619b922b6c2a4182d4...d3fbc942e015de5a2ef2b031abb6f933b20aa1ae
- **10 commits, 9 files, +346 / -182 Git lines, net +164**. This is source change, not runtime proof.

| Changed file | Status | Additions | Deletions |
| --- | --- | ---: | ---: |
| `README.md` | modified | +6 | -0 |
| `Taskfile.yml` | modified | +10 | -0 |
| `ci/Assemble-Gamp4-Release.sh` | modified | +8 | -0 |
| `ci/Publish-AndroidSourceStudy.ps1` | added | +154 | -0 |
| `ci/Run-DockerValidation.ps1` | modified | +3 | -0 |
| `ci/Test-DockerPublishPolicy.ps1` | modified | +100 | -181 |
| `ci/dockerhub-source-study-promotion-approval.example.json` | added | +14 | -0 |
| `ci/gamp4-docker-release.yml` | modified | +10 | -1 |
| `docs/GAMP4_DOCKER_RELEASE.md` | modified | +41 | -0 |
| **TOTAL** | **9 files** | **+346** | **-182** |

### Source identity and destination, never conflated

- Requested mutable tag: `paulcheek/android-source-study-v1:latest`.
- Requested immutable companion: `paulcheek/android-source-study-v1:sha-07eb2a4561f0`.
- GitHub historic source SHA: `07eb2a4561f0f5e82db4668d2b4fa453e2a4d733` (Git commit lookup, corresponds to preview `preview-319-07eb2a4`).
- **Operator-claimed** manifest digest: `sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30`.
- Docker Hub current source-tag registry manifest, local `RepoDigest`, mutable `latest` state, publisher authentication, actual push and post-push digest: **NOT VERIFIED / NOT TESTED**.
- PR #20 candidate source SHA differs from historical `07eb...`; new matrix GAMP4 results cannot retroactively certify this older image.

### Changed gates and negative controls

| Gate | Before | Candidate | Measurement |
| --- | --- | --- | --- |
| Docker target | Existing legacy script pushed mutable latest from runtime CI variables | New fixed source/target/digest in local GAMP4 release YAML and standalone PowerShell publisher | Source committed, registry NOT CHECKED |
| Manifest | Image ID could be confused with registry manifest | Publisher uses Docker Buildx registry manifest inspection, exact digest, single-platform check and local matching RepoDigest | Source checked, Docker NOT RUN |
| Validation | Old `-Mode Publish` could run after trusted-main environment assertions without new exact-source GAMP4 evidence | Retired old entrypoint; new path requires historical Git SHA, supplier 12-URS/CTest evidence hashes plus explicit QA/license/SBOM/source-origin/operator approvals | New gate source inspected; evidence NOT SUPPLIED |
| Permission | Generic mutable tag update | Requires literal acknowledgement and `DOCKERHUB_USERNAME=paulcheek` before local `docker tag`/`docker push`, re-reads latest before mutation | Not executed |
| Post-push | Old source only wrote image ID and claimed push | New script re-reads remote latest manifest and throws deviation if digest differs | NOT TESTED |
| Negative controls | Legacy mock expected unaudited pushes | Replaced with mock-only tests: legacy blocked, read-only inspection, missing approval, source digest mismatch, multi-platform index rejection | Test file committed, test NOT EXECUTED |
| Release notes | Generic 13-profile release listing only | Adds distinct historic Docker Hub target and unverified digest, no false publication | Source committed |

**Release HOLD** pending verified Docker Hub manifest and accessible local Docker engine, actual historical source-matched GAMP4/CTest evidence, independent QA and licensing authorization, target image provenance, registry authentication, operator push decision and post-push digest readback. The historic image is not a Docker macOS/Windows/Android runtime. No repository tag, Docker Hub tag, release binary or source content was actually published during this update. PR #20 remains draft and unmerged.

**Canonical entrypoints:** [publication script](../ci/Publish-AndroidSourceStudy.ps1), [unapproved release approval example](../ci/dockerhub-source-study-promotion-approval.example.json), [GAMP4 Docker release policy](GAMP4_DOCKER_RELEASE.md).
## T-CROSS: all-family gain/dispersion/stability exact-token lexicon

**Source audit only / compiler and CTest execution NOT OBSERVED.** A separate exploratory record, preserving preceding frozen audits.

- Base: `cc0f789fee53a6c22e86c31d45b822f7716699f0` from main, after the native OS core transition.
- Candidate: `d9408bdfe6e1b4db0bc5dc457e4e4466863fd9b3` before this audit documentation append.
- [Exact Git comparison](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/compare/cc0f789fee53a6c22e86c31d45b822f7716699f0...d9408bdfe6e1b4db0bc5dc457e4e4466863fd9b3): 15 commits, 8 changed files, **+499 / -3 lines, net +496**. These are Git source counts, not measured acceptance.

| Changed file | Status | Added | Removed |
| --- | --- | ---: | ---: |
| `CMakeLists.txt` | modified | +12 | -0 |
| `README.md` | modified | +7 | -0 |
| `Taskfile.yml` | modified | +1 | -1 |
| `ci/t-cross-posix.sh` | modified | +4 | -2 |
| `docs/EXPLORATORY_OPERAND_MODES.md` | added | +59 | -0 |
| `shared/quantum_operand_lexicon.hpp` | added | +277 | -0 |
| `tests/quantum_operand_lexicon_test.cpp` | added | +125 | -0 |
| `tests/std_free_portable_probe.cpp` | modified | +14 | -0 |
| **TOTAL** | **8 files** | **+499** | **-3** |

### Algebraic safety and provenance boundaries

- Existing fourteen literal source operands have distinct identities, with three conceptual semantic slots each: 14 × 3 = 42. Explicit notation coverage does not equal complete numerical semantics.
- Twenty-two exact source records: fourteen historical/user-supplied originals, four user-specified coefficient mode variants `-p`, `-P`, `=p`, `=P`, and four inferred shift variants `-/`, `=/`, `-\`, `=\`. Inferred glyphs are tagged `Status::ProposedNotation`, `Notation::Inferred`; no rule labels them confirmed.
- `+/` is user-associated with shift up/inverse-square and `+\` with shift down/root-of-square, superseding *earlier interpretation labels* but preserving old literal/provenance. No numerical evaluator or physics law is claimed.
- The `quantum_operand_exact_symbol_registry` CTest and native core static assertions check exact bytes, case, 42 semantic slots, inferred notation status, Unicode full-width equals as a negative, and invalid enum IDs. New source has no `std::` or C++ standard header imports. The legacy parity engine and Docker release PR are untouched.
- T-CROSS whole-repository STD-free remains a separate blocked condition. No compiler, target OS test, zero-call disassembly, new GAMP4 run, Docker execution, or release publication was observed for this candidate.

**State: DRAFT / HOLD pending actual C++20 build and CTest.** No merge or original mathematical record replacement is authorized by this source-ledger update.
