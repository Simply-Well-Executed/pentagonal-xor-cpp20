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
    return 0;
}
