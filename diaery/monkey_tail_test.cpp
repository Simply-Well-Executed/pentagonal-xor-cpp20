// SPDX-License-Identifier: 0BSD
#include "monkey_tail.hpp"
using namespace swe::diaery::monkey_tail;
static_assert(conjugate_triangle(3,4).state==State::Verified);
static_assert(conjugate_triangle(3,4).product_real==25LL);
static_assert(conjugate_triangle(3,4).product_imaginary==0LL);
static_assert(conjugate_triangle(0,1).product_real==1LL);
static_assert(conjugate_triangle(-3,4).product_real==25LL);
static_assert(conjugate_triangle(1000000001LL,2).state==State::InvalidDomain);
static_assert(conjugate_triangle(-9223372036854775807LL-1LL,2).state==State::InvalidDomain);
static_assert(round_nearest_even(5,2).value==2ULL); // 2.5 -> 2
static_assert(round_nearest_even(7,2).value==4ULL); // 3.5 -> 4
static_assert(round_nearest_even(9,4).value==2ULL); // 2.25 -> 2
static_assert(round_nearest_even(11,4).value==3ULL); // 2.75 -> 3
static_assert(round_nearest_even(3,3).exact);
static_assert(round_nearest_even(1,0).state==State::ZeroDenominator);
static_assert(negative_gain_to_zero(3,4).shrinking);
static_assert(negative_gain_to_zero(-3,4).shrinking);
static_assert(negative_gain_to_zero(-3,4).alternating_sign);
static_assert(!negative_gain_to_zero(0,4).alternating_sign);
static_assert(!negative_gain_to_zero(4,3).shrinking);
int main(){return 0;}
