# Release pre-flight: no paused code and usable audit APIs

> **Document status: DUE FOR UPDATE (2026-11-19).** Verify claims against code and tests; sharpen active verbs; check copyright, project-mark language, accessibility, and 0BSD scope. This mark is not a failure verdict. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Status:** active release validation, not a rewriting of prior results.

## Scope

The repository-wide review covers the following tracked surfaces:

- Production executable: `pentagonal_xor_cpp20.cpp`
- Public computational APIs: `shared/pentagonal_xor_std_free.h`,
  `shared/deca_sigma.hpp`, `shared/tri_pi.hpp`, `shared/coreao.hpp`
- Shared guard: `shared/swe_guard.cpp` and its C11 test
- Root and shared CMake targets, all C++ tests, PowerShell CI scripts
- Both GitHub Actions workflows, package staging, installed headers,
  README and technical/legal documentation

## Release safety rules

`ci/Test-ReleaseSafety.ps1` scans the complete C/C++ and PowerShell
codebase in both workflows **before building**. It rejects:

- Runtime debugger traps such as `DebugBreak`, `__debugbreak`,
  `__builtin_trap`, `SIGTRAP`, and machine-code `int 3` markers
- Deliberate execution-pausing operations, interactive prompts or
  inactive `#if 0` source blocks
- Debug-only conditional source branches using `_DEBUG`
- The retired pause-related source/variable terminology

Failures stop that CI job and are retained in its logs. The existing
`if: always()` evidence collection still attempts to publish source,
toolchain and partial-output evidence. The scan does not silently
remove source statements: an offender requires a separate reviewed commit.

**Not breakpoints:** `const`, `constexpr`, `static_assert`, normal
range/domain guards, GitHub artifact retention and debug information.
`static_assert` is a compile-time invariant check. `/Zi`,
`/DEBUG:FULL`, `-g`, and generated PDB/debug files help inspect
failures without pausing the released executable. These remain permitted.

## The shipped, callable audit APIs

Before this review, the Triπ and COREAO algorithms were compile-tested
but their public headers were missing from the installed release package.
That was a **distribution gap**, not a runtime debugger breakpoint.

CMake now installs:

    include/swe/pentagonal_xor_std_free.h
    include/swe/deca_sigma.hpp
    include/swe/tri_pi.hpp
    include/swe/coreao.hpp
    lib/cmake/SWEAuditHeaders/SWEAuditHeadersTargets.cmake

The installed CMake target `swe::swe_audit_headers` exposes the public
include directory and declares C++20. These algorithms are header-only:
the package consumer compiles them directly, without adding a second
parity CLI mode or needing a separately built audit runtime binary.

Every root build matrix job stages its package, compiles an independent
consumer in `tests/consumer` **against the installed target and
headers**, then executes the example. The consumer checks the original
DecaSigma return witnesses, the three Triπ route inspections, COREAO's
single scalar, and the public parity API.

## Local reproduction in PowerShell 7

```powershell
./ci/Test-ReleaseSafety.ps1

cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure

cmake --install build --config Release --prefix stage
./ci/Test-InstalledAudit.ps1 -Stage stage
```

On Windows, use the normal generator and an appropriate architecture.
On other platforms, CMake's native generator is used.

## Unchanged architectural boundaries

- The production Pentagonal XOR executable has **one closed-form
  calculation**, not a switchable parity engine or debug-only branch.
- DecaSigma, Triπ, and COREAO are shipped as usable header-only APIs
  and keep their own domain checks.
- CORE remains the original source. Derived routes are read-only audits,
  not a new source of identity, authority, or cryptographic assurance.
- Compiler and debug-symbol fingerprints remain available under
  `docs/BUILD_EVIDENCE.md`. No third-party compiler is relicensed 0BSD.
- Earlier failed Windows compiler logs remain preserved in PR #3, with
  separate corrective commits and validation runs.

**Release decision:** a clean source scan, successful CMake/CTest,
successful installed-consumer execution, and uploaded failure/success
evidence are required. These conditions must be evidenced by CI,
not assumed from the source diff.

## Version 0.2.0 release-candidate additions

The installed CMake package additionally contains `include/swe/stability10d.hpp`
and `include/swe/coreo_stereo.hpp`; the downstream consumer compiles both.
Independent CTest targets cover exact six-byte golden vectors, independent
odd-parity minterms, the PR #17 Coreo stereo fixture, tampering and invalid
inputs. All original C++20/C11 tests remain enabled.

The ship workflow adds an Android Java 17 build lane with JUnit, lint and
an **experimental debug APK**. The release manifest fingerprints the APK
alongside all seven C++ archives. Neither this debug build nor the numeric
Stability score proves word recognition, production Android signing or
a speech-to-text model. Evidence is per commit, failures must remain in
Actions history, and no new release tag is created before verification.


## Compile-time negative witnesses and fail-closed policy (2026-11-16)

Review of the feature/pentagonal-gates-cor-vista-20261116 CI history identified a shared release gate blocker: two opt-in, deliberately unsafe Windows API no-op stub declarations and one unevaluated C++20 requires expression were mistaken for runtime pauses. They are retained as historical negative-test evidence, not authorized production Windows APIs or a performance optimization.

The checker now recognizes only those three exact full source statements at three exact paths for the explicit-pause rule. Every other line, unexpected signature or code addition is rejected. No folders, source files or other rules are excluded. In particular a real pause, debugger trap, disabled source block or an extra executable API call still fails closed. The canonical source mathematics are untouched.

Run ci/Test-ReleaseSafetyPolicy.ps1 for a PowerShell 7 isolated regression: it tests all three known witnesses and multiple real rejected cases in a temporary sandbox, then removes the synthetic files. CTest invokes this check on hosts with PowerShell 7, and the standalone Android workflow invokes it directly. Historical cancelled runs remain evidence, not proof of an actual source defect.
