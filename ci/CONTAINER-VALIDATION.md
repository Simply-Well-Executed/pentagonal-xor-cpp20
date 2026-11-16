# Shared Guard: pinned container validation

SPDX-License-Identifier: 0BSD

## Boundaries

The shared C/C++20 project and project-owned CI scripts retain their own 0BSD license.
Build images, compilers, Android NDK, libc, C++ runtimes and other tools retain their
respective licenses. Building in an image does **not** transfer its license to the
project source, nor does 0BSD waive redistribution obligations for linked/packaged
third-party software. Perform a package/SBOM/license review before distributing
any binary or derived container image. Do not publish proprietary or third-party
SDK files as 0BSD. A 0BSD-only *source* policy can coexist with tools carrying
other licenses without incorporating their code.

## Validated image identification

The two allowed image repositories and tag labels are:

- Linux: `paulcheek/opensuse-leap-hardened:latest`
- Android: `paulcheek/android-source-study-v1:latest`

These names are **not** immutable on their own. CI accepts a complete
`sha256:` followed by 64 lowercase hexadecimal digits, builds with
`repository:latest@sha256:<full digest>`, and logs the exact reference.
No digest is guessed or truncated. If the configured digest is missing, the
optional container job is skipped rather than substituting the mutable tag.
If the digest cannot be pulled or the required compiler is missing, the
corresponding job fails, with evidence.

Set repository Actions **variables** after checking image manifests:

- `SWE_LINUX_IMAGE_DIGEST`: a complete digest for the openSUSE image.
- `SWE_ANDROID_IMAGE_DIGEST`: a complete digest for the Android study image.

Check the manifest architectures first: Linux jobs request `linux/amd64`
and `linux/arm64`. The Android job requests `linux/amd64` and uses the
Android NDK to cross-compile `arm64-v8a`; a Docker Linux image is **not**
itself an Android operating system and does not substitute for Android
device/emulator testing. The Android image must include `/bin/sh`,
`cmake`, and `ANDROID_NDK_HOME` with an NDK CMake toolchain file;
otherwise the job fails. The openSUSE image must include `/bin/sh`,
CMake, CTest, and a C++ compiler. CI deliberately does not download
unreviewed tools at run time.

This configuration is digest-locked per run and records the exact digest.
For change-controlled validation, additionally approve and retain changes
to the repository variables in the release record. A committed lockfile
or protected environment variable can provide stricter independent review.
The workflow **never** runs an unpinned tag as a fallback.

## Platform coverage

The original Windows x64/x86, macOS Intel/Apple Silicon and Linux x64/ARM64
native runner jobs remain enabled as baselines. The new digest-gated Linux
Docker jobs build and execute CTest inside the selected image. The new
digest-gated Android job compiles a Release NDK target without claiming that
its executables were run on Android. No Android Debug build is invoked.
No Node or npm command is used by the project-run build scripts.

**Important infrastructure caveat:** The retained GitHub
`actions/checkout` and `actions/upload-artifact` actions themselves
use a Node runtime internally. They are not a Node build dependency.
A literal prohibition on any Node execution within *all* GitHub Actions
requires replacing those actions and arranging a supported alternative
for 90-day binary evidence retention. This change does not falsely claim
that existing hosted Actions infrastructure is Node-free.

## Validation records and limitations

- Record exact source SHA, requested image name, full digest, architecture,
  compiler/tool versions, configure/build/test output, and SHA-256 checksums.
- The container build uses a non-root UID, read-only image filesystem,
  disabled network, dropped capabilities, and an isolated temporary directory.
- Compile/test evidence is uploaded even when a job fails; existing artifact
  retention is 90 days, **not** indefinite regulatory retention.
- CI reports actual status. An Android compile does not count as a runtime
  test; a skipped job does not count as successful validation.
- PowerShell remains the runner's orchestration language. Container commands
  use the image's POSIX shell because PowerShell presence in either image is
  not assumed.
- A quality-management program must separately define reviewed requirements,
  traceability, approved evidence retention, environmental qualification,
  change control and independent release approval. Docker alone does not
  establish GAMP compliance.

Run the existing `ci/Test-ReleaseSafety.ps1` preflight before each build.
Do not use `docker push` as a build command: push publishes an image, while
pull + a full digest + run selects the validated execution environment.
