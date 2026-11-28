/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
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
