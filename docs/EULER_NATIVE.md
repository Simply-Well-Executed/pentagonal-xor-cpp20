# Native Euler number: integer-derived, guarded decimal witness

Status: EXPLORATORY / review required. Version: EULER-NATIVE-INTEGER-SERIES-v1.
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

- tests/euler_native_test.cpp checks the exact lower interval, upper interval, rounded result, ASCII placement, and the optional binary64 numeric neighborhood.
- The calculations are compile-time asserted; a CTest executable also returns a nonzero status if the witnesses disagree.
- The source uses no standard-library headers and does not touch the production Pentagonal XOR hot path.
- The test must be built with an explicitly validated ISO C++20 /O2 or -O2 toolchain and executed on the desired supported platforms.
- CI results are not claimed until actually observed.

## Change classification and HOLD

This is a new experiment, not a retroactive edit to a fixed Pi protocol. Keep previous records intact.

The repository's current root CMake file still contains a GCC/Clang Release -O3 policy, which conflicts with the current -O2-only requirement. This proposal does not silently rewrite that unrelated history or approve an O3 execution. Full release/policy compliance remains HOLD pending separately scoped review. No GNU/POSIX dependency is introduced by these Euler source files.

Chosen, never assumed.
