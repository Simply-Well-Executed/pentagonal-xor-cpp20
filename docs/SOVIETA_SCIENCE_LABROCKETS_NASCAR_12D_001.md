<!-- SPDX-License-Identifier: 0BSD -->
# Sovieta Science Labrockets: NASCAR Precision Head-to-Head, 1D to 12D

**Record:** SOVIETA-LABROCKETS-NASCAR-12D-001. **Status:** EXPLORATORY engineering test, not scientific proof of speed. **Canonical 21 constants and original Q62 sources:** UNCHANGED. **Release:** HOLD. **Orientation:** LTR, Rocket LEFT, Car RIGHT. **Owner of requested layout:** user; no identity, governance, or scientific authority inferred from a fun project title.

## Starting grid, LTR and never reversed

| 🚀 LEFT: Rocket lane | 🏎️ RIGHT: NASCAR car lane |
|---|---|
| `fast9`: one exact 9-digit block `123456789` per lap | `slow9`: nine exact one-digit appends `1,2,3,4,5,6,7,8,9` per lap |
| Uses owned 512-bit logical register and 32-bit partial products | Uses the same owned register, limb arithmetic, and guard |
| Performs one base-10^9 append per dimension | Performs nine base-10 appends per dimension |
| **Candidate** operation-count advantage, speed unmeasured | Independent numerical cross-check, speed unmeasured |
| Must match car bit-for-bit, without silently losing bits | Must match rocket bit-for-bit, without silently losing bits |

**Track direction:** 🚀 ROCKET LEFT `==>` 🏎️ NASCAR RIGHT, reading LTR. This is a presentation order, not memory topology, pointer ownership, or relative timing. An exact numerical tie is a VALID equality. Who wins in elapsed time must be measured on pinned hardware/compiler/workload; calling the left lane “rocket” does not establish a speedup.

## Complete 1D–12D race grid, no prefixes and no ellipses

The first fraction is exactly `2345678`. Append the 9-character sequence `123456789` to the **tail** for each higher dimension. These are literal decimal test fixtures, NOT approximations to pi, e, or measured physical dimensions. **No zero-padding or prepending to any value.**

```text
1D  1.2345678
2D  1.2345678123456789
3D  1.2345678123456789123456789
4D  1.2345678123456789123456789123456789
5D  1.2345678123456789123456789123456789123456789
6D  1.2345678123456789123456789123456789123456789123456789
7D  1.2345678123456789123456789123456789123456789123456789123456789
8D  1.2345678123456789123456789123456789123456789123456789123456789123456789
9D  1.2345678123456789123456789123456789123456789123456789123456789123456789123456789
10D  1.2345678123456789123456789123456789123456789123456789123456789123456789123456789123456789
11D  1.2345678123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789
12D  1.2345678123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789
```

## Register width qualification

For a decimal value `1.<fraction>` with `F` fractional digits, carry the **exact** integer numerator `1<all F digits>` alongside an exact decimal scale `10^F`. Both the integer and its scale MUST be retained. The table's bits refer to the integer numerator only, not to an independent 10^F bitset or a combined numerator/denominator buffer. “Dimension” here means a series index (1D through 12D).

| Stage | Fractional digits | Numerator bits | Smallest approved logical register | Construction |
|---|---:|---:|---:|---|
| 1D | 7 | 24 | 32 | seed |
| 2D | 16 | 54 | 64 | 1 × 9-digit append |
| 3D | 25 | 84 | 128 | 2 × 9-digit append |
| 4D | 34 | 114 | 128 | 3 × 9-digit append |
| 5D | 43 | 144 | 192 | 4 × 9-digit append |
| 6D | 52 | 174 | 192 | 5 × 9-digit append |
| 7D | 61 | 203 | 224 | 6 × 9-digit append |
| 8D | 70 | 233 | 256 | 7 × 9-digit append |
| 9D | 79 | 263 | 384 | 8 × 9-digit append |
| 10D | 88 | 293 | 384 | 9 × 9-digit append |
| 11D | 97 | 323 | 384 | 10 × 9-digit append |
| 12D | 106 | 353 | 384 | 11 × 9-digit append |

