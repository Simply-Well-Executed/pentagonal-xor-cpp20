<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

## What changed?

Describe the exact native C++20 / PowerShell code or documentation change, affected paths, and source provenance.

## Local native T-CROSS validation

- [ ] Official latest stable PowerShell version was independently verified before local execution.
- [ ] `ci/Invoke-NativeCore.ps1` was run on a locally controlled Windows machine.
- [ ] StrictMode 2.0 and Magic Comb SelfTest / Verify passed.
- [ ] CMake `CXX_STANDARD 20`, `CXX_STANDARD_REQUIRED ON`, and `CXX_EXTENSIONS OFF` verified; MSVC Release `/O2` and GCC/Clang Release `-O3` verified; no O0.
- [ ] CTest checks passed, with actual test count, commit SHA and local evidence identified.
- [ ] POSIX T-CROSS produced a current Git-derived `.iron-t-cross-diff.yml` (base/head SHAs, +/- source lines) or exporter failure was recorded.
- [ ] Baseline versus candidate comparison includes negative controls, limitations and NOT TESTED results.
- [ ] Machine-code CALL gate examines the isolated parity probe object, with CLI and other code explicitly out of that narrow scope.
- [ ] C++ Core Guidelines and PowerShell beginners reference were considered for changed code.

## STD-free T-CROSS

- [ ] New throughput source and test files contain zero standard-library headers, standard library functions, or `std::` names; run `gate-no-std-throughput`.
- [ ] Whole-repository source inventory performed with `gate-no-std-repository`; any legacy residual `std::` is marked FAIL, never interpreted as deleted.
- [ ] Built throughput CLI checked for libc/UCRT and C++ standard-library runtime imports with `gate-no-std-binary`, or clearly labeled NOT TESTED.
- [ ] Native C++20 consteval, constexpr packing/unpacking, scoped plaintext and CLI CTests were executed, including invalid input controls.
- [ ] The existing parity algorithm, historical source and test evidence were not silently changed or reclassified.

## Pentagonal Suite T-CROSS

- [ ] `include/pentagonal_suite.hpp`, `src/suite_main.cpp` and `tests/pentagonal_suite_test.cpp` pass `gate-suite-no-comments` with zero source comments.
- [ ] All ten named native parity, throughput, suite and portable-core sources pass `gate-no-std-throughput` with zero standard-library headers or `std::` calls.
- [ ] CTest validates all exact and LUT OR/AND/NAND modes, XOR reversibility, NOP/NIL, complete NAN blocks and partial tail, COR exhaustion and VISTA end-of-range behavior.
- [ ] Distinguish quiet-NaN bit-pattern recognition from proof of payload provenance, and ordinary zero-fill from guaranteed secure erasure.
- [ ] Preserve established parity and original throughput CLI; record the new suite source delta with the Git-derived T-CROSS +/-DIFF YAML exporter.
- [ ] Treat compiler/linker/runtime results as NOT TESTED until executed on the PR checkout.

## Native OS/ABI matrix (local, fail-closed)

- [ ] Source SHA and host/ABI configuration recorded in `build/iron-matrix/<profile>/iron-build-result.yml`.
- [ ] Target is one of the declared macOS arm64/x86_64/universal, Windows Win32/x64/ARM64, Linux or Android NDK ABI profiles. macOS 32-bit x86 is recorded **UNSUPPORTED** rather than claimed green.
- [ ] Native compiler/SDK exists locally; no runner image, GitHub Actions, network fetch, installer or container was introduced.
- [ ] `SWE_STD_FREE_CORE_ONLY=ON` created an archive and headers with no legacy build target leakage, or exact failure is captured.
- [ ] Cross-compiled archive status `ARCHIVE_BUILT_NOT_RUNTIME_TESTED` is not misreported as hardware execution, CTest pass or CRT-free linkage.
- [ ] `sh ci/Export-StdFreeInventory.sh > .iron-std-free-inventory.yml` completed; unresolved standard-library references remain explicitly BLOCKED.
- [ ] Independent STD-free five-feature parity CLI compared with canonical CLI, including decimal-grammar compatibility and invalid-input test outcomes.
- [ ] Full-repository STD-free goal not marked passed until **all** tracked C/C++ source, public APIs, tests, binaries and linked runtime dependencies have measured acceptance evidence.

