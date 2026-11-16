# Docker source study v1 · licensing-safe publication

**Docker Hub target:** `paulcheek/android-source-study-v1:latest`

**Important:** The public image is a **source-study/build-harness image
without Google's Android SDK**. The Android validation image is a
separate, disposable Docker stage and is never pushed to Docker Hub.

## Source image

`docker/android-source-study-v1.Dockerfile` uses the supported
Microsoft .NET 10 SDK image (Ubuntu 24.04, with PowerShell 7), Java 17,
a C++ toolchain and the project's 0BSD source. It contains no Android
SDK, Gradle, JUnit, application signing key or saved credentials.

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
calls `javac`, `aapt2`, `d8`, `zipalign`, and `apksigner`,
then archives APK bytes and verification logs. The SDK-bearing
image does not become the public Docker Hub image.

```powershell
pwsh -NoLogo -NoProfile -File ./ci/Run-DockerValidation.ps1 -Mode Android
```

`-Mode Linux` similarly verifies C++20, C11, test suites and the
installed consumer in an offline non-root container. Native macOS and
Windows coverage remains on native runners because Docker on Ubuntu
cannot reproduce their native operating systems.

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
`docker login --password-stdin` and then exactly:

```powershell
docker push paulcheek/android-source-study-v1:latest
```

No current PR check represents an actual registry publication. The
GitHub Actions push log and resulting Docker Hub digest are the
publication authority, not the workflow code.

**Do not** publish the `android-sdk` or `validation` stages.
[Google's Android SDK License §3.4](https://developer.android.com/studio/terms)
generally restricts redistribution, subject to separately licensed
open-source components. Publishing an SDK-containing image merely because
the *project's own* source is 0BSD would incorrectly conflate rights.

## Validation artifacts

Source SHA, Docker image ID, native CTest XML, Java assertions,
Android APK inner-file SHA-256, C++ compiler/PDB fingerprints, source
preflight and all failure logs are retained by the
[shared-guard workflow](../.github/workflows/shared-guard.yml).
The [GAMP 4-style validation matrix](GAMP4_VALIDATION.md) lists every
requirement, test, risk and remaining operational gap.

A build passing Docker/JDK/SDK tests is not an Android device-level test,
GMP qualification, or verification of model-free word recognition.
