// SPDX-License-Identifier: 0BSD
#include "accessible_seals.hpp"
using namespace swe::diaery::seals;
static_assert(contract());
static_assert(COVERAGE>=swe::diaery::access::COUNT);
static_assert(index({0,0,0,true},0)==0);
static_assert(index({0,1,0,true},0)==PAGES*TERMINALS);
static_assert(!visible({0,0,0,false},0));
static_assert(visible(choose_root({1,1,4,false},0),0));
static_assert(choose_branch({0,1,1,true},1).expanded==false);
static_assert(choose_page({0,0,0,false},3).expanded);
int main(){return 0;}
