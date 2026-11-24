# Constant formats register 001: ULL, long, Q62, 32-bit, and wide projections

**Status:** Source-backed, derived notes, not a new mathematical authority. **Source snapshot:** `32e93ce56536d7ca5c185c0483277a350c0b3dad` (before this documentation change), project 0BSD. `RELEASE=HOLD`.

This register answers the requested **three separate representations**: exact `unsigned long long` constants, portable `long` segments, and multiply-then-shift acceleration forms. It then records the 32-bit and logical 128/192/224/256/384/448/480/512-bit paths. The repository tree at this snapshot had **497 blobs**, including archived/retired files and non-numeric configuration files. The 21 entries below are the **verified source-backed canonical constants used by TOOLONG / SlidingToolong / QuadDi-NI**, **not** a claim that all numeric tokens in all 497 files and historical branches were certified. Other source constants, including operational and geometric values, appear in the appendix and remain independently owned.

## 1. Owner values and source provenance

| Canonical display name | Exact stored integer | Scale / interpretation | Existing authority | Role |
|---|---:|---|---|---|
| `EULERS_JIG` | `2864498081537971875` | 10^17 | [shared/eulers_jig_canonical.hpp](../shared/eulers_jig_canonical.hpp) | AREA / 64 |
| `EULERS_JIG_TRIANGLE_TOTAL` | `1833278772184302` | 10^12 | [shared/eulers_jig_canonical.hpp](../shared/eulers_jig_canonical.hpp) | display total |
| `EULERS_JIG_TRIANGLE_COUNT` | `64` | raw / integer | [shared/eulers_jig_canonical.hpp](../shared/eulers_jig_canonical.hpp) | count |
| `SCALE_X1E17` | `100000000000000000` | raw / integer | [shared/eulers_jig_canonical.hpp](../shared/eulers_jig_canonical.hpp) | scale |
| `Q62_UNIT` | `4611686018427387904` | raw / integer | [shared/sigma_form_area_q62.hpp](../shared/sigma_form_area_q62.hpp) | 2^62 |
| `Q62_SHIFT` | `62` | raw / integer | [shared/toolong_q62_core.hpp](../shared/toolong_q62_core.hpp) | shift amount |
| `EULER_SOURCE_Q62` | `12535862302449814170` | raw / integer | [shared/toolong_q62_core.hpp](../shared/toolong_q62_core.hpp) | Q62 source |
| `PI_SOURCE_Q62` | `14488038916154245685` | raw / integer | [shared/toolong_q62_core.hpp](../shared/toolong_q62_core.hpp) | Q62 source |
| `INTENSITY_SOURCE_Q62` | `315868046760674477` | raw / integer | [shared/toolong_q62_core.hpp](../shared/toolong_q62_core.hpp) | Q62 source |
| `EULER_RESULT_X1E17` | `271828182845904523` | 10^17 | [shared/toolong_q62_core.hpp](../shared/toolong_q62_core.hpp) | approx e |
| `PI_RESULT_X1E17` | `314159265358979323` | 10^17 | [shared/toolong_q62_core.hpp](../shared/toolong_q62_core.hpp) | approx pi |
| `INTENSITY_RESULT_X1E17` | `6849296450333523` | 10^17 | [shared/toolong_q62_core.hpp](../shared/toolong_q62_core.hpp) | model output |
| `Q64_FRACTION_ALL_ONES` | `18446744073709551615` | raw / integer | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | 64-bit mask |
| `MATH_E_Q64_FRACTION` | `13249961062380153451` | raw / integer | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | Q64 fractional part |
| `MATH_PI_Q64_FRACTION` | `2611923443488327891` | raw / integer | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | Q64 fractional part |
| `INTENSITY_Q64_FRACTION` | `1263472187042697908` | raw / integer | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | Q62 lifted by 2 |
| `SCALE_X1E18` | `1000000000000000000` | raw / integer | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | decimal scale |
| `MATH_E_RESULT_X1E18` | `2718281828459045235` | 10^18 | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | approx e |
| `MATH_PI_RESULT_X1E18` | `3141592653589793238` | 10^18 | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | approx pi |
| `INTENSITY_RESULT_X1E18` | `68492964503335230` | 10^18 | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | model output |
| `SIGMA_Q64_XOR_EXPLORATORY` | `9391969674587680780` | raw / integer | [shared/slidingtoolong.hpp](../shared/slidingtoolong.hpp) | exploratory XOR |

