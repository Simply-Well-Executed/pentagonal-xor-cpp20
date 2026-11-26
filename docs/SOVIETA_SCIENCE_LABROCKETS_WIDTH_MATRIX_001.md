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

**Executed historical 10-width qualification: PASS.** [Native MSVC /O2 and guarded-width run #45317871859](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45317871859) completed **SUCCESS** on code commit `004d7623ca70831049935ce246d87a4bd96c7d71` and emitted this exact `SOVIETA_WIDTHS` signature. [DIAERY XHTML Strict regression run #45317871778](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45317871778) also completed **SUCCESS** for that code. These runs support the previous 10-width revision only; new micro-width code requires its own run. The earlier 21 canonical source constants remain unchanged. `RELEASE=HOLD`.


## Micro-width addendum 002: 16, 8, 4, 2, 1, nobit, nop

**Status: EXPLORATORY ADDITIVE EXTENSION.** The ten canonical source widths above remain unchanged and source-authoritative. Six additional **numeric bit capacities**, sorted ascending, are `nobit=0`, `1`, `2`, `4`, `8`, `16`, followed by the original ten widths. `nop` is a **separate checked identity operation**, not a seventeenth numeric width. No code silently aliases `nop` to zero width.

| Extension | Numerical meaning | Valid data and boundary |
|---|---|---|
| `nobit` | Exactly zero data bits | Only integer 0 is representable; any set bit MUST yield `REJECT_LOSS` |
| `1bit` | 1 data bit | 0–1 accepted; 2 rejected |
| `2bit` | 2 data bits | 0–3 accepted; 4 rejected |
| `4bit` | 4 data bits | 0–15 accepted; 16 rejected |
| `8bit` | 8 data bits | 0–255 accepted; 256 rejected |
| `16bit` | 16 data bits | 0–65535 accepted; 65536 rejected |
| `nop` | No-operation (guarded identity) | Original value **and** semantic width unchanged; corrupted source or width seal rejected |

This experiment uses the [separate C++20 micro-width registry](../shared/fudgelessslippingtoolong_micro_widths_001.hpp) rather than appending invalid small widths to the historical [32-bit-limb core](../shared/slipslidingtoolong.hpp). In particular, the original `bits / 32` loop logic cannot safely be reused as-is for 1, 2, 4, 8 or 16 bits. The extension checks **the actual bit boundary inside each 32-bit or 64-bit word**, and rejects nonzero bits outside the declared capacity. Nothing is *truncated* to make a value fit.

### Expanded numeric matrix, all 100 stages

There are **16 numeric sizes** total: `0, 1, 2, 4, 8, 16, 32, 64, 128, 192, 224, 256, 384, 448, 480, 512`. `nop` is a control operation, not a register bit capacity.

| Matrix | Expected checks | Outcome meaning |
|---|---:|---|
| 100 stages × 16 numeric widths | **1,600** | 89 exact fits + **1,511** `REJECT_LOSS` |
| 16 source × 16 destination width routes | **256** | 136 lossless equal/wider routes + **120** rejected narrowing routes |
| `nop` on each stage’s unmodified 3,072-bit wide value | **100** | All 100 identity checks must preserve source, with guards intact |
| `nop` on the 16 zero / max-bit route controls | **16** | Identity preserves width, tag, data, and seal |
| Micro-boundary probes | **11** | Zero-capacity versus set-bit + each 1/2/4/8/16 max and max+1 |
| Tamper probes | **3** | Corrupt width seal, value digest, or source guard; NOP must reject |

The 1D seed already requires **24 significant bits**. Therefore none of the six new capacities can hold the nonzero value at ANY of the 100 stages. Their **600 additional stage checks must all reject**, whereas the ten established sizes retain exactly 89 accepted and 911 rejected. The original 512→128 and 512→512 baselines are repeated by the existing tests.

### Two arithmetic lanes, no numerical source mutation

**🚀 LEFT rocket:** one nine-digit block append in owned 32-bit or 64-bit limbs. **🏎️ RIGHT car:** nine one-digit appends; must agree bit-for-bit at every stage. A source value is never conflated with its logical capacity. An independent 64-bit-limb boundary inspector computes exact fit/overflow separately from the 32-bit-limb inspector.

A `nop` must not become `wait()`, a pause instruction, a memory-order barrier, or a license to skip guard validation. **Zero capacity** is also different from the **number zero**: the latter can be represented in any width, including `nobit`.

The full 100-stage decimal series is preserved without re-copying or shortening in the [existing canonical derived 100D ledger](./SOVIETA_SCIENCE_LABROCKETS_BIDI_100D_001.md). The 12 named symbolic operators and two ordered metadata passes remain separate and unmutated.

### Expected new native log

```text
SOVIETA_MICRO_WIDTHS|NOBIT_1_2_4_8_16_32_64_128_192_224_256_384_448_480_512=PASS|NUMERIC_WIDTHS=16|ROUTES=256|ROUTE_PASS=136|ROUTE_REJECT_LOSS=120|STAGE_PROJECTIONS=1600|LOSSLESS=89|REJECT_LOSS=1511|NOP_IDENTITY=100|LIMB32_LIMB64=PASS|100D_ROUNDTRIP=PASS|NO_SPEED_CLAIM|RELEASE=HOLD
```

**Executed micro-width CI evidence: PASS** on code commit `2e331bff8f59634e883e9a4e26992234705e58fe`. [QUADDI NI native C++20 MSVC /O2 + Oracle workflow #45318367577](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45318367577) completed **SUCCESS**, with the exact `SOVIETA_MICRO_WIDTHS` and historical `SOVIETA_WIDTHS` lines above emitted by the guarded-width qualification. The independent [DIAERY XHTML Strict regression #45318367588](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45318367588) completed **SUCCESS** on the same tested code commit.

**Evidence boundaries:** the 16 numeric widths, 256 route checks, 1,600 stage-width checks, NOBIT zero-capacity controls, explicit NOP identity checks, 32-/64-bit limb parity, and 100D reverse walk passed the specified C++20 test. No full-program performance benchmark, native sub-byte register hardware, zero-byte bookkeeping, or real-world time travel claim is implied. The documentation commit updating this status happens after the tested code commit. `RELEASE=HOLD`, PR remains draft, no auto-merge.
