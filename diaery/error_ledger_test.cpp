/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "error_ledger.hpp"
using namespace swe::diaery::error;
constexpr Paper p1{Metric::ErectLength,{10,20,30,0,0,0,0,0},3,true,true,true,true};
constexpr Paper p2{Metric::ErectLength,{15,25,0,0,0,0,0,0},2,true,true,true,true};
constexpr Paper good[]={p1,p2};
constexpr auto aggregate=sigma_mean(good,2);
static_assert(aggregate.state==State::Valid);
static_assert(aggregate.numerator==100 && aggregate.denominator==2);
static_assert(sum_paper(p1).numerator==60);
constexpr Paper mixed[]={p1,Paper{Metric::ErectCircumference,{10},1,true,true,true,true}};
static_assert(sigma_mean(mixed,2).state==State::Incomparable);
constexpr Paper unknown{Metric::VaginalDepth,{0},0,false,true,true,true};
static_assert(sum_paper(unknown).state==State::MissingComponent);
constexpr Paper disallowed_mri{Metric::VaginalDepth,{1},1,true,true,false,true};
static_assert(sum_paper(disallowed_mri).state==State::NullRecord);
constexpr Paper maxed{Metric::ErectLength,{LIMIT,1},2,true,true,true,true};
static_assert(sum_paper(maxed).state==State::Overflow);
static_assert(!admissible_depth(0.0,true,true));
static_assert(!admissible_depth(3.0,false,true));
static_assert(!admissible_depth(3.0,true,false));
static_assert(admissible_depth(3.0,true,true));
static_assert(proposed_euler_jig_log_gate(true)==State::InvalidPhysicalModel);
int main(){return 0;}