**No copying the source authority:** `shared/constant_formats_register_001.hpp` references the original C++ symbols directly, rather than defining another conflicting canonical coefficient. Display names in this document are aliases, not replacements.

The Jig is the displayed 64-triangle area quotient: `1833.278772184302 / 64 = 28.64498081537971875`, represented as `2864498081537971875 / 10^17`. This **is not** the established mathematical constant `e`.

The Q62 outputs are **approximations** to conventional Euler/π and a **specific intensity model**. The intensity assumption used in the original source discussion is `P=1, α=0.15, ω=1`; it is not a universal physical constant. Intensity here: `6849296450333523 / 10^17 = 0.06849296450333523` W/m² under that model.

For Q64, values named `fraction` are fraction fields, not the complete real number. `MATH_E_Q64` carries separate integer part 2; `MATH_PI_Q64` carries separate integer part 3; intensity has integer part 0. The intensity Q64 fraction is the earlier Q62 source word shifted two bits, **not two new measurement bits**.

## 2. Entire audited register in unsigned long long notation

These are portable 64-bit **unsigned** decimal literals. The canonical authority remains the files cited above.

```cpp
// C++20, literal views of the source records. Decimal precision varies by record.
constexpr unsigned long long EULERS_JIG = 2864498081537971875ULL;
constexpr unsigned long long EULERS_JIG_TRIANGLE_TOTAL = 1833278772184302ULL;
constexpr unsigned long long EULERS_JIG_TRIANGLE_COUNT = 64ULL;
constexpr unsigned long long SCALE_X1E17 = 100000000000000000ULL;
constexpr unsigned long long Q62_UNIT = 4611686018427387904ULL;
constexpr unsigned long long Q62_SHIFT = 62ULL;
constexpr unsigned long long EULER_SOURCE_Q62 = 12535862302449814170ULL;
constexpr unsigned long long PI_SOURCE_Q62 = 14488038916154245685ULL;
constexpr unsigned long long INTENSITY_SOURCE_Q62 = 315868046760674477ULL;
constexpr unsigned long long EULER_RESULT_X1E17 = 271828182845904523ULL;
constexpr unsigned long long PI_RESULT_X1E17 = 314159265358979323ULL;
constexpr unsigned long long INTENSITY_RESULT_X1E17 = 6849296450333523ULL;
constexpr unsigned long long Q64_FRACTION_ALL_ONES = 18446744073709551615ULL;
constexpr unsigned long long MATH_E_Q64_FRACTION = 13249961062380153451ULL;
constexpr unsigned long long MATH_PI_Q64_FRACTION = 2611923443488327891ULL;
constexpr unsigned long long INTENSITY_Q64_FRACTION = 1263472187042697908ULL;
constexpr unsigned long long SCALE_X1E18 = 1000000000000000000ULL;
constexpr unsigned long long MATH_E_RESULT_X1E18 = 2718281828459045235ULL;
constexpr unsigned long long MATH_PI_RESULT_X1E18 = 3141592653589793238ULL;
constexpr unsigned long long INTENSITY_RESULT_X1E18 = 68492964503335230ULL;
constexpr unsigned long long SIGMA_Q64_XOR_EXPLORATORY = 9391969674587680780ULL;

// Euler's Jig: 2864498081537971875 / 100000000000000000
// Output: 28.64498081537971875
// Intensity Q62 output: 6849296450333523 / 100000000000000000
// Output: 0.06849296450333523 W/m^2 for the stated model
```

