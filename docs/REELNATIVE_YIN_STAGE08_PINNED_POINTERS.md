# REELNATIVE-YIN: exactly pinned OpenBSD2 stage #08

**CANONICAL stage**: 08, `OPENBSD2_YIN`, fixed historical reference `OPENBSD_2_9_BASE`,
`CORE=OPENBSD2`. **Display name**: REELNATIVE-YIN. **Short searchable registry
name**: `openbsd2`. These names are pointers to the distinct stage #08
*Linux/amd64 witness diagnostic*, NOT a substitute operating-system version,
native OpenBSD build, new kernel, or replacement for stage #09 YANG.

The existing authoritative serial ten-stage manifest and witness computation
are unchanged. Stage #08 requires the distinct **stage #07**
`OPENBSD4_RETURN` predecessor, then stage #09 YANG must reference the #08
witness and stage #10 COREAO must preserve both YIN and YANG.

## One exactly pinned manifest across four names

| Registry | Full REELNATIVE-YIN name | Short user-friendly pointer |
| --- | --- | --- |
| GitHub Container Registry | `ghcr.io/simply-well-executed/reelnative-yin-evidence` | `ghcr.io/simply-well-executed/openbsd2` |
| Docker Hub | `paulcheek/reelnative-yin-evidence` | `paulcheek/openbsd2` |

The workflow publishes the same `sha-<40-character-Git-commit>` tag at
all four repositories and requires **all four reported OCI manifest
digests to match**. A pull reference using `@sha256:<observed-manifest-digest>`
is the exact pinned frame, while SHA-derived tags are human-readable locators
but remain technically mutable. The digest must be observed from actual
completed workflow output, never inferred. The two shorter repo names are
content aliases, not HTTP redirects, and no `latest` tag is created or
overwritten.

## Preconditions and non-claims

The GitHub-hosted native Linux compiler used for this diagnostic is
Clang 18, `-std=c++20 -O2`, with freestanding, statically linked,
no-linked-libc ABI. The `FROM scratch` image contains only the
compiled Linux/amd64 witness-printing program, evidence records and license.
It is compiled twice and run in Docker twice with literal stdout compared
to the independently computed PowerShell ledger. The image embeds the
authenticated **ledger output relative to the repository's existing
metadata contract**, not an authentication of historical OpenBSD source.
No OpenBSD target native compilation, historical source signature,
hardware ABI validation or production release is claimed. Existing native
build and GNU/POSIX policy gates remain **HOLD**.

The existing `openbsd1` stage #09 YANG images, stage #10 COREAO images,
historical tags, and original unmerged PR remain intact.

**Failure never becomes policy. Chosen, never assumed.**
