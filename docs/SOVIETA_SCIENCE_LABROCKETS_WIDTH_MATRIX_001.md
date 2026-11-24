<!-- SPDX-License-Identifier: 0BSD -->
# Sovieta Science Labrockets: 32/64-bit limbs × all ten core logical register widths

**Experiment:** SOVIETA-WIDTH-MATRIX-001. **Status:** EXPLORATORY, new code qualification pending. **Scope:** 1D → 100D → 1D, 10 registered widths, 32-bit and 64-bit storage/compute parity, loss rejection, and a 512→128 route with 512→512 control. **Release:** HOLD. Preserve historical sources and references; do not convert a failed numerical witness into policy.

## LTR track orientation and separation of concerns

| 🚀 LEFT: rocket | 🏎️ RIGHT: NASCAR |
|---|---|
| One nine-digit append per step, radix `10^9` | Nine individual decimal-digit appends per step, radix `10` |
| Native ISO C++20 32-bit limb implementation | Independent 64-bit-limb implementation with two 32-bit partial products per word |
| Also verified in 64-bit-limb arithmetic | Also verified in 32-bit-limb arithmetic |
| Source bit pattern must be identical | Source bit pattern must be identical |

**Logical width ≠ limb width.** A 512-bit logical register may consist of sixteen 32-bit limbs or eight 64-bit limbs. A 128-bit register may use four 32-bit or two 64-bit limbs. At non-64-aligned widths (32, 224, 480), the 64-bit lane's final word has **32 unused upper bits**, which MUST remain zero. Native MSVC/C++20 has 64-bit integral arithmetic but does not promise compiler-independent native 128-bit multiplication, so the 64-bit code explicitly performs two 32-bit partial-product steps and propagates the carry. No undefined shifts by a type's width.

## Every canonical logical width, source-based fit limits

