# MINZA Temporal PENTAGONAL · five-sample ring and audit plates

> **Document status: DUE FOR UPDATE (2026-11-19).** Verify claims against code and tests; sharpen active verbs; check copyright, project-mark language, accessibility, and 0BSD scope. This mark is not a failure verdict. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Status:** EXPLORATORY presentation adapter; does not replace the canonical Pentagonal XOR / six-stage Machine.

## Two separate source-preserving experimental definitions

**v5 center-reference agreement** (original sticky-reverse plate):
```text
indices = [(i−2) mod N, (i−1) mod N, i, (i+1) mod N, (i+2) mod N]
h[k](pixel) = RGBmean_absdiff(source[index[k]], source[i]) <= tolerance
AND_center(pixel) = h[0] & h[1] & h[2] & h[3] & h[4]
```
The center sample compares to itself in one operand. The change plate is the logical complement of `AND_center`. Report the percent of image pixels passing as agreement only.

**v6 cyclic-edge gate** (C++20 reference header):
```text
b[k](pixel) = RGBmean_absdiff(source[index[k]], source[index[(k+1)%5]]) <= tolerance
mask = Σ(b[k] << k) for k=0..4
AND = mask == 0x1F
OR  = mask != 0
XOR = odd_parity(mask)
local_COR_mismatch = 0x1F XOR mask
```
The fifth edge closes the **mathematical sample ring**; it does not claim physical continuity between beginning and end of the video. These v5/v6 operands are **not interchangeable**; changing definition requires a new experiment ID, exact provenance and separate results.

For N=67 preview samples:
```text
i=0  → [65,66,0,1,2]
i=1  → [66,0,1,2,3]
i=65 → [63,64,65,66,0]
i=66 → [64,65,66,0,1]
```

## Canonical boundaries

Original scheduler: `XOR → OR → AND → NOP → COR → FLOP` and then XOR only at the next scheduled native tick.
- **NOP:** preserve datum and prior logical held value; no image modification.
- **COR:** repository's [PENTAGONAL_GATE_CASCADE.md](PENTAGONAL_GATE_CASCADE.md) defines a six-block, 64-bit operand that is different from the local 5-bit overlay mismatch.
- **VISTA A/B/C:** read-only CAPTURE / VERIFY / SYNTHESIZE views of the **same** gate trace; not independently authenticated witnesses. See [VISTA_9X9.md](VISTA_9X9.md).
- **FLOP:** scheduler redirect, not another XOR execution.
- **AND/OR/XOR:** all 32 Boolean configurations require independent tests.

## What the masks cannot establish

Pixel stability is not a biometric match and cannot prove a person stayed identical throughout a reel. The results are sensitive to motion, registration, camera shifts, occlusion, compression, illumination, and resampling. In particular, wraparound is an analysis convention, not a time-reversal recording or evidence that the last frame was adjacent to the first.

Source and experiment definitions must remain frozen for comparisons. Threshold, sample spacing, crop, normalization, color space, source checksums and renderer belong in the record. A gate pass is not a forensic attestation.

## Presentation contract

Each gate projection is a separate nine-tile transparent origami plate with a bold ON/OFF button. OFF recovers baseline; SHOWCASE FAN changes only display separation, not computed data. Prefer displaying the exact five original source indices and the 5-bit mask alongside any image. Report disagreement, not fabricated confidence.

**Provenance:** `source → sample indices → RGB8 comparisons → gate mask → overlay → local audit`. No mutation, no hidden interpolation, no cross-clip identity claim.
