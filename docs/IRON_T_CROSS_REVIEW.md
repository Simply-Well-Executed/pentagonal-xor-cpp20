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
