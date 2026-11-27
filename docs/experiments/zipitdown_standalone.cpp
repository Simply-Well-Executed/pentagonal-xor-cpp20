// SPDX-License-Identifier: 0BSD
// GENSHIN independent ZIPITDOWN: Three-step unsigned XOR exchange of separate local fields.
// Layout payloads are occurrence IDs, never authoritative audit verdicts.
// Standalone translation unit; no includes or shared ZIPIT dependency.
namespace genshin_zipitdown {
struct Layout {
    unsigned pass, yin, yang, airgap;
    constexpr bool operator==(const Layout&) const = default;
};
inline constexpr Layout CORE{0u, 1u, 2u, 3u};
constexpr Layout ZIPITDOWN(Layout x = CORE) noexcept {
    x.pass ^= x.airgap;
    x.airgap ^= x.pass;
    x.pass ^= x.airgap;
    return x;
}
static_assert(ZIPITDOWN() == Layout{3u, 1u, 2u, 0u});
static_assert(ZIPITDOWN(ZIPITDOWN()) == CORE);
constexpr Layout sample{11u, 22u, 33u, 44u};
static_assert(ZIPITDOWN(sample) == Layout{44u, 22u, 33u, 11u});
static_assert(ZIPITDOWN(ZIPITDOWN(sample)) == sample);
static_assert(sample == Layout{11u, 22u, 33u, 44u});
} // namespace genshin_zipitdown
