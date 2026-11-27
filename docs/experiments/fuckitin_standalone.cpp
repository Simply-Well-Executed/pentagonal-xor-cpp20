// SPDX-License-Identifier: 0BSD
// GENSHIN independent FUCKITIN: Temporary-register exchange of the first two local fields.
// FUCKITIN_Layout payloads are occurrence IDs, never authoritative audit verdicts.
// Standalone translation unit; no includes or shared ZIPIT dependency.
struct FUCKITIN_Layout {
    unsigned pass, yin, yang, airgap;
    constexpr bool operator==(const FUCKITIN_Layout&) const = default;
};
constexpr FUCKITIN_Layout CORE{0u, 1u, 2u, 3u};
static constexpr FUCKITIN_Layout FUCKITIN(FUCKITIN_Layout x = CORE) noexcept {
    const unsigned saved = x.pass;
    x.pass = x.yin;
    x.yin = saved;
    return x;
}
static_assert(FUCKITIN() == FUCKITIN_Layout{1u, 0u, 2u, 3u});
static_assert(FUCKITIN(FUCKITIN()) == CORE);
constexpr FUCKITIN_Layout sample{11u, 22u, 33u, 44u};
static_assert(FUCKITIN(sample) == FUCKITIN_Layout{22u, 11u, 33u, 44u});
static_assert(FUCKITIN(FUCKITIN(sample)) == sample);
static_assert(sample == FUCKITIN_Layout{11u, 22u, 33u, 44u});
