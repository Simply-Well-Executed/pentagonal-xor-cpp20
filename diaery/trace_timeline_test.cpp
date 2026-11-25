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
int main(){return 0;}
