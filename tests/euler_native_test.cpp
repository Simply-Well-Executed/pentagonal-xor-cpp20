// SPDX-License-Identifier: 0BSD
// EULER-Q64-v4: independent exact expected integer and ties-even canaries.
#include "../shared/euler_native.hpp"
static_assert(swe::euler_native::e_scaled18 == 271828182845904524ULL);
static_assert(swe::euler_native::round_nearest_even(1u, 2u) == 0u);
static_assert(swe::euler_native::round_nearest_even(3u, 2u) == 2u);
static_assert(swe::euler_native::round_nearest_even(5u, 2u) == 2u);
static_assert(swe::euler_native::round_nearest_even(7u, 2u) == 4u);
int main() {
    return swe::euler_native::e_scaled18 == 271828182845904524ULL ? 0 : 1;
}
