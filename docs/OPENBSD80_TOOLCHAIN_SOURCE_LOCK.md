# OpenBSD 8.0 source and toolchain custody

**CURRENT PROPOSAL, 2026-11-20. RELEASE HOLD.** Official latest OpenBSD release: **8.0**, October 20, 2026.

## Canonical source record

Canonical official SHA-256 pins live in [the source lock](../ci/OPENBSD_80_SOURCE_LOCK.tsv): five entries covering the official 8.0 release `src.tar.gz`, `sys.tar.gz`, `ports.tar.gz`, `xenocara.tar.gz`, and `openbsd-80-base.pub`. Source base tag is `OPENBSD_8_0_BASE`. The patch-bearing **OPENBSD_8_0** CVS -stable branch is mutable and must NOT be called an immutable source pin without separately verified commit/file revisions and the applicable errata.

The upstream authority is the [OpenBSD 8.0 release page](https://www.openbsd.org/80.html), [signed source archive directory](https://cdn.openbsd.org/pub/OpenBSD/8.0/), [official SHA256 manifest](https://cdn.openbsd.org/pub/OpenBSD/8.0/SHA256), [8.0 errata](https://www.openbsd.org/errata80.html), and [release(8) manual](https://man.openbsd.org/OpenBSD-8.0/release.8). The source verifier is [ci/Assert-OpenBSD80SourceLock.ps1](../ci/Assert-OpenBSD80SourceLock.ps1).

MagicComb HEAD+TAIL checks the repository's exact source-lock bytes without network fetch or source compilation. An optional archive directory additionally requires all locked file hashes and an authentic signature verified using the pinned release base public key and OpenBSD's `signify -C`. A locked checksum without verification does NOT prove that source was compiled or that source has been patched to -stable.

## Applicability and restrictions

| Component | OpenBSD 8.0 source reference | Status |
|---|---|---|
| OpenBSD base compiler, assembler and linker | LLVM/Clang 22.1.6 with OpenBSD patches in src.tar.gz | Source candidate only; binary version is not a source build attestation; third-party license requires separate approval |
| Kernel drivers | sys.tar.gz | Candidate only for OpenBSD hardware; patched -stable snapshot and hardware review pending; cannot replace Windows drivers |
| Base shell, make, libraries, signify | src.tar.gz | Source reference only; project POSIX/GNU restrictions remain |
| Ports and other language/toolchain packages, including CMake | ports.tar.gz | Recipe/patch lock only; upstream distfiles, license notices and dependency closure NOT pinned |
| Graphics stack | xenocara.tar.gz | Source reference only; third-party rights not automatically approved |
| MSVC, Windows SDK/drivers | Microsoft vendor source | Cannot be replaced or version-pinned by an OpenBSD tarball |
| Oracle Java, PowerShell, .NET | Their own official distributors | Maintain independent source, version, license and hash pins |

The official OpenBSD release also includes legacy GCC (GNU) and third-party components. Therefore **adopting OpenBSD does not satisfy a zero-GNU or zero-POSIX policy** by itself. The project's exact license approvals still govern.

## Build applicability and actual evidence

OpenBSD's `release(8)` manual explains native Kernel/base builds, but its documented pipeline uses make and shell procedures. Those steps remain incompatible with this repository's currently forbidden authored POSIX execution until independently reviewed. **No OpenBSD source archive has been downloaded or compiled by this change.** Current Windows GitHub runners cannot certify a native OpenBSD source build. The pinned base release is not the fully patched November 2026 -stable snapshot.

OpenBSD-only CMake compiler detection now requires Clang 22.1.6, ISO C++20 and `-O2` to prevent accidental incompatible compiler substitutions. This is version matching, not provenance. All other project compilation configurations now use `-O2`, keeping previous `-O3` experimental results in Git history.

Before promoting a toolchain: authenticate source archives and errata, freeze the exact stable snapshot, prove source-built compiler/linker binary hashes, review all component licenses and NOTICE files, build/test on native hardware, perform HEAD+TAIL, and obtain explicit release approval.

**Failure never becomes policy. Chosen, never assumed.**
