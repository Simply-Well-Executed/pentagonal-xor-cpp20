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
