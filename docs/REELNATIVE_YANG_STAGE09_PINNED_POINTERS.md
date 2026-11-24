# REELNATIVE-YANG: pinned stage-09 OpenBSD2 YANG evidence

**Classification: stage-09 witness diagnostic / experimental / OpenBSD-native HOLD.**

The preferred human-friendly name for this *diagnostic image* is
**REELNATIVE-YANG**. The user-requested short alias **openbsd1** is a
**registry-address pointer**, not an OS version, kernel, native ABI,
personality, stage renumbering or change to CORE. The current canonical
stage 09 is `OPENBSD2_YANG`, release 2.9, with original
`CORE=OPENBSD2` unchanged. Its immediate predecessor is
`OPENBSD2_YIN` at stage 08. Stage 10
`COREAO_OPENBSD2` remains independent.

## Exact pinned frame (versioned tag and immutable digest)

The one-shot GitHub Actions workflow builds and tests a new stage-09
freestanding Linux/amd64 C++20 evidence runner from existing authoritative
source, then publishes *the same manifest bytes* at four repo names:

| Registry | Preferred name | Human-friendly short alias |
| --- | --- | --- |
| GitHub Container Registry | `ghcr.io/simply-well-executed/reelnative-yang-evidence` | `ghcr.io/simply-well-executed/openbsd1` |
| Docker Hub | `paulcheek/reelnative-yang-evidence` | `paulcheek/openbsd1` |

All four names use the same exact `sha-<40-character-source-commit>` tag.
Each resulting registry manifest digest must match the preferred GHCR image
digest. The actual digest is **not** assumed before the upload and is
retrieved from the publisher's successful `docker push` output.

A registry alias is another name for identical OCI content, not an HTTP
redirect. An *immutable pin* is the `@sha256:<manifest-digest>` image
reference; a tag by itself is technically mutable even when derived
from a Git commit. No pre-existing `latest` tag is overwritten or
silently linked. Users can search the short repo names and choose the
specific pinned version. Registry visibility and publication must be
confirmed from actual CI outcomes.

## Guardrails

- Keep the canonical ten-stage manifest, HEAD/TAIL witness chain, CORE,
  YIN and YANG separate and unmodified.
- Validate stage 08's witness as the `previous_sha256` of stage 09;
  validate stage 10 still references both stage 08 and 09.
- Require source manifest/tamper checks, metadata annex checks, two identical
  Clang 18 `-O2` freestanding ELF builds, two matching restricted Docker runs,
  and identical manifest digests at both registries and both aliases.
- The OCI file is a `FROM scratch` Linux/amd64 userspace diagnostic, not
  a native OpenBSD 1 or 2.9 image. No OpenBSD native build is claimed.
- The pre-existing GNU/POSIX compliance failure and native OpenBSD
  build HOLD remain as evidence. No production release, merge or
  overwrite of another package follows from this publication.

**Failure never becomes policy. Chosen, never assumed.**