**Signed `long long` is not interchangeable with unsigned long long.** The Euler/π Q62 source words, the maximal Q64 fraction, E's Q64 fraction, and the exploratory Sigma XOR exceed signed 64-bit max `9223372036854775807`, so an `LL` cast would be wrong. The code uses `ULL` throughout.

## 3. Everything above in portable signed long format

On Windows x64, `long` is commonly **32-bit signed** (LLP64), while many Unix 64-bit platforms use **64-bit signed** `long` (LP64). A value that exceeds the range is **not** representable as a single `long`.

To retain every bit on either platform, split each 64-bit source word into **four signed `long` chunks in base `2^16`**, high chunk first. Each chunk is between 0 and 65535 and fits in a 32-bit signed `long`.

```cpp
// Signed long (32-bit safe), NO includes. This is FOUR long values, not one.
constexpr long JIG_LONG16[4] = {10176L, 48596L, 4571L, 54947L};
constexpr unsigned long long restore_jig =
  (static_cast<unsigned long long>(JIG_LONG16[0]) << 48u) |
  (static_cast<unsigned long long>(JIG_LONG16[1]) << 32u) |
  (static_cast<unsigned long long>(JIG_LONG16[2]) << 16u) |
   static_cast<unsigned long long>(JIG_LONG16[3]);
static_assert(restore_jig == 2864498081537971875ULL);
```

| Source alias | Signed `long` chunks `[bits 63:48, 47:32, 31:16, 15:0]` | Fits ONE Windows signed long? | Fits ONE signed 64-bit long? |
|---|---|---|---|
| `EULERS_JIG` | `10176L, 48596L, 4571L, 54947L` | NO | YES |
| `EULERS_JIG_TRIANGLE_TOTAL` | `6L, 33627L, 31229L, 34030L` | NO | YES |
| `EULERS_JIG_TRIANGLE_COUNT` | `0L, 0L, 0L, 64L` | YES | YES |
| `SCALE_X1E17` | `355L, 17784L, 23946L, 0L` | NO | YES |
| `Q62_UNIT` | `16384L, 0L, 0L, 0L` | NO | YES |
| `Q62_SHIFT` | `0L, 0L, 0L, 62L` | YES | YES |
| `EULER_SOURCE_Q62` | `44536L, 21592L, 41659L, 19098L` | NO | NO |
| `PI_SOURCE_Q62` | `51471L, 55970L, 8552L, 49717L` | NO | NO |
| `INTENSITY_SOURCE_Q62` | `1122L, 12368L, 41745L, 1197L` | NO | YES |
| `EULER_RESULT_X1E17` | `965L, 47690L, 50808L, 22155L` | NO | YES |
| `PI_RESULT_X1E17` | `1116L, 7727L, 63134L, 41211L` | NO | YES |
| `INTENSITY_RESULT_X1E17` | `24L, 21862L, 6626L, 11091L` | NO | YES |
| `Q64_FRACTION_ALL_ONES` | `65535L, 65535L, 65535L, 65535L` | NO | NO |
| `MATH_E_Q64_FRACTION` | `47073L, 20834L, 35565L, 10859L` | NO | NO |
| `MATH_PI_Q64_FRACTION` | `9279L, 27272L, 34211L, 2259L` | NO | YES |
| `INTENSITY_Q64_FRACTION` | `4488L, 49474L, 35908L, 4788L` | NO | YES |
| `SCALE_X1E18` | `3552L, 46771L, 42852L, 0L` | NO | YES |
| `MATH_E_RESULT_X1E18` | `9657L, 18155L, 49331L, 24947L` | NO | YES |
| `MATH_PI_RESULT_X1E18` | `11161L, 11743L, 41522L, 18902L` | NO | YES |
| `INTENSITY_RESULT_X1E18` | `243L, 22013L, 725L, 45374L` | NO | YES |
| `SIGMA_Q64_XOR_EXPLORATORY` | `33366L, 64168L, 33546L, 12300L` | NO | NO |

