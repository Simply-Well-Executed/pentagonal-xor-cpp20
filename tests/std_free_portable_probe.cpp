#include "pentagonal_suite.hpp"
#include "../shared/deca_sigma.hpp"
#include "../shared/tri_pi.hpp"
#include "../shared/euler_pi_inverse_square.hpp"
#include "../shared/sigma_form_geometry.hpp"
#include "../shared/sigma_form_area_q62.hpp"
#include "../shared/right_triangle_f123_original.hpp"
#include "../shared/f123_eighth_fold_experiment.hpp"
#include "../shared/sigma_e_redaxualchoice.hpp"
#include "../shared/sigma_f_redaxualchoice.hpp"
#include "../shared/four_pyramid_reduxualchoice.hpp"
#include "../shared/u_circle_fit.hpp"
#include "../shared/coreao.hpp"
#include "../shared/coreao_gain.hpp"
#include "../shared/coreo_stereo.hpp"
#include "../shared/stability10d.hpp"
#include "../shared/quantum_operand_lexicon.hpp"

static_assert(swe::deca_sigma::witnesses_return(swe::deca_sigma::run(2.0)));
static_assert(swe::tri_pi::inspect(swe::deca_sigma::run(2.0)).passed());
// The same exact inverse-square static asserts also compile in the STD-free path.
// Each new area lane compiles independently in the include-free core.
namespace ai = swe::sigma_form_area;
static_assert(ai::airgap_a(ai::unit,ai::unit).value==ai::unit/2ULL);
static_assert(ai::sigma_b({0ULL,0ULL},
    {ai::unit/2ULL,ai::unit},{ai::unit,0ULL}).valid);
static_assert(ai::sigma_c(ai::unit/4ULL,ai::unit).value==ai::unit/4ULL);
constexpr ai::WidthSample std_free_d[]={
    {0ULL,ai::unit/2ULL},{ai::unit,ai::unit/2ULL}
};
static_assert(ai::sigma_d(std_free_d).value==ai::unit/2ULL);
// The follow-on ordered folds keep distinct source lanes and fail closed.
constexpr ai::Q62Area std_a=ai::airgap_a(ai::unit,ai::unit);
constexpr ai::Q62Area std_b=ai::sigma_b(
    {0ULL,0ULL},{ai::unit/2ULL,ai::unit},{ai::unit,0ULL});
constexpr ai::Q62Area std_c=ai::sigma_c(ai::unit/4ULL,ai::unit);
constexpr ai::Q62Area std_d=ai::sigma_d(std_free_d);
constexpr auto std_path=ai::path_bcd(std_b,std_c,std_d,std_a);
static_assert(std_path.valid);
static_assert(ai::fold_b_into_c(ai::start_fold(std_path)).valid);
static_assert(ai::right_square_pyramid(
    3ULL*ai::unit/4ULL,ai::unit/2ULL,
    5ULL*ai::unit/8ULL).right_square_geometry_verified);

// Independently verified four-face right triangular pyramid (tetrahedron).
constexpr auto right_tri_core = ai::right_triangle_pyramid(
    ai::unit/2ULL,ai::unit/2ULL,ai::unit,3ULL*ai::unit/4ULL);
static_assert(right_tri_core.right_triangle_geometry_verified);
static_assert(right_tri_core.total.value==ai::unit);
static_assert(!ai::right_triangle_pyramid(
    ai::unit/2ULL,ai::unit/2ULL,ai::unit,
    ai::unit/2ULL).right_triangle_geometry_verified);

// Read-only C++20 folded oblique-face witness using an exact
// 90-degree rotated cross product (no transcendentals).
constexpr auto probe_f3=ai::right_triangle_f3_rotated_90(
    ai::unit/2ULL,ai::unit/2ULL,ai::unit,3ULL*ai::unit/4ULL);
static_assert(probe_f3.valid && probe_f3.area_preserved);
static_assert(probe_f3.oblique_face.value==3ULL*ai::unit/8ULL);
static_assert(swe::right_triangle_f123_original::F3_Q62 ==
              probe_f3.oblique_face.value);