At 12D, the exact numerator is **353 bits** and the fractional decimal scale is `10^106`; numerator fits an approved **384-bit** owned register. It is NOT representable losslessly as one 64-bit integer. The implementation retains the value in 512-bit owned software storage during the comparison and checks exact `GLINTING(512→384)` followed by `GLIZZING(384→512)` round-trip.

## Inspection, head-to-head, and precision rules

- **Preflight:** strict source length, stage indices 1 through 12, allowed width, head/tail provenance, no premature rounding.
- **Rocket lane LEFT:** append one 9-digit block with exact carry propagation across all 16 limbs.
- **Car lane RIGHT:** independently append 9 decimal digits, one at a time, to the same preceding stage.
- **GLINT / PEEK / VALIDATE:** compare values bit-for-bit; calculate the actual bit length; reject all insufficient register widths (`REJECT_LOSS`), and validate every permitted wider projection by a round trip.
- **Ties-to-ODD:** split exact `N = H × 10^9 + T`. Recover `T=N-H×10^9`, reconstruct N, and round once. At exactly half choose odd H/H+1. For EVERY stage, test seven tails: `0`, `123456789`, `499999999`, `500000000`, `500000001`, `987654321`, `999999999`.
- **Time and speed:** `LEVEL_9` is **digits per append**, not a benchmarked 9× speed multiplier. The number of operations does not establish a femtosecond timing gain. A speed claim needs repeated timings with environmental and compiler controls plus measured evidence.
- **No numerical mutation:** the original literal sequence is the test input; logs, tables and renderers are derived records. Failed checks are evidence, never automatically new policy. `RELEASE=HOLD`.

### Pre-shift source text specimen: exact whitespace

The requested sequence is preserved for text/renderer validation:

```text
wait()
 (wait)
pause()
```

This *literal text* is not valid as an automatically meaningful C++20 synchronization primitive: `wait()` and `pause()` need explicit declarations, `(wait)` is a parenthesized expression rather than a second call, and whitespace alone does not create an execution barrier. Making a thread wait before `>>` could slow rather than accelerate execution. We therefore **do not insert a real sleep or pointer-data race**. `PRE_SHIFT_WHITESPACE` preserves the exact bytes, with a compile-time whitespace witness, separate from an actual safe bit-shift.

## Implementation and provenance

- [12D C++20 test header](../shared/fudgelessslippingtoolong_12d.hpp): `fast9` (rocket LEFT), `slow9` (car RIGHT), exact 512-bit owned limb register, checked `split9` head/tail reconstruction, seven tails per stage, logical-width checks, and `SERIES_12_PASS`.
- [Native compile-time qualification](../quaddi-ni/SlipSlidingToolongTest.cpp): static assertions, plus `FUDGELESSSLIPPINGTOOLONG|1D_TO_12D=PASS` only after actual compiled validation.
- [Precision policy](../shared/precision_rounding_policy_001.hpp): existing Q62 scalar round-first ties-to-odd with complete 128-bit product and signed residual.
- [Existing width and guard core](../shared/slipslidingtoolong.hpp): `GUARDMEMORYGUARD`, `GLINTING`, `GLIZZING`, `MXRING`, no claim of native 512-bit hardware scalar arithmetic.

**Qualification register (executed):** [QUADDI NI workflow run #45315833046](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45315833046) completed **SUCCESS** against tested source commit `bb6ad1136872a64f61aca7ef6e204f6ddbe6d786`, with native MSVC /O2 compile-time assertion and runtime gate output:

```text
FUDGELESSSLIPPINGTOOLONG|1D_TO_12D=PASS|LEVEL_9=DECIMAL_CHUNK|FAST9_SLOW9=EXACT_MATCH|BITS_12D=353|GLINTING_384=PASS|MIDPOINT_ODD=PASS|NO_SPEED_CLAIM|RELEASE=HOLD
```

[DIAERY XHTML Strict run #45315833073](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45315833073) also completed **SUCCESS** against that code revision. The report/documentation commit comes afterward and is not part of those compiled artifacts. The result establishes exact numerical and guard checks, NOT a speed benchmark, physical dimensionality, thread or memory barrier, or real-time ballistics/economics capability. `RELEASE=HOLD`.

**Finish line:** two independent lanes, one exact result, twelve visible stages, all decimal tails retained, and no premature victory lap. 🚀 | 🏎️
