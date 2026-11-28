/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
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
