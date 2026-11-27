// SPDX-License-Identifier: 0BSD
// GENSHIN independent FUCKITIN: Temporary-register exchange of the first two local fields.
// Layout payloads are occurrence IDs, never authoritative audit verdicts.
// Standalone translation unit; no includes or shared ZIPIT dependency.
namespace genshin_fuckitin {
struct Layout {
    unsigned pass, yin, yang, airgap;
    constexpr bool operator==(const Layout&) const = default;
};
inline constexpr Layout CORE{0u, 1u, 2u, 3u};
constexpr Layout FUCKITIN(Layout x = CORE) noexcept {
    const unsigned saved = x.pass;
    x.pass = x.yin;
    x.yin = saved;
    return x;
}
static_assert(FUCKITIN() == Layout{1u, 0u, 2u, 3u});
static_assert(FUCKITIN(FUCKITIN()) == CORE);
constexpr Layout sample{11u, 22u, 33u, 44u};
static_assert(FUCKITIN(sample) == Layout{22u, 11u, 33u, 44u});
static_assert(FUCKITIN(FUCKITIN(sample)) == sample);
static_assert(sample == Layout{11u, 22u, 33u, 44u});
} // namespace genshin_fuckitin
