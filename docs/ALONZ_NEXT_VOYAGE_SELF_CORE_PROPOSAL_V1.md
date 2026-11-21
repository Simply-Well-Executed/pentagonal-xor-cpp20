# ALONZ next voyage: edition-as-CORE bootstrap (proposal v1)

<!-- SPDX-License-Identifier: 0BSD -->

**Status:** EXPLORATORY, NEXT RUN ONLY. No mutation to existing canonical manifest.
**Origin:** user instruction on 2026-11-20: "when we loop through this again ... the CANONICAL_CORE will be this edition itself".

## The next-loop rule

At the start of a **new, explicitly versioned** serial voyage, the immutable, verified artifact from the **completed previous edition** is selected as the new experiment's CANONICAL_CORE source.

```text
Edition E source / frozen ten-stage ledger (unchanged)
   -> THAILONGA postflight CORE=OPENBSD2
   -> separately built and tested ALONZ stage06 evidence image
   -> *actual verified registry digest* + source commit + build receipts
   -> NEW edition E+1 manifest input: CANONICAL_CORE = EDITION_E@sha256:<verified-digest>
   -> NEW run identifiers, independent YIN and YANG, fresh witnesses
```

This means the previous edition serves as a *chosen input to the next iteration*, rather than a later output retroactively changing its own original input. No circular self-referential digest is constructed. A hash does not certify correctness or prove an image is native OpenBSD.

## What must not be conflated

- The **existing** `ci/OPENBSD_SERIAL_COREAO.tsv` has original stage06 `core=NONE` and retains that historical field. It is frozen and must not be edited in place.
- The **current ALONZ** image's *derived view* uses `CORE=OPENBSD2;CORE_MODE=THAILONGA_POSTFLIGHT;CANONICAL_CORE=NONE`. Its source input, postflight reference, and registry nickname `openbsd4` are distinct identities.
- The **new E+1** manifest, when separately approved and frozen, may designate the completed E artifact as its `CANONICAL_CORE`. This is a new experiment with a new identity and provenance. An E+1 source build must not assert that an E+1 binary already existed at its own input stage.
- "Walking up and down stairs" is the owner's proposed iterative compatibility methodology; it is not an official statement of how OpenBSD builds an operating system across different historic versions.
- The current image is a Linux/amd64 freestanding C++20 OCI diagnostic and not an OpenBSD release, kernel, userland, or native OpenBSD image.

## Entry gate for choosing the next CORE

1. Complete the existing ALONZ stage06 / THAILONGA OCI pipeline for this edition. Verify successful offline HEAD/TAIL, unmodified original witnesses, and actual remote registry push receipts.
2. Record the exact immutable **content digest** of the selected edition, source commit SHA, descriptive repository, architecture, license review state, and independent validation. Do not use mutable `latest` or the short `openbsd4` tag as the authority.
3. Freeze a new protocol ID and the precise bytes used to encode the selected prior-edition artifact identifier. The new manifest must explicitly distinguish `CORE_SOURCE=EDITION_E` from `CORE_DIGEST`, historical stage names and THAILONGA projections.
4. Preserve the prior E source, its independent YIN/YANG witnesses, stage06 and stage07 source values, the stage10 reconciliation, archive/retired context and all negative-test evidence unchanged.
5. Build a *new* E+1 test ledger, independently hash/validate it and carry new source/binary/test results. Any unavailable native OpenBSD host, old compiler mismatch, source verification gap or GNU/POSIX policy issue remains HOLD, not PASS.
6. Explicitly approve the selection of **which** verified artifact constitutes the edition's CORE. A running or merely tagged registry image does not automatically qualify as that selection.

## Current values

```text
PROPOSED_NEXT_PROTOCOL=ALONZ_VOYAGE_E_PLUS_1
EDITION_E_SOURCE_COMMIT=PENDING_FINAL_CI_VERIFICATION
EDITION_E_REGISTRY_MANIFEST_SHA256=PENDING_REMOTE_PUSH_VERIFICATION
NEXT_CANONICAL_CORE=NOT_YET_SELECTED
MUTATE_ORIGINAL_MANIFEST=NO
NATIVE_OPENBSD_BUILD_ATTESTATION=NONE
RELEASE=HOLD
```

**Failure never becomes policy. Chosen, never assumed. RETIRE != ERASE.**
