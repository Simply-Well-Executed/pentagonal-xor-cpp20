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

## Native-only dependency and transport policy

- [ ] No GitHub Actions workflow, runner (hosted or self-hosted), VM/container image, Node, MCP integration, remote build service, runtime download or CI API invocation was introduced.
- [ ] Calls and transitive dependencies in changed code were inventoried; network assumptions were not inferred from names alone.
- [ ] No unexplained change was made to algorithm identity, public API contracts, original input records or experimental protocol.
- [ ] Historical evidence is preserved and any retired behavior is labeled explicitly.

## Review and publication

State what passed, what failed, what was not run, and whether the PR should remain draft. A historical GitHub Actions result is not a current local-native test. No release or merge is implied by this checklist.
