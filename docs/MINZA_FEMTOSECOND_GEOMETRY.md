<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Scope: original first-party contributions only; existing third-party rights, source media exclusions and other reservations remain authoritative.
-->

# MINZA Hanzeye · femtosecond addressing and figure-eight / joint geometry

> **Document status: DUE FOR UPDATE (2026-11-19).** Verify claims against code and tests; sharpen active verbs; check copyright, project-mark language, accessibility, and 0BSD scope. This mark is not a failure verdict. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Status:** EXPLORATORY mathematical and visualization protocol. Not clinical age estimation or a new camera measurement.

## Time interval accuracy

One femtosecond equals `10^-15` second. A 60-fps video has nominal frame period `10^15 / 60 = 16,666,666,666,6662⁄3` femtoseconds, which is **not an integer number of femtoseconds**. An exact 15-place requested decimal timestamp must remain distinct from the displayed recorded or sampled frame. One half-second preview sample is not the next native frame.

C++20 reference uses `std::chrono::duration<std::int64_t,std::femto>` for exact *bounded* intervals. A signed 64-bit femtosecond count spans about ±9,223 seconds (approximately 2.56 h) from zero. Long-duration or absolute wall-clock applications need a segmented or wider representation rather than silent overflow. Samples should be identified by source-native integer frame index where possible, with rational frame rate and provenance. Half-open time windows are `[begin,end)`.

## 3 × 3 sticky reverse origami

Nine fixed UV tiles are grouped into three MINZA bands `[0,1,2]`, `[3,4,5]`, `[6,7,8]`. A reverse visit walks `8→7→...→0` but never reassigns source coordinates. Independent layered skins inherit the exact same transforms and hinge relationships. The back surface remains invisible and **unknown**, not generated.

## Figure-eight and hexagon overlays

An adjustable figure-eight curve:
```text
x(t) = cx + a sin(t)
y(t) = cy + b sin(2t)
t ∈ [0,2π]
```
The displayed curve can be approximated using a documented segment count (e.g., 2048) to estimate screen-pixel arc length. A regular hexagon with circumradius `r` has `perimeter = 6r`, `area = 3√3 r2/2`. The joint/neck overlay may be positioned on any user-selected region; its geometry does not confer anatomical calibration.

Pixels-to-millimeters conversion requires a calibrated scale and suitable perspective correction. A photograph alone does not establish metric joint dimensions. Any historical foot-as-example analogy applies to generic geometry, **not** to a validated facial, neck or biological age estimator. Displaying a toy `age-equivalent` number must be explicitly labeled nonclinical and nonpredictive, or omitted.

## Image enhancement boundaries

TEXTURE STRIP isolates visible high-frequency contrast; it does not undo unknown retouching, recover blocked details, or reconstruct hidden anatomy. An image plate is reversible; source coordinates and colors remain intact, and annotations are not evidence.

The user-defined MINZA / Hanzeye 12-axis presentation retains D11 as declaration-instance provenance rather than falsely claiming twelve measured anatomical dimensions. The nine audit eyes ask questions of the same evidence; they do not create new observations.

**Provenance:** source frame + capture metadata → pixel coordinates → geometry parameters → derived overlay → optional plate state → audit.