The widths below come directly from [the repository's own register registry](../shared/slipslidingtoolong.hpp), **not** from an invented uniform power-of-two sequence. A test value `1.2345678` extended by `123456789` on every subsequent stage is represented as an **exact integer numerator** plus decimal scale `10^(7+9*(D-1))`.

| Logical bits | 32-bit limbs | 64-bit limbs (ceil) | Unused bits in last 64-bit limb | Stages fitting exactly | Accepted | Loss rejected |
|---:|---:|---:|---:|---|---:|---:|
| 32 | 1 | 1 | 32 | 1D–1D | 1 | 99 |
| 64 | 2 | 1 | 0 | 1D–2D | 2 | 98 |
| 128 | 4 | 2 | 0 | 1D–4D | 4 | 96 |
| 192 | 6 | 3 | 0 | 1D–6D | 6 | 94 |
| 224 | 7 | 4 | 32 | 1D–7D | 7 | 93 |
| 256 | 8 | 4 | 0 | 1D–8D | 8 | 92 |
| 384 | 12 | 6 | 0 | 1D–13D | 13 | 87 |
| 448 | 14 | 7 | 0 | 1D–15D | 15 | 85 |
| 480 | 15 | 8 | 32 | 1D–16D | 16 | 84 |
| 512 | 16 | 8 | 0 | 1D–17D | 17 | 83 |
| **Total** | | | | | **89** | **911** |

All ten sizes are checked across all 100 stages: **1,000 per-stage width projections**. For every stage and every width, both independently assembled 32-bit and 64-bit wide values MUST either pass with bit-identical reconstructed values or fail with `REJECT_LOSS`; neither side may silently round, wrap or zero away significant digits. The full 100D numerator is **2,984 bits**, requiring **94 × 32-bit** or **47 × 64-bit** limbs at minimum, contained in the dedicated 3,072-bit lab-owned allocation (**96 × 32** vs **48 × 64**). The existing canonical 512-bit storage policy is not enlarged.

## Explicit 512→128 versus 512→512 baselines

This is a **logical projection**, not arithmetic subtraction `512 - 128 = 384`. A 512-bit value that actually contains only 128 significant bits (or fewer) can be projected to 128 bits exactly; one with significant high bits cannot.

| Tested stage | Significant numerator bits | `GLINTING(512→128)` | `project(512→512)` |
|---|---:|---|---|
| 1D | 24 | PASS | PASS |
| 4D | 114 | PASS | PASS |
| 5D | 144 | **REJECT_LOSS** | PASS |
| 17D | 502 | **REJECT_LOSS** | PASS |
| 18D | 532 | Not representable in source 512: reject before projection | **REJECT_LOSS** at 3,072→512 entry |

The same exact witness runs for **all 100 input stages**, with all ten widths. A 512-bit capacity is a baseline, not a promise that a 2,984-bit 100D numerator fits.

## Original route matrix (10 × 10)

The 1D seed fits in every listed logical width. Every source→destination pair in the original width registry is tested with `project`, then restored to its source width and compared, preserving the tag and guard. Expected counts: **100 routes = 45 GLINTING + 45 GLIZZING + 10 equal-width**, all exact. Higher-stage rejection is checked independently, rather than falsely reporting a high-bit-loss route as successful.

## 32- versus 64-bit engine qualification

[New native C++20 matrix implementation](../shared/fudgelessslippingtoolong_width_matrix_001.hpp) adds a second **48 × 64-bit** wide buffer, separate source and destination checks, exact two-half partial products for decimal append, guard/checksum validation, and reversible repacking to the older **96 × 32-bit** format. At every stage it compares:

1. 🚀 Rocket 32-bit result against 🏎️ car 32-bit result.
2. 🚀 Rocket 64-bit result against 🏎️ car 64-bit result.
3. Repacked 64-bit result against original 32-bit result, bit-for-bit.
4. For each core width, independent 32-/64-bit narrowing checks, then widened reconstitution where representable.
5. Full descending 100D→1D inverse reconstruction with exact `123456789` tails and the same immutable source seed.

The 64-bit arithmetic is **actually calculated** using 64-bit limbs, but avoids a non-portable `__int128` or unsafe 64×64 multiplication with discarded high bits. It reuses the existing 32-bit-half carry technique. No STOP/SLEEP/PAUSE is injected into code; any `wait()\n (wait)\npause()` specimen remains data-only and cannot establish a speed advantage.

**Speed and memory:** the number of limbs changes from 96 to 48 in the 3,072-bit lab allocation, but the raw numeric payload remains **384 bytes** in both formats (excluding metadata and ABI layout padding). Fewer limb iterations may help performance, but binary correctness alone cannot decide the clock-time winner. This experiment contains **no timing benchmark** and asserts no 9× accelerator.

## Canonical expanded decimal source and operator provenance

The **complete 1D through 100D decimal sequence**, without shortened tails or prepending, is retained exactly once in the [100D source ledger](./SOVIETA_SCIENCE_LABROCKETS_BIDI_100D_001.md) and its PR comment. This new matrix *references* that exact same 100D series, rather than creating a second competing decimal source.

The earlier **BiDi, TriDi, PiDi, PiPi, SigmaSigmaSigma, Sigma, SigmaSigma, GammaGammaGamma, GammaGammaGamma, Gamma, GammGamma, Gamma** sequence stays in its original two-pass metadata audit. Eight operator positions still have unresolved numeric domains, so no new numerical transformations are presumed. This width matrix is a new representation/qualification layer, **not a DOGMA, identity or source-record change**.

## Verification and audit

The new implementation and [native test entry](../quaddi-ni/SlipSlidingToolongTest.cpp) declare a separate log string:

```text
SOVIETA_WIDTHS|32_64_128_192_224_256_384_448_480_512=PASS|PAIR_ROUTES=100|STAGE_PROJECTIONS=1000|LOSSLESS=89|LOSS_REJECTED=911|512_TO_128=PASS_OR_REJECT_LOSS|512_BASELINE=PASS|LIMB32_LIMB64_PARITY=PASS|100D_ROUNDTRIP=PASS|NO_SPEED_CLAIM|RELEASE=HOLD
```

This is an **expected output signature**, not an assertion that the new CI has completed. The initial 100D test has prior successful evidence documented in the linked source ledger, but **the width/limb matrix requires new native compilation before we mark it PASS**. The test never changes the earlier 21 canonical constants or merges the draft PR.
