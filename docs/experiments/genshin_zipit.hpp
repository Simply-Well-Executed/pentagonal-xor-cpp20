// SPDX-License-Identifier: 0BSD
#ifndef GENSHIN_ZIPIT_HPP
#define GENSHIN_ZIPIT_HPP
// Structural role permutations; not authority to change an audit verdict.
namespace genshin_zipit {
enum class Role : unsigned { PASS, YIN, YANG, AIRGAP };
struct Layout {
    Role pass, yin, yang, airgap;
    constexpr bool operator==(const Layout&) const = default;
};
inline constexpr Layout CORE{
    Role::PASS, Role::YIN, Role::YANG, Role::AIRGAP
};
constexpr Layout ZIPITUP(Layout x = CORE) noexcept {
    return x;
}
constexpr Layout ZIPITDOWN(Layout x = CORE) noexcept {
    return {x.airgap, x.yin, x.yang, x.pass};
}
constexpr Layout ZIPITOUT(Layout x = CORE) noexcept {
    return {x.yang, x.yin, x.pass, x.airgap};
}
constexpr Layout FUCKITIN(Layout x = CORE) noexcept {
    return {x.yin, x.pass, x.yang, x.airgap};
}
// The verdict stays separate from the role layout.
struct Evidence {
    bool yin, yang, airgap;
    constexpr bool passed() const noexcept {
        return yin && yang && airgap;
    }
};
static_assert(ZIPITUP() == CORE);
static_assert(ZIPITDOWN() ==
    Layout{Role::AIRGAP, Role::YIN, Role::YANG, Role::PASS});
static_assert(ZIPITOUT() ==
    Layout{Role::YANG, Role::YIN, Role::PASS, Role::AIRGAP});
static_assert(FUCKITIN() ==
    Layout{Role::YIN, Role::PASS, Role::YANG, Role::AIRGAP});
static_assert(ZIPITDOWN(ZIPITDOWN()) == CORE);
static_assert(ZIPITOUT(ZIPITOUT()) == CORE);
static_assert(FUCKITIN(FUCKITIN()) == CORE);
static_assert(ZIPITUP(ZIPITDOWN()) == ZIPITDOWN());
static_assert(ZIPITDOWN(ZIPITOUT()) != ZIPITOUT(ZIPITDOWN()));
static_assert(Evidence{true,true,true}.passed());
static_assert(!Evidence{true,true,false}.passed());
static_assert(!Evidence{true,false,true}.passed());
static_assert(!Evidence{false,true,true}.passed());
} // namespace genshin_zipit
#endif
