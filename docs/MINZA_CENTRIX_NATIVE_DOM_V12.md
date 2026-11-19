# MINZA · Centrix Native DOM v12 · Transparent globe and paired light

> **Status: EXPLORATORY · DUE FOR UPDATE (2026-11-19).**
> Copyright (c) 2026 Paul Allen Cheek; SPDX-License-Identifier: 0BSD for original project source and documentation. Exclude the recorded source media and any third-party marks or artwork from this declaration.

## Purpose and source custody

**Restore** the original Sticky Reverse Unfold globe design using CSS 3D native DOM circles and no SVG or Canvas for the **globe renderer**. **Retain** the original 3×3 source-UV tiles, plate ON/OFF controls, NINZA/PING byte traces, DOLPHIN subject selector and reversible nine-loop FLOP/FLIP presentation.

**Limit:** The legacy analytical RGB/CMYK/gate pixel pipeline still uses offscreen Canvas buffers. A complete Canvas-free decoder has *not* been implemented. The source files of account-private ChatGPT Sites named **Hanzeye · One word, both sides** and **Unicorn Coin Club · SDL6 Playroom** could not be accessed in this chat. The v12 design therefore references the earlier local **Sticky Reverse Unfold** XHTML, **not** copied Site source code.

## Sphere and transparency

**Place** the CSS ring shell inside the same rotating `world` element as the original face tiles. **Draw** nine transparent bordered great-circle elements, six latitude rings and five passive reference spokes. **Remove** the prior opaque SVG radial fill and independent sphere lighting. **Set** the photographed front tiles to `backface-visibility:visible` so looking through the rear shows their actual image, mirrored by the viewpoint. **Leave** the reverse surface observationally unknown.

**Draw** one sparse nine-cell rear selector with transparent backgrounds and `UNOBSERVED` labels. The rear plate starts ON but has a user-controlled ON/OFF switch. Hiding it must not erase a source frame. Use native buttons for the nine rear cells. A rear selection maps bijectively to its front UV counterpart.

## One centered Centrix light, no second emitter

The centered cross is geometric coordinate **C=(0,0,0)** within the 520-pixel world, not the old `translateZ(220px)` front-facing badge. Present the cross as the **only modeled light source**. The presentation layer makes no physical inverse-square or calibrated radiance claim.

```text
front n ∈ 0..8
rear(n) = 3 * floor(n/3) + 2 - (n mod 3)
Manhattan distance d(a,b) = |row(a)-row(b)| + |col(a)-col(b)|
R10 display exposure e ∈ [0.25,2.50]
illumination(selected,target,e) =
  clamp(e * (0.40 + 0.60 * 0.58^d(selected,target)), 0.07, 2.70)
```

**Apply** this single result to each source tile and its geometrically mirrored rear cell. Selecting rear tile Rn **selects** front `mirror(n)`; selecting front tile n **focuses** rear `mirror(n)`. The symmetric neighborhood receives equal derived intensities in both views. **Maintain** the cross ON/OFF button as a glyph visibility control; R10 alone controls illumination intensity and retains its bounded MIN/MID/MAX/COREAO MIRROR operation.

**Record** the selection side, front ID, rear ID, exposure, and unchanged-source flag. **Avoid** independent rear lamps, separate exposure sliders, blanket image light filters, false underside photographs, or fabricated measurement.

## Validation

The C++20 kernel [minza_centrix_light_cxx20.hpp](../shared/minza_centrix_light_cxx20.hpp) defines the mapping and bounded radiance. The [independent test](../tests/minza_centrix_light_test.cpp) checks all 81 front/target combinations at three exposure values, involutive mirror mapping, radiance bounds and equal mirrored values.

Local v12 viewer checks: both HTML/XHTML snapshots had the same SHA-256; all 120 embedded JPEG payloads matched v11 exactly, and the XHTML parsed as XML. **Chromium headless visual rendering timed out**; do not mark it visually verified. Keep this document **DUE FOR UPDATE**.

## Rights and evidence

No personal video frames or embedded JPEGs were pushed to the public repo. This document and the mathematical reference are exploratory derivatives, not claims of biometric identity, unseen anatomy, unregistered trademark exclusivity or independent witness integrity.

**Source → UV coordinates → one Centrix exposure + symmetric tile mapping → reversible CSS display → audit.** Chosen, never assumed.
