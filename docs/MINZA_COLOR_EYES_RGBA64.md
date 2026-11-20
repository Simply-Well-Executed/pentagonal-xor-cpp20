# MINZA Color Eyes · RGB / CMYK / MONOCHROMATIX / RGBA64

> **Document status: DUE FOR UPDATE (2026-11-19).** Verify claims against code and tests; sharpen active verbs; check copyright, project-mark language, accessibility, and 0BSD scope. This mark is not a failure verdict. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Status:** EXPLORATORY derived image visualization, not calibrated color management or printer forensics.
**C++20 reference:** [MINZA_Color_Eyes_Gate_Core_CXX20.hpp](../shared/MINZA_Color_Eyes_Gate_Core_CXX20.hpp).
**Tests:** [minza_color_eyes_test.cpp](../tests/minza_color_eyes_test.cpp).

## 8-bit source → 64-bit representation

Decode an 8-bit RGB image (JPEG alpha conventionally opaque). Preserve `r,g,b,a` each in `[0,255]`.
Exact expansion to unsigned 16-bit channels:

```text
R16 = 257 × R8; G16 = 257 × G8; B16 = 257 × B8; A16 = 257 × A8
RGBA64 = (R16 << 48) | (G16 << 32) | (B16 << 16) | A16
format: 0xRRRRGGGGBBBBAAAA; unsigned decimal must not be rounded via IEEE-754 double
```

Example: `RGBA8(255,0,0,255) → 0xFFFF00000000FFFF`.
This is **RGBA16 × 4**, not a recovery of extra sensor detail. RGB24 decimal >255 or RGBA64 decimal >255 only denotes a multibyte integer. It **does not** establish embedded bits, printer tracking dots, image tampering, or steganography. This is the explicit rebuttal to interpreting all values above `0xFF` as covert data.

For accurate decimal output in browser code use native `BigInt`; C++20 can use `std::uint64_t`. A 64-bit integer can exceed the exact consecutive-integer range of JavaScript `Number`.

## RGB eyes and five user-defined directional pairs

RED / 红 hóng, GREEN / 绿 lǜ, BLUE / 蓝 lán: independent source-channel inspection.
Named directional axes:
- RED ↔ GREEN and the **distinct reverse direction** GREEN ↔ RED.
- BLUE ↔ YELLOW.
- PURPLE ↔ BLUE.
- LAVENDER ↔ LYLACK (retain the user's supplied spelling).

These are directional color axes, **not** a claim that every pair is a scientific additive-color complement. In the original browser prototype each RGB pixel is projected onto a line between declared endpoint colors, then tinted; the projection is a derived display convention.

## CMYK eyes (device-independent approximation only)

Given decoded `r,g,b` on the byte scale and `m = max(r,g,b)`:
```text
K = 255 − m
if m = 0: C = M = Y = 0
else: C = 255*(m−r)/m; M = 255*(m−g)/m; Y = 255*(m−b)/m
```

Round and clamp for byte output. These channels drive reversible inspection plates named CYAN / 青 qīng, MAGENTA / 洋红 yánghóng, YELLOW / 黄 huáng and BLACK / 黑 hēi. They do **not** substitute for ICC printer profiles, Lexmark firmware conversion, paper-ink colorimetry, or authentication of tracking marks.

## Color-vision and accessibility plates

Protanopia, deuteranopia and tritanopia are **approximate** RGB display transforms, not the actual Windows 11 / World of Warcraft shader matrices or a medical simulation of an individual's visual experience. Every control has conspicuous words `ON` and `OFF`, discernible keyboard focus, and a text label. Follow [WCAG 2.2 Use of Color](https://www.w3.org/WAI/WCAG22/Understanding/use-of-color): never convey the only meaning through hue.

## MONOCHROMATIX · hexagonal sampling

An optional reversible plate renders grayscale luminance and a hexagon overlay:
`Y8 ≈ round(0.2126R + 0.7152G + 0.0722B)`.
A regular hexagon of circumradius `r` has perimeter `6r` and area `3√3 r2/2`. The hexagon is a spatial grouping, not a concealed-information detector. Retain original coordinate, unmodified RGB8, RGB24 hex/decimal, RGBA64 hex/decimal, channel bins and bit layout for every audited pixel.

## Scientific boundaries

Colors are correlated, not six independent witnesses. Color transformations are lossy renderings and do not strip unknown retouching or prove personal identity. All channels remain traceable back to one source pixel. Filter OFF must remove the overlay, not reconstruct a guessed source.

**C++20 does not itself guarantee speed or correctness:** the kernel has deterministic integer packing, explicit bounds, and tests. Measured performance and a validated pipeline require separate evidence.
