// SPDX-License-Identifier: 0BSD
#ifndef GENSHIN_ZIPIT_HPP
#define GENSHIN_ZIPIT_HPP
// Structural role permutations; not authority to change an audit verdict.
struct genshin_zipit {
enum class Role : unsigned { PASS, YIN, YANG, AIRGAP };
struct Layout {
    Role pass, yin, yang, airgap;
    constexpr bool operator==(const Layout&) const = default;
};
inline static constexpr Layout CORE{
    Role::PASS, Role::YIN, Role::YANG, Role::AIRGAP
};
static constexpr Layout ZIPITUP(Layout x = CORE) noexcept {
    return x;
}
static constexpr Layout ZIPITDOWN(Layout x = CORE) noexcept {
    return {x.airgap, x.yin, x.yang, x.pass};
}
static constexpr Layout ZIPITOUT(Layout x = CORE) noexcept {
    return {x.yang, x.yin, x.pass, x.airgap};
}
static constexpr Layout FUCKITIN(Layout x = CORE) noexcept {
    return {x.yin, x.pass, x.yang, x.airgap};
}
// The verdict stays separate from the role layout.
struct Evidence {
    bool yin, yang, airgap;
    constexpr bool passed() const noexcept {
        return yin && yang && airgap;
    }
};
};

// Same frozen synthetic fixture values; evaluated after type completion.
static_assert([]() constexpr {
    static_assert(genshin_zipit::ZIPITUP() == genshin_zipit::CORE);
    static_assert(genshin_zipit::ZIPITDOWN() ==
        genshin_zipit::Layout{genshin_zipit::Role::AIRGAP, genshin_zipit::Role::YIN, genshin_zipit::Role::YANG, genshin_zipit::Role::PASS});
    static_assert(genshin_zipit::ZIPITOUT() ==
        genshin_zipit::Layout{genshin_zipit::Role::YANG, genshin_zipit::Role::YIN, genshin_zipit::Role::PASS, genshin_zipit::Role::AIRGAP});
    static_assert(genshin_zipit::FUCKITIN() ==
        genshin_zipit::Layout{genshin_zipit::Role::YIN, genshin_zipit::Role::PASS, genshin_zipit::Role::YANG, genshin_zipit::Role::AIRGAP});
    static_assert(genshin_zipit::ZIPITDOWN(genshin_zipit::ZIPITDOWN()) == genshin_zipit::CORE);
    static_assert(genshin_zipit::ZIPITOUT(genshin_zipit::ZIPITOUT()) == genshin_zipit::CORE);
    static_assert(genshin_zipit::FUCKITIN(genshin_zipit::FUCKITIN()) == genshin_zipit::CORE);
    static_assert(genshin_zipit::ZIPITUP(genshin_zipit::ZIPITDOWN()) == genshin_zipit::ZIPITDOWN());
    static_assert(genshin_zipit::ZIPITDOWN(genshin_zipit::ZIPITOUT()) != genshin_zipit::ZIPITOUT(genshin_zipit::ZIPITDOWN()));
    static_assert(genshin_zipit::Evidence{true,true,true}.passed());
    static_assert(!genshin_zipit::Evidence{true,true,false}.passed());
    static_assert(!genshin_zipit::Evidence{true,false,true}.passed());
    static_assert(!genshin_zipit::Evidence{false,true,true}.passed());
    return true;
}());
#endif
