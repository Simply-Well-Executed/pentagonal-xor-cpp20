/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#include "../shared/TALON.hpp"
constexpr auto result=TALON::run();
static_assert(TALON::twist({0,1},0).y==0);
static_assert(TALON::twist({0,1},1).y==1);
static_assert(TALON::twist({.6,.8},1).x==.6);
static_assert(TALON::abs(TALON::drag*TALON::vt2-TALON::weight)<1e-12);
static_assert(result.passed && result.completed==256);
static_assert(result.orb.lcd.a==1 && result.orb.lcd.c==1);
static_assert(result.orb.speed>31.23 && result.orb.speed<31.24);
static_assert(result.orb.kinetic>4.39 && result.orb.kinetic<4.40);
static_assert(TALON::redux(result.orb)>.95 && TALON::redux(result.orb)<.96);
constexpr bool atomix() noexcept {
    auto f=result.orb;f.dissipated+=1;
    if(TALON::airgap(f))return false;
    f=result.orb;f.numerical=1.01;
    if(TALON::airgap(f))return false;
    f=result.orb;f.analytical=0;
    return !TALON::airgap(f);
}
static_assert(atomix());
int main() {
    const auto r=TALON::run();
    return r.passed && r.completed==256 && atomix()?0:1;
}
