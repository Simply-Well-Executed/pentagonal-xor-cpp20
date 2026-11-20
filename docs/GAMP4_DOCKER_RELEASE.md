# GAMP4-gated Docker release custody, native OS/ABI matrix

**Status: CANDIDATE SOURCE COMMITTED. Docker execution, target-hardware testing, release assembly, QA signoff and GitHub release publication NOT PERFORMED.** This is a supplier-side *GAMP 4-style* validation procedure, not ISPE certification, GMP compliance, FDA approval or operational PQ.

This is an **explicitly requested exception** to the earlier absolute container-image prohibition. It allows opt-in **locally controlled, preloaded, digest-pinned Docker** for release validation and artifact transport. It does not reauthorize GitHub Actions runners, remote builders, Node/npm, automatic SDK downloads, container registry pushes or unattended GitHub releases. Older Docker and Android source is retained with its existing provenance.

## Authority and execution surfaces

| Source | Purpose | May claim |
| --- | --- | --- |
| [ci/gamp4-docker-release.yml](../ci/gamp4-docker-release.yml) | Declarative, fixed 13-profile release policy | Required gates and unsupported targets |
| [docker/compose.release.yml](../docker/compose.release.yml) | Offline non-root Linux build containers, read-only exported Git-committed source snapshot (without .git or untracked files), dropped capabilities | Explicit operator-invoked local container execution |
| [ci/Invoke-LocalDocker-Gamp4.sh](../ci/Invoke-LocalDocker-Gamp4.sh) | Verify local Unix/npipe Docker daemon, preloaded @sha256 image, clean source SHA; no pull | Docker preflight if actually run |
| [ci/docker/Run-Gamp4-Container.sh](../ci/docker/Run-Gamp4-Container.sh) | Full Release CMake, CTest, independent exact 12-URS checks, archive and hash | Linux platform supplier test evidence after real execution |
| [ci/docker/Build-Android-NDK.sh](../ci/docker/Build-Android-NDK.sh) | Four NDK ABI archive builds after identical-SHA Linux source validation | Cross-compiled core archive, **not** Android execution |
| [ci/Run-Native-Macos-Gamp4.sh](../ci/Run-Native-Macos-Gamp4.sh) | Native Apple Clang full CTest, URS, installed archive and CLI packages | Native macOS evidence, host architecture recorded |
| [ci/Run-Native-Windows-Gamp4.ps1](../ci/Run-Native-Windows-Gamp4.ps1) | Native local MSVC Windows x86/x64/ARM64 full CTest and GAMP4 evidence | Windows platform test evidence, if run on compatible host |
| [ci/Build-Windows-ReleaseContainer.ps1](../ci/Build-Windows-ReleaseContainer.ps1) | Optional local Windows x64 container smoke, cached digest base, no push | Windows Server Core container smoke only |
| [ci/Assemble-Gamp4-Release.sh](../ci/Assemble-Gamp4-Release.sh) | Check all 13 profiles, 12 URS, SHA256 receipts and package files, assemble tarballs and release notes | **STAGED_NOT_PUBLISHED** |
| [docker/ReleasePayload.Dockerfile](../docker/ReleasePayload.Dockerfile) | Scratch OCI payload of verified release archives only | Non-executable artifact transport, not a native OS container |

Canonical mathematics and source identity remain in the existing parity implementation, Pentagonal Suite, DecaSigma and related public headers. The full GAMP4 CTest suite may use existing C/C++ standard libraries. The **separate STD-free core-only archive** is source-library-free within its defined code boundaries, not an assertion that the full repository or linked executables have zero CRT imports.

## Release profile coverage

