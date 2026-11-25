// SPDX-License-Identifier: 0BSD
#include "circle_band.hpp"
using namespace swe::diaery::circle_band;
constexpr unsigned char all[8]{1,1,1,1,1,1,1,1};
constexpr unsigned char missing[8]{1,1,1,1,1,1,0,1};
static_assert(assess({1.,2.,8,all,false}).nice);
static_assert(assess({1.,2.,8,all,false}).firm);
static_assert(!assess({1.,2.,8,all,false}).research_valid);
static_assert(assess({1.,2.,8,all,true}).research_valid);
static_assert(assess({1.,2.,8,missing,true}).geometry==Status::Gap);
static_assert(!assess({1.,2.,8,missing,true}).firm);
static_assert(assess({1.,2.,8,missing,true}).missing_count==1ULL);
static_assert(assess({2.,1.,8,all,true}).geometry==Status::InvalidRadius);
static_assert(assess({1.,2.,0,all,true}).geometry==Status::InvalidCount);
static_assert(complete(1.,2.,72,false).present_count==72ULL);
static_assert(!complete(1.,2.,72,false).research_valid);
static_assert(assess_cylinder({3.25,0.,0.,false,false}).state==CylinderState::MissingRadius);
static_assert(assess_cylinder({3.25,.5,.8,true,true}).state==CylinderState::ExploratoryGeometry);
static_assert(assess_cylinder({3.25,.5,.8,true,true}).stretched_diameter_inches==1.6);
int main(){return 0;}
