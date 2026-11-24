// SPDX-License-Identifier: 0BSD
// EULER-Q64-v4: Q64 e/4 witness, round-to-nearest, ties-to-even.
// The historical truncated EULER-Q64-v3 remains in Git provenance.
// Native __uint128_t is required; unsupported toolchains must HOLD.
#ifndef SWE_EULER_NATIVE_HPP
#define SWE_EULER_NATIVE_HPP
#if !defined(__SIZEOF_INT128__)
#error "Euler Q64 requires native __uint128_t; unsupported compiler."
#endif
namespace swe::euler_native {
constexpr unsigned long long round_nearest_even(__uint128_t numerator,
                                                __uint128_t denominator) {
    // Contract: denominator > 0; intended quotient fits unsigned long long.
    const __uint128_t quotient = numerator / denominator;
    const __uint128_t remainder = numerator % denominator;
    // Compare remainder to its complement to avoid doubling overflow.
    const bool up = remainder > (denominator - remainder) ||
        (remainder == (denominator - remainder) && (quotient & 1u) != 0);
    return static_cast<unsigned long long>(quotient + (up ? 1u : 0u));
}
inline constexpr __uint128_t e_q64_numerator =
    static_cast<__uint128_t>(12535862302449814170ULL) *
    static_cast<__uint128_t>(400000000000000000ULL);
inline constexpr __uint128_t e_q64_denominator =
    static_cast<__uint128_t>(1u) << 64;
inline constexpr unsigned long long e_scaled18 =
    round_nearest_even(e_q64_numerator, e_q64_denominator);
// Historical truncated witness is provenance only, never the new policy.
static_assert(e_q64_numerator / e_q64_denominator == 271828182845904523ULL);
static_assert(e_q64_numerator % e_q64_denominator == 9613202787723640832ULL);
static_assert(e_scaled18 == 271828182845904524ULL);
// Ties-even canaries: both even neighbors and ordinary rounding directions.
static_assert(round_nearest_even(5u, 2u) == 2u);
static_assert(round_nearest_even(7u, 2u) == 4u);
static_assert(round_nearest_even(9u, 2u) == 4u);
static_assert(round_nearest_even(11u, 2u) == 6u);
static_assert(round_nearest_even(5u, 4u) == 1u);
static_assert(round_nearest_even(7u, 4u) == 2u);
}
#endif
