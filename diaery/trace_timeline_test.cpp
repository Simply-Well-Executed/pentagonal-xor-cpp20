/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "trace_timeline.hpp"
using namespace swe::diaery::trace;
static_assert(FS_PER_SECOND==1000000000000000ULL);
constexpr auto t0=split_femtoseconds(1000000000000001ULL);
static_assert(t0.time.seconds==1ULL && t0.time.remainder_femtoseconds==1ULL);
static_assert(split_femtoseconds(FS_PER_SECOND-1).time.seconds==0ULL);
static_assert(advance(FS_PER_SECOND,1ULL).fs==FS_PER_SECOND+1ULL);
static_assert(advance(~0ULL,1ULL).status==Status::Overflow);
static_assert(x_subunit_count(12u)==48u && y_subunit_count(12u)==36u);
static_assert(x_is_major(4u)&&!x_is_major(3u));
static_assert(y_is_major(3u)&&!y_is_major(2u));
static_assert(parse("Y=MX+B")==Pattern::YEqualsMXPlusB);
static_assert(parse("Y=X")==Pattern::YEqualsX);
static_assert(parse("Y=0")==Pattern::YEqualsZero);
static_assert(parse("Y=BAD")==Pattern::Invalid);
static_assert(parse(nullptr)==Pattern::Invalid);
static_assert(calculate(parse("Y=MX+B"),2.,-3.,6.).y==0.);
static_assert(calculate(parse("Y=X"),2.,-3.,6.).y==2.);
static_assert(calculate(parse("Y=0"),2.,-3.,6.).y==0.);
static_assert(calculate(parse("Y=BAD"),2.,-3.,6.).status==Status::InvalidPattern);
// DARN is exact; YARN is derived and never overwrites its authority.
// One sploon = 10^15 / 16 = 62,500,000,000,000 fs exactly.
constexpr U64 SPLOON_FS=FS_PER_SECOND/16ULL;
static_assert(SPLOONS_PER_DARN==16u);
static_assert(SPLOON_FS*16ULL==FS_PER_SECOND);
static_assert(make_yarn(0ULL).shown_yarn_seconds==0ULL);
constexpr auto s8=make_yarn(8ULL*SPLOON_FS);
static_assert(s8.sploon==8u && s8.threshold_carry==0u &&
              s8.angular_bonus==0u && s8.shown_yarn_seconds==0ULL);
// At 9/16 + .1 > .6, start a one-second display carry.
constexpr auto s9=make_yarn(9ULL*SPLOON_FS);
static_assert(s9.sploon==9u && s9.threshold_carry==1u &&
              s9.angular_bonus==0u && s9.shown_yarn_seconds==1ULL);
constexpr auto s14=make_yarn(14ULL*SPLOON_FS);
static_assert(s14.threshold_carry==1u&&s14.angular_bonus==0u);
// At 15/16 + .1 > 1, add an EXTRA second beyond the first carry.
constexpr auto s15=make_yarn(15ULL*SPLOON_FS);
static_assert(s15.sploon==15u && s15.threshold_carry==1u &&
              s15.angular_bonus==1u && s15.shown_yarn_seconds==2ULL);
constexpr auto through_one=make_yarn(FS_PER_SECOND+15ULL*SPLOON_FS);
static_assert(through_one.exact_seconds==1ULL);
static_assert(through_one.shown_yarn_seconds==3ULL);
static_assert(through_one.remainder_femtoseconds==15ULL*SPLOON_FS);
static_assert(split_femtoseconds(FS_PER_SECOND+15ULL*SPLOON_FS).time.seconds==1ULL);
// Exact predefined length conversion is rational and independent from pi.
constexpr auto inch=inches_to_micrometres(1ULL);
static_assert(inch.numerator==25400ULL && inch.denominator==1ULL);
constexpr auto back=micrometres_to_inches(25400ULL);
static_assert(back.numerator==25400ULL && back.denominator==25400ULL);
static_assert(inches_to_micrometres(~0ULL).denominator==0ULL);
static_assert(n_minus_1663_pi_squared(0u).status==Status::Valid);
// The displayed '-' must remain an arithmetic subtraction.
static_assert(n_minus_1663_pi_squared(1u).derived -
              n_minus_1663_pi_squared(0u).derived > 0.999999999998);
static_assert(n_minus_1663_pi_squared(1u).derived -
              n_minus_1663_pi_squared(0u).derived < 1.000000000002);
static_assert(n_minus_1663_pi_squared(15u).derived<0.0);
static_assert(n_minus_1663_pi_squared(16u).status==Status::NullInput);
static_assert(make_yarn(FS_PER_SECOND).exact_seconds==1ULL);
int main(){return 0;}
