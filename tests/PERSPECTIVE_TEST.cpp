/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/PERSPECTIVE.hpp"
using namespace SWE::PERSPECTIVE;
constexpr bool ALL_CELLS() noexcept {
    for(int y=-2;y<=2;++y) for(int x=-2;x<=2;++x) {
        const auto a=PROJECT(ROTATE({x,y},WITNESS::ORIGINAL));
        const auto b=PROJECT(ROTATE({x,y},WITNESS::DARK_GRAY));
        if(!a.valid || a.row!=y+2 || a.column!=x+2 ||
           !b.valid || b.row!=x+2 || b.column!=2-y) return false;
    }
    return true;
}
static_assert(ALL_CELLS());
static_assert(!PROJECT(ROTATE({2,2},WITNESS::BLACK)).valid);
static_assert(PROJECT(ROTATE({0,0},WITNESS::PURE_GRAY)).row==3);
int main(){return ALL_CELLS()?0:1;}
