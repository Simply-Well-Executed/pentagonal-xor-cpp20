// SPDX-License-Identifier: 0BSD
#include "../shared/physicals_swimming.hpp"
using MSL::SWIMMING;
constexpr bool initial() {
    SWIMMING x{};
    if (x.ACCELERATION() != 0 || !x.SWIM() || x.SPACE != 0 || x.BOYANCY == 42u) return false;
    x.MASS = 0.1;
    if (!(x.ACCELERATION() < 0)) return false;
    x.MASS = 0.03;
    if (!(x.ACCELERATION() > 0)) return false;
    x.VELOCITY = 1;
    if (!(x.DRAG() < 0)) return false;
    x.VELOCITY = -1;
    return x.DRAG() > 0;
}
static_assert(initial());
constexpr bool drag_witness() {
    SWIMMING x{};
    x.CURRENT = 2;
    const double above = x.DAMPENING(12, 1);
    const double below = x.DAMPENING(-8, 1);
    return above > 2 && above < 12 && below < 2 && below > -8 &&
           x.DAMPENING(2, 1) == 2 && x.DAMPENING(12, 0) == 12;
}
static_assert(drag_witness());
int main() {
    SWIMMING a{}, b{};
    for (unsigned i = 0; i < SWIMMING::STEPS; ++i) {
        if (!a.SWIM(0.15, 0.30) || !b.SWIM(0.15, 0.30)) return 1;
        if (a.BOYANCY != b.BOYANCY || a.SPACE != b.SPACE ||
            a.VELOCITY != b.VELOCITY) return 2;
    }
    if (a.TIME() != 900 || a.TICK != 90000u) return 3;
    const auto old = a;
    if (a.SWIM(-1) || a.TICK != old.TICK || a.BOYANCY != old.BOYANCY ||
        a.SPACE != old.SPACE) return 4;
    SWIMMING strong{};
    strong.DENSITY = 1000;
    strong.Cd = 100;
    strong.VELOCITY = 10;
    if (!strong.SWIM() || !(strong.VELOCITY > 0 && strong.VELOCITY < 10)) return 5;
    SWIMMING neutral{};
    for (unsigned i = 0; i < SWIMMING::STEPS; ++i)
        if (!neutral.SWIM()) return 6;
    if (neutral.SPACE != 0 || neutral.VELOCITY != 0 || neutral.TIME() != 900) return 7;
    return 0;
}
