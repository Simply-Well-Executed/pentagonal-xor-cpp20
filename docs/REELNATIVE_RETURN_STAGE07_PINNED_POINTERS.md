# REELNATIVE-RETURN: stage 07 / openbsd3 exact pinned pointers

**Canonical identity:** `OPENBSD4_RETURN`, stage `07`, fixed source
reference `OPENBSD_4_9_BASE`, release `4.9`, pass `RETURN`,
core `NONE`. This stage is **not** `OPENBSD4_FIRST` (stage 05),
`OPENBSD3` (stage 06), `OPENBSD2_YIN` (stage 08), or
`COREAO_OPENBSD2` (stage 10). The canonical CORE remains
`OPENBSD2` downstream. All stage identities and witnesses are
unchanged in the authoritative ten-stage ledger.

**Display name:** REELNATIVE-RETURN. **User-friendly registry pointer:**
`openbsd3`. That is a discoverability alias and does not claim that
OpenBSD 3 is inside the image, that stage 07 is OpenBSD 3.9, or that
OpenBSD 4.9 was natively built.

## Four names, one observed OCI digest

| Registry | Descriptive image name | Short pointer |
| --- | --- | --- |
| GHCR | `ghcr.io/simply-well-executed/reelnative-return-evidence` | `ghcr.io/simply-well-executed/openbsd3` |
| Docker Hub | `paulcheek/reelnative-return-evidence` | `paulcheek/openbsd3` |

All use one source-commit tag `sha-<40-character-commit>`.
The workflow requires exactly matching Docker CLI remote-push manifest
digests across all four images. A user can pull an exact immutable
manifest with `@sha256:<digest>` after a successful run. Tags are
human-friendly references, not immutable-by-construction; the observed
digest is the exact pinned frame.

The evidence diagnostic prints the stage 05 FIRST witness, stage 06
predecessor witness, and distinct stage 07 RETURN witness, then explicitly
states `NATIVE_OPENBSD_BUILDS=0` and `RELEASE_STATUS=HOLD`. The workflow
first independently computes the canonical ledger twice, rejects an
intentionally altered 4.9 return stage, validates ten native-evidence
HOLD entries, compiles the Linux/amd64 diagnostic twice using Clang 18
C++20 `-O2` without a linked libc or standard library, and runs the
`FROM scratch` container twice without network/capabilities.
The entire Docker environment is **Linux**, not native OpenBSD, and
there are no actual historical OpenBSD 4.9 binary or source authenticity
attestations. Existing GNU/POSIX release HOLD remains unchanged.
The previously published `openbsd1` and `openbsd2` are preserved.
The GitHub PR is not merged, and no existing `latest` tag is touched.

**Failure never becomes policy. Chosen, never assumed.**