static_assert(swe::right_triangle_f123_original::TOTAL_Q62 ==
              right_tri_core.total.value);
// F123 experimental 8-denominator path compiles without STL or
// any additional source dependencies beyond the original two headers.
constexpr auto eighth_probe=swe::f123_eighth_fold_experiment::calculate(probe_f3);
static_assert(eighth_probe.valid);
static_assert(eighth_probe.f2_over_2_q62==
              eighth_probe.four_times_f2_over_8_q62);
static_assert(eighth_probe.final_egress_q62==
              3ULL*ai::unit/128ULL);
static_assert(!eighth_probe.geometric_tip_verified);

// Read-only bounded finite SigmaE from frozen F123 -> Eighth Fold.
constexpr auto sigma_e_probe =
    swe::sigma_e_redaxualchoice::calculate(eighth_probe);
static_assert(sigma_e_probe.valid &&
              sigma_e_probe.all_terms_equivalent);
static_assert(sigma_e_probe.sigma_q62 ==
              21ULL*ai::unit/256ULL);
static_assert(!sigma_e_probe.geometric_tip_verified);

// Separate SigmaF carbon-copy, alongside (not over) SigmaE.
namespace sigma_f_core=swe::sigma_f_redaxualchoice;
static_assert(sigma_f_core::BASE ==
              swe::right_triangle_f123_original::BASE_Q62);
static_assert(sigma_f_core::SIGMA_F123 ==
              swe::right_triangle_f123_original::LATERAL_Q62);
static_assert(sigma_f_core::SIGMA_REDAXUALCHOICE ==
              14123288431433875456ULL);
static_assert(sigma_f_core::SIGMA_REDAXUALCHOICE !=
              sigma_e_probe.sigma_q62);
static_assert(sigma_f_core::TOTAL == ai::unit);

// FOUR-PYRAMID-REDUXUALCHOICE-PR34-008: no STL in new source.
namespace four_core=swe::four_pyramid_reduxualchoice;
static_assert(four_core::CORE.right_square_geometry_verified);
static_assert(four_core::NINE.exact_partition_verified);
static_assert(four_core::NINE.sum.value==four_core::BASE);
static_assert(four_core::TOP3.rational_geometry_verified);
static_assert(four_core::STEP_UP.exact_sample_ratio);
static_assert(four_core::SIGMA_REDUXUALCHOICE ==
              15132094747964866560ULL);
static_assert(four_core::SIGMA_REDUXUALCHOICE !=
              sigma_f_core::SIGMA_REDAXUALCHOICE);
static_assert(!four_core::missing_tip_or_fold_verified);

// U circle-fit ISO C++20 include-free lane (source arithmetic shared).
namespace uc_portable=swe::u_circle_fit;
static_assert(uc_portable::VALIDATE(uc_portable::FACE,
              uc_portable::HEIGHT_CIRCLE)==uc_portable::U::NotNice);
static_assert(uc_portable::TRIANGLE_RESULT==uc_portable::U::Nice);
static_assert(uc_portable::TESSELLATION_RESULT==uc_portable::U::Nice);
static_assert(uc_portable::ASTRIX_RESULT==uc_portable::U::Nice);
static_assert(uc_portable::SUM_NINE_DOUBLE_AREA()==7200ULL);
static_assert(uc_portable::A==600ULL && uc_portable::T==3600ULL);
static_assert(uc_portable::TOTAL==four_core::TOTAL);
static_assert(uc_portable::SIGMA==four_core::SIGMA_REDUXUALCHOICE);
static_assert(!uc_portable::PHYSICAL_FOLD_VERIFIED);

static_assert(swe::sigma_form::square_pyramid_z(
    0.0,0.0,0.0,0.0,1.0,2.0).value == 2.0);
static_assert(swe::sigma_form::inverse_square_intensity(
    1.0,0.15,1.0,swe::sigma_form::geometry_pi).valid);
static_assert(swe::inverse_square::e1.numerator == 271828182845904523ULL);
static_assert(swe::inverse_square::p1.numerator == 314159265358979323ULL);
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
