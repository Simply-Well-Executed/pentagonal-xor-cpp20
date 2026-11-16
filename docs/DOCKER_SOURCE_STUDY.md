# Docker source study v1 · licensing-safe publication

**Docker Hub destinations:** `paulcheek/android-source-study-v1:latest` and `paulcheek/opensuse-leap-hardened:latest`.

**Important:** The public image is a **source-study/build-harness image
without Google's Android SDK**. The Android validation image is a
separate, disposable Docker stage and is never pushed to Docker Hub.

## Source image

`docker/android-source-study-v1.Dockerfile` uses the supported
Microsoft .NET 10 SDK image (Ubuntu 24.04, with PowerShell 7), Java 17,
a C++ toolchain and the project's 0BSD source. It contains no Android
SDK, Gradle, JUnit, application signing key or saved credentials.
Its third-party OS and .NET packages keep their own licenses; the 0BSD
license governs the project's authored files, not all image layers.

To build the source-only image locally:

```powershell
docker build --pull --target source --file docker/android-source-study-v1.Dockerfile --tag paulcheek/android-source-study-v1:latest .
docker run --rm --network none --read-only --cap-drop ALL --security-opt no-new-privileges paulcheek/android-source-study-v1:latest
```

The default container command checks repository source safety.

## Private Android verification

`ci/Run-DockerValidation.ps1 -Mode Android` constructs an ephemeral
SDK-acquisition stage. The SDK is downloaded from Google's official
distribution URL, compared against a fixed SHA-256, and used only after
license acceptance in the temporary validation environment. Android
API 35 and build tools 35.0.0 are installed there.

After the download phase, tests run in a non-root container with
`--network none`, all Linux capabilities dropped, no privilege
escalation, memory and process limits. The Gradle-free Android build
calls `javac`, `aapt2`, `d8` and `zipalign`,
then validates and fingerprints an **unsigned, non-installable** APK
candidate and records the Java test results. No developmental signing identity
is introduced by `shared-guard.yml`. The SDK-bearing
image does not become the public Docker Hub image.

```powershell
pwsh -NoLogo -NoProfile -File ./ci/Run-DockerValidation.ps1 -Mode Android
```

`-Mode Linux` similarly verifies C++20, C11, test suites and the
installed consumer in an offline non-root container. Native macOS and
Windows coverage remains on native runners because Docker on Ubuntu
cannot reproduce their native operating systems. A separate capability
probe records whether Docker Engine exists on Windows/macOS runners.
On macOS, Docker hosts **Linux** containers in a VM, not macOS containers;
Windows containers require a Windows Docker daemon and appropriately
licensed Windows base layers. No Windows or Darwin container builds are
claimed complete merely because these probes run.

Docker is a *partial* isolation barrier, not a complete trust boundary.
A hardened self-hosted build runner or disposable VM would provide
stronger separation from kernel-level risk.

## Trusted publication (never from pull requests)

The `publish-docker` job in `shared-guard.yml` depends on **all**
native, Linux Docker and Android Docker checks. It executes only after
a trusted push to `main` or a version tag and only with Docker Hub
credentials. Configure GitHub environment
`docker-hub-release` with reviewers/approval and secrets:

- `DOCKERHUB_USERNAME` must equal `paulcheek`.
- `DOCKERHUB_TOKEN` must be a scoped Docker Hub access token.

A tested source image is tagged both
`paulcheek/android-source-study-v1:sha-<12-hex-prefix>` and
`paulcheek/android-source-study-v1:latest`. Push uses
`docker login --password-stdin` and then, after the separate OS license review gate, these two exact commands:

```powershell
docker push paulcheek/android-source-study-v1:latest
docker push paulcheek/opensuse-leap-hardened:latest
```

The openSUSE source-image Dockerfile pins vendor Leap 16.0's OCI index to
`sha256:a4115e2f5aea9712a7c73974fd94000a034fc3051ef29b5f7722fbf04070dd9d`.
Its C++20/C11/installed-consumer validation stages execute offline.
Package-manager installed toolchain **versions are not individually pinned**;
RPM names, versions and per-package licenses are inventoried for review.
Publishing Leap requires environment variable
`OS_LICENSE_REVIEW_APPROVED=true` after compliance review.
The repository source remains 0BSD, but openSUSE packages are not thereby
relicensed. A strict zero-copyleft requirement for the *entire OS image*
would require omitting the openSUSE image, not declaring it magically 0BSD.

No current PR check represents an actual registry publication. The
GitHub Actions push log and resulting Docker Hub digest are the
publication authority, not the workflow code.

**Do not** publish the `android-sdk` or `validation` stages.
[Google's Android SDK License §3.4](https://developer.android.com/studio/terms)
generally restricts redistribution, subject to separately licensed
open-source components. Publishing an SDK-containing image merely because
the *project's own* source is 0BSD would incorrectly conflate rights.

## Windows and macOS container feasibility

Native Windows x64 CI currently has a Windows-container daemon.
The experimental PowerShell-only
[`ci/Test-WindowsOciSmoke.ps1`](../ci/Test-WindowsOciSmoke.ps1)
builds a disposable Windows Server Core 2022 image containing the
newly compiled 0BSD binaries and executes parity smoke tests in it.
The Microsoft base layer is **not redistributed**, avoiding
unreviewed proprietary-image distribution. The step is observational
and does not replace the native C++20 build.

GitHub-hosted macOS Intel and Apple Silicon runners did not expose a
Docker daemon during capability validation. macOS ABI verification
remains native to the ephemeral GitHub macOS runner VM. A Linux
container running on a Mac is not a Darwin container.

## Validation artifacts

Source SHA, Docker image ID, native CTest results, Java assertions,
unsigned Android candidate SHA-256, native compiler/PDB fingerprints,
source preflight and failure logs are recorded by the
[shared-guard workflow](../.github/workflows/shared-guard.yml).
Because the guard expressly excludes JavaScript-based artifact actions,
**only logs, evidence hashes and GitHub job summaries are persisted**;
per-job binary ZIPs are not automatically uploaded by this guard.
The separate release workflow may produce its own independent downloadable
artifacts using a different toolchain; do not conflate those artifacts
with source hashes from the guard.
The [GAMP 4-style validation matrix](GAMP4_VALIDATION.md) lists every
requirement, test, risk and remaining operational gap.

A build passing Docker/JDK/SDK tests is not an Android device-level test,
GMP qualification, or verification of model-free word recognition.
