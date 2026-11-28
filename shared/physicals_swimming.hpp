// SPDX-License-Identifier: 0BSD
#ifndef SWE_PHYSICALS_SWIMMING_HPP
#define SWE_PHYSICALS_SWIMMING_HPP
namespace MSL {
struct SWIMMING final {
    static constexpr double GRAVITY = 9.81;
    static constexpr unsigned STEPS = 90000u;
    static constexpr double DELTA = 0.01;
    static_assert(~0u == 0xffffffffu);
    double LEFT = 0, RIGHT = 0; // thrust in N
    double SPACE = 0, VELOCITY = 0;
    unsigned TICK = 0, BOYANCY = 42u;
    double MASS = 0.065, VOLUME = 0.000065;
    double DENSITY = 1000, AREA = 0.002, Cd = 0.47, CURRENT = 0;
    constexpr double TIME() const noexcept { return TICK * DELTA; }
    constexpr double BUOYANT() const noexcept { return DENSITY * VOLUME * GRAVITY; }
    constexpr double WEIGHT() const noexcept { return MASS * GRAVITY; }
    constexpr double BIAS() const noexcept { return LEFT - RIGHT; }
    constexpr double DRAG() const noexcept {
        const double u = VELOCITY - CURRENT;
        return -0.5 * DENSITY * Cd * AREA * u * (u < 0 ? -u : u);
    }
    constexpr double ACCELERATION(double disturbance = 0) const noexcept {
        return (BUOYANT() - WEIGHT() + DRAG() + BIAS() + disturbance) / MASS;
    }
    // Bounded force perturbations in N. No cryptographic claim.
    constexpr bool SWIM(double salt = 0, double thrash = 0) noexcept {
        if (!(MASS > 0 && VOLUME >= 0 && DENSITY >= 0 && AREA >= 0 && Cd >= 0 &&
              salt >= 0 && salt <= 0.15 && thrash >= 0 && thrash <= 0.30) ||
            TICK == ~0u) return false;
        const unsigned next = 1664525u * BOYANCY + 1013904223u;
        const double noise = salt * (2.0 * next / 4294967296.0 - 1.0);
        const double burst = TICK % 3000u < 300u ? (TICK % 2u ? -thrash : thrash) : 0;
        const double v = VELOCITY + ACCELERATION(noise + burst) * DELTA;
        const double x = SPACE + v * DELTA;
        // Explicit numerical envelope; reject before state mutation.
        if (!(v >= -100 && v <= 100 && x >= -1e9 && x <= 1e9)) return false;
        VELOCITY = v;
        SPACE = x;
        BOYANCY = next;
        ++TICK;
        return true;
    }
};
}
#endif
