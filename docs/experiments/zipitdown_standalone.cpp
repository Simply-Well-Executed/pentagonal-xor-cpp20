// SPDX-License-Identifier: 0BSD
// GENSHIN independent ZIPITDOWN: Three-step unsigned XOR exchange of separate local fields.
// ZIPITDOWN_Layout payloads are occurrence IDs, never authoritative audit verdicts.
// Standalone translation unit; no includes or shared ZIPIT dependency.
struct ZIPITDOWN_Layout {
    unsigned pass, yin, yang, airgap;
    constexpr bool operator==(const ZIPITDOWN_Layout&) const = default;
};
constexpr ZIPITDOWN_Layout CORE{0u, 1u, 2u, 3u};
static constexpr ZIPITDOWN_Layout ZIPITDOWN(ZIPITDOWN_Layout x = CORE) noexcept {
    x.pass ^= x.airgap;
    x.airgap ^= x.pass;
    x.pass ^= x.airgap;
    return x;
}
static_assert(ZIPITDOWN() == ZIPITDOWN_Layout{3u, 1u, 2u, 0u});
static_assert(ZIPITDOWN(ZIPITDOWN()) == CORE);
constexpr ZIPITDOWN_Layout sample{11u, 22u, 33u, 44u};
static_assert(ZIPITDOWN(sample) == ZIPITDOWN_Layout{44u, 22u, 33u, 11u});
static_assert(ZIPITDOWN(ZIPITDOWN(sample)) == sample);
static_assert(sample == ZIPITDOWN_Layout{11u, 22u, 33u, 44u});
