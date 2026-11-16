# GAMP 4-style automated validation and Docker build custody

**Scope:** `.github/workflows/shared-guard.yml`, experimental Pentagonal XOR
C++20/C11, Coreo audio statistics, Android offline acoustic study and
source-only Docker Hub image `paulcheek/android-source-study-v1`.

**Classification:** an internal, risk-based, GAMP 4-*style* validation
protocol. It is **not** a formal GAMP 4 certification, pharmaceutical
software qualification, FDA/EMA validation, or GMP suitability statement.
GAMP 4 is an older methodology; use the contract's actual QMS and current
regulatory requirements when regulated validation is intended.

## Validation strategy

The source algorithm and original measurements are authoritative.
The build, test, fingerprint and manifest are derived evidence. A failed
CI run is retained in GitHub Actions and must not be relabeled passing.
Every run records its actual commit SHA and runner/toolchain identity.

| ID | Risk / user requirement | Design element | Automated verification | Evidence and release gate |
| --- | --- | --- | --- | --- |
| URS-01 | HIGH: preserve five-input XOR mathematics and all 32 Boolean states | `shared/pentagonal_xor_std_free.h` | C11 strict compiler and root parity-reference CTest, seven native platforms plus Docker Linux | `evidence/ctest-root.xml`, `ctest-shared.xml`; fail-closed |
| URS-02 | HIGH: do not silently mutate source during COREAO, TriPi or DecaSigma transformations | C++20 public headers and installed consumer | Complete CTest and post-install compile/run | `test-root.log`, `consumer.log` |
| URS-03 | HIGH: nine-ligand/three-ring Stability, exact six-byte LE witnesses | `shared/stability10d.hpp`, Java `Stability.java` | Independent minterms/negative-domain CTests and 26,654 pure Java assertions | Native+Docker CTest; `build-and-test.log` |
| URS-04 | HIGH: reject model-weight claims and unverified speech recognition | SDK-free source study; no trained model | Tests on invalid frames; no model files; offline Android container execution | Android manifest, APK SHA256, source preflight |
| URS-05 | HIGH: compile and sign experimental Android APK without Gradle | Java 17, Android SDK 35, AAPT2, D8, ZIP alignment, debug signing | Docker Android stage, offline unprivileged run, `apksigner verify` | `app-debug.apk`, manifest, `SHA256SUMS`, red log on failure |
| URS-06 | HIGH: resist malicious branch-based Docker Hub publication | Dedicated publish job, gated by full native and Docker matrix | Only trusted `push` to main/version tags; Docker Hub credentials checked inside job; all PRs blocked | Docker published image ID/digest after successful push |
| URS-07 | HIGH: no unlicensed Google Android SDK redistribution | Two-stage Docker design, publish only `source` target | SDK paths absent in published image, SDK acquired only in temporary validation image | Docker image inspect and offline inspection run |
| URS-08 | MEDIUM: every target's build/test identity and failures traceable | Seven native jobs + Docker Linux/Android | SHA-256, CTest XML, native PDB and compiler fingerprints collected even on failures | 90-day Actions artifacts; review failures |
| URS-09 | MEDIUM: reproducible experimental counterarccosine second-wave phase | `tests/counterarccosine_secondwave_test.cpp` and dedicated SVG | 257 deterministic phase witnesses; mathematical complementary identity | CTest results and white paper |
| URS-10 | MEDIUM: +9 dBi RF calculation physically well-scoped | `shared/coreao_gain.hpp` | Independent `pow(10,0.9)` reference and inverse witness | `coreao_gain_test`; theoretical calculation only |

### Platform and execution environment

| Platform | Actual execution mode | Notes |
| --- | --- | --- |
| Linux x64 and ARM64 | Native GitHub runner | C++20, strict C11, all root/shared tests and installed consumer |
| Windows x64, x86 and ARM64 | Native GitHub runner | MSVC/CMake with debug symbols/PDB evidence; Windows ARM64 now a required guard lane |
| macOS Intel and Apple Silicon | Native GitHub runner | Clang/CMake root/shared and installed consumer |
| Linux x64 source-study Docker | Non-root offline container | C++20/C11 root/shared and consumer verification, no Android SDK |
| Android Gradle-free Docker | SDK acquisition in disposable image; testing in non-root network-disabled container | 26,654 Java checks, AAPT2/D8 packaging, v2/v3 debug signature; Google SDK **never pushed** |

Docker cannot provide native macOS kernels or true native Windows compiler
environments on an Ubuntu runner. Calling those native jobs “Docker-isolated”
would falsify validation provenance; use the actual native runners above.

### Failures, audit limitations and controlled publication

- Pre-flight scans for runtime debugger traps, paused or retired code.
- A genuine failed job remains red; evidence-upload steps use
  `if: !cancelled()` so superseded PR runs can terminate.
- Native compiler executables are SHA256-fingerprinted, not redistributed.
  Locally compiled libraries, binaries and PDB outputs may appear in
  evidence archives according to the project's 0BSD rules.
- Docker container isolation is defense in depth, **not** a guarantee
  against malicious kernel-level behavior or a replacement for hardened
  runners, dependency scanning, registry signing or provenance attestation.
- The `source` Docker image is **SDK-free**. Its source and JDK/PowerShell
  toolchain are **not** a substitute for Android's own separately licensed
  SDK. The SDK stage is validated but is not publicly pushed.
- The only automated Docker Hub destination is
  `paulcheek/android-source-study-v1:latest`, plus an immutable-source
  `sha-<12-prefix>` tag. Publishing requires *all* native, Linux Docker
  and Android Docker gates plus trusted main/version-tag push.
- Configure GitHub environment `docker-hub-release` with approval rules
  and two secrets: `DOCKERHUB_USERNAME=paulcheek` and
  `DOCKERHUB_TOKEN` (Docker Hub-scoped access token, never paste into code).
  The workflow fails conspicuously if authorization is unavailable.
- A green source build is not GMP operational qualification or physical
  Android/antenna performance qualification. Device/emulator tests,
  calibration traceability, cybersecurity review, approved SOPs and
  accountable QA signoff remain **OPEN**.

### Third-party licensing warning

Google's [Android SDK license](https://developer.android.com/studio/terms)
generally restricts redistribution of SDK components (§3.4), subject to
open-source exceptions. The Docker Hub image does not include that SDK.
A source study container is **not** a way to make third-party code 0BSD.

### Review and release status

The shared-guard validation workflow is a gate and evidence collector.
A Docker Hub push, in particular `docker push
paulcheek/android-source-study-v1:latest`, is not considered complete
until the actual published registry digest is observed. A PR-only green
workflow is **not** proof that a push happened.

Original failed checks, stage changes, SHA-256 values and the PR decision
must remain linked to the corresponding commit; do not rebase history to
hide them.