## Native-only dependency and transport policy

- [ ] No GitHub Actions workflow, runner (hosted or self-hosted), VM/container image, Node, MCP integration, remote build service, runtime download or CI API invocation was introduced.
- [ ] Calls and transitive dependencies in changed code were inventoried; network assumptions were not inferred from names alone.
- [ ] No unexplained change was made to algorithm identity, public API contracts, original input records or experimental protocol.
- [ ] Historical evidence is preserved and any retired behavior is labeled explicitly.

## Review and publication

State what passed, what failed, what was not run, and whether the PR should remain draft. A historical GitHub Actions result is not a current local-native test. No release or merge is implied by this checklist.

## Local GAMP4 Docker release and 13-profile acceptance

- [ ] Operator-requested Docker exception reviewed independently; old absolute container ban is superseded **only** for this local release workflow, not for hosted runners.
- [ ] No GitHub Actions, remote Docker daemon, unpinned base, image download, new SDK installer, registry push or automated release publisher is present.
- [ ] Linux amd64/arm64/i386 containers have an actual local SHA256-pinned builder, read-only source, no network, non-root user and dropped privileges.
- [ ] Full native CMake and all CTests passed independently on every Linux/macOS/Windows target advertised as GAMP4 tested.
- [ ] Exactly one PASS row for each GAMP4-style URS-11..22, zero FAIL rows, and one PASS summary were observed for each executable native target.
- [ ] macOS arm64/x86_64 were tested on matching native Apple hardware; universal package has both separately verified source-SHA receipts. macOS i386 is UNSUPPORTED.
- [ ] Windows x86/x64/ARM64 native tests were executed on compatible Windows hosts; optional x64 Docker Server Core smoke was not substituted for host validation.
- [ ] Android four NDK ABIs were built using separately licensed installed NDK; report CROSS_COMPILED_NOT_DEVICE_TESTED, never device OQ PASS.
- [ ] All 13 required profile package trees, SHA256 manifests and validation receipts refer to the **same** source commit. Any missing/stale/edited item must block assembly.
- [ ] Third-party OS and SDK licenses, runtime dependencies, SBOM expectations and release-authentication limitations reviewed.
- [ ] `ci/Assemble-Gamp4-Release.sh` produced STAGED_NOT_PUBLISHED after all gates. Actual GitHub release assets, immutable tag and digest are verified separately if later published.
- [ ] Confirmed GAMP4-style supplier verification is **not** independent QA approval, PQ or GMP certification; no status promoted without explicit evidence.

## Accessibility and provenance (required review)

- [ ] Reviewed [ACCESSIBILITY.md](../ACCESSIBILITY.md); described applicable accessibility impact or justified N/A.
- [ ] Verified meaningful labels, logical reading order, keyboard access, text equivalents, index/home and voluntary return as applicable.
- [ ] Recorded actual assistive technology checks and NOT TESTED limitations; no claim of certification.
- [ ] Reviewed unexplained similarity under PENCE; retained source identity, original notices and rights evidence; no unauthorized relicensing or deduplication.
- [ ] Preserved source history, user choice and existing audit data. Material unresolved regressions or rights conflicts are explicitly HOLD.

## PENNYWORTH attribution (required)

- [ ] Every added or changed file has verified attribution and an exact license identifier or path-specific custody record; existing notices and immutable evidence are preserved.
- [ ] PENCE review records any uncertain or overlapping rights; no unauthorized relicense, release or deduplication.
