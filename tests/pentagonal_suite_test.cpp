#include "pentagonal_suite.hpp"

static_assert(pxor::pentagonal_nan_bits(7) ==
              (0x7FF8000000000000ull |
               (pxor::pentagonal(7) & 0x0007FFFFFFFFFFFFull)));
static_assert(pxor::extract_nan_payload(pxor::pentagonal_nan_bits(7)) ==
              (pxor::pentagonal(7) & 0x0007FFFFFFFFFFFFull));
static_assert(pxor::is_pentagonal_nan(pxor::pentagonal_nan_bits(7)));
static_assert(!pxor::is_pentagonal_nan(0x7FF0000000000000ull));
static_assert(!pxor::is_pentagonal_nan(0x7FF0000000000001ull));

constexpr bool test_exact_modes() {
    pxor::u8 x[17]{};
    pxor::u8 o[17]{};
    pxor::u8 a[17]{};
    pxor::u8 n[17]{};
    pxor::u8 untouched[17]{};
    pxor::u8 scrubbed[17]{};
    for (pxor::usize i = 0; i < 17; ++i) {
        x[i] = o[i] = a[i] = n[i] = untouched[i] = scrubbed[i] =
            static_cast<pxor::u8>(0x35u + i);
    }
    pxor::apply_pentagonal_xor({x, 17}, 7);
    pxor::apply_pentagonal_or({o, 17}, 7);
    pxor::apply_pentagonal_and({a, 17}, 7);
    pxor::apply_pentagonal_nand({n, 17}, 7);
    pxor::apply_pentagonal_nop({untouched, 17}, 7);
    pxor::apply_pentagonal_nil({scrubbed, 17}, 7);
    for (pxor::usize i = 0; i < 17; ++i) {
        const pxor::u8 b = static_cast<pxor::u8>(0x35u + i);
        const pxor::u8 key = pxor::pentagonal_key_byte(7 + i);
        if (x[i] != static_cast<pxor::u8>(b ^ key) ||
            o[i] != static_cast<pxor::u8>(b | key) ||
            a[i] != static_cast<pxor::u8>(b & key) ||
            n[i] != static_cast<pxor::u8>(~(b & key)) ||
            untouched[i] != b ||
            scrubbed[i] != 0) return false;
    }
    pxor::apply_pentagonal_xor({x, 17}, 7);
    for (pxor::usize i = 0; i < 17; ++i) {
        if (x[i] != static_cast<pxor::u8>(0x35u + i)) return false;
    }
    return true;
}
static_assert(test_exact_modes());

constexpr bool test_cor_vista() {
    pxor::PentagonalCor cor{1, 4};
    if (cor.done() || cor.remaining() != 4) return false;
    pxor::u64 out = 999;
    const pxor::u64 expected[]{1, 5, 12, 22};
    for (const pxor::u64 value : expected) {
        if (!cor.try_next(out) || out != value) return false;
    }
    if (!cor.done() || cor.remaining() != 0) return false;
    if (cor.try_next(out) || out != 22) return false;
    cor.reset(4, 2);
    if (cor.next() != 22 || cor.next() != 35 || !cor.done()) return false;
    pxor::PentagonalCor empty{7, 0};
    if (!empty.done() || empty.try_next(out)) return false;
    pxor::PentagonalCor final{~pxor::u64{0}, 2};
    if (!final.try_next(out) || !final.done()) return false;
    if (final.try_next(out)) return false;

    constexpr pxor::PentagonalVista vista{1, 4};
    if (vista.size() != 4 ||
        vista[0] != 1 || vista[1] != 5 ||
        vista[2] != 12 || vista[3] != 22) return false;
    auto sub = vista.subvista(1, 9);
    if (sub.size() != 3 || sub[0] != 5 || sub[2] != 22) return false;
    auto none = vista.subvista(100, 5);
    if (none.size() != 0) return false;
    pxor::u64 expected_iter[]{1, 5, 12, 22};
    pxor::usize index = 0;
    for (const auto value : vista) {
        if (index >= 4 || value != expected_iter[index]) return false;
        ++index;
    }
    if (index != 4) return false;
    constexpr pxor::PentagonalVista edge{~pxor::u64{0} - 1, 4};
    if (edge.size() != 2 || edge.subvista(1, 5).size() != 1) {
        return false;
    }
    pxor::usize steps = 0;
    for (const auto value : edge) {
        (void)value;
        ++steps;
    }
    return steps == 2;
}
static_assert(test_cor_vista());

constexpr bool test_nan_tail() {
    pxor::u8 bytes[19]{};
    pxor::apply_pentagonal_nan({bytes, 19}, 7);
    if (pxor::pack_u64_le(bytes) != pxor::pentagonal_nan_bits(7)) {
        return false;
    }
    if (pxor::pack_u64_le(bytes + 8) != pxor::pentagonal_nan_bits(8)) {
        return false;
    }
    return bytes[16] == 255 && bytes[17] == 255 && bytes[18] == 255;
}
static_assert(test_nan_tail());

template <pxor::Op Mode>
bool test_fast_mode(pxor::usize len) {
    pxor::u8 raw[4100]{};
    for (pxor::usize i = 0; i < 4100; ++i) raw[i] = 0xA5;
    const pxor::ByteSlice slice{raw + 1, len};
    pxor::fast_op_buffer<Mode>(slice);
    for (pxor::usize i = 0; i < len; ++i) {
        const pxor::u8 key = pxor::PENTAGONAL_LUT[i & 255u];
        pxor::u8 expected = 0;
        if constexpr (Mode == pxor::Op::Xor) {
            expected = static_cast<pxor::u8>(0xA5u ^ key);
        } else if constexpr (Mode == pxor::Op::Or) {
            expected = static_cast<pxor::u8>(0xA5u | key);
        } else if constexpr (Mode == pxor::Op::And) {
            expected = static_cast<pxor::u8>(0xA5u & key);
        } else if constexpr (Mode == pxor::Op::Nand) {
            expected = static_cast<pxor::u8>(~(0xA5u & key));
        } else if constexpr (Mode == pxor::Op::Nop) {
            expected = 0xA5u;
        } else {
            expected = 0;
        }
        if (slice[i] != expected) return false;
    }
    return raw[0] == 0xA5u &&
           raw[len + 1] == 0xA5u &&
           raw[len + 2] == 0xA5u;
}

int main() {
    constexpr pxor::usize lengths[]{
        0, 1, 2, 7, 8, 9, 15, 16, 17, 248, 255, 256, 257, 4097
    };
    for (const pxor::usize length : lengths) {
        if (!test_fast_mode<pxor::Op::Xor>(length) ||
            !test_fast_mode<pxor::Op::Or>(length) ||
            !test_fast_mode<pxor::Op::And>(length) ||
            !test_fast_mode<pxor::Op::Nand>(length) ||
            !test_fast_mode<pxor::Op::Nop>(length) ||
            !test_fast_mode<pxor::Op::Nil>(length)) return 1;
    }
    pxor::u8 nan[18]{};
    nan[17] = 0x5A;
    pxor::apply_pentagonal_nan({nan, 17}, 1);
    if (pxor::pack_u64_le(nan) != pxor::pentagonal_nan_bits(1) ||
        pxor::pack_u64_le(nan + 8) != pxor::pentagonal_nan_bits(2) ||
        nan[16] != 255 || nan[17] != 0x5A) return 2;

    pxor::write_literal("PASS: PENTAGONAL SUITE exact/periodic modes, ");
    pxor::write_literal("NAN, COR/VISTA overflow, 14 SWAR sizes\n");
    return 0;
}
