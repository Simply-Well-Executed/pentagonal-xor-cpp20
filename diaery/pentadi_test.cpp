// SPDX-License-Identifier: 0BSD
#include "pentadi.hpp"
using namespace swe::diaery::pentadi;
constexpr auto pending=evaluate({State::Null,State::Null,State::Null,State::Null});
static_assert(pending.status==State::Null&&pending.common_null==State::Null);
static_assert(!promoted(pending));
constexpr auto all=evaluate({State::Pass,State::Pass,State::Pass,State::Pass});
static_assert(all.status==State::Pass&&all.pass_count==4u);
static_assert(all.common_null==State::Null&&promoted(all));
static_assert(!promoted(evaluate({State::Pass,State::Null,State::Pass,State::Pass})));
static_assert(!promoted(evaluate({State::Pass,State::Pass,State::Fail,State::Pass})));
static_assert(!promoted(evaluate({State::Pass,State::Pass,State::Pass,State::Null})));
int main(){return 0;}
