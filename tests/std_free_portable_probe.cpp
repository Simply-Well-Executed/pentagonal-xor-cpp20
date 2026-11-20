#include "pentagonal_suite.hpp"
#include "../shared/deca_sigma.hpp"
#include "../shared/tri_pi.hpp"
#include "../shared/coreao.hpp"
#include "../shared/coreao_gain.hpp"
#include "../shared/coreo_stereo.hpp"
#include "../shared/stability10d.hpp"
#include "../shared/quantum_operand_lexicon.hpp"

static_assert(swe::deca_sigma::witnesses_return(swe::deca_sigma::run(2.0)));
static_assert(swe::tri_pi::inspect(swe::deca_sigma::run(2.0)).passed());
static_assert(swe::coreao::inspect(swe::deca_sigma::run(2.0)).passed());
static_assert(swe::coreao::rf_gain::apply(1.0).passed());
static_assert(swe::stability10d::clamp01(-1.0) == 0.0);
static_assert(swe::quantum_operands::record_count == 24u);
static_assert(swe::quantum_operands::lookup_exact("+", 1u) != nullptr);
static_assert(swe::quantum_operands::lookup_exact("+=", 2u)->mode ==
              swe::quantum_operands::Mode::Gain);
static_assert(!swe::quantum_operands::evaluable(
              *swe::quantum_operands::lookup_exact("+=", 2u)));
static_assert(swe::quantum_operands::conceptual_mode_slots == 42u);
static_assert(swe::quantum_operands::lookup_mode(
                  swe::quantum_operands::Operand::ShiftUp,
                  swe::quantum_operands::Mode::Dispersion)->notation ==
              swe::quantum_operands::Notation::Inferred);
static_assert(swe::quantum_operands::mode_slot(
                  swe::quantum_operands::Operand::SemicolonEquality,
                  swe::quantum_operands::Mode::Stability).applicable);
static_assert(swe::quantum_operands::lookup_exact(";=", 2u)->status ==
              swe::quantum_operands::Status::Exploratory);
static_assert(swe::quantum_operands::lookup_exact("++", 2u)->status ==
              swe::quantum_operands::Status::InterpretedNoncanonical);
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
