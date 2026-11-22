# GitHub Container Registry and Android/Linux Python-free development requirement

**Historical shell instructions:** POSIX commands and shell behavior described below are preserved as historical protocol/evidence. The 17 wrappers are removed from active CI; links target archived text. See the [discarded-files registry](../discardedfiles.md). Retained task names invoke the native fixed transcript and exit 42 / **RELEASE=HOLD**, without performing these former operations.

**Status: source change and GitHub-only validation proposal.** The historical Docker Hub digest is immutable evidence. A Python-free image must be rebuilt and independently tested and must carry a different digest; it must never inherit prior GAMP4 approval by implication.

## Two distinct image identities

| Product | Tag | Provenance |
| --- | --- | --- |
| Existing source-study historical mirror | `ghcr.io/simply-well-executed/android-source-study-v1:sha-07eb2a4561f0` | Exact manifest mirror of `paulcheek/android-source-study-v1@sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30`; only claim equivalence after registry readback |
| Rebuilt Android/Linux source candidate | `ghcr.io/simply-well-executed/android-source-study-v1:python-free-candidate-<GitSHA12>` | New image built on GitHub-hosted Linux amd64, MCR base resolved to a digest, stripped of installed Python distribution packages, verified at runtime and pushed under a candidate tag only after tests pass |

The historical `latest` Docker Hub push was completed by [GitHub Actions run #44746338664](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44746338664) on November 20, 2026. Re-push verification does not mean that image is Python-free. It is not overwritten by this change.

## Contract: no Python in this project's Android/Linux source runtime

1. The required Android app stays **Gradle-free**, built with Java 17, Android SDK where separately qualified, and **PowerShell 7** scripts. Native C11/C++20 tests run through CMake/CTest. Python scripts, pip, Python-backed package manifests, Python command entrypoints, Python virtual environments, or Python runtime executables are prohibited in the delivered Linux source-study image.
2. `ci/Assert-AndroidLinux-NoPython.sh source` checks tracked Python-related files and actual Python command entrypoints in the Android/Linux build scripts. It is an audit gate, not a substitute for review of untrusted build inputs.
3. `docker/android-source-study-v1.Dockerfile` explicitly purges **package-manager-owned** Python distribution packages from the new image and runs `ci/Assert-AndroidLinux-NoPython.sh image` during the build. It does not delete arbitrary OS directories to hide an interpreter. A required dependency on Python fails the build instead of silently bypassing the requirement.
4. The runtime guard rejects `python`, `python3`, `pip`, `pypy`, `conda`, `pytest`, and related executable entrypoints, checks known executable directories, and rejects installed Debian Python runtime packages. These gates cannot prove the absence of embedded interpreter code hidden in arbitrary binaries, which requires a separate supply-chain audit.
5. `ci/docker/Build-Android-NDK.sh` rejects an NDK builder environment if Python remains. A pre-existing Android NDK/toolchain that actually requires Python is **not qualified** under this policy until a replacement toolchain is selected and validated.
6. The cloud-only candidate builder runs C++20 Release compilation with explicit **`-O2`**, full CTest, standalone Android Java 17 acceptance tests, and a second Python-free image check inside an offline, non-root, read-only container. The pushed GHCR candidate is pulled by its **registry digest** and audited again.

This project policy is scoped to **its built container and Android development lane**. It does not claim to remove Python from the user's phones, the Android operating system, upstream Microsoft/Ubuntu SDK build infrastructure, or every component of GitHub-hosted runners. The historic Docker Hub image may have different contents and is not silently mutated.

## GitHub repository discovery

The GitHub Actions workflow `cloud-ghcr-android-no-python.yml` runs **only on GitHub-hosted Ubuntu 24.04**. No Node marketplace actions, no Python build steps, and no self-hosted/local compute are used. A `packages: write` scoped `GITHUB_TOKEN` publishes under the organization `simply-well-executed` and repository-source metadata links the package back to `Simply-Well-Executed/pentagonal-xor-cpp20`.

GitHub Container Registry **normally creates a private package even in a public repository**. Visibility and repository linkage must be checked after publication. If the package remains private, an administrator can change its visibility through the package's GitHub settings. A public README link by itself does not change access controls. See [GitHub Packages access and visibility](https://docs.github.com/en/packages/learn-github-packages/configuring-a-packages-access-control-and-visibility).

## Release boundaries

Publishing an exact historical mirror or a separately labeled candidate is not the full 13-profile GAMP4 release. QA signoff, SBOM/license review, Android device operational qualification, and macOS/Windows tests remain independent and must not be inferred from the Linux/Java CTest receipt. No external image may be described as a verified production release without those source-matched gates.
