# ALONZ ENVELOPE: canonical stage 06 / registry pointer openbsd4

SPDX-License-Identifier: 0BSD

**STATUS:** source-authored packaging proposal, subject to actual CI build and remote registry digests. Native historical OpenBSD build: **NOT DONE**. Product release: **HOLD**.

## Naming: three independent coordinates

- Canonical serial ordinal **06**: `OPENBSD3`, historical target OpenBSD **3.9**, CORE **NONE**.
- The descriptive image is **ALONZ ENVELOPE**, `alonz-envelope-stage06-evidence`. It is a specially compiled **Linux/amd64** freestanding C++20 diagnostic, *not* an OpenBSD executable, distro, compiler or native OS image.
- The user's requested human-friendly registry pointer is `openbsd4`. This counts the repository's previous **openbsd1**, **openbsd2**, **openbsd3** publication aliases, **not** a claim that stage 06 is OpenBSD 4.x.
- Stage 05 `OPENBSD4_FIRST` and stage 07 `OPENBSD4_RETURN` each remain a separate exact OpenBSD 4.9 witness, despite the numbering of this short registry label.

## Stratum-0 addressing lesson captured

The user-supplied source distinguishes **name**, **address**, and **route**, while further distinguishing bus, media, link, internet, transport, session, presentation, application and storage-fabric namespaces. The image intentionally prints one compact mnemonic to keep these three roles separate; it **does not** implement PCIe, DMA, a router, certificate validation, a SAN controller or all of Web9.X. The mnemonic is a design reminder, not an assertion that any OSI naming clause reduces to one universal equation.

## Canonical source authority

- `ci/OPENBSD_SERIAL_COREAO.tsv` and `ci/Invoke-OpenBSDSerialCoreao.ps1` are authoritative for stage identities and witness bytes.
- Stage 06 witness must equal `d220105c5b3fc0ca018e2ace6e0b7b02461852e28557ae4a28251731ca28fb22` for **this frozen protocol**; Stage 05 and 07 are verified independently.
- Stage 08 YIN and stage 09 YANG must remain separate. The resulting COREAO=OPENBSD2 is explicitly **later evidence** and must not be injected into historical stage 06 or overwritten onto its `core=NONE` field.
- Stage 06 OpenBSD 3.9 source metadata is only a historical CVS-tag reference and is not a verified downloaded snapshot, actual C++20 toolchain or native binary.
- This source newly authored under repository **0BSD** has no dependence on external historical binary artifacts. Other components retain their original license identities.

## Build and publication gates

The dedicated PowerShell build gate validates the ten-stage ledger, checks the negative test and source/evidence annex, checks embedded source witness values, compiles twice via Clang 18 C++20 `-O2` with no linked standard library, compares binary hashes, builds an offline `FROM scratch` OCI image, and runs the image twice without network/capabilities to verify exact output.

A dedicated GitHub Actions workflow, triggered only by the explicit stage06 publication commit on the existing working feature branch, publishes separately to GHCR and Docker Hub with unique `sha-<40-char source commit>` tags and compares the reported remote image digests across both registries and both aliases. Registry tags are not immutable by definition; digest-pinned references are authoritative. A CI success for the diagnostic remains **RELEASE=HOLD** for native OpenBSD and the repository's independent GNU/POSIX licensing policy. No `latest` tag, existing alias, original source evidence or PR merge is overwritten by this source change.

**Chosen, never assumed. MOVE ≠ EDIT. RETIRE ≠ ERASE.**
