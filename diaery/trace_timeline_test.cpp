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
int main(){return 0;}
