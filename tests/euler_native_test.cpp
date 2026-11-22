// SPDX-License-Identifier: 0BSD
// Independent expected integer for Euler Q64: e/4 seed -> 18 decimal digits.
#include "../shared/euler_native.hpp"
static_assert(swe::euler_native::e_scaled18 == 271828182845904523ULL);
int main() {
    return swe::euler_native::e_scaled18 == 271828182845904523ULL ? 0 : 1;
}
