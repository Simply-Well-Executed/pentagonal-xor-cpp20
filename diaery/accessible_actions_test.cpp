/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
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
