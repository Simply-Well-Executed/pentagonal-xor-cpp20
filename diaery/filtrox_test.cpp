// SPDX-License-Identifier: 0BSD
#include "filtrox.hpp"
using namespace swe::diaery::filtrox;
using swe::diaery::research::Record;
constexpr Measurement pl=Measurement::PenileLength;
constexpr Measurement pc=Measurement::PenileCircumference;
constexpr Measurement vd=Measurement::VaginalDepth;
static_assert(reference(pl).minimum_cm==4.0&&reference(pl).maximum_cm==26.0);
static_assert(reference(pc).minimum_cm==3.0&&reference(pc).maximum_cm==19.0);
static_assert(reference(vd).minimum_cm==6.5&&reference(vd).maximum_cm==12.5);
// Exact metric sample edges convert back to inch coordinates for comparison.
static_assert(assess({"A",pl,4.01/2.54,true}).position==Position::WithinObserved);
static_assert(assess({"B",pl,3.99/2.54,true}).position==Position::BelowObserved);
static_assert(assess({"C",pc,2.99/2.54,true}).position==Position::BelowObserved);
static_assert(assess({"D",vd,6.51/2.54,true}).position==Position::WithinObserved);
static_assert(assess({"E",vd,6.49/2.54,true}).position==Position::BelowObserved);
static_assert(assess({"F",vd,12.51/2.54,true}).position==Position::AboveObserved);
static_assert(assess({"G",pl,0.,true}).position==Position::NullInput);
static_assert(assess({"H",vd,0.,true}).position==Position::NullInput);
static_assert(assess({"I",pl,0.,false}).position==Position::NullInput);
static_assert(assess({"J",pl,0.244,true}).position==Position::BelowObserved);
static_assert(assess({"K",pc,0.1978,true}).position==Position::BelowObserved);
static_assert(clamp_to_study(0.,pl)==lowerbound_inches(pl));
static_assert(clamp_to_study(0.,vd)==lowerbound_inches(vd));
static_assert(clamp_to_study(200.,pl)==upperbound_inches(pl));
static_assert(assess({"SOURCE",vd,2.9,true}).original_inches==2.9);
// Samples outside a paper are not automatically biologically impossible.
int main(){return 0;}