**One-`long` literals on both Windows and LP64:** `TRIANGLE_COUNT = 64L`, `Q62_SHIFT = 62L`. Also valid as an independently sourced operational constant: `TOOLONG_BASE = 1000000000L` from [shared/toolong.hpp](../shared/toolong.hpp). On Windows, no positive signed long above `2147483647` fits exactly. The four-chunk register above avoids any silent cast.

## 4. 128-bit-to-64-bit Q62 fixed-point multiply-and-shift

This exact formula is the established reference. It is an **unsigned fixed-point scaling operation**, not evidence of a universally faster runtime or a physical accelerator.

```cpp
// ORIGINAL INTENSITY MODEL: P=1, alpha=0.15, omega=1.
// Decimal scale 10^17; Q62 shift 62.
// OPTIONAL compiler extension ONLY, e.g. GCC/Clang supporting __uint128_t.
// NOT standard C++20 and NOT the MSVC production path.
#if defined(__SIZEOF_INT128__)
static constexpr unsigned long long I = (unsigned long long)(
  ((__uint128_t)315868046760674477ULL *
                   100000000000000000ULL) >> 62
);
static_assert(I == 6849296450333523ULL);
#endif
```

For **strict ISO C++20, including MSVC**, use the repository's already proven partial-product engine:

```cpp
// Include shared/toolong_q62_core.hpp before this snippet.
// The original header uses only 32-bit limbs and 64-bit intermediates.
static constexpr unsigned long long I_PORTABLE =
    swe::toolong_q62::multiply_shift62(
        swe::toolong_q62::intensity.source_word);
static_assert(I_PORTABLE == 6849296450333523ULL);
static_assert(swe::toolong_q62::multiply_shift62(
  swe::toolong_q62::euler.source_word) == 271828182845904523ULL);
static_assert(swe::toolong_q62::multiply_shift62(
  swe::toolong_q62::pi.source_word) == 314159265358979323ULL);
```

**Independent mathematical checks:**

`floor(source × 10^17 / 2^62)`; all computations below use positive integer operands and exact 128-bit intermediate arithmetic.

| Source | Q62 source word | Integer quotient after `>> 62` | Exact remainder | Exact quotient verified |
|---|---:|---:|---:|---|
| Euler | 12535862302449814170 | 271828182845904523 | 2403300696930910208 | `TRUE` |
| Pi | 14488038916154245685 | 314159265358979323 | 3925840205563691008 | `TRUE` |
| I | 315868046760674477 | 6849296450333523 | 1790435901504094208 | `TRUE` |

This operation rounds **down** on nonnegative inputs by discarding the remainder. It is not automatically an exact real-valued identity and does not mean the values are mathematical perfect numbers.

## 5. 32-bit representations and rejection

The 64-bit integer is exactly representable as **two 32-bit words**, not a single 32-bit scalar when its high word is nonzero.

```cpp
// Source: intensity result 6849296450333523 = 0x0018556619e22b53.
// Both are EXACT 32-bit words. One 32-bit scalar cannot store their combination.
constexpr unsigned int I_LO32 = 0x19e22b53u;
constexpr unsigned int I_HI32 = 0x00185566u;
static_assert(sizeof(unsigned int) == 4);
constexpr unsigned long long I_RESTORED =
  (static_cast<unsigned long long>(I_HI32) << 32u) | I_LO32;
static_assert(I_RESTORED == 6849296450333523ULL);
static_assert(I_HI32 != 0u); // GLINTING 64->32 MUST reject data loss.
```

Every audited source can be represented losslessly with this high/low pairing:

