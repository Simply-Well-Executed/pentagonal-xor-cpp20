// SPDX-License-Identifier: 0BSD
// GENSHIN independent ZIPITOUT: Read-only indexed gather with a fixed permutation.
// ZIPITOUT_Layout payloads are occurrence IDs, never authoritative audit verdicts.
// Standalone translation unit; no includes or shared ZIPIT dependency.
struct ZIPITOUT_Layout {
    unsigned pass, yin, yang, airgap;
    constexpr bool operator==(const ZIPITOUT_Layout&) const = default;
};
constexpr ZIPITOUT_Layout CORE{0u, 1u, 2u, 3u};
static constexpr ZIPITOUT_Layout ZIPITOUT(ZIPITOUT_Layout x = CORE) noexcept {
    const unsigned lane[4] = {
        x.pass, x.yin, x.yang, x.airgap
    };
    constexpr unsigned route[4] = {2u, 1u, 0u, 3u};
    return {lane[route[0]], lane[route[1]],
            lane[route[2]], lane[route[3]]};
}
static_assert(ZIPITOUT() == ZIPITOUT_Layout{2u, 1u, 0u, 3u});
static_assert(ZIPITOUT(ZIPITOUT()) == CORE);
constexpr ZIPITOUT_Layout sample{11u, 22u, 33u, 44u};
static_assert(ZIPITOUT(sample) == ZIPITOUT_Layout{33u, 22u, 11u, 44u});
static_assert(ZIPITOUT(ZIPITOUT(sample)) == sample);
static_assert(sample == ZIPITOUT_Layout{11u, 22u, 33u, 44u});
