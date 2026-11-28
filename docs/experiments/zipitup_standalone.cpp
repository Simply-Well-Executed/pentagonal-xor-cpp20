// SPDX-License-Identifier: 0BSD
// GENSHIN independent ZIPITUP: Identity via explicit aggregate copy.
// ZIPITUP_Layout payloads are occurrence IDs, never authoritative audit verdicts.
// Standalone translation unit; no includes or shared ZIPIT dependency.
struct ZIPITUP_Layout {
    unsigned pass, yin, yang, airgap;
    constexpr bool operator==(const ZIPITUP_Layout&) const = default;
};
constexpr ZIPITUP_Layout CORE{0u, 1u, 2u, 3u};
static constexpr ZIPITUP_Layout ZIPITUP(ZIPITUP_Layout x = CORE) noexcept {
    return {x.pass, x.yin, x.yang, x.airgap};
}
static_assert(ZIPITUP() == ZIPITUP_Layout{0u, 1u, 2u, 3u});
static_assert(ZIPITUP(ZIPITUP()) == CORE);
constexpr ZIPITUP_Layout sample{11u, 22u, 33u, 44u};
static_assert(ZIPITUP(sample) == ZIPITUP_Layout{11u, 22u, 33u, 44u});
static_assert(ZIPITUP(ZIPITUP(sample)) == sample);
static_assert(sample == ZIPITUP_Layout{11u, 22u, 33u, 44u});