| Source | Upper 32 bits | Lower 32 bits | Fits a single 32-bit unsigned scalar? |
|---|---|---|---|
| `EULERS_JIG` | `0x27c0bdd4u` | `0x11dbd6a3u` | NO |
| `EULERS_JIG_TRIANGLE_TOTAL` | `0x0006835bu` | `0x79fd84eeu` | NO |
| `EULERS_JIG_TRIANGLE_COUNT` | `0x00000000u` | `0x00000040u` | YES |
| `SCALE_X1E17` | `0x01634578u` | `0x5d8a0000u` | NO |
| `Q62_UNIT` | `0x40000000u` | `0x00000000u` | NO |
| `Q62_SHIFT` | `0x00000000u` | `0x0000003eu` | YES |
| `EULER_SOURCE_Q62` | `0xadf85458u` | `0xa2bb4a9au` | NO |
| `PI_SOURCE_Q62` | `0xc90fdaa2u` | `0x2168c235u` | NO |
| `INTENSITY_SOURCE_Q62` | `0x04623050u` | `0xa31104adu` | NO |
| `EULER_RESULT_X1E17` | `0x03c5ba4au` | `0xc678568bu` | NO |
| `PI_RESULT_X1E17` | `0x045c1e2fu` | `0xf69ea0fbu` | NO |
| `INTENSITY_RESULT_X1E17` | `0x00185566u` | `0x19e22b53u` | NO |
| `Q64_FRACTION_ALL_ONES` | `0xffffffffu` | `0xffffffffu` | NO |
| `MATH_E_Q64_FRACTION` | `0xb7e15162u` | `0x8aed2a6bu` | NO |
| `MATH_PI_Q64_FRACTION` | `0x243f6a88u` | `0x85a308d3u` | NO |
| `INTENSITY_Q64_FRACTION` | `0x1188c142u` | `0x8c4412b4u` | NO |
| `SCALE_X1E18` | `0x0de0b6b3u` | `0xa7640000u` | NO |
| `MATH_E_RESULT_X1E18` | `0x25b946ebu` | `0xc0b36173u` | NO |
| `MATH_PI_RESULT_X1E18` | `0x2b992ddfu` | `0xa23249d6u` | NO |
| `INTENSITY_RESULT_X1E18` | `0x00f355fdu` | `0x02d5b13eu` | NO |
| `SIGMA_Q64_XOR_EXPLORATORY` | `0x8256faa8u` | `0x830a300cu` | NO |

There is no rounding-down-into-32 that preserves the original integer once its upper word is nonzero. **Discarding its upper bits is truncation, not an equivalent accelerator.**

## 6. 128, 192, 224, 256, 384, 448, 480 and 512-bit cases

Use `shared/slipslidingtoolong.hpp` for **logical multi-limb values**. That header has 16 checked 32-bit storage limbs, one `GUARDMEMORYGUARD` compound check and explicit `GLINTING` / `GLIZZING` moves. These sizes are **not assumed to be native hardware scalar types**.

| Width move | Arithmetic width difference | Behavior for zero-extended Q62 integer RESULTS |
|---|---|---|
| `128 → 64` | `128−64` | EXACT for zero-extended Q62 outputs |
| `64 → 32` | `64−32` | REJECT_LOSS for all three Q62 outputs |
| `512 → 384` | `512−128` | EXACT for zero-extended Q62 outputs |
| `512 → 448` | `512−64` | EXACT for zero-extended Q62 outputs |
| `512 → 480` | `512−32` | EXACT for zero-extended Q62 outputs |
| `512 → 256` | `512−256` | EXACT for zero-extended Q62 outputs |
| `256 → 128` | `256−128` | EXACT for zero-extended Q62 outputs |
| `256 → 192` | `256−64` | EXACT for zero-extended Q62 outputs |
| `256 → 224` | `256−32` | EXACT for zero-extended Q62 outputs |

