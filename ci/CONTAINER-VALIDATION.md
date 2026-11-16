# Shared Guard: pinned container and runtime qualification

SPDX-License-Identifier: 0BSD

## Immutable image identification

Image repositories retain the requested human-friendly `latest` names, but the
workflow pulls **only** the complete, reviewed linux/amd64 manifest digests:

| Use | Image | Manifest digest |
| --- | --- | --- |
| openSUSE C/C++ | `paulcheek/opensuse-leap-hardened:latest` | `sha256:3e127aa5462cdd8cf5d4a35878983cb7895d0ebeb5679de63e0f35ca67de562d` |
| Android source study | `paulcheek/android-source-study-v1:latest` | `sha256:8ed78faab11b97aaa8f7884dd91b39968e565a7f5359066c32a3f520f9ef35c4` |

These were inspected through Docker Hub manifests on 2026-11-15 PT. Both
images publish a linux/amd64 platform; neither is an Android OS emulator.
A floating `latest` tag is *not* an immutable security or audit control.
The digest must change by reviewed source commit if the image is rebuilt.

## Android environment initialization: source versus SDK/NDK

The published `source` stage in
`docker/android-source-study-v1.Dockerfile` is deliberately **SDK-free**.
It contains PowerShell, CMake, JDK 17, and source-study tooling but does
**not** ship an Android SDK or NDK.

Previous versions of `ci/Invoke-PinnedContainerGuard.ps1` expected
`ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake` in this SDK-free
image. The test exited with generic shell status `1`, because it checked
the optional NDK using `test` and the shell had `-e` enabled. The
Docker pull and digest verification succeeded; the NDK precondition was
wrong for the image's published scope.

`ci/Initialize-AndroidRuntime.ps1` now executes **inside** the pinned
public image. It identifies CMake, Java and javac; checks Java 17; records
whether an NDK path exists; and compiles and executes the standalone Java
17 acceptance tests. Absence of an NDK is an **informational, expected
condition** in this public image, not a silently ignored failed build.
Results are retained in `android-source-runtime.json`. A missing
required source-study tool, broken Java tests, or an evidence-write
failure remains a hard CI failure with a diagnostic numeric code.

**Scope boundary:** A passing public-image source-study run does not
claim JNI/NDK cross-compilation, an APK build, Android device execution,
or a signed release. The independent `docker-android` job uses the
**disposable `android-sdk` stage** to build and validate a *non-signed
Release APK* with Android SDK 35. Its Dockerfile stage is not published
to Docker Hub. A separate NDK stage, separately acquired and qualified,
would be necessary to claim Android native C/C++ cross-compilation.
Never auto-install an unreviewed NDK into the public `source` stage.

## Exit-code handling

`ci/Invoke-PinnedContainerGuard.ps1` retains the exact exit status,
stage name, UTC start/end, diagnostic category and log filename in
`container-stage-results.json`; it also fails the workflow for every
nonzero code. Known cases include:

| Exit | Diagnostic meaning |
| --- | --- |
| 0 | Pass |
| 1, 2 | Generic shell/command failure; inspect stage output |
| 21-25 | Source image lacks required tools, Java 17, or test inputs |
| 31, 32 | Java compilation or Java acceptance-test failure |
| 90, 91 | Unexpected runtime exception / evidence recording failure |
| 124 | Timeout (when assigned by the tool) |
| 125 | Docker CLI/daemon/invocation failure |
| 126, 127 | Command not executable / command not found |
| 130, 137, 143 | Interrupted, killed (possible OOM), or terminated |
| Any other integer | Unknown tool-specific code; preserve unchanged and fail |

No finite hand-authored table can enumerate every error code emitted by
arbitrary CMake, compiler, OS, Git, Docker, Android SDK or CI runner.
The **fallback case covers all unknown numeric results** without labeling
them as successful or guessing their causes. Logs and structured evidence
remain authoritative; diagnostic classifications are interpretation,
not a replacement for the original exit code.

## Platform, licensing, validation evidence

- Native jobs remain authoritative for Windows x64/x86/ARM64, macOS
  Intel/Apple Silicon, and Linux x64/ARM64. Docker-based macOS builds are
  not claimed. The openSUSE Docker job executes CTest on linux/amd64.
- Container validation runs without network connectivity and Linux
  capabilities, with a non-root identity and read-only root filesystem.
- Project-owned source and scripts remain **0BSD**. SDKs, system libraries,
  compilers and container layers retain their distinct third-party
  licenses. An OS package inventory and linked binary/SBOM review are
  necessary before claiming copyleft-free redistribution.
- `shared-guard.yml` uses direct PowerShell and Git commands rather than
  Node-based `uses:` actions. Other workflows such as `ship.yml` and
  `android-standalone.yml` may have different runtime, signing and
  artifact behavior and must be audited separately for repository-wide bans.
- `Record-GuardEvidence.ps1` retains SHA-256 manifest entries in the
  GitHub job log/summary. Files on a transient runner are **not** a
  guaranteed permanent archive; an externally governed custody process is
  required when full validation artifacts must outlive runner cleanup.
- This is GAMP-style traceability and evidence collection, not a claim of
  regulatory certification. Qualification, traceable requirements,
  independent review, change control and retention policy remain required.

`docker push` is image publication, not a CI build/run operation.
