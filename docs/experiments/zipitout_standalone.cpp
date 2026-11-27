// SPDX-License-Identifier: 0BSD
// GENSHIN independent ZIPITOUT: Read-only indexed gather with a fixed permutation.
// Layout payloads are occurrence IDs, never authoritative audit verdicts.
// Standalone translation unit; no includes or shared ZIPIT dependency.
namespace genshin_zipitout {
struct Layout {
    unsigned pass, yin, yang, airgap;
    constexpr bool operator==(const Layout&) const = default;
};
inline constexpr Layout CORE{0u, 1u, 2u, 3u};
constexpr Layout ZIPITOUT(Layout x = CORE) noexcept {
    const unsigned lane[4] = {
        x.pass, x.yin, x.yang, x.airgap
    };
    constexpr unsigned route[4] = {2u, 1u, 0u, 3u};
    return {lane[route[0]], lane[route[1]],
            lane[route[2]], lane[route[3]]};
}
static_assert(ZIPITOUT() == Layout{2u, 1u, 0u, 3u});
static_assert(ZIPITOUT(ZIPITOUT()) == CORE);
constexpr Layout sample{11u, 22u, 33u, 44u};
static_assert(ZIPITOUT(sample) == Layout{33u, 22u, 11u, 44u});
static_assert(ZIPITOUT(ZIPITOUT(sample)) == sample);
static_assert(sample == Layout{11u, 22u, 33u, 44u});
} // namespace genshin_zipitout