| OS target | Qualification path | Limit |
| --- | --- | --- |
| Linux amd64, arm64, x86 (i386) | Docker container for each `linux/amd64`, `linux/arm64`, `linux/386`; full CTest + URS-11 through URS-22 | CPU emulation is not native hardware qualification |
| macOS Apple Silicon arm64 | Native macOS Xcode build, CTest, URS | Docker may wrap an artifact but cannot run macOS kernel |
| macOS Intel x86_64 | Native macOS Xcode build, CTest, URS | Same |
| macOS universal arm64+x86_64 | Native dual-architecture archive, requires distinct validated arm64 and x86_64 receipts first | Not proof of both CPUs unless both host receipts supplied |
| Windows x86, x64/AMD64, arm64 | Native compatible Windows/MSVC build, CTest and URS | Windows x64 container smoke optional; matching Windows daemon required |
| Android arm64-v8a, armeabi-v7a, x86, x86_64 | Offline Docker with preinstalled read-only NDK, matching validated Linux source SHA | Core archive only; no APK, JNI installation, device test or Android OQ claim |
| macOS i386 | No modern runtime path | UNSUPPORTED, expressly excluded from release count |

**Windows x86-64 and AMD64 are aliases of the single windows-x64 architecture.** Do not create three conflicting x64 binaries. The 13 required release profiles are 3 Linux + 3 modern macOS + 3 Windows + 4 Android. Any missing profile keeps the release staging gate red.

## Source-to-container isolation

The host wrapper exports **only committed source bytes** using local `git archive HEAD`. The snapshot directory is located under `build/gamp4-source-snapshots/<GitSHA>`, separate from the writable `build/gamp4-release/<GitSHA>` artifact mount. The wrapper generates and rechecks the snapshot's SHA256 file manifest, rejects symlinks, refuses any unexpected additions, and writes `.swe-source-commit` for the container to cross-check against its declared source SHA.

The Docker container never receives `.git`, developer untracked files or the host's full checkout as a bind mount. The source snapshot is mounted read-only at `/src`, while only the separate package/evidence directory is mounted read-write at `/out`. This prevents an ordinary container process from changing its mounted source via a writable `/out` alias. The snapshot is *not* a signed attestation, and an independently compromised Docker daemon or host can still undermine these controls.

The Docker wrapper accepts writable output only inside the ignored repository `build/gamp4-release/` directory and additionally rejects overlap with its separate source snapshot. Operators can transfer completed reports and archives to a separately reviewed consolidation location, rather than giving a container an arbitrary write-mount outside the build tree.

## Offline, pinned and least-privilege Docker preflight

The operator must independently review and preload image manifests by digest in the *local* Docker store and verify third-party base and SDK licenses. No `latest` tag can substitute for a digest. The POSIX wrapper refuses remote Docker endpoints and uncached images. Docker Compose uses `pull_policy: never`, `network_mode: none`, non-root host UID:GID, read-only root filesystem, read-only committed source snapshot and NDK mounts, dropped capabilities and no-new-privileges.

Required preloaded Linux images contain CMake, CTest, Git, C++20 compiler, POSIX shell, SHA256 tools, find, grep and awk. The preloaded NDK builder also contains the operator-installed Android NDK exposed by a **read-only bind mount**. No build stage runs `apt`, `zypper`, `curl`, `wget`, `npm` or a network checkout. Registry access for initial image acquisition, if required, happens **outside** this workflow and needs separate human review. No container runs with the Docker socket mounted.

For the native Linux amd64 source/GAMP4 qualification:

```sh
export SWE_LINUX_BUILDER_IMAGE='registry.example/reviewed-linux-builder@sha256:REPLACE_WITH_64_HEX_DIGEST'
sh ci/Invoke-LocalDocker-Gamp4.sh linux-amd64
```

For each other Linux architecture, supply a preloaded matching-platform builder image (and local native or supported emulation capability), then invoke the corresponding profile:

```sh
sh ci/Invoke-LocalDocker-Gamp4.sh linux-arm64
sh ci/Invoke-LocalDocker-Gamp4.sh linux-x86
```

For Android, the Linux amd64 GAMP4 evidence must first exist under the **same output batch and source commit**:

```sh
export SWE_ANDROID_BUILDER_IMAGE='registry.example/reviewed-ndk-builder@sha256:REPLACE_WITH_64_HEX_DIGEST'
export SWE_ANDROID_NDK_DIR='/absolute/path/to/installed/ndk'
sh ci/Invoke-LocalDocker-Gamp4.sh android-arm64-v8a
sh ci/Invoke-LocalDocker-Gamp4.sh android-armeabi-v7a
sh ci/Invoke-LocalDocker-Gamp4.sh android-x86
sh ci/Invoke-LocalDocker-Gamp4.sh android-x86_64
```

