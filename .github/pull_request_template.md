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
