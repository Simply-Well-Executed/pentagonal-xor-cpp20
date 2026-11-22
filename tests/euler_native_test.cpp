// SPDX-License-Identifier: 0BSD
// C++20 native integer Euler-series witnesses (no standard-library includes).
#include "../shared/euler_native.hpp"

namespace {
constexpr char expected[] = "2.718281828459045";
constexpr bool same_ascii(const char* a, const char* b) noexcept {
    for (unsigned i = 0u; i <= 17u; ++i) {
        if (a[i] != b[i]) return false;
        if (a[i] == '\0') return true;
    }
    return false;
}
constexpr auto e = swe::euler_native::result;
// Exact Q62 fixed-point form from the PR #3 Pi comment, now for Euler.
static_assert(swe::euler_native::euler_q62_seed == 12535862302449814170ULL);
static_assert(swe::euler_native::decimal_scale17 == 100000000000000000ULL);
static_assert(swe::euler_native::euler_scaled18 == 271828182845904523ULL);
static_assert(swe::euler_native::portable_euler_scaled18 == 271828182845904523ULL);
static_assert(swe::euler_native::q62_decimal_portable(
    14488038916154245685ULL, 100000000000000000ULL) == 314159265358979323ULL);
static_assert(e.lower_scaled18 == 2718281828459045226ULL);
static_assert(e.upper_scaled18 == 2718281828459045248ULL);
static_assert(e.rounded_scaled15 == 2718281828459045ULL);
static_assert(e.certified);
static_assert(same_ascii(swe::euler_native::ascii.value, expected));
// Distinct steps: 16-significant-digit constant vs. binary64 adapter.
static_assert(swe::euler_native::binary64() > 2.71828182845904);
static_assert(swe::euler_native::binary64() < 2.71828182845905);
} // namespace

int main() {
    const auto independently_recomputed = swe::euler_native::compute();
    if (!independently_recomputed.certified) return 1;
    if (swe::euler_native::euler_scaled18 != 271828182845904523ULL) return 6;
    if (swe::euler_native::portable_euler_scaled18 !=
        swe::euler_native::euler_scaled18) return 7;
    if (swe::euler_native::q62_decimal_portable(
            14488038916154245685ULL, 100000000000000000ULL) !=
        314159265358979323ULL) return 8;
    if (independently_recomputed.lower_scaled18 != e.lower_scaled18) return 2;
    if (independently_recomputed.upper_scaled18 != e.upper_scaled18) return 3;
    if (independently_recomputed.rounded_scaled15 != e.rounded_scaled15) return 4;
    if (!same_ascii(swe::euler_native::decimal16().value, expected)) return 5;
    return 0;
}
