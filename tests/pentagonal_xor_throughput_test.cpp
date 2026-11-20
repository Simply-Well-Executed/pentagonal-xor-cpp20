// SPDX-License-Identifier: 0BSD
// Optional periodic XOR engine's isolated C++20 behavior and memory-scope tests.
#include "pentagonal_xor.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

static_assert(pxor::pentagonal(std::uint64_t{1}) == 1);
static_assert(pxor::pentagonal(std::uint64_t{2}) == 5);
static_assert(pxor::pentagonal(std::uint64_t{3}) == 12);
static_assert(pxor::pentagonal(std::uint64_t{10}) == 145);
static_assert(pxor::fast_wrap<256>(256) == 0);
static_assert(pxor::fast_wrap<256>(263) == 7);
static_assert(pxor::fixed_mul(98304, 131072) == 196608);
static_assert(pxor::fixed_mul(-98304, 131072) == -196608);

// Compile-time access-boundary tests: public mutable, shared read-only,
// and private auditor token. This does NOT imply OS-enforced memory secrecy.
static_assert(std::is_same_v<
    decltype(std::declval<pxor::ByteSlice>()[0]), pxor::u8&>);
static_assert(std::is_same_v<
    decltype(std::declval<pxor::ConstByteSlice>()[0]), const pxor::u8&>);
static_assert(!std::is_default_constructible_v<pxor::EngineKey>);
static_assert(sizeof(pxor::u64) == 8);
static_assert(sizeof(pxor::u8) == 1);

// The constant-evaluation branch and constexpr scoped destructor must compile
// under ISO C++20 (no "if consteval" and no compiler-specific builtin).
constexpr bool test_constexpr_scoped_plaintext() {
    constexpr pxor::ObfuscatedData literal("IDKFA", 42);
    bool correct = false;
    literal.with_decrypted([&](pxor::StringSlice view) {
        correct = view.size() == 5 &&
                  view[0] == 'I' && view.data()[1] == 'D' &&
                  view.data()[2] == 'K' && view.data()[3] == 'F' &&
                  view.data()[4] == 'A';
    });
    return correct;
}
static_assert(test_constexpr_scoped_plaintext());

int main() {
    // Covers short tails, 8-byte SWAR, unaligned spans and LUT wrap.
    constexpr std::array<std::size_t, 14> sizes{
        0, 1, 2, 7, 8, 9, 15, 16, 17, 248, 255, 256, 257, 4097
    };
    for (std::size_t len : sizes) {
        std::vector<std::byte> storage(len + 3, std::byte{0x5A});
        std::span<std::byte> payload{storage.data() + 1, len};
        pxor::fast_xor_buffer(payload); // former public facade still works
        for (std::size_t i = 0; i < len; ++i) {
            const auto expected =
                std::byte{0x5A} ^
                static_cast<std::byte>(pxor::PENTAGONAL_LUT[i & 255u]);
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

    // The first 256 periodic bytes agree with the seeded stream for n=1;
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

    auto sequence = pxor::pentagonal_numbers();
    auto cursor = sequence.begin();
    if (*cursor++ != 1 || *cursor++ != 5 || *cursor++ != 12) return 7;

    // Scoped public ownership, writable slice and read-only borrow.
    pxor::FixedBuffer<pxor::u8, 9> owned{};
    owned.fill(0x5A);
    const pxor::ByteSlice mutable_view{owned};
    pxor::fast_xor_buffer(mutable_view);
    const pxor::ConstByteSlice borrowed{owned};
    for (std::size_t i = 0; i < borrowed.size(); ++i) {
        if (borrowed[i] != static_cast<pxor::u8>(
                0x5A ^ pxor::PENTAGONAL_LUT[i])) return 8;
    }
    pxor::fast_xor_buffer(mutable_view);
    for (std::size_t i = 0; i < owned.size(); ++i) {
        if (owned[i] != 0x5A) return 9;
    }

    // Runtime callback receives no owning plaintext container and returns void.
    const pxor::ObfuscatedData text("PUBLIC/PRIVATE/SHARED", 9);
    bool callback_ok = false;
    text.with_decrypted([&](pxor::StringSlice view) {
        callback_ok = view.size() == 21 &&
                      view.data()[0] == 'P' &&
                      view.data()[7] == 'P' &&
                      view.data()[15] == 'S';
    });
    if (!callback_ok) return 10;
    constexpr pxor::ObfuscatedData literal("IDKFA", 42);
    if (pxor::EngineAuditor::first_cipher_byte(literal) !=
        static_cast<pxor::u8>('I' ^ static_cast<unsigned char>(
            (pxor::pentagonal(pxor::u64{42}) ^
             (pxor::pentagonal(pxor::u64{42}) >> 8) ^
             (pxor::pentagonal(pxor::u64{42}) >> 16)) & 0xFFu))) return 11;

    std::puts("PASS: 14 SWAR sizes, unaligned spans, roundtrip, LUT wrap, "
              "constexpr scoped plaintext, and memory-view access scopes");
    return 0;
}
