<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Scope: original first-party contributions only; existing third-party rights, source media exclusions and other reservations remain authoritative.
-->

# NINZA Unforging and PING Untwirling · MINZA Pinwheel Atlas

> **DUE FOR UPDATE (2026-11-19).** This exploratory presentation adapter requires source/visual review before claims of deployed operation. Copyright (c) 2026 Paul Allen Cheek for original contributions; SPDX-License-Identifier: 0BSD for owned writing and code. Third-party media and marks retain their rights.

## Authority and chosen terminology

The source of truth is shared/sigma_penta_ping.hpp. Preserve the nine zero-based lanes and six bytes per lane. In this adapter, UNFORGE means **separate and inspect** the two stored NINZA copies. UNTWIRL means **invert** the recorded PING permutation. Neither word describes cryptanalysis, invention of missing data, or an authenticated recovery.

MINZA[0] groups loops 0,1,2; MINZA[1] groups loops 3,4,5; MINZA[2] groups loops 6,7,8. The source operator makes three overlapping five-ligand parity ring scores per loop, quantizes each into U16 and stores three little-endian words: P0 low/high, P1 low/high, P2 low/high = six bytes. Nine lanes contribute exactly 54 bytes.

## NINZA · UNFORGE

Read primary[lane][column] and mirrored[lane][column]. Preserve both. Compute xor_difference[lane][column] = primary XOR mirrored; count differences without modifying either copy. The existing producer sets mirrored bytes to **identical copies** of primary bytes. Equality therefore adds no entropy, independent evidence, authentication, or protection against coherent tampering. Display both byte streams and their difference as plaintext. Do not silently choose a winner when they disagree.

## PING · UNTWIRL

Use only the existing column-dependent bijection, indexed from zero:

    s = (position + column) mod 9
    lane(column,position) = s if column is even, else 8-s
    braid[column * 9 + position] = primary[lane(column,position)][column]
    restored[lane(column,position) * 6 + column] = braid[column * 9 + position]

Recover each of the 54 bytes exactly when the same permutation and intact braid are supplied. Check every destination exactly once. Compare the untwirled output against primary and the producer's cached restored array separately; report both mismatch counts. A successful local roundtrip does **not** prove that either compared source was independently trustworthy.

FLOP/FLIP is a separate reversible presentation route. Odd-index FLIP toggles route direction but **does not alter** PING's column mapping, source order, or the native six-gate controller. The nine terminal-route labels remain FLOP-FLIP-FLOP-FLIP-FLOP-FLIP-FLOP-FLIP-FLOP.

## Atlas image-sampling adapter

The v8 offline viewer must declare its source-to-byte mapping before applying the C++ algorithm. Its experimental sampler uses the original 306 × 396 source crop, nine fixed 3 × 3 tile centers, radius 20 pixels, and five directed rays per lane. The direction angle in degrees is (9 × lane + 72 × field) mod 360. For each ray, read the unchanged source pixel and round 0.2126R + 0.7152G + 0.0722B to one byte. These five bytes are caller-derived measurements; this is **not** the original camera capture's five-ligand definition and is not a covert/identity detector.

Build the nine continuous ligands F0..F4, MIN, MAX, MEAN, RANGE divided by 255; derive three overlapping five-ligand parity rings with the canonical formula P5 = (1 - product(1 - 2h))/2 and quantize floor(65535 × P5 + 0.5), as the repository source specifies. The viewer shows the complete mapping and 6-byte primary/mirror lane text. The overlay artwork is display-only and can be disabled with separate bold ON/OFF buttons. PING exposes six large byte-column selection buttons without checkboxes/radios.

## Tests and audit boundaries

shared/minza_ninza_ping_unfold.hpp provides read-only C++20 unforge() and untwirl() adapters; tests/minza_ninza_ping_unfold_test.cpp tests 54-position bijection, matches, a changed mirrored byte, a changed braided byte, and no source mutation. Include test results and exact source commit in any review record.

Record protocol version, source frame/time/index, five source pixel coordinates per lane, 54 derived bytes, mirror XOR masks, braid offsets, inverse lane positions, roundtrip flags, selection actions and plate ON/OFF states. The audit cannot independently authenticate the user's source media.

Source → declared sampler → nine five-byte inputs → nine parity/hexabyte lanes → NINZA copy comparison → PING permutation inverse → optional origami display → audit. **Chosen, never assumed.**

Related: [SIGMA_PENTA_HAN_PRESENTATION.md](SIGMA_PENTA_HAN_PRESENTATION.md), [MINZA_PINWHEEL_ATLAS.md](MINZA_PINWHEEL_ATLAS.md), [VISTA_9X9.md](VISTA_9X9.md), and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
