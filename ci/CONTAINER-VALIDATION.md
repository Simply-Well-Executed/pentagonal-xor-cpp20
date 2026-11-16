# Shared Guard: pinned container validation

SPDX-License-Identifier: 0BSD

## License boundary

Project-owned C/C++ source and CI scripts retain **0BSD**. Build images,
compilers, SDKs, system libraries and runtime dependencies retain **their
own** licenses. Docker does not relicense project source, nor does 0BSD
remove obligations associated with distributing linked dependencies,
container layers or toolchains. Do not bundle third-party tools under the
project's 0BSD notice. Review the actual linked binaries, image SBOM and
license inventory before publishing releases. A strict no-copyleft
redistribution policy needs review of the image contents and linkages,
not merely the top-level source license.

## Proven, committed digests

On 2026-11-15 (America/Los_Angeles), a read-only `docker buildx
imagetools inspect` GitHub Actions job confirmed the two published images
are **linux/amd64 only**. The workflow pins their complete architecture-
specific image manifest digests:

| Flavor | Repository and tag | linux/amd64 manifest digest |
| --- | --- | --- |
| Linux | `paulcheek/opensuse-leap-hardened:latest` | `sha256:3e127aa5462cdd8cf5d4a35878983cb7895d0ebeb5679de63e0f35ca67de562d` |
| Android | `paulcheek/android-source-study-v1:latest` | `sha256:8ed78faab11b97aaa8f7884dd91b39968e565a7f5359066c32a3f520f9ef35c4` |

The Docker runner invokes the digest-qualified reference
`repository:latest@sha256:<64 hex>`. The human-facing `latest` label is
not trusted as the content identifier. A digest change requires a
reviewed source change. Do **not** replace digests with mutable tags.

The openSUSE image has no published `linux/arm64` manifest in the
verified index. The existing **native** Linux ARM64 runner remains active.
No emulated container run is presented as a native ARM64 validation.

## Build boundaries

- Linux Docker: read-only base filesystem; non-root user; no container
  network; all capabilities dropped; checkout mounted at `/work`; CMake
  Release build and actual CTest execution inside image.
- Android Docker: Linux x64 image with Android NDK cross-compilation for
  `arm64-v8a`, `android-24`, Release. An Android study container is **not**
  an Android emulator. CTest execution on a device is not claimed.
- Windows x64/x86: native Windows runners. macOS Intel/Apple Silicon:
  native macOS runners. Existing Linux x64/ARM64 jobs remain as baselines.
- Images must already include the necessary toolchain: Linux needs shell,
  CMake, CTest and C++ compiler; Android needs shell, CMake and a working
  NDK at `ANDROID_NDK_HOME`. CI deliberately does **not** install
  unreviewed packages to compensate for missing tools.
- Project build scripts never invoke Node/npm or the Android Debug variant.

**Infrastructure caveat:** The retained GitHub `actions/checkout` and
`actions/upload-artifact` actions execute Node internally. They are
separate from the project's build commands, but a literal ban on *all*
Node runtime execution is not yet satisfied by the hosted workflow.
Replacing these actions requires another supported checkout and evidence
retention mechanism. Do not claim the entire workflow is Node-free.

## Evidence and GAMP methodology

The runscript records the exact source SHA, committed digest and requested
platform, image inspection, toolchain versions, build logs, and test logs.
The existing `Write-CiEvidence.ps1` captures compiler/source provenance,
SHA-256 evidence checksums, project-created binaries, and a source archive.
Evidence is uploaded even on failure, with a 90-day GitHub artifact
retention period. This is **not** an indefinitely retained validation
archive. Copy approved records to a governed repository according to the
project's documented retention requirements.

A failed image toolchain preflight is evidence of an unqualified image,
not a successful build. Android compile-only passes cannot establish
Android runtime or device safety. A qualified GAMP validation program also
needs reviewed requirements, independent approvals, traceability,
change control, environment qualification and documented record custody.

Do not confuse `docker push` (publish an image) with pulling and running
a digest-qualified image for a build.
