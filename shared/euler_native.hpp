// SPDX-License-Identifier: 0BSD
// Euler's number, Q64 fraction e/4, modeled on the native PR #3 pattern.
// All 18 output digits are truncated. The decimal point follows digit one.
// No fallback or extra arithmetic handler: compiler must provide __uint128_t.
#ifndef SWE_EULER_NATIVE_HPP
#define SWE_EULER_NATIVE_HPP
#if !defined(__SIZEOF_INT128__)
#error "Euler Q64 requires native __uint128_t; unsupported compiler."
#endif
namespace swe::euler_native {
inline constexpr unsigned long long e_scaled18 =
    (unsigned long long)(((__uint128_t)(unsigned long long)
    12535862302449814170ULL * 400000000000000000ULL) >> 64);
// 271828182845904523 => 2.71828182845904523
static_assert(e_scaled18 == 271828182845904523ULL);
}
#endif
