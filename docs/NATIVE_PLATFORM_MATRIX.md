# IRON native platform matrix and staged STD-free migration

**Current status: CONFIGURED / NOT TESTED across the specified hosts.** The platform matrix is a local, declaration-only YAML file, not GitHub Actions, a container recipe, a VM runner, or a request to download SDKs. No OS/CPU success is inferred from the presence of a profile.

## Canonical paths

- [`ci/native-platform-matrix.yml`](../ci/native-platform-matrix.yml) lists a fixed whitelist of target OS/CPU combinations and their initial verification states.
- [`ci/Build-PlatformMatrix.sh`](../ci/Build-PlatformMatrix.sh) uses installed POSIX sh, CMake, Git and platform SDKs for local macOS/Linux or installed-NDK Android cross compilation.
- [`ci/Build-PlatformMatrix.ps1`](../ci/Build-PlatformMatrix.ps1) uses local PowerShell 7, MSVC, CMake, Git and Visual Studio's existing `vswhere` to create Win32/x64/ARM64 archives. It requires an exact version match with the operator's independently verified latest stable PowerShell release.
- [`Taskfile.yml`](../Taskfile.yml) exposes fixed profile aliases, but the Task CLI is **optional**. Run the POSIX or PowerShell script directly without installing a Go-based task runner or any other orchestrator.
- [`CMakeLists.txt`](../CMakeLists.txt) has the new explicit opt-in `-DSWE_STD_FREE_CORE_ONLY=ON`, which compiles `swe_std_free_portable_core` as a platform static archive and installs the three STD-free public headers. It skips legacy C++ targets **for that particular build**, but those source files remain present and unchanged in the default full build.

The static archive currently exports a native probe symbol and is not a drop-in executable, APK, package installer, or universal application. It is an initial compile/link-independent foundation for a multi-OS STD-free source library. Its object compilation still relies on a real C++20 compiler and SDK; its runtime import table and use by third-party applications require later verification.

## Platform profiles

| Profile(s) | Architecture | Toolchain needed locally | Expected scope |
| --- | --- | --- | --- |
| `macos-arm64` | Apple Silicon ARM64 | macOS + Xcode Command Line Tools | Native ARM64 archive |
| `macos-x86_64` | Intel 64-bit | macOS + Xcode Command Line Tools | Intel x86_64 archive |
| `macos-universal` | ARM64 + Intel x86_64 | macOS + matching two-arch Xcode SDK | Universal core archive |
| `macos-i386` | Intel 32-bit | Modern macOS cannot run it | **UNSUPPORTED** (not a failed build) |
| `windows-x86` | Win32 x86 | Windows + local Visual Studio MSVC x86 tools | 32-bit archive |
| `windows-x64`, `windows-amd64`, `windows-x86-64` | Same AMD64/x86_64 architecture | Windows + local Visual Studio MSVC x64 tools | One canonical x64 archive |
| `windows-arm64` | ARM64 | Windows + installed ARM64 MSVC components | ARM64 archive |
| `linux-x86_64` | x86_64 | Native x86_64 Linux, local compiler | x86_64 archive |
| `linux-arm64` | ARM64 | Native ARM64 Linux, local compiler | ARM64 archive |
| `linux-x86` | 32-bit x86 | Native 32-bit Linux + installed C++20 toolchain | x86 archive, NOT TESTED |
| `android-arm64-v8a` | ARM64 | Installed Android NDK | arm64-v8a archive |
| `android-armeabi-v7a` | ARM32 | Installed Android NDK | armeabi-v7a archive |
| `android-x86` | Intel 32-bit | Installed Android NDK | x86 archive |
| `android-x86_64` | Intel 64-bit | Installed Android NDK | x86_64 archive |

