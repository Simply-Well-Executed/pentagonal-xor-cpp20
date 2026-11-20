// SPDX-License-Identifier: 0BSD
// End-to-end verification of the independent periodic XOR engine.
#include "pentagonal_xor.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <span>
#include <vector>

static_assert(pxor::pentagonal(std::uint64_t{1}) == 1);
static_assert(pxor::pentagonal(std::uint64_t{2}) == 5);
static_assert(pxor::pentagonal(std::uint64_t{3}) == 12);
static_assert(pxor::pentagonal(std::uint64_t{10}) == 145);
static_assert(pxor::fast_wrap<256>(256) == 0);
static_assert(pxor::fast_wrap<256>(263) == 7);
static_assert(pxor::fixed_mul(98304, 131072) == 196608);
static_assert(pxor::fixed_mul(-98304, 131072) == -196608);

constexpr pxor::ObfuscatedData test_literal("IDKFA", 42);
constexpr auto test_plain = test_literal.decrypt(42);
static_assert(test_plain[0] == 'I' && test_plain[1] == 'D' &&
              test_plain[2] == 'K' && test_plain[3] == 'F' &&
              test_plain[4] == 'A' && test_plain[5] == '\0');

int main() {
    // Covers short tails, 8-byte SWAR, unaligned spans and LUT wrap.
    constexpr std::array<std::size_t, 14> sizes{
        0, 1, 2, 7, 8, 9, 15, 16, 17, 248, 255, 256, 257, 4097
    };
    for (std::size_t len : sizes) {
        std::vector<std::byte> storage(len + 3, std::byte{0x5A});
        std::span<std::byte> payload{storage.data() + 1, len};
        pxor::fast_xor_buffer(payload);
        for (std::size_t i = 0; i < len; ++i) {
            const auto expected =
                std::byte{0x5A} ^ pxor::PENTAGONAL_LUT[i & 255u];
            if (payload[i] != expected) return 1;
        }
        if (storage[0] != std::byte{0x5A} ||
            storage[len + 1] != std::byte{0x5A} ||
            storage[len + 2] != std::byte{0x5A}) return 2;
        pxor::fast_xor_buffer(payload);
        for (std::byte byte : payload) {
            if (byte != std::byte{0x5A}) return 3;
        }
    }

    // The first 256 bytes of the periodic LUT agree with seeded n=1;
    // after the wrap the streams intentionally diverge.
    std::array<std::byte, 257> exact{};
    std::array<std::byte, 257> periodic{};
    pxor::apply_pentagonal_xor(exact, 1);
    pxor::fast_xor_buffer(periodic);
    for (std::size_t i = 0; i < 256; ++i) {
        if (exact[i] != periodic[i]) return 4;
    }
    if (exact[256] == periodic[256]) return 5;

    pxor::apply_pentagonal_xor(exact, 1);
    for (std::byte b : exact) {
        if (b != std::byte{0}) return 6;
    }

    // C++20 lazy sequence is bounded by the consumer, not generated eagerly.
    auto numbers = pxor::pentagonal_numbers();
    auto cursor = numbers.begin();
    if (*cursor++ != 1 || *cursor++ != 5 || *cursor++ != 12) return 7;

    std::puts("PASS: 14 sizes, misalignment, roundtrip, LUT wrap, "
              "constexpr obfuscation and pentagonal sequence");
    return 0;
}
