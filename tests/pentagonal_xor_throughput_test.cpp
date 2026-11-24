// SPDX-License-Identifier: 0BSD
// Strict ISO C++20 core language. No C or C++ standard library.
// Tests compile-time storage, SWAR, public/private/shared access scopes.
#include "pentagonal_xor.hpp"

static_assert(pxor::pentagonal(1) == 1);
static_assert(pxor::pentagonal(2) == 5);
static_assert(pxor::pentagonal(3) == 12);
static_assert(pxor::pentagonal(10) == 145);
static_assert(pxor::fast_wrap<256>(256) == 0);
static_assert(pxor::fast_wrap<256>(263) == 7);
static_assert(pxor::fixed_mul(98304, 131072) == 196608);
static_assert(pxor::fixed_mul(-98304, 131072) == -196608);
static_assert(sizeof(pxor::u8) == 1 && sizeof(pxor::u64) == 8);
static_assert(pxor::same_type<
    decltype(pxor::ByteSlice{}[0]), pxor::u8&>);
static_assert(pxor::same_type<
    decltype(pxor::ConstByteSlice{}[0]), const pxor::u8&>);

constexpr bool verify_explicit_le() {
    pxor::u8 bytes[8]{1,2,3,4,5,6,7,8};
    if (pxor::pack_u64_le(bytes) != 0x0807060504030201ull) return false;
    pxor::u8 inverse[8]{};
    pxor::unpack_u64_le(inverse, pxor::pack_u64_le(bytes));
    for (pxor::usize i = 0; i < 8; ++i) {
        if (inverse[i] != bytes[i]) return false;
    }
    return true;
}
static_assert(verify_explicit_le());

constexpr bool verify_constexpr_swar() {
    pxor::FixedBuffer<pxor::u8, 17> data{};
    data.fill(0x5A);
    pxor::fast_xor_buffer(pxor::ByteSlice{data});
    for (pxor::usize i = 0; i < data.size(); ++i) {
        if (data[i] != static_cast<pxor::u8>(
            0x5A ^ pxor::PENTAGONAL_LUT[i])) return false;
    }
    pxor::fast_xor_buffer(pxor::ByteSlice{data});
    for (pxor::usize i = 0; i < data.size(); ++i) {
        if (data[i] != 0x5A) return false;
    }
    return true;
}
static_assert(verify_constexpr_swar());

constexpr pxor::ObfuscatedData literal("IDKFA", 42);
static_assert(pxor::EngineAuditor::first_cipher_byte(literal) ==
    static_cast<pxor::u8>('I' ^ static_cast<pxor::u8>(
       (pxor::pentagonal(42) ^ (pxor::pentagonal(42) >> 8) ^
        (pxor::pentagonal(42) >> 16)) & 255u)));

int main() {
    constexpr pxor::usize lengths[]{
        0,1,2,7,8,9,15,16,17,248,255,256,257,4097
    };
    // 4097+3 contiguous bytes, one-byte offset tests unaligned input.
    pxor::u8 backing[4100]{};
    for (const pxor::usize count : lengths) {
        for (pxor::usize i = 0; i < 4100; ++i) backing[i] = 0x5A;
        const pxor::ByteSlice view{backing + 1, count};
        pxor::fast_xor_buffer(view);
        for (pxor::usize i = 0; i < count; ++i) {
            if (view[i] != static_cast<pxor::u8>(
                0x5A ^ pxor::PENTAGONAL_LUT[i & 255u])) return 1;
        }
        if (backing[0] != 0x5A || backing[count + 1] != 0x5A ||
            backing[count + 2] != 0x5A) return 2;
        pxor::fast_xor_buffer(view);
        for (pxor::usize i = 0; i < count; ++i) {
            if (view[i] != 0x5A) return 3;
        }
    }

    pxor::u8 exact[257]{};
    pxor::u8 periodic[257]{};
    pxor::apply_pentagonal_xor(pxor::ByteSlice{exact, 257}, 1);
    pxor::fast_xor_buffer(pxor::ByteSlice{periodic, 257});
    for (pxor::usize i = 0; i < 256; ++i) {
        if (exact[i] != periodic[i]) return 4;
    }
    if (exact[256] == periodic[256]) return 5;
    pxor::apply_pentagonal_xor(pxor::ByteSlice{exact, 257}, 1);
    for (const pxor::u8 value : exact) {
        if (value != 0) return 6;
    }

    auto cursor = pxor::pentagonal_numbers().begin();
    if (*cursor++ != 1 || *cursor++ != 5 || *cursor++ != 12) return 7;

    pxor::FixedBuffer<pxor::u8, 9> owned{};
    owned.fill(0x5A);
    const pxor::ByteSlice mutable_view{owned};
    pxor::fast_xor_buffer(mutable_view);
    const pxor::ConstByteSlice borrowed{owned};
    for (pxor::usize i = 0; i < borrowed.size(); ++i) {
        if (borrowed[i] != static_cast<pxor::u8>(
            0x5A ^ pxor::PENTAGONAL_LUT[i])) return 8;
    }
    pxor::fast_xor_buffer(mutable_view);
    for (pxor::usize i = 0; i < owned.size(); ++i) {
        if (owned[i] != 0x5A) return 9;
    }

    bool callback_ok = false;
    constexpr pxor::ObfuscatedData sample("PUBLIC/PRIVATE/SHARED", 9);
    sample.with_decrypted([&](pxor::StringSlice view) {
        callback_ok = view.size() == 21 &&
            view.data()[0] == 'P' &&
            view.data()[7] == 'P' &&
            view.data()[15] == 'S';
    });
    if (!callback_ok) return 10;
    pxor::write_literal("PASS: STD-free 14 SWAR sizes, constexpr LE, LUT wrap, ");
    pxor::write_literal("scoped literal and borrowed memory views\n");
    return 0;
}
