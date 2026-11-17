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
| URS-05 | HIGH: test Android without developmental signing | Java 17, SDK 35, AAPT2, D8 and ZIP alignment | Isolated Docker Android builds a deliberately **unsigned, non-installable** release candidate and checks its structure and SHA256 | `app-unsigned.apk`, source/SDK evidence, negative tests, red log on failure |
| URS-06 | HIGH: resist unauthorized Docker Hub publication | Protected publish job, dependent on native/Android/Linux/Leap validation | Only trusted main/version-tag pushes with Docker credentials; OpenSUSE additionally requires explicit OS package-license review and exact manifest hash; PRs blocked | Exact push result, registry digest and package-license manifest |
| URS-07 | HIGH: no unlicensed Google Android SDK redistribution | Two-stage Docker design, publish only `source` target | SDK paths absent in published image, SDK acquired only in temporary validation image | Docker image inspect and offline inspection run |
| URS-08 | MEDIUM: retain identity and genuine failures while avoiding a JS runtime in guard | Seven native jobs + Docker Linux/Android/Leap + PowerShell evidence | SHA256, CTest, Windows PDB/compiler fingerprints and GitHub job summary | **Job logs and summary persisted; binary artifacts are not uploaded by this JS-free guard**. The separate ship workflow is a distinct artifact channel |
| URS-09 | MEDIUM: reproducible experimental counterarccosine second-wave phase | `tests/counterarccosine_secondwave_test.cpp` and dedicated SVG | 257 deterministic phase witnesses; mathematical complementary identity | CTest results and white paper |
| URS-10 | MEDIUM: +9 dBi RF calculation physically well-scoped | `shared/coreao_gain.hpp` | Independent `pow(10,0.9)` reference and inverse witness | `coreao_gain_test`; theoretical calculation only |

### Platform and execution environment

| Platform | Actual execution mode | Notes |
| --- | --- | --- |
| Linux x64 and ARM64 | Native GitHub runner | C++20, strict C11, all root/shared tests and installed consumer |
| Windows x64, x86 and ARM64 | Native GitHub runner | MSVC/CMake with debug symbols/PDB evidence; Windows ARM64 now a required guard lane |
| macOS Intel and Apple Silicon | Native GitHub runner | Clang/CMake root/shared and installed consumer |
| Linux x64 source-study Docker | Non-root offline container | C++20/C11 root/shared and consumer verification, no Android SDK |
| Pinned openSUSE Leap 16.0 Docker | Vendor OS base pinned by full index SHA-256; C++ acceptance stages offline | Root/shared CTest, installed consumer, RPM package-license manifest; **package updates are not pinned individually** |
| Windows and macOS OCI capability | Native GitHub VM, optional engine probe | Capability classified explicitly; no claim a Linux image validates Darwin or the Windows ABI |
| Android Gradle-free Docker | SDK acquisition in disposable image; testing in non-root network-disabled container | 26,654 Java assertions, AAPT2/D8 packaging, structurally verified **unsigned** APK; Google SDK **never pushed** |

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
- The two configured Docker Hub destinations are
  `paulcheek/android-source-study-v1:latest` and
  `paulcheek/opensuse-leap-hardened:latest`, plus separate immutable-source
  `sha-<12-prefix>` tags. Publishing requires *all* native, Linux Docker
  and Android Docker gates plus trusted main/version-tag push.
- Configure GitHub environment `docker-hub-release` with approval rules,
  secrets `DOCKERHUB_USERNAME=paulcheek` and `DOCKERHUB_TOKEN`. The Android
  source image can publish after its own SDK-free checks without waiting
  for the OpenSUSE RPM review; its third-party layers retain their original
  licenses and notices.
- Set `OS_LICENSE_REVIEW_APPROVED=true` **only after**
  examining the RPM license inventory and satisfying source/notice delivery,
  and set `OS_PACKAGE_LICENSE_MANIFEST_SHA256` to the reviewed inventory hash.
  Without approval, the workflow skips the OpenSUSE image; with approval but
  a missing or changed hash, publication fails before registry login or push.
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
A Docker Hub push, including either requested `:latest` destination,
is not considered complete until the actual published registry digest
is observed. Publication of a Linux OS container does not relicense its
underlying GNU, MIT, Apache and other packages as 0BSD. A PR-only green
workflow is **not** proof that a push happened.

