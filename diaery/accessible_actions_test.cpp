// SPDX-License-Identifier: 0BSD
#include "accessible_actions.hpp"
using namespace swe::diaery::access;
static_assert(contract());
static_assert(COLUMNS==6u);
static_assert(ROWS*COLUMNS>=COUNT);
static_assert(BUTTON_HEIGHT>=44u&&BUTTON_GAP>=4u);
static_assert(actions[0].virtual_key=='D');
static_assert(actions[9].virtual_key=='C');
static_assert(COUNT>=55u);
static_assert(actions[0].kind==Kind::Key);
int main(){return 0;}