```cpp
// Include shared/slipslidingtoolong.hpp and the canonical Q62 header.
// Validated by the committed C++20 test, not by assuming physical registers.
using namespace swe::slipslidingtoolong;
constexpr unsigned long long source_i =
  swe::toolong_q62::intensity.expected_integer;
constexpr auto owned_i = []{
  auto r = make_register(64u, 99u);
  r.limb[0] = static_cast<unsigned int>(source_i & 0xffffffffULL);
  r.limb[1] = static_cast<unsigned int>(source_i >> 32u);
  r.digest = checksum(r);
  return r;
}();
static_assert(GUARDMEMORYGUARD(owned_i));
static_assert(GLIZZING(owned_i,512u).status == Result::PASS);
static_assert(GLINTING(GLIZZING(owned_i,512u).value,384u).status ==
              Result::PASS); // 512 - 128 = 384
static_assert(GLINTING(GLIZZING(owned_i,256u).value,224u).status ==
              Result::PASS); // 256 - 32 = 224
static_assert(GLINTING(owned_i,32u).status == Result::REJECT_LOSS);
```

The wide cases mean **declared bit widths and loss-checked projection**, not automatic acceleration, memory spying, a license to access adjacent physical registers, or thread-crash durability. `GUARDMEMORYGUARD` checks its own authorized storage and retains `RELEASE=HOLD`.

## 7. Other identified constants, explicitly separate domains

The following are real repository declarations but **not interchangeable with Q62 arithmetic sources**:

- `1000000000u`: TOOLONG decimal limb base, [shared/toolong.hpp](../shared/toolong.hpp), also used by GammaGamma's decimal-base handling.
- `2048u`: resource cap on exact factorial argument, [shared/gammagamma.hpp](../shared/gammagamma.hpp). **Not a mathematical precision cap**.
- `16`: π output-format display digit count derived from 18 source digits minus two reserved positions, [shared/pi_precision.hpp](../shared/pi_precision.hpp). **Not exact π precision or binary64 guaranteed round-trip precision**.
- `60`, `15`, `4`, `8`, `3`: width, strip, panel count, triangle count, fold count for the **exploratory** three-geometric-folds model, [shared/three_geometric_folds.hpp](../shared/three_geometric_folds.hpp).
- `32,64,128,192,224,256,384,448,480,512`: logical register widths, [shared/slipslidingtoolong.hpp](../shared/slipslidingtoolong.hpp), with eight slots in `MXRING`.
- `2864498081537971875ULL` remains an **AREA/64 Jig**, not a replacement for mathematical Euler's number or an independent claim of proof.
- `98696044010893585656621344341538329` is the **BiPi-square decimal text** in [shared/gammagamma.hpp](../shared/gammagamma.hpp); it is **too large for unsigned long long**, so it requires the existing TOOLONG arbitrary-precision engine or limbs. This deliberately does **not** appear in the ULL table.
- `0.15` is a fractional user-supplied model parameter; **not an integer `long` or `long long` without an explicit rational scale**, e.g. `15/100`. No truncation or unapproved scale is assumed.

Historical or retired sources under `Archive/Retired` remain history, not CURRENT authority. This note is a focused **canonical-stack** audit. Discovering additional prior values requires explicit provenance and review; guessing a missing constant or equating similar labels would violate source custody.

**Status:** recorded mathematical equalities and static-checkable conversions are proven only to their named input protocol. Cross-language agreement or run time acceleration is a separate claim requiring the actual CI witness. No automatic merge or release.


## 8. Historical precision policy: nearest ties-to-even (SUPERSEDED)

> **RETIRED FOR NEW CONVERSIONS:** The following section is retained as a traceable historical policy witness from commit `d0fc87b9c2193a8d69725a791b033816b16dc1fd`. Its references to "new" conversions and the former header describe that historical snapshot, not the current rule. **Section 9 is now authoritative for new conversions.** Source constants and historical outputs remain unchanged.