Original failed checks, stage changes, SHA-256 values and the PR decision
must remain linked to the corresponding commit; do not rebase history to
hide them.

## Experimental Windows OCI execution witness

The Windows x64 guard additionally attempts a **non-gating** Windows
Server Core LTSC 2022 Docker smoke using host-compiled C++20 executables.
It verifies the 32-state parity self-test and executable startup inside
the Windows container, using a temporary build context created entirely
by PowerShell 7. Its proprietary Microsoft base image is never pushed
to Docker Hub. If unavailable, the step remains visibly unsuccessful
while the authoritative native Windows compiler/CTest results still gate
release. This is execution inside a Windows container, **not** an MSVC
compiler installed inside a container.

The macOS feasibility probe confirmed no Docker daemon on the tested
GitHub-hosted Mac runners. Linux containers on Docker Desktop for macOS
would not reproduce the Darwin kernel; native macOS VM tests therefore
remain the correct validation source.

## 2026-11 safety and release update

`shared-guard.yml` now contains **no JavaScript-based Actions steps**, no
Node/npm/npx/yarn commands and no Android developmental signing. Its
Git retrieval, Docker orchestration, checksum manifests and summaries
are PowerShell 7 based. The distinct `ship.yml` and
`android-standalone.yml` workflows retain their own prior build choices
and must not be described as covered by this guard-only restriction.

The Android guard output is unsigned. It is **not installable** until an
authorized production signing process is separately established. The
absence of a disposable signing key prevents a debug identity being
misrepresented as production approval. The source project continues under
0BSD. The distributed Docker OS images contain independently licensed
packages, including copyleft components: those packages remain under their
original licenses; project files are **not converted** to a copyleft license.
Strict zero-copyleft *image* redistribution is incompatible with
publishing an unmodified general-purpose openSUSE base containing GPL
packages. The OpenSUSE image push therefore requires explicit, auditable
package-license review rather than an automatic assertion of zero copyleft.

## Exact OpenSUSE package-license acceptance gate

Before publishing the OpenSUSE Docker Hub image, the protected
`docker-hub-release` environment must supply
`OS_LICENSE_REVIEW_APPROVED=true` **and**
`OS_PACKAGE_LICENSE_MANIFEST_SHA256` equal to the SHA-256 of the
actual sorted `opensuse-os-licenses.tsv` produced by the guard.
The publisher reconstructs this manifest from its own image and
fails closed if its contents changed. This binds the approved package
inventory to the actual publication candidate, avoiding a blanket
license-approval flag for unexpectedly updated RPM packages.
This OpenSUSE-specific RPM gate does not block the separately verified
Android source image, nor does that separation waive license or notice
obligations for any third-party image layers.
Review of a GPL/LGPL package permits distribution *subject to its
license*, but does not change the project's authored source from 0BSD.
If the requirement is zero copyleft **inside the container**, do not
approve or publish an openSUSE base that includes copyleft OS packages.


## Pentagonal-COR new supplier-side requirements (URS-11 through URS-22)

This whole-repository GAMP4-style matrix is extended, not replaced, by
[the Pentagonal gate-specific validation plan](PENTAGONAL_GAMP4_VALIDATION.md).
The added C++20 `pentagonal_gamp4_validation_test` records twelve
named URS checks covering all 32 gate truth states, 30 single-bit
ruptures, 64-bit register packing and bypass, non-destructive source
handling, six-step FLOP closure, independent VISTA display, NOP, invalid
ligands and the externally supplied witness limitation.

The `ship.yml` multi-platform CTest runs this test, and its
PowerShell-only evidence step `ci/Invoke-PentagonalGamp4.ps1` checks
exactly one PASS witness per URS ID and retains execution logs and
SHA-256 hashes. The existing GAMP4 guarded-workflow custody remains
in effect for the wider original project. CI pass is supplier
verification, not formal regulated installation/operational/
performance qualification or a claim of GxP/Part 11 approval.
