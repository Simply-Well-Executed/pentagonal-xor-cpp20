#include "pentagonal_suite.hpp"

static_assert(sizeof(pxor::u8) == 1);
static_assert(sizeof(pxor::u64) == 8);
static_assert(pxor::pentagonal(1) == 1);
static_assert(pxor::pentagonal(2) == 5);
static_assert(pxor::pentagonal(3) == 12);
static_assert(pxor::fast_wrap<256>(257) == 1);
static_assert(pxor::pack_u64_le(pxor::PENTAGONAL_LUT.data()) != 0);
static_assert(pxor::is_pentagonal_nan(pxor::pentagonal_nan_bits(7)));

constexpr bool native_suite_constant_contract() {
    pxor::u8 data[17]{};
    for (pxor::usize i = 0; i < 17; ++i) data[i] = 0x5A;
    pxor::fast_xor_buffer({data, 17});
    pxor::fast_xor_buffer({data, 17});
    for (pxor::usize i = 0; i < 17; ++i) {
        if (data[i] != 0x5A) return false;
    }
    pxor::apply_pentagonal_nil({data, 17});
    for (pxor::usize i = 0; i < 17; ++i) {
        if (data[i] != 0) return false;
    }
    pxor::PentagonalCor cor{1, 3};
    pxor::u64 found = 0;
    if (!cor.try_next(found) || found != 1) return false;
    if (!cor.try_next(found) || found != 5) return false;
    if (!cor.try_next(found) || found != 12) return false;
    if (cor.try_next(found) || !cor.done()) return false;
    constexpr pxor::PentagonalVista v{1, 3};
    if (v.size() != 3 || v[2] != 12) return false;
    return true;
}
static_assert(native_suite_constant_contract());

extern "C" pxor::u64 swe_std_free_native_core_probe(pxor::u64 seed) noexcept {
    pxor::u8 buffer[16]{};
    for (pxor::usize i = 0; i < 16; ++i) {
        buffer[i] = static_cast<pxor::u8>(i);
    }
    pxor::apply_pentagonal_xor({buffer, 16}, seed);
    pxor::fast_or_buffer({buffer, 16});
    pxor::fast_and_buffer({buffer, 16});
    pxor::fast_nand_buffer({buffer, 16});
    pxor::u64 witness = 0;
    for (pxor::usize i = 0; i < 16; ++i) {
        witness = witness * 131u + buffer[i];
    }
    return witness;
}
