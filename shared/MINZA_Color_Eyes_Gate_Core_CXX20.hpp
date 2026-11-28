/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) Paul Allen Cheek
// MINZA color-eye reference kernel: C++20, constexpr, no image decoder.
#pragma once
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace minza::color_eyes {
struct rgba8 { std::uint8_t r, g, b, a; };
struct rgba16 { std::uint16_t r, g, b, a; };

[[nodiscard]] constexpr rgba16 expand_rgba16(rgba8 p) noexcept {
    return {static_cast<std::uint16_t>(p.r * 257u),
            static_cast<std::uint16_t>(p.g * 257u),
            static_cast<std::uint16_t>(p.b * 257u),
            static_cast<std::uint16_t>(p.a * 257u)};
}

// Big-endian lexical packing of channel fields into one unsigned 64-bit word.
[[nodiscard]] constexpr std::uint64_t pack_rgba64(rgba8 p) noexcept {
    const rgba16 q = expand_rgba16(p);
    return (std::uint64_t{q.r} << 48u) |
           (std::uint64_t{q.g} << 32u) |
           (std::uint64_t{q.b} << 16u) |
            std::uint64_t{q.a};
}
[[nodiscard]] constexpr rgba16 unpack_rgba64(std::uint64_t w) noexcept {
    return {static_cast<std::uint16_t>((w >> 48u) & 0xFFFFu),
            static_cast<std::uint16_t>((w >> 32u) & 0xFFFFu),
            static_cast<std::uint16_t>((w >> 16u) & 0xFFFFu),
            static_cast<std::uint16_t>(w & 0xFFFFu)};
}

struct gate5 {
    std::uint8_t ligand_mask{};
    bool xor_odd{};
    bool any_or{};
    bool all_and{};
};
[[nodiscard]] constexpr gate5 evaluate(std::uint8_t five_bits) noexcept {
    const auto m = static_cast<std::uint8_t>(five_bits & 0x1Fu);
    bool parity = false;
    for (unsigned k = 0; k < 5; ++k) parity ^= ((m >> k) & 1u) != 0;
    return {m, parity, m != 0, m == 0x1Fu};
}

// Each of the five operands is one edge of a five-sample temporal ring.
// This is color tolerance, not biometric identification.
[[nodiscard]] constexpr std::uint8_t edge_mask(
    const std::array<rgba8,5>& samples, std::uint8_t tolerance) noexcept {
    std::uint8_t mask = 0;
    for (unsigned i = 0; i < 5; ++i) {
        const auto& a = samples[i];
        const auto& b = samples[(i + 1u) % 5u];
        auto d = [](std::uint8_t x,std::uint8_t y) constexpr -> unsigned {
            return x >= y ? unsigned(x-y) : unsigned(y-x);
        };
        // Integer comparison avoids divisions and floating-point roundoff:
        // mean absolute RGB distance <= tolerance.
        if (d(a.r,b.r) + d(a.g,b.g) + d(a.b,b.b) <= 3u*tolerance)
            mask |= static_cast<std::uint8_t>(1u << i);
    }
    return mask;
}

// Abstract circular sample addressing; index end-to-start wrap is not a
// physical claim that the last and first video frames were adjacent.
[[nodiscard]] constexpr std::array<std::size_t,5> window5(
    std::size_t index, std::size_t count) noexcept {
    if (count == 0) return {};
    const std::size_t i = index % count;
    // Avoid unsigned overflow, including when count == SIZE_MAX.
    const std::size_t before1 = i == 0u ? count - 1u : i - 1u;
    const std::size_t before2 = before1 == 0u ? count - 1u : before1 - 1u;
    const std::size_t after1 = i == count - 1u ? 0u : i + 1u;
    const std::size_t after2 = after1 == count - 1u ? 0u : after1 + 1u;
    return {{before2, before1, i, after1, after2}};
}

using femtoseconds = std::chrono::duration<std::int64_t, std::femto>;
struct sample_interval {
    femtoseconds begin{};
    femtoseconds end{};
    [[nodiscard]] constexpr bool contains(femtoseconds t) const noexcept {
        return begin <= t && t < end;
    }
};
// Frame-period representation is rational: 1/60 second, not an integer
// number of femtoseconds. Use observed frame indices for source evidence.
inline constexpr std::int64_t femtoseconds_per_second = 1'000'000'000'000'000LL;

static_assert(pack_rgba64({255,0,0,255}) == 0xFFFF00000000FFFFull);
static_assert(unpack_rgba64(0xFFFF00000000FFFFull).r == 0xFFFFu);
static_assert(evaluate(0).any_or == false);
static_assert(evaluate(1).xor_odd == true);
static_assert(evaluate(3).xor_odd == false);
static_assert(evaluate(31).all_and == true);
static_assert(evaluate(31).xor_odd == true);
static_assert(window5(0,67) == std::array<std::size_t,5>{65,66,0,1,2});
static_assert(sample_interval{femtoseconds{146'000'000'000'000'000LL},
  femtoseconds{179'000'000'000'000'000LL}}.contains(femtoseconds{153'000'000'000'000'001LL}));
} // namespace minza::color_eyes