Native ARM64 and Intel macOS can be compiled as separate products or Apple universal Mach-O binaries. Apple [documents universal builds](https://developer.apple.com/documentation/apple-silicon/building-a-universal-macos-binary) and [incompatibility of 32-bit apps starting with macOS Catalina 10.15](https://support.apple.com/103076). Android's [official NDK ABI list](https://developer.android.com/ndk/guides/abis) and [CMake toolchain guidance](https://developer.android.com/ndk/guides/cmake) are normative for Android targets. SDK installations, deployment API levels, binary deployment and hardware execution must be independently validated.

## Local operator examples

POSIX, macOS Apple Silicon:

```sh
sh ci/Build-PlatformMatrix.sh macos-arm64
sh ci/Build-PlatformMatrix.sh macos-universal
```

POSIX, Linux x86_64:

```sh
sh ci/Build-PlatformMatrix.sh linux-x86_64
```

Android NDK, installed locally and without downloads:

```sh
ANDROID_NDK_HOME=/absolute/path/to/installed/ndk sh ci/Build-PlatformMatrix.sh android-arm64-v8a
ANDROID_NDK_HOME=/absolute/path/to/installed/ndk sh ci/Build-PlatformMatrix.sh android-x86_64
```

Windows PowerShell 7 on a native Windows workstation (independently check the release first; this is an **example version, not a permanent pin**):

```powershell
pwsh -NoLogo -NoProfile -File .\ci\Build-PlatformMatrix.ps1 -Profile windows-x64 -LatestStablePowerShellVersion 7.7.0 -ReleaseEvidenceUri 'https://github.com/PowerShell/PowerShell/releases/tag/v7.7.0'
```

Use `-Profile windows-x86`, `windows-amd64`, or `windows-arm64` for other Windows targets. The x64 and AMD64 aliases use the **same canonical build directory**. The script rejects unsupported Visual Studio versions, absent compilers or SDKs, mismatched PowerShell versions and dirty tracked files instead of silently falling back or downloading tools.

Each successful local build writes `build/iron-matrix/<profile>/iron-build-result.yml` with the local HEAD SHA and **`ARCHIVE_BUILT_NOT_RUNTIME_TESTED`**. Failures after evidence setup are recorded as `FAIL` with their stage. These records are derived **local evidence**, not proof that the OS profile was executed by ChatGPT. No script sends them to an external API.

## Remaining complete-repository work

The original repository had dozens of library-backed C++ headers, tests, tools and benchmarks, including `shared/toolong.hpp` and `shared/gammagamma.hpp`. They are not removed or automatically converted by this profile. Existing mathematical contracts, experiment provenance and tests must be preserved, including textual exactness and negative controls.

The strict whole-repository STD-free requirement remains a separate gate. To produce an up-to-date, Git-derived per-file inventory:

```sh
sh ci/Export-StdFreeInventory.sh > .iron-std-free-inventory.yml
sh ci/t-cross-posix.sh gate-no-std-repository
```

An inventory entry with no `std::` token is only a **source candidate**, not proof of no standard-library calls, compiler runtime, dynamic imports or transitive dependencies. The old application has an independent [STD-free parity CLI candidate](../src/parity_std_free_main.cpp) with its own tests; it is not automatically promoted over the original. The default full build still includes the original projects and may still require STL. The archive-only path is explicitly scoped rather than used to launder the whole-repository audit into a green result.

### T-CROSS build-state labels

- **CONFIGURED:** manifest and build parameters committed, not compiled on a matching host.
- **ARCHIVE_BUILT_NOT_RUNTIME_TESTED:** installed local compiler produced an archive; no target-device execution or link-time CRT isolation proof.
- **FULL_CTEST_PASS:** actual full native build and CTest suite passed, with source SHA, host/toolchain and exact test count captured. No result has been recorded yet for the new matrix.
- **STD_FREE_SOURCE_CANDIDATE:** a syntax/token inspection, insufficient for final dependency certification.
- **BLOCKED / UNSUPPORTED:** explicit failures or outdated architectures, not silent exclusions.

**PR #19 stays DRAFT and UNMERGED until maintainer promotion.** No GitHub Actions hosted runners, third-party package managers, Docker containers, Node-backed workflows or automatic network installations were added.
