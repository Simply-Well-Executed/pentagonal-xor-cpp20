// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#include "../shared/LCD.hpp"

// Independent descending-divisor oracle for the finite 8-bit domain.
constexpr bool witness(U a, U c) noexcept {
    const L l = LCD(a,c);
    if (!c) return l.a == a && l.c == 0;
    U g = c;
    while (a % g || c % g) --g;
    return l.a == a/g && l.c == c/g;
}

static_assert(LCD(15,7).a == 15 && LCD(15,7).c == 7);
static_assert(LCD(10000,30).a == 1000 && LCD(10000,30).c == 3);
static_assert(LCD(0,255).a == 0 && LCD(0,255).c == 1);
static_assert(LCD(1,0).a == 1 && LCD(1,0).c == 0);
static_assert(LCD(0,0).a == 0 && LCD(0,0).c == 0);
static_assert(LCD(~U{0},~U{0}).a == 1 && LCD(~U{0},~U{0}).c == 1);
static_assert(LCD(~U{0},1).a == ~U{0} && LCD(~U{0},1).c == 1);
static_assert(LCD(1,~U{0}).a == 1 && LCD(1,~U{0}).c == ~U{0});
static_assert(LCD(~U{0},0).a == ~U{0} && LCD(~U{0},0).c == 0);

int main() {
    for (U cycle=0; cycle<256; ++cycle)
        for (U c=0; c<256; ++c)
            if (!witness(cycle,c)) return 1;
    return 0;
}
