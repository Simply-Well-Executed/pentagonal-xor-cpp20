// SPDX-License-Identifier: 0BSD
// GENSHIN independent ZIPITUP: Identity via explicit aggregate copy.
// Layout payloads are occurrence IDs, never authoritative audit verdicts.
// Standalone translation unit; no includes or shared ZIPIT dependency.
namespace genshin_zipitup {
struct Layout {
    unsigned pass, yin, yang, airgap;
    constexpr bool operator==(const Layout&) const = default;
};
inline constexpr Layout CORE{0u, 1u, 2u, 3u};
constexpr Layout ZIPITUP(Layout x = CORE) noexcept {
    return {x.pass, x.yin, x.yang, x.airgap};
}
static_assert(ZIPITUP() == Layout{0u, 1u, 2u, 3u});
static_assert(ZIPITUP(ZIPITUP()) == CORE);
constexpr Layout sample{11u, 22u, 33u, 44u};
static_assert(ZIPITUP(sample) == Layout{11u, 22u, 33u, 44u});
static_assert(ZIPITUP(ZIPITUP(sample)) == sample);
static_assert(sample == Layout{11u, 22u, 33u, 44u});
} // namespace genshin_zipitup
