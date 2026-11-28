/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "research_filter.hpp"
using namespace swe::diaery::research;
constexpr Record inputs[]{
    {"study-A/row-1",Measurement::PenileLength,0.0,true},
    {"study-B/row-2",Measurement::VaginalDepth,0.0,true},
    {"study-C/row-3",Measurement::PenileLength,4.0,true},
    {"study-D/row-4",Measurement::VaginalDepth,2.9,true},
    {"study-E/row-5",Measurement::PenileLength,0.0,false},
    {"study-F/row-6",Measurement::PenileLength,-1.0,true}
};
constexpr auto totals=count(inputs,6u);
static_assert(totals.included==2u);
static_assert(totals.excluded==4u);
static_assert(totals.zero==2u);
static_assert(totals.missing==1u);
static_assert(assess(inputs[0]).disposition==Disposition::ExcludedZero);
static_assert(assess(inputs[1]).disposition==Disposition::ExcludedZero);
static_assert(assess(inputs[4]).disposition==Disposition::ExcludedMissing);
static_assert(assess(inputs[5]).disposition==Disposition::ExcludedNegative);
static_assert(assess(inputs[0]).source_id==inputs[0].source_id);
static_assert(inputs[0].value_inches==0.0);
static_assert(assess({nullptr,Measurement::VaginalDepth,0.0,true}).disposition==Disposition::ExcludedZero);
static_assert(!included(assess({nullptr,Measurement::PenileLength,0.0,true})));
static_assert(included(assess({nullptr,Measurement::VaginalDepth,1.0,true})));
int main(){return 0;}
