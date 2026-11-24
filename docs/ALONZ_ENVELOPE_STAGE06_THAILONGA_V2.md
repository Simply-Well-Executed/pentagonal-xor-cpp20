# ALONZ ENVELOPE stage 06: THAILONGA CORE applied to derived OCI view

SPDX-License-Identifier: 0BSD

**Version:** ALONZ_STAGE06_THAILONGA_V2  
**Scope:** a new postflight *derived* Linux/amd64 OCI witness for the historical stage 06; original stage/source evidence is unchanged.  
**Release:** HOLD for native OpenBSD, GNU/POSIX licensing policy and any general production claim. Publishing this diagnostic may be separately authorized.

## Why the image now says CORE=OPENBSD2

The operator explicitly chose to **use the previously established THAILONGA CORE** rather than displaying only `CORE=NONE` in the image label. The v1 stage06 packaging proposal omitted that important projection.

The corrected, versioned OCI label is:

```text
STAGE=06;IDENTITY=OPENBSD3;OPENBSD_RELEASE=3.9;CORE=OPENBSD2;CORE_MODE=THAILONGA_POSTFLIGHT;CANONICAL_CORE=NONE
```

Interpret every field in its own namespace:

- `STAGE=06`: the sixth visit in the immutable, ten-stage serial experiment.
- `IDENTITY=OPENBSD3`: **project stage identity**, historically targeting OpenBSD 3.9, not the human-friendly OCI registry alias.
- `CORE=OPENBSD2`: the **THAILONGA-derived CORE of the image's retrospective view**. This is not evidence that a native OpenBSD 2.9 or 3.9 binary was compiled.
- `CORE_MODE=THAILONGA_POSTFLIGHT`: the CORE is valid in the **after-stage-10 reconstructed view**; YIN and YANG did not exist as inputs to historical stage 06.
- `CANONICAL_CORE=NONE`: the original frozen `ci/OPENBSD_SERIAL_COREAO.tsv` stage06 source row really has no CORE. That source remains authoritative and unmodified.

**No temporal backdating**: this is a *display/evidence projection*, not an amendment to stage06. Historical `CORE=NONE` and later `CORE=OPENBSD2` are not contradictory when their provenance and temporal scopes are preserved.

## Precisely where THAILONGA comes from

The OCI build invokes the existing, previously tested and frozen `ci/Invoke-OpenBSDThailongaPostflightV1.ps1`, producing an **append-only stage07 postflight JSONL record**. Its subject is stage07 `OPENBSD4_RETURN`, which itself records:

- Stage05 `OPENBSD4_FIRST` predecessor chain;
- Stage06 `OPENBSD3` witness `d220105c5b3fc0ca018e2ace6e0b7b02461852e28557ae4a28251731ca28fb22` as stage07 predecessor;
- Stage07 `OPENBSD4_RETURN` witness;
- Stage08 `OPENBSD2_YIN` and stage09 `OPENBSD2_YANG`, **with distinct original hashes**;
- Stage10 `COREAO_OPENBSD2` reconciliation and the existing THAILONGA SHA-256.

The build first runs `ci/Test-OpenBSDThailongaPostflightV1.ps1` for independent HEAD/TAIL comparisons, rejection of a modified YANG witness, and frozen-input integrity. It then regenerates the record and rejects misidentified subjects, mismatched stage06 predecessor, wrong YIN/YANG/COREAO hash, missing explicit HOLD state, or incorrect postflight mode.

The verified report is copied into the scratch image at:

```text
/evidence/thailonga-stage07-postflight.jsonl
```

It is deliberately named **stage07**, not fraudulently renamed to stage06. The stage06 ALONZ program references the THAILONGA CORE and exposes the fact that this is postflight only. No new cryptographic primitive, key or trust anchor is inferred from this SHA-256 record.

## The named image and alias are not the OpenBSD OS

| Role | Name |
| --- | --- |
| Display name | ALONZ ENVELOPE |
| Descriptive GHCR image | `ghcr.io/simply-well-executed/alonz-envelope-stage06-evidence` |
| Short GHCR pointer | `ghcr.io/simply-well-executed/openbsd4` |
| Descriptive Docker Hub image | `paulcheek/alonz-envelope-stage06-evidence` |
| Short Docker Hub pointer | `paulcheek/openbsd4` |

The requested nickname `openbsd4` is a **registry publication-order label**, *not* the source stage's operating-system release number. Do not replace the existing `openbsd1` through `openbsd3` witnesses or their digest receipts.

The image remains Linux/amd64 `FROM scratch`, compiled as freestanding native C++20 using Clang 18, `-O2`, no linked standard library, and no shell in the image. Build-time GitHub hosted Linux and PowerShell are tools of the diagnostic pipeline, not a native OpenBSD environment. The build gate compiles twice and runs the image twice offline, read-only, non-root, no network, to detect drift. A successful CI result verifies **only those tests**.

## Source, revisions and promotion

- The original stage06 package proposal `docs/ALONZ_ENVELOPE_STAGE06_V1.md` remains as historical context. V2 explicitly supersedes its *image-view CORE field*, not its source facts.
- Original stage06, stage07, YIN, YANG and stage10 ledgers remain frozen. No historical value is reassigned.
- Tags `sha-<commit>` are human-readable pointers that can technically change. The exact remotely observed `@sha256:<digest>` is the content-addressed image identity. A success claim requires actual registry push receipts, not just a Dockerfile or CI workflow.
- If publication is authorized, use the workflow `.github/workflows/alonz-envelope-stage06-openbsd4-once.yml`, never overwrite a previous digest or `latest`.
- Failure of an existing GNU/POSIX policy gate keeps general release HOLD. An isolated evidence package never certifies an actual OpenBSD native build.
- New independent CORE-first historical stage experiments require their own ID, source ledger and separately reviewed build evidence.

**Chosen, never assumed. Historical input ≠ derived output. MOVE ≠ EDIT.**