**Status: explicitly requested forward policy, 2026-11-23.** Preserve every original canonical source record and its historical derived output as evidence. For **new** Q62-to-decimal conversions, do not discard significant digits/bits by using the lower integer neighbor as a final result. Use **nearest integer, ties to even**, and retain the exact 128-bit numerator and Q62 remainder as a witness. Exact-to-narrower register projection still **rejects loss**, not rounds, unless the caller explicitly requests a numerical conversion with a named scale and rounding mode.

New implementation: [shared/precision_rounding_policy_001.hpp](../shared/precision_rounding_policy_001.hpp), and compile-time C++20 test in [quaddi-ni/SlipSlidingToolongTest.cpp](../quaddi-ni/SlipSlidingToolongTest.cpp). Neither file replaces the historical Q62 arithmetic function, original output fields, or the 21-record registry. No includes, STL, or compiler-specific 128-bit integer are required by this new header. Multiplication retains exact high/low 64-bit product words. Any result beyond unsigned long long returns valid=false. The remainder is exposed rather than hidden.

| Existing source (Q62 word) | Decimal scale | New nearest/ties-even result | Exact Q62 remainder | Historical lower-integer result |
|---|---:|---:|---:|---:|
| Euler 12535862302449814170 | 10^17 | 271828182845904524 | 2403300696930910208 | 271828182845904523 |
| π 14488038916154245685 | 10^17 | 314159265358979324 | 3925840205563691008 | 314159265358979323 |
| Intensity 315868046760674477 | 10^17 | 6849296450333523 | 1790435901504094208 | 6849296450333523 |
| Euler | 10^18 | 2718281828459045235 | 974576877172162560 | 2718281828459045235 |
| π | 10^18 | 3141592653589793239 | 2364913908217806848 | 3141592653589793238 |
| Intensity | 10^18 | 68492964503335234 | 4069300959758778368 | 68492964503335233 |

The **historical** intensity x10^18 value 68492964503335230 resulted from multiplying a previously reduced x10^17 value by ten. The **new** direct nearest/ties-even x10^18 value is 68492964503335234, four integer units higher. Do not label either a source mutation: they follow different derivations. Likewise the π source Q62 word ending 685 and the mathematically lower neighbor ending 684 differ by one binary Q62 unit; preserve the source and name the comparison.

**Scope of no truncation:** source data stays exact as (source_word × decimal_scale) / 2^62, with the numerator and remainder retained. A fixed-width user-facing numeric rendering is explicitly rounded. For irrational π and e, **no finite 64-bit representation is itself exact**. Use symbolic/exact multi-limb formulations if exactness is required. RELEASE=HOLD, draft PR; no automatic endorsement and no retroactive claims about earlier CI runs.

## 9. ROUND-FIRST / nearest ties-to-ODD (current exploratory conversion policy)

**Status:** Updated at explicit user request, 2026-11-23. Only derived Q62-to-decimal conversions change policy; source words, original `multiply_shift62` historical records, and the earlier ties-even witness remain in Git history. No changes to DOGMA or identity rules are implied by the informal cultural naming used to explain the numerical method. `RELEASE=HOLD`.

### Algorithm and subtractive head/tail witness

For a nonnegative Q62 source word `w` and requested nonzero decimal scale `S`, the **exact** numerator is `N = w*S` (two checked unsigned 64-bit product limbs), and `D = 2^62`. FIRST find the prospective display boundary, WITHOUT committing a rounded/truncated output:

1. Isolate integer **head** `H` and remainder **tail** `T` from `N / D`.
2. Recover the tail by the inverse/subtractive difference `T = N - H*D`, using checked 128-bit limb subtraction. Verify `N = H*D + T` and `0 <= T < D` before any rounding. This is an exact decomposition, not loss of the tail.
3. Compare `T` with `D/2`. If below, output `H`. If above, output `H+1`. If **exactly tied**, output the **odd** integer of those two neighbors: `H` if odd, otherwise `H+1`.
4. Retain the signed residual `R*D - N`, encoded as sign and magnitude, alongside source numerator and exact tail. Return invalid on overflow. Never multiply the tail by three, saturate, wrap, silently narrow, or round an already-rounded display value.

