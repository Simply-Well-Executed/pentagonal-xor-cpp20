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

## Requested Docker Hub destination and historic immutable image

The explicitly authorized publication **destination** is `paulcheek/android-source-study-v1:latest`. The intended immutable **source** is `paulcheek/android-source-study-v1:sha-07eb2a4561f0`; the complete Git source commit is `07eb2a4561f0f5e82db4668d2b4fa453e2a4d733`, the same commit targeted by GitHub preview release `preview-319-07eb2a4`. The operator supplied expected registry manifest digest:

```text
sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30
```

**The supplied digest has not been independently confirmed against Docker Hub in this editing session.** An image config ID and Docker registry manifest digest are distinct identifiers; a matching Git tag alone is not proof of the contents of a Docker image. `latest` is mutable, while the digest is content addressed. The publisher must look up the immutable tag in the actual Docker registry and match its manifest digest **before** attempting to move `latest`. It must also verify the newly published `latest` registry digest **after** a push.

The source commit `07eb2a4...` is **historical and differs from the current draft PR #20 source**. Do not claim that new PR #20 CTest/URS runs validate this older source image. To promote this existing image, collect genuine GAMP4-style supplier OQ evidence and a complete CTest log **from the exact historical source commit**, and independently approve the provenance relationship between that source and the digest-qualified image. The only authorized publisher for this target is [`ci/Publish-AndroidSourceStudy.ps1`](../ci/Publish-AndroidSourceStudy.ps1). The historical `ci/Run-DockerValidation.ps1 -Mode Publish` entrypoint now fails closed: its previous code is retained for Git audit, not as a supported shortcut.

The new publisher will not download images, build software, install SDKs, trigger hosted runners, or log in using embedded secrets. A separately authorized Docker Hub login is performed by the operator; `DOCKERHUB_USERNAME` must equal `paulcheek`. A local Unix or Windows named-pipe Docker daemon is required. Docker Buildx `imagetools inspect` queries the live Docker Hub registry **only when the operator directly invokes the inspection/publish script**; this explicit release operation is distinct from offline builds.

The normal release matrix in `ci/gamp4-docker-release.yml` still requires 13 platform receipts for the proposed **new** multi-OS bundle. Its condition does not silently validate the old `07eb2a4` source-study image. The Docker Hub promotion is a **separate historic-image publication decision** with an exact digest pin and its own source-matched evidence.

### Operator-controlled inspection and promotion

Run from an appropriately prepared local host with PowerShell 7, Docker CLI, Buildx, and a Docker daemon. To inspect the source and current `latest` without modifying either tag:

```powershell
pwsh -NoLogo -NoProfile -File .\ci\Publish-AndroidSourceStudy.ps1
```

If the immutable source is a multi-platform manifest index, the publisher **rejects `docker tag` + `docker push`** because that sequence may discard platforms. Use a separately reviewed manifest-preserving promotion instead. The script also blocks if the actual source tag's registry digest differs from the operator-supplied value or if the current `latest` manifest cannot be read.

After actual GAMP4 results and independent release review, make a local copy of [`ci/dockerhub-source-study-promotion-approval.example.json`](../ci/dockerhub-source-study-promotion-approval.example.json), then fill its two SHA256 evidence hashes and change its five review booleans to `true` **only after those checks genuinely pass**. The template intentionally defaults every approval to `false`.

```powershell
$env:DOCKERHUB_USERNAME = 'paulcheek'
# Authenticate with the Docker CLI under an authorized account.
# Use a secure credential mechanism; never place tokens in repository files.
pwsh -NoLogo -NoProfile -File .\ci\Publish-AndroidSourceStudy.ps1 -Publish -Gamp4EvidenceJson '.\evidence\historical-gamp4.json' -CtestLog '.\evidence\historical-ctest.log' -ReleaseApprovalJson '.\evidence\source-study-promotion-approved.json' -AcknowledgeLatestReplacement 'paulcheek/android-source-study-v1:latest@sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30'
```

The historical GAMP4 JSON must match the existing `SWE-PENTAGONAL-GAMP4-SUPPLIER-VALIDATION-v1` schema, report 12 unique URS-11..22 results, pass and exit zero. The reviewed CTest text must show a nonzero test count and zero failed tests. The approval JSON binds the exact source commit, immutable source tag, mutable target, digest, and SHA256 hashes of both evidence files, plus separate source-origin, SBOM, licensing, operator and QA review.

When `-Publish` is supplied and `latest` currently points elsewhere, the script verifies that the local preloaded immutable source image has the matching `RepoDigest`, rechecks the remote `latest` pointer for concurrent changes, executes the literal `docker tag` and **`docker push paulcheek/android-source-study-v1:latest`**, then checks the *remote* registry manifest. A mismatch after pushing is a **RELEASE DEVIATION**, not success; a push is inherently non-atomic with the registry readback. Preserve terminal output as an audit log.

**Current evidence classification: TARGET CONFIGURED / DIGEST NOT VERIFIED / RELEASE HOLD / PUSH NOT PERFORMED.** Do not update a release post, push a registry tag or mark GAMP4 acceptance complete until actual independent validation and registry evidence exist.

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