The scripts deliberately do not distribute Google SDK/NDK packages or make Android application signing claims. Android artifacts are libraries, not installable APKs.

## Native Apple/Windows gates

Run these on real hosts at the same checked-out commit, with a common *operator-controlled* `SWE_RELEASE_OUTPUT` directory. Evidence from another host must be transferred and reconciled securely before release assembly; no automatic remote transport is built into these scripts.

macOS:

```sh
sh ci/Run-Native-Macos-Gamp4.sh macos-arm64
sh ci/Run-Native-Macos-Gamp4.sh macos-x86_64
sh ci/Run-Native-Macos-Gamp4.sh macos-universal
```

The two first commands require native matching CPUs. The universal command refuses execution unless both separate architecture receipts already exist.

Windows (example version **only**; independently check the official latest stable PowerShell release before invoking):

```powershell
pwsh -NoLogo -NoProfile -File .\ci\Run-Native-Windows-Gamp4.ps1 -Profile windows-x64 -LatestStablePowerShellVersion 7.7.0 -ReleaseEvidenceUri 'https://github.com/PowerShell/PowerShell/releases/tag/v7.7.0'
```

Repeat using `windows-x86` and `windows-arm64` on capable hosts. `windows-amd64` and `windows-x86-64` resolve to `windows-x64`. For optional **Windows x64 Server Core** Docker smoke, supply a previously cached, license-reviewed base image referenced by its exact SHA256 digest:

```powershell
pwsh -NoLogo -NoProfile -File .\ci\Build-Windows-ReleaseContainer.ps1 -BaseImage 'mcr.microsoft.com/windows/servercore@sha256:REPLACE_WITH_REVIEWED_DIGEST'
```

Windows container compatibility and Docker BuildKit WCOW support remain host/version dependent. If unsupported, the optional image remains **NOT TESTED**, never a substitute for native Windows GAMP4 evidence.

## Validation protocol and release custody

For each executable Linux/macOS/Windows target, the gate requires a successful Release CMake configuration, full CTest, a separately executed `pentagonal_gamp4_validation_test` with exactly one `PASS|URS-NN|` record for each URS-11 through URS-22, no `FAIL|` records, and exactly one `GAMP4_STYLE_SUMMARY|PASS|cases=12|failures=0`. No skipped, failed, duplicate or missing result is accepted. Each stage writes a unique per-profile report, output logs, binary digest and package SHA256 manifest to `build/gamp4-release/<SHA>/evidence/<profile>/`.

GAMP4 receipts are **derived evidence**, not authenticated signatures. Independent QA must review the environment, software bill of materials, third-party OS/NDK licenses, runtime imports and any QMS/regulated-use requirements. Missing evidence and changes to source after testing require a new comparison/run. A failing record must not be overwritten or deleted to make the matrix pass.

After all thirteen required target packages and gate receipts have been assembled under the identical Git SHA:

```sh
sh ci/Assemble-Gamp4-Release.sh
sh ci/Build-Release-OciPayload.sh
```

The first script verifies individual file SHA256 sums and complete 13-profile coverage, then writes local `RELEASE_NOTES.md`, `SHA256SUMS`, `RELEASE_GATE.yml`, and individual `.tar.gz` bundles. The second script creates an **optional local OCI image from scratch** holding the already-staged archives. It cannot execute Windows/macOS/Android binaries; it never pushes to a registry. A successful gate says `STAGED_NOT_PUBLISHED`.

**Only after independent release review** may an authorized maintainer use the GitHub Releases UI to draft a prerelease tied to that exact reviewed source SHA, attach the bundles and checksum file, and paste the generated release notes. Do not reuse an earlier published preview as proof of this new source revision. The connected GitHub release endpoint exposed here is read-only; this change does not claim to have created, tagged or published a new release.

**CURRENT STATUS:** New source, policy and gate scripts only. No Docker engine in this execution environment, no native Windows/macOS/Android test host evidence, no approved third-party image digest, no 13-profile release bundle, and no human QA authorization. **RELEASE HOLD.**
