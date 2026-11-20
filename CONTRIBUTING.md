# Contributing

Thank you for improving the Pentagonal Recursive XOR Engine.

## Native-only execution rule (IRON)

- Keep all targets in required ISO C++20 with extensions OFF. Current explicit Release policy: MSVC `/O2`, GCC/Clang `-O3`; non-Release configurations retain optimized O2. Do not insert O0. Follow the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#main).
- Use the latest stable locally installed PowerShell Core and `Set-StrictMode -Version 2.0` for verification scripts. Follow the [PowerShell beginners reference](https://www.reddit.com/r/PowerShell/wiki/beginners/).
- **Do not add GitHub Actions workflows, GitHub-hosted or self-hosted runner jobs, VM/OCI/container images, Node/npm-based actions, runtime downloads, MCP client/server dependencies, remote build services, or CI API calls.** This rule applies to new proposed work and does not silently delete historical source or records.
- Local Git commands used to inspect already checked-out source are permitted. No automated Git network fetch is performed by the native verifier. A code and binary dependency review is still required before certifying network isolation.
- Use `CXX_STANDARD 20` (numeric language level), `CXX_STANDARD_REQUIRED ON` (boolean strictness), and `CXX_EXTENSIONS OFF` (ISO mode). The parity-core object may satisfy a machine-CALL-free gate only when its disassembly is actually inspected; CLI and test harness calls have separate contracts.
- Do not substitute an unverified PowerShell version as 'latest'. The operator must independently confirm the release and pass the version and matching official release link; the local script fails closed if the installed version differs.

## STD-free source and binary boundaries

- The new throughput engine must use **no C/C++ standard-library includes, `std::` APIs, compiler builtins or implicit heap allocation** in the seven tracked native throughput/suite source files enumerated by `gate-no-std-throughput`. Use C++20 core-language features with native Win32/POSIX APIs where operating-system interaction is unavoidable.
- Run `sh ci/t-cross-posix.sh gate-no-std-throughput` for the new module. Run `gate-no-std-repository` for strict whole-repository inventory and `gate-no-std-binary` for direct runtime imports of the new CLI. These are distinct tests; a pass in one does not make another pass.
- Do not label system libraries such as POSIX libc, Windows UCRT, or compiler-provided runtime support as absent without a compiled binary import/dependency audit. No assumption of heap secrecy, guaranteed NUMA locality, zero machine calls or full zero-CRT operation follows from header-free source.
- The Pentagonal Suite operation family is an additive source module: `include/pentagonal_suite.hpp`, `src/suite_main.cpp`, `tests/pentagonal_suite_test.cpp`. These three files must contain **zero code comments** and remain STD-free; run `sh ci/t-cross-posix.sh gate-suite-no-comments` and `gate-no-std-throughput` before review. Do not replace or silently rename the existing XOR formula, parity executable, or persisted stream identity. See [Pentagonal Suite protocol](docs/PENTAGONAL_SUITE.md).
- The existing parity and historical modules are preserved pending staged, independent migration and regression evidence. See [STD-free T-CROSS](docs/STD_FREE_NATIVE_T_CROSS.md).

## Review workflow

1. Start from a locally reviewed checkout of the repository and create a topic branch.
2. Make targeted source or documentation changes. Preserve provenance, original algorithm definitions, experimental protocol IDs, and source history.
3. The canonical audit command is `sh ci/t-cross-posix.sh`, which generates a fresh local `.iron-t-cross-diff.yml` from the frozen baseline and Git HEAD, then attempts nine fail-closed POSIX gates. The [Taskfile v3](Taskfile.yml) is a declarative alternative; its external Task binary is not required or installed by the project. For the Windows-specific Magic Comb and MSVC /O2 runner, use `ci/Invoke-NativeCore.ps1` only after independently reviewing the latest stable PowerShell release.
4. Review the local `.iron-native-evidence.json` report and the CTest output. The report records an operator assertion, not a live-verified release fact. Do not include private workstation paths or identifiers in public issue text.
5. Open a pull request with Git-derived T-CROSS YAML and side-by-side evidence and explicit PASS, FAIL, and NOT TESTED labels. Do not merge because a historical GitHub Actions check is green.

## Boundaries

No GitHub-hosted CI or automatic release job is configured on the current native-only PR branch. Historical workflow definitions remain in Git history and may remain on `main` until the approved PR is merged. Manual-use historical Docker and Java sources are not called by the native entrypoint, but remain subject to separate audit.

The project is licensed under [0BSD](LICENSE).
