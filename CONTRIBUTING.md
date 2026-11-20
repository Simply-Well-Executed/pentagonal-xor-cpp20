# Contributing

Thank you for improving the Pentagonal Recursive XOR Engine.

## Native-only execution rule (IRON)

- Keep project behavior native C++20, with CMake extensions ON and MSVC `/O2` (GCC/Clang `-O2`) only. Follow the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#main).
- Use the latest stable locally installed PowerShell Core and `Set-StrictMode -Version 2.0` for verification scripts. Follow the [PowerShell beginners reference](https://www.reddit.com/r/PowerShell/wiki/beginners/).
- **Do not add GitHub Actions workflows, GitHub-hosted or self-hosted runner jobs, VM/OCI/container images, Node/npm-based actions, runtime downloads, MCP client/server dependencies, remote build services, or CI API calls.** This rule applies to new proposed work and does not silently delete historical source or records.
- Local Git commands used to inspect already checked-out source are permitted. No automated Git network fetch is performed by the native verifier. A code and binary dependency review is still required before certifying network isolation.
- Do not substitute an unverified PowerShell version as 'latest'. The operator must independently confirm the release and pass the version and matching official release link; the local script fails closed if the installed version differs.

## Review workflow

1. Start from a locally reviewed checkout of the repository and create a topic branch.
2. Make targeted source or documentation changes. Preserve provenance, original algorithm definitions, experimental protocol IDs, and source history.
3. Run `ci/Invoke-NativeCore.ps1` on a locally controlled Windows machine after independently confirming the newest stable PowerShell release. The executable is a PowerShell 7 script; it checks version, runs Magic Comb, configures MSVC C++20 at `/O2`, builds, and runs CTest.
4. Review the local `.iron-native-evidence.json` report and the CTest output. The report records an operator assertion, not a live-verified release fact. Do not include private workstation paths or identifiers in public issue text.
5. Open a pull request with T-CROSS side-by-side evidence and explicit PASS, FAIL, and NOT TESTED labels. Do not merge because a historical GitHub Actions check is green.

## Boundaries

No GitHub-hosted CI or automatic release job is configured on the current native-only PR branch. Historical workflow definitions remain in Git history and may remain on `main` until the approved PR is merged. Manual-use historical Docker and Java sources are not called by the native entrypoint, but remain subject to separate audit.

The project is licensed under [0BSD](LICENSE).