The meaning of `round-first` here is **round only after examining the complete exact head/tail witness**, before final fixed-width rendering. No numerical rounding is performed to create `H` or `T`; they remain a lossless pair. In mathematical form:

```text
N = source_word * decimal_scale              // exact 128-bit
N = H * (2^62) + T                          // exact decomposition
T = N - H * (2^62)                          // inverse/subtractive tail witness
R = H                         when 2*T < 2^62
R = H+1                       when 2*T > 2^62
R = odd(H, H+1)               when 2*T = 2^62
signed_residual = R * (2^62) - N             // exact audit witness
```

**Ties to odd:** `0.5→1`, `1.5→1`, `2.5→3`, `3.5→3`. At exact integers `1.0→1`, and at nonties `1.25→1`, `1.75→2`. Ties are the only inputs for which changing ties-even to ties-odd can change a result.

| Q62 input | Requested scale | ROUND-FIRST ties-odd | Exact remainder | Halfway tie? |
|---|---:|---:|---:|---|
| Euler `12535862302449814170` | 10^17 | `271828182845904524` | `2403300696930910208` | NO |
| π `14488038916154245685` | 10^17 | `314159265358979324` | `3925840205563691008` | NO |
| Intensity `315868046760674477` | 10^17 | `6849296450333523` | `1790435901504094208` | NO |
| Euler | 10^18 | `2718281828459045235` | `974576877172162560` | NO |
| π | 10^18 | `3141592653589793239` | `2364913908217806848` | NO |
| Intensity | 10^18 | `68492964503335234` | `4069300959758778368` | NO |

These six outputs do **not change** between ties-even and ties-odd, because none is halfway. The new policy is therefore independently qualified using **four intentionally exact halfway fixtures**. The prior Q62 π stored word ending `685` versus an independently derived lower neighbor ending `684` remains a separate one-ULP source/derivation witness, not permission to alter the source.

**Implementation:** [shared/precision_rounding_policy_001.hpp](../shared/precision_rounding_policy_001.hpp) exposes `round_first_nearest_odd` with the exact product, head, remainder, midpoint flag, signed residual flag/magnitude and overflow guard. [quaddi-ni/SlipSlidingToolongTest.cpp](../quaddi-ni/SlipSlidingToolongTest.cpp) includes compile-time tie/parity/nearby/exact/overflow checks. Historical ties-even source is retrievable from prior Git commit `d0fc87b9c2193a8d69725a791b033816b16dc1fd`.

**Precision boundary:** No finite 64-bit rendering makes irrational π or e exact. A precise rational remainder and one final rounding can avoid premature information loss; the number of math operations does NOT by itself guarantee increased accuracy. Per-operation rounding may accumulate error. This qualification is mathematical and compile-time, not a speed or real-hardware claim. **Executed CI evidence:** [QUADDI NI run #45315049763](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45315049763) finished **SUCCESS** on commit `e455682367016f73b62063778b22fe6ad5df174c` and its MSVC /O2 stage printed `PRECISION_ROUNDING|EXACT_128_PRODUCT=PASS|HEAD_TAIL_RECONSTRUCT=PASS|NEAREST_TIES_ODD=PASS|ROUND_FIRST=PASS|RELEASE=HOLD`. This verifies C++20 compile-time tie tests and exact witness assertions on that specific commit; Oracle model runs in the same workflow qualify guarded widths, not a separate ties-odd parity implementation. [DIAERY regression run #45315049933](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45315049933) also finished **SUCCESS** on that commit. New documentation-only commits do not retroactively modify those run results. `RELEASE=HOLD`.
