# Native Euler number: integer-derived, guarded decimal witness

Status: EXPLORATORY / review required. Version: EULER-Q62-MINZA-v2 (v1 integer-series witness retained).
License: 0BSD. No POSIX, Python, Node, standard-library includes, remote fetch, or runtime package dependency in the new Euler header.

## Provenance

This implementation follows the separation in merged PR #17:
- The owner's 18-digit Pi source is a preserved exact integer token, not an independently computed mathematical pi.
- The Pi-derived output convention reserves two positions and uses 16 significant digits for floating-point display.
- Exact sources, computed results, and floating-point presentation must remain distinct.

References:
- https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/17
- shared/pi_precision.hpp
- docs/GAMMAGAMMA_PI_PRECISION.md

Do not replace the original Pi integer or GammaGamma contracts. Euler's number e is mathematically a different constant and has its own computed source and audit ID.

## Authority correction: PR #3 native Pi Q62 record (2026-11-21)

The maintainer explicitly identifies PR #3's own comment as the canonical
authority for the **native multiply/shift conversion pattern**, rather than
the later PR #17 Pi-derived *display policy*. The original, unchanged Pi
authority is:

    const uint64_t result = (uint64_t)(((__uint128_t)(uint64_t)14488038916154245685ULL * 100000000000000000ULL) >> 62);
    // 314159265358979323

Source: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/3#issuecomment-6902016891

The original author glyph is **ℼ (U+213C)**. It is not silently normalized
into ordinary **π (U+03C0)**. The original PR #3 source comment retains its
exact code points and remains authoritative for that symbol.

Euler counterpart, using the same Q62 seed / 10^17 decimal scale / shift:

    const uint64_t result = (uint64_t)(((__uint128_t)(uint64_t)12535862302449814170ULL * 100000000000000000ULL) >> 62);
    // 271828182845904523

Place the decimal point explicitly after the first digit to display
**2.71828182845904523** (18 significant digits, truncated).

Both expressions convert *already chosen fixed-point seeds* to scaled decimal
integers. They are not independently complete algorithms for deriving the
irrational constants from first principles. The Euler seed was independently
cross-checked against the factorial-series expansion, whose exact rational
bound agrees with the same 18-digit truncated result.

The Q62 seed may be checked independently as
`floor(2^62 * sum(1/k!, k=0..40)) = 12535862302449814170`.
The positive omitted series remainder is smaller than `1/(40 * 40!)`,
which is too small to cross that Q62 integer boundary. This is an audit
of the **chosen seed**, not a claim that the single Q62 multiplication
computes the irrational e from scratch.

The new `euler_scaled18` uses the native `__uint128_t` multiplication where
supported. Because `__uint128_t` is a compiler extension and not ISO C++20,
the same header also implements the exact equivalent operation using four
32x32 multiplies and carry propagation for compilers without that type,
including MSVC. This portable equivalent uses no floating point or standard
library headers. Static and runtime tests compare both forms, and independently
test the original PR #3 Pi seed through the portable operation.

**Custody:** PR #3 authority remains unchanged. PR #17 output formatting is a
separate historical rule. The original v1 factorial-series check is retained,
not erased or relabeled as a Q62 experiment. The v2 Q62 result has 18 significant
digits; the v1 rounded display has 16. They must not be conflated.

## Native algorithm

The new independent, include-free C++20 header shared/euler_native.hpp uses:

    e = SUM (1/k!) for k >= 0

It evaluates the integer lower sum:

    lower = SUM floor(10^18 / k!), k = 0..20

This uses only 64-bit unsigned integer multiplication, division, and addition. No floating-point or external mathematics library is used in the constant calculation. The 21 floor operations lose fewer than 21 units at scale 10^18, and the omitted factorial-series tail is less than 1 additional unit at that scale. Therefore:

    lower <= e*10^18 < lower + 22

For the current witness:

    lower = 2718281828459045226
    upper = 2718281828459045248

Both endpoints round to the same 15-fractional-digit integer when divided by 1000 with half-up rounding:

    e*10^15 rounded = 2718281828459045
    ASCII output = 2.718281828459045

This is a *certified decimal-rounding interval*, not a claim of infinite precision. The implementation's 16 significant digits align with the current display policy, but the independent integer source interval is not rounded in the audit. If the display policy changes, a new version and revalidation are required.

## Domain and error separation

- The optional binary64() adapter converts the certified decimal result to a double for models that explicitly permit binary64. It is not an exact real-number representation of e.
- This change does NOT repair overflow, underflow, division by zero, sign loss, or determinant non-injectivity in inverse-square-SELF, Euler roots, or QUADRATICISIZOR. Those each require separate domain and range guards.
- Original COREAO traces, Pi records, ROOT operators, BiDi witnesses, and numerical experiments remain unchanged.
- e is positive, but a negative base to the real-valued e-th root remains outside the current real-number domain.
- Do not silently replace std::exp(), pow(), or any existing constant without establishing the caller's numerical range and accuracy requirements.

## Verification

- tests/euler_native_test.cpp checks the exact lower interval, upper interval, rounded result, ASCII placement, and the optional binary64 numeric neighborhood, plus Q62 Euler output, its 64-bit-limb parity witness, and the PR #3 Pi seed as regression witness.
- The calculations are compile-time asserted; a CTest executable also returns a nonzero status if the witnesses disagree.
- The source uses no standard-library headers and does not touch the production Pentagonal XOR hot path.
- The test must be built with an explicitly validated ISO C++20 /O2 or -O2 toolchain and executed on the desired supported platforms.
- CI results are not claimed until actually observed.

## Change classification and HOLD

This is a new experiment, not a retroactive edit to a fixed Pi protocol. Keep previous records intact.

The repository's current root CMake file still contains a GCC/Clang Release -O3 policy, which conflicts with the current -O2-only requirement. This proposal does not silently rewrite that unrelated history or approve an O3 execution. Full release/policy compliance remains HOLD pending separately scoped review. No GNU/POSIX dependency is introduced by these Euler source files.

Chosen, never assumed.
