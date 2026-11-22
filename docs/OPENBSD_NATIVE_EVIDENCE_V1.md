# OpenBSD serial COREAO: native-build evidence annex v1

**Status: EXPERIMENTAL / HOLD.** This annex adds source-provenance evidence without changing
the canonical ten-stage manifest, its SHA-256 witness chain, the original CORE, or the
existing source-contract protocol. No native OpenBSD source or project binary was built
by this change. The existing Windows MSVC C++20 /O2 smoke result is separate evidence.

## Exact source evidence and verification boundary

- 8.0: fixed release `OPENBSD_8_0_BASE`, `ci/OPENBSD_80_SOURCE_LOCK.tsv`.
  The published `src.tar.gz` SHA-256 is
  `9984c74ff875b5ff041e4676a7f3e4e1ead2cc47a284d705eb9f37cbec4fe795`.
  Source archive download, independent signature verification, native compiler,
  binary and patched -stable snapshot remain unattested.
- 7.9: fixed release reference `OPENBSD_7_9_BASE`,
  `ci/OPENBSD_79_SOURCE_LOCK.tsv`. The official release-directory
  `SHA256.sig` publishes `src.tar.gz` SHA-256
  `fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b`.
  The same file records published kernel, ports, Xenocara and base-key checksums.
  This is a **published-checksum metadata checkpoint**, NOT an independently
  authenticated source checkout and not the old ledger's `SOURCE_UNPINNED` promotion.
- 6.9 / 5.9 / 4.9 / 3.9 / 2.9: retain the conventional fixed-release CVS tag
  references `OPENBSD_6_9_BASE`, `OPENBSD_5_9_BASE`, `OPENBSD_4_9_BASE`,
  `OPENBSD_3_9_BASE`, `OPENBSD_2_9_BASE`. These are **tag names for planned
  retrieval**, not audited file-level CVS revision manifests or authenticated
  source-content hashes. Each stays source-unverified.
- The mutable `OPENBSD_x_y` branch and fixed `OPENBSD_x_y_BASE` release tag are
  **different source states**. An `-stable` build requires its own complete
  checked-out tree, point-in-time revision list, hashes, patches, and attestation.
- The official project release page for OpenBSD 2.9 dates to June 1, 2001. It
  identifies GCC 2.95.3 among bundled outside components. That vintage toolchain
  cannot be described as a native C++20 compiler. GNU licensing is not
  implicitly approved by its inclusion in an OpenBSD release.

The ten rows in `ci/OPENBSD_SERIAL_NATIVE_EVIDENCE.tsv` explicitly separate
the release reference, published source checksum (when known), native host,
toolchain, native compilation, binary SHA-256, tests, and status. Stage 05 and
07 remain distinct OpenBSD 4.9 passes. Stages 08 and 09 remain independently
identified OpenBSD 2.9 YIN and YANG passes; stage 10 reconciles without
building a substitute CORE. **CORE = OpenBSD 2**. None is dropped or merged.

## Native build evidence acceptance protocol (future new version only)

1. On an actually booted, version- and architecture-matched OpenBSD target,
   record the OS release, architecture, compiler executable, version, executable
   checksum, all relevant flags, and toolchain licenses. Use `-O2` for
   prescribed compilations; never silently fall back to `-O0`.
2. Acquire an official fixed-release source snapshot or authenticated CVS
   revision collection. Verify the source file checksums. For releases that
   support `signify`, verify the original official `SHA256.sig` with an
   independently trusted release key, for example
   `signify -C -p openbsd-79-base.pub -x SHA256.sig src.tar.gz`,
   from a directory containing the actual downloaded files.
   Merely copying a published hash or key DOES NOT verify authenticity.
   Historical releases predate signify and require a separately reviewed
   authenticity strategy, not an invented signature.
3. Build the desired **project component** using the actual native compiler,
   recording all stdout/stderr, exit status, compiler and source hashes,
   binary SHA-256, tests and ABI measurements. A kernel/userland rebuild is a
   different task and follows the relevant release-specific OpenBSD manual.
   The current C++20 portable core is a candidate only where a verified
   C++20 compiler exists.
4. For old systems, a **separately compiled** legacy C or other version-appropriate
   portable probe may gather genuine native ABI observations. Such a probe is
   **not** proof that the full C++20 CORE compiled, not a binary-compatibility
   certificate, and not permission to silently reduce numerical precision,
   integer widths or feature availability. Never force the historical compiler
   to accept C++20 it cannot parse.
5. Process one target at a time in exactly the original 10-stage order,
   including two distinct OpenBSD 4.9 visits, two OpenBSD 2.9 witnesses and
   COREAO reconciliation. Do not advance a target to PASS until its source
   and independent native build/test evidence are attached and verified.
   An unavailable host remains HOLD; a demonstrated impossible combination
   may be marked IMPOSSIBLE with explicit evidence, never deleted.
6. Any genuine new build measurements must be recorded under a **new,
   explicitly versioned evidence protocol**, with the old evidence and hashes
   retained. The v1 validator deliberately rejects premature manual promotion
   to PASS, fictitious binary digests and stage renaming.

## How this annex is checked

`ci/Test-OpenBSDNativeEvidence.ps1` verifies the complete 10-row annex against
the unchanged canonical manifest, separately checks published 7.9 checksum
metadata, requires all unattested build fields to be HOLD, and emits file SHA-256
witnesses. The dedicated Windows workflow executes the check twice as an
independent HEAD+TAIL comparison and rejects a deliberately modified 4.9 return
stage in a temporary copy. These are **metadata tests only**.

Official research entry points:

- https://www.openbsd.org/80.html
- https://www.openbsd.org/79.html
- https://cdn.openbsd.org/pub/OpenBSD/7.9/SHA256.sig
- https://www.openbsd.org/29.html
- https://www.openbsd.org/stable.html
- https://www.openbsd.org/faq/faq5.html
- https://www.openbsd.org/anoncvs.html
- https://man.openbsd.org/signify.1
- https://docs.github.com/en/actions/concepts/runners/github-hosted-runners

Do not treat a hosted Windows/Linux/macOS action as native OpenBSD. GitHub's
hosted runner operating systems do not include OpenBSD. Native OpenBSD host
availability has not been established in this repository.

**Failure never becomes policy. Chosen, never assumed.**
