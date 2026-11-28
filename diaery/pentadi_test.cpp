/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
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
static_assert(evaluate({State::Null,State::Pass,State::Fail,State::Pass}).status==State::Fail);
static_assert(evaluate({State::Null,State::Pass,State::Fail,State::Pass}).pass_count==2u);
int main(){return 0;}
