// SPDX-License-Identifier: 0BSD
// Compile and execute with native MSVC C++20 /O2, not clang-cl.
#include "../shared/toolong_q62.hpp"
#include "../shared/sigma_form_geometry.hpp"
#include "../shared/sigma_form_area_q62.hpp"
#include "../shared/right_triangle_f123_original.hpp"
#include "../shared/f123_eighth_fold_experiment.hpp"
#include "../shared/sigma_e_redaxualchoice.hpp"
#include "../shared/sigma_f_redaxualchoice.hpp"
#include "../shared/four_pyramid_reduxualchoice.hpp"
#include "../shared/u_circle_fit.hpp"
#include "../shared/xdr_rdx_fold_tork.hpp"
#include "../shared/minza_direct_ping_bidi.hpp"
#include "../shared/minza_direct_ping_tridi.hpp"
#include "../shared/three_geometric_folds.hpp"
#include "../shared/rectangloid_primitive_cxx20.hpp"
using namespace swe::toolong_q62;

static_assert(sigma_sigma_sigma());
static_assert(circle_of_sanity::centrix.all_consistent);
static_assert(!circle_of_sanity::centrix.release_authorized);
static_assert(euler.expected_integer % 100ULL == 23ULL);
static_assert(pi.expected_integer % 100ULL == 23ULL);
static_assert(intensity.expected_integer % 100ULL == 23ULL);
static_assert(!externally_authenticated_approval);
// These inputs are synthetic unit-test examples, not inferred image data.
namespace fg = swe::sigma_form;
static_assert(fg::square_pyramid_z(0,0,0,0,1,2).value == 2.0);
static_assert(fg::square_pyramid_z(1,0,0,0,1,2).value == 0.0);
static_assert(!fg::square_pyramid_z(2,0,0,0,1,2).valid);
static_assert(!fg::square_pyramid_z(0,0,0,0,0,2).valid);
constexpr auto veil_mid = fg::quadratic_bezier(
    {0.0,0.0},{1.0,2.0},{2.0,0.0},0.5);
static_assert(veil_mid.valid && veil_mid.value.x == 1.0 &&
              veil_mid.value.y == 1.0);
static_assert(!fg::quadratic_bezier(
    {0.0,0.0},{1.0,2.0},{2.0,0.0},1.5).valid);
constexpr auto pleat = fg::pleat_x(0.0,1.0,0.25,1.0,0.0);
static_assert(pleat.valid && pleat.value == 1.0);
constexpr auto billow = fg::damped_wave(
    0.0,0.0,2.0,0.15,1.0,1.0,fg::geometry_pi*0.5);
static_assert(billow.valid && fg::near(billow.value,2.0,1e-12));
static_assert(!fg::damped_wave(-1.0,0.0,2.0,0.15,1.0,1.0,0.0).valid);
constexpr auto arm = fg::two_link_arm(
    {0.0,0.0},1.0,1.0,0.0,fg::geometry_pi*0.5);
static_assert(arm.valid && fg::near(arm.elbow.x,1.0,1e-12) &&
              fg::near(arm.elbow.y,0.0,1e-12) &&
              fg::near(arm.hand.x,1.0,1e-12) &&
              fg::near(arm.hand.y,1.0,1e-12));
constexpr auto diffuse = fg::lambert(
    0.1,0.4,2.0,{0.0,0.0,1.0},{0.0,0.0,1.0});
static_assert(diffuse.valid && fg::near(diffuse.value,0.9,1e-12));
constexpr auto shadow = fg::lambert(
    0.1,0.4,2.0,{0.0,0.0,1.0},{0.0,0.0,-1.0});
static_assert(shadow.valid && fg::near(shadow.value,0.1,1e-12));
static_assert(!fg::lambert(
    0.1,0.4,2.0,{0.0,0.0,2.0},{0.0,0.0,1.0}).valid);
constexpr auto projected = fg::pinhole_project(
    {1.0,2.0,4.0},{2.0,2.0,0.0,0.0});
static_assert(projected.valid && projected.value.x==0.5 &&
              projected.value.y==1.0);
static_assert(!fg::pinhole_project(
    {1.0,2.0,0.0},{2.0,2.0,0.0,0.0}).valid);
constexpr auto inverse_sample = fg::inverse_square_intensity(
    1.0,0.15,1.0,fg::geometry_pi);
static_assert(inverse_sample.valid);
static_assert(fg::near(inverse_sample.value,
    static_cast<double>(intensity.expected_integer)/1e17,1e-14));
static_assert(!fg::inverse_square_intensity(
    1.0,0.15,0.0,fg::geometry_pi).valid);
static_assert(!fg::attenuation(-0.01).valid);
static_assert(!fg::sine(65.0).valid);
// Exact wide-integer independent witness when compiler supports it.
// MSVC continues to use canonical, include-free four-limb Q62 arithmetic.
#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
constexpr unsigned long long sigma_form_wide_q62(
    unsigned long long word) noexcept {
    return static_cast<unsigned long long>(
        (static_cast<__uint128_t>(word) *
         static_cast<__uint128_t>(multiplier)) >> 62u);
}
static_assert(sigma_form_wide_q62(euler.source_word)==euler.expected_integer);
static_assert(sigma_form_wide_q62(pi.source_word)==pi.expected_integer);
static_assert(sigma_form_wide_q62(intensity.source_word)==intensity.expected_integer);
#endif

// SIGMA-AREA-BCD-AIRGAP-001: independently declared normalized inputs.
// Never call these synthetic unit-square tests measurements of the images.
namespace fa = swe::sigma_form_area;
constexpr fa::u64 Q = fa::unit;
constexpr fa::Q62Area airgap_area = fa::airgap_a(Q,Q);
constexpr fa::Q62Area sigma_b_area =
    fa::sigma_b({0ULL,0ULL},{Q/2ULL,Q},{Q,0ULL});
constexpr fa::Q62Area sigma_c_area = fa::sigma_c(Q/4ULL,Q);
constexpr fa::WidthSample sigma_d_profile[] = {
    {0ULL,Q/2ULL}, {Q/2ULL,(3ULL*Q)/4ULL}, {Q,Q/2ULL}
};
constexpr fa::Q62Area sigma_d_area = fa::sigma_d(sigma_d_profile);
constexpr fa::BcdAirgap form_area_audit = fa::assemble(
    airgap_area,sigma_b_area,sigma_c_area,sigma_d_area);
static_assert(form_area_audit.inputs_consistent);
static_assert(!form_area_audit.physical_area_verified);
static_assert(airgap_area.value == 2305843009213693952ULL);
static_assert(sigma_b_area.value == 1537228672809129301ULL);
static_assert(sigma_c_area.value == 1152921504606846976ULL);
static_assert(sigma_d_area.value == 2882303761517117440ULL);
static_assert(fa::decimal17(airgap_area) == 50000000000000000ULL);
static_assert(fa::decimal17(sigma_b_area) == 33333333333333333ULL);
static_assert(fa::decimal17(sigma_c_area) == 25000000000000000ULL);
static_assert(fa::decimal17(sigma_d_area) == 62500000000000000ULL);
static_assert(fa::sigma_b({Q,0ULL},{Q/2ULL,Q},{0ULL,0ULL}).value
              == sigma_b_area.value);
static_assert(!fa::airgap_a(0ULL,Q).valid);
static_assert(!fa::sigma_b({Q+1ULL,0ULL},{0ULL,0ULL},{Q,0ULL}).valid);
static_assert(!fa::assemble(fa::airgap_a(0ULL,Q),
    sigma_b_area,sigma_c_area,sigma_d_area).inputs_consistent);
static_assert(!fa::relative_to_airgap(
    sigma_b_area,fa::airgap_a(0ULL,Q)).valid);
constexpr fa::WidthSample invalid_d[] = {
    {Q/2ULL,Q/2ULL}, {0ULL,Q/2ULL}
};
static_assert(!fa::sigma_d(invalid_d).valid);
constexpr auto b_relative = fa::relative_to_airgap(
    sigma_b_area,airgap_area);
static_assert(b_relative.valid &&
              b_relative.numerator == sigma_b_area.value &&
              b_relative.denominator == airgap_area.value);
#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
static_assert(fa::wide_matches_native_u128(Q,Q));
static_assert(fa::wide_matches_native_u128(
    14488038916154245685ULL,100000000000000000ULL));
#endif


// SIGMA-FORM-FOLD-PR34-002: all three distinct paths, all nine stages.
// These are explicitly normalized illustrative parameters, not photos.
constexpr fa::Q62Area sigma_a_square = fa::sigma_a_square_base(3ULL*Q/4ULL);
static_assert(sigma_a_square.valid);
static_assert(sigma_a_square.value==2594073385365405696ULL);
constexpr fa::OrderedPath abc =
    fa::path_abc(sigma_a_square,sigma_b_area,sigma_c_area,airgap_area);
constexpr fa::OrderedPath cdb =
    fa::path_cdb(sigma_c_area,sigma_d_area,sigma_b_area,airgap_area);
constexpr fa::OrderedPath bcd =
    fa::path_bcd(sigma_b_area,sigma_c_area,sigma_d_area,airgap_area);
static_assert(fa::paired_and(abc,cdb));
static_assert(abc.formal_sum.value==5284223562781381973ULL);
static_assert(cdb.formal_sum.value==5572453938933093717ULL);
static_assert(bcd.formal_sum.value==5572453938933093717ULL);
static_assert(cdb.first!=bcd.first && cdb.second!=bcd.second);
static_assert(!fa::paired_and(bcd,abc));
constexpr auto fold0=fa::start_fold(bcd);
constexpr auto fold1=fa::fold_b_into_c(fold0);
constexpr auto fold2=fa::fold_d_into_c(fold1);
constexpr auto fold3=fa::push_c_into_d(fold2);
constexpr auto fold4=fa::twirl_to_rope(fold3,3u);
constexpr auto fold5=fa::loop_into_needle(fold4);
constexpr auto fold6=fa::insert_into_airgap_a(fold5);
constexpr auto fold7=fa::twist_a_90(fold6);
constexpr auto fold8=fa::spindle_with_c(fold7);
static_assert(fold0.valid && fold0.stage==fa::FoldStep::SeedBCD);
static_assert(fold1.valid && fold1.stage==fa::FoldStep::FoldBIntoC);
static_assert(fold2.valid && fold2.stage==fa::FoldStep::FoldDIntoC);
static_assert(fold3.valid && fold3.stage==fa::FoldStep::PushCIntoD);
static_assert(fold4.valid && fold4.stage==fa::FoldStep::TwirlToRope);
static_assert(fold5.valid && fold5.stage==fa::FoldStep::LoopIntoNeedle);
static_assert(fold6.valid && fold6.stage==fa::FoldStep::InsertIntoAirgapA);
static_assert(fold7.valid && fold7.stage==fa::FoldStep::TwistA90);
static_assert(fold8.valid && fold8.stage==fa::FoldStep::SpindleWithC);
static_assert(fold8.quarter_turns==8u);
static_assert(fold8.phase.real==1 && fold8.phase.imag==0);
static_assert(fold0.source.formal_sum.value==
              fold8.source.formal_sum.value);
static_assert(!fold8.physical_shape_verified);
static_assert(!fa::fold_d_into_c(fold0).valid);
static_assert(!fa::twirl_to_rope(fold3,0u).valid);
static_assert(!fa::start_fold(abc).valid);
static_assert(fa::multiply_by_i(
    fa::multiply_by_i(
    fa::multiply_by_i(
    fa::multiply_by_i({1,0})))).real==1);
// Independent geometric closure, not derived from B/C/D folding.
// s=3/4, h=1/2, l=5/8 -> l^2=h^2+(s/2)^2.
constexpr auto pyramid=fa::right_square_pyramid(
    3ULL*Q/4ULL,Q/2ULL,5ULL*Q/8ULL);
static_assert(pyramid.right_square_geometry_verified);
static_assert(pyramid.base.value==2594073385365405696ULL);
static_assert(pyramid.one_triangle.value==1080863910568919040ULL);
static_assert(pyramid.lateral.value==4323455642275676160ULL);
static_assert(pyramid.total.value==6917529027641081856ULL);
static_assert(fa::decimal17(pyramid.total)==150000000000000000ULL);
static_assert(!fa::right_square_pyramid(
    3ULL*Q/4ULL,Q/2ULL,Q/2ULL).right_square_geometry_verified);
constexpr auto closure=fa::evaluate_closure(fold8,pyramid);
static_assert(closure.trace.valid && !closure.formal_areas_equal);
static_assert(!closure.folding_proves_pyramid);


// SIGMA-TRIANGLE-PYRAMID-PR34-003:
// Three mutually perpendicular edges, base a RIGHT triangle.
// Sample x=1/2, y=1/2, z=1, oblique double-area=3/4.
// The oblique face identity is: (1/4)^2+(1/2)^2+(1/2)^2=(3/4)^2.
constexpr auto triangle_pyramid = fa::right_triangle_pyramid(
    Q/2ULL,Q/2ULL,Q,3ULL*Q/4ULL);
static_assert(triangle_pyramid.right_triangle_geometry_verified);
static_assert(triangle_pyramid.base.value==576460752303423488ULL);
static_assert(triangle_pyramid.face_xz.value==1152921504606846976ULL);
static_assert(triangle_pyramid.face_yz.value==1152921504606846976ULL);
static_assert(triangle_pyramid.face_oblique.value==1729382256910270464ULL);
static_assert(triangle_pyramid.lateral.value==4035225266123964416ULL);
static_assert(triangle_pyramid.total.value==4611686018427387904ULL);
static_assert(fa::decimal17(triangle_pyramid.total)==100000000000000000ULL);
static_assert(triangle_pyramid.total.value!=pyramid.total.value);
static_assert(!fa::right_triangle_pyramid(
    0ULL,Q/2ULL,Q,3ULL*Q/4ULL).right_triangle_geometry_verified);
static_assert(!fa::right_triangle_pyramid(
    Q/2ULL,0ULL,Q,3ULL*Q/4ULL).right_triangle_geometry_verified);
static_assert(!fa::right_triangle_pyramid(
    Q/2ULL,Q/2ULL,0ULL,3ULL*Q/4ULL).right_triangle_geometry_verified);
static_assert(!fa::right_triangle_pyramid(
    Q/2ULL,Q/2ULL,Q,Q/2ULL).right_triangle_geometry_verified);
static_assert(!fa::right_triangle_pyramid(
    Q+1ULL,Q/2ULL,Q,3ULL*Q/4ULL).right_triangle_geometry_verified);
constexpr auto triangle_closure=fa::evaluate_triangle_closure(
    fold8,triangle_pyramid);
static_assert(triangle_closure.trace.valid &&
              !triangle_closure.formal_areas_equal &&
              !triangle_closure.folding_proves_pyramid);


// F3 gap resolved by rotating PB and PC before the cross product.
// x=y=1/2, z=1 -> R(PB)=(-1,0,-1/2),
// R(PC)=(-1,1/2,0), cross=(1/4,1/2,-1/2).
// Magnitude=3/4, F3=3/8. Two right-angle SIN face factors
// equal 1; the third SIN factor is already captured by |cross|.
constexpr auto rotated_f3=fa::right_triangle_f3_rotated_90(
    Q/2ULL,Q/2ULL,Q,3ULL*Q/4ULL);
static_assert(rotated_f3.valid &&
              rotated_f3.orientation_verified &&
              rotated_f3.area_preserved);
static_assert(rotated_f3.rotated_PB.x==-static_cast<fa::s64>(Q));
static_assert(rotated_f3.rotated_PB.y==0);
static_assert(rotated_f3.rotated_PB.z==-static_cast<fa::s64>(Q/2ULL));
static_assert(rotated_f3.rotated_PC.x==-static_cast<fa::s64>(Q));
static_assert(rotated_f3.rotated_PC.y==static_cast<fa::s64>(Q/2ULL));
static_assert(rotated_f3.rotated_PC.z==0);
static_assert(rotated_f3.rotated_cross.x==static_cast<fa::s64>(Q/4ULL));
static_assert(rotated_f3.rotated_cross.y==static_cast<fa::s64>(Q/2ULL));
static_assert(rotated_f3.rotated_cross.z==-static_cast<fa::s64>(Q/2ULL));
static_assert(rotated_f3.oblique_face.value==1729382256910270464ULL);
static_assert(rotated_f3.oblique_face.value==
              triangle_pyramid.face_oblique.value);
static_assert(!fa::right_triangle_f3_rotated_90(
    Q/2ULL,Q/2ULL,Q,Q/2ULL).valid);
static_assert(!fa::right_triangle_f3_rotated_90(
    0ULL,Q/2ULL,Q,3ULL*Q/4ULL).valid);
// Immutable-by-convention F1/F2/F3 baseline, separately from any folds.
// Frozen values were captured at original source commit
// 65a86d5ecd474efb41a7694d52b656e43636ba3c.
namespace f123 = swe::right_triangle_f123_original;
static_assert(f123::F1_Q62 == triangle_pyramid.face_xz.value);
static_assert(f123::F2_Q62 == triangle_pyramid.face_yz.value);
static_assert(f123::F3_Q62 == triangle_pyramid.face_oblique.value);
static_assert(f123::F3_Q62 == rotated_f3.oblique_face.value);
static_assert(f123::BASE_Q62 == triangle_pyramid.base.value);
static_assert(f123::LATERAL_Q62 == triangle_pyramid.lateral.value);
static_assert(f123::TOTAL_Q62 == triangle_pyramid.total.value);
static_assert(f123::F1_Q62 + f123::F2_Q62 + f123::F3_Q62 +
              f123::BASE_Q62 == f123::TOTAL_Q62);


// F123-EIGHTH-FOLD-PR34-005: symbolic F3-labelled cross slot, with
// F1 left of the norm and F2/2 = 2F2/4 = 4F2/8.
// The original frozen F1/F2/F3 source is NOT rewritten.
namespace ef = swe::f123_eighth_fold_experiment;
constexpr auto eighth=ef::calculate(rotated_f3);
static_assert(eighth.valid && eighth.fraction_equivalent);
static_assert(eighth.original_reference_unchanged);
static_assert(!eighth.geometric_tip_verified);
static_assert(eighth.f1_q62==f123::F1_Q62);
static_assert(eighth.f2_q62==f123::F2_Q62);
static_assert(eighth.frozen_f3_q62==f123::F3_Q62);
static_assert(eighth.gap.valid &&
              eighth.gap.norm_q62==2ULL*f123::F3_Q62);
static_assert(eighth.f2_over_2_q62==Q/8ULL);
static_assert(eighth.twice_f2_over_4_q62==Q/8ULL);
static_assert(eighth.four_times_f2_over_8_q62==Q/8ULL);
static_assert(eighth.first_multiplication_q62==3ULL*Q/16ULL);
static_assert(eighth.final_egress_q62==3ULL*Q/128ULL);
static_assert(eighth.final_egress_q62==108086391056891904ULL);
static_assert(fa::decimal17(
    {eighth.final_egress_q62,true})==2343750000000000ULL);
static_assert(!ef::calculate(fa::right_triangle_f3_rotated_90(
    Q/2ULL,Q/2ULL,Q,Q/2ULL)).valid);
#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
constexpr unsigned long long eighth_u128_expected =
    static_cast<unsigned long long>(
        (static_cast<__uint128_t>(
             (static_cast<__uint128_t>(f123::F1_Q62) *
              (2ULL*f123::F3_Q62)) >> 62u) *
         (4ULL*f123::F2_Q62/8ULL)) >> 62u);
static_assert(eighth_u128_expected==eighth.final_egress_q62);
#endif


// SIGMA-E-REDAXUALCHOICE-PR34-006:
// E is the independent frozen BCD F3-cross-slot E, not Σ(F1+F2+F3).
// Each k in [-3,+3] is a distinct fraction witness equal to E/2.
namespace sigmae=swe::sigma_e_redaxualchoice;
constexpr auto esigma=sigmae::calculate(eighth);
static_assert(esigma.source_verified && esigma.all_terms_equivalent);
static_assert(esigma.valid && !esigma.infinite_sum_converges);
static_assert(!esigma.geometric_tip_verified);
static_assert(esigma.source_e_q62==108086391056891904ULL);
static_assert(esigma.minus3.index==-3 && esigma.plus3.index==3);
static_assert(esigma.minus3.numerator_q62==13510798882111488ULL);
static_assert(esigma.minus3.denominator_numerator==2ULL &&
              esigma.minus3.denominator_denominator==8ULL);
static_assert(esigma.zero.numerator_q62==108086391056891904ULL);
static_assert(esigma.plus3.numerator_q62==864691128455135232ULL);
static_assert(esigma.plus3.denominator_numerator==16ULL &&
              esigma.plus3.denominator_denominator==1ULL);
static_assert(esigma.minus3.result_q62==54043195528445952ULL);
static_assert(esigma.minus2.result_q62==esigma.minus3.result_q62);
static_assert(esigma.minus1.result_q62==esigma.minus3.result_q62);
static_assert(esigma.zero.result_q62==esigma.minus3.result_q62);
static_assert(esigma.plus1.result_q62==esigma.minus3.result_q62);
static_assert(esigma.plus2.result_q62==esigma.minus3.result_q62);
static_assert(esigma.plus3.result_q62==esigma.minus3.result_q62);
static_assert(esigma.sigma_q62==378302368699121664ULL);
static_assert(fa::decimal17({esigma.sigma_q62,true})==
              8203125000000000ULL);
static_assert(!sigmae::term(eighth.final_egress_q62,-4).valid);
static_assert(!sigmae::term(eighth.final_egress_q62,4).valid);
static_assert(!sigmae::term(0ULL,0).valid);
static_assert(!sigmae::term(eighth.final_egress_q62+1ULL,-3).valid);
static_assert(!sigmae::term(~0ULL,3).valid);
constexpr auto ecorrupt=[]() constexpr {
    auto x=eighth;
    ++x.final_egress_q62;
    return x;
}();
static_assert(!sigmae::calculate(ecorrupt).valid);
#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
constexpr unsigned long long sigma_e_u128 =
    static_cast<unsigned long long>(
       (static_cast<__uint128_t>(21ULL) *
        static_cast<__uint128_t>(Q)) / 256ULL);
static_assert(sigma_e_u128 == esigma.sigma_q62);
#endif


// SIGMA-F-REDAXUALCHOICE-PR34-007: independent CARBON-COPY F lane.
// F is F1+F2+F3, not the eighth-fold E or SigmaE.
namespace sf=swe::sigma_f_redaxualchoice;
static_assert(sf::Q == Q);
static_assert(sf::X==f123::x && sf::Y==f123::y && sf::Z==f123::z);
static_assert(sf::OBLIQUE==f123::oblique_double_area);
static_assert(sf::F1==f123::F1_Q62 && sf::F2==f123::F2_Q62 &&
              sf::F3==f123::F3_Q62);
static_assert(sf::BASE==f123::BASE_Q62 && sf::TOTAL==f123::TOTAL_Q62);
static_assert(sf::SIGMA_F123==f123::LATERAL_Q62);
static_assert(sf::F==4035225266123964416ULL);
static_assert(sf::R_NEG3==2017612633061982208ULL);
static_assert(sf::R_NEG2==sf::R_NEG3);
static_assert(sf::R_NEG1==sf::R_NEG3);
static_assert(sf::R_ZERO==sf::R_NEG3);
static_assert(sf::R_POS1==sf::R_NEG3);
static_assert(sf::R_POS2==sf::R_NEG3);
static_assert(sf::R_POS3==sf::R_NEG3);
static_assert(sf::SIGMA_REDAXUALCHOICE==14123288431433875456ULL);
static_assert(fa::decimal17({sf::SIGMA_REDAXUALCHOICE,true})==
              306250000000000000ULL);
static_assert(sf::SIGMA_REDAXUALCHOICE!=esigma.sigma_q62);
static_assert(sf::TOTAL==Q && sf::BASE+sf::F==sf::TOTAL);


// FOUR-PYRAMID-REDUXUALCHOICE-PR34-008: independent square base
// triangulated into nine exact nonoverlapping triangles in 3 strips.
namespace four=swe::four_pyramid_reduxualchoice;
static_assert(four::CORE.right_square_geometry_verified);
static_assert(four::SIDE==3ULL*Q/4ULL);
static_assert(four::HEIGHT==Q/2ULL && four::SLANT==5ULL*Q/8ULL);
static_assert(four::BASE==pyramid.base.value);
static_assert(four::F1==pyramid.one_triangle.value);
static_assert(four::F2==four::F1 && four::F3==four::F1 &&
              four::F4==four::F1);
static_assert(four::F==pyramid.lateral.value);
static_assert(four::TOTAL==pyramid.total.value);
static_assert(four::NINE.exact_partition_verified);
static_assert(four::NINE.sum.value==four::BASE);
static_assert(four::NINE.face[0].value ==
              2ULL*four::NINE.face[1].value);
static_assert(four::NINE.face[3].value==four::NINE.face[0].value);
static_assert(four::NINE.face[6].value==four::NINE.face[0].value);
static_assert(!four::nine_base_triangles(0ULL,four::CORE).
              exact_partition_verified);
static_assert(!four::nine_base_triangles(Q/2ULL,four::CORE).
              exact_partition_verified);
// Three distinct rational point samples all at 2/3 the height;
// never fabricate a fourth diamond vertex or a proven tip.
static_assert(four::TOP3.rational_geometry_verified &&
              !four::TOP3.physical_fold_or_diamond_verified);
static_assert(four::TOP3.on_PA.x_numerator !=
              four::TOP3.on_PB.x_numerator);
static_assert(four::TOP3.on_PB.y_numerator !=
              four::TOP3.on_PC.y_numerator);
static_assert(four::TOP3.on_PA.z_numerator ==
              four::TOP3.on_PB.z_numerator);
static_assert(!four::upper_markers(0ULL,Q/2ULL).
              rational_geometry_verified);
static_assert(four::STEP_UP.exact_sample_ratio &&
              !four::STEP_UP.shape_transition_proved);
static_assert(four::STEP_UP.numerator==15ULL &&
              four::STEP_UP.denominator==14ULL);
static_assert(four::STEP_UP.three_lateral_q62==
              f123::LATERAL_Q62);
static_assert(four::R_NEG3==2161727821137838080ULL);
static_assert(four::R_NEG2==four::R_NEG3 &&
              four::R_NEG1==four::R_NEG3 &&
              four::R_ZERO==four::R_NEG3 &&
              four::R_POS1==four::R_NEG3 &&
              four::R_POS2==four::R_NEG3 &&
              four::R_POS3==four::R_NEG3);
static_assert(four::SIGMA_REDUXUALCHOICE ==
              15132094747964866560ULL);
static_assert(four::SIGMA_REDUXUALCHOICE !=
              sf::SIGMA_REDAXUALCHOICE);
static_assert(four::TOTAL==6917529027641081856ULL);
static_assert(!four::missing_tip_or_fold_verified);


// U-CIRCLE-FIT-PR34-009: exact projected containment with bounded
// inputs; independent Nice/NotNice and Q62 cross-lane checks.
namespace uc=swe::u_circle_fit;
static_assert(uc::Q==Q);
static_assert(uc::TRIANGLE_RESULT==uc::U::Nice);
static_assert(uc::TESSELLATION_RESULT==uc::U::Nice);
static_assert(uc::ASTRIX_RESULT==uc::U::Nice);
static_assert(uc::VALIDATE(uc::FACE,uc::HEIGHT_CIRCLE)==
              uc::U::NotNice);
static_assert(uc::VALIDATE(uc::FACE,uc::ENCLOSING_CIRCLE)==
              uc::U::Nice);
static_assert(uc::DISTANCE2(uc::FACE.a,uc::ENCLOSING_CIRCLE)==4624ULL);
static_assert(uc::DISTANCE2(uc::FACE.b,uc::ENCLOSING_CIRCLE)==4624ULL);
static_assert(uc::DISTANCE2(uc::FACE.c,uc::ENCLOSING_CIRCLE)==4624ULL);
static_assert(uc::VALIDATE(uc::QUAKER,uc::QUAKER_CIRCLE)==uc::U::Nice);
static_assert(uc::SUM_NINE_DOUBLE_AREA()==7200ULL);
static_assert(uc::A==600ULL && uc::T==3600ULL);
static_assert(uc::A*uc::D_DENOMINATOR==uc::T*uc::D_NUMERATOR);
static_assert(uc::SIGMA==four::SIGMA_REDUXUALCHOICE);
static_assert(uc::BASE==four::BASE && uc::TOTAL==four::TOTAL);
static_assert(uc::VALIDATE(uc::DEGENERATE,uc::BASE_CIRCLE)==
              uc::U::NotNice);
static_assert(!uc::INSIDE({1000001,0},uc::BASE_CIRCLE));
static_assert(!uc::INSIDE({0,0},{0,0,0ULL}));
static_assert(uc::SQUARE(4000001LL)==~0ULL);
static_assert(uc::DOUBLE_AREA({{1000001,0},{1,0},{0,1}})==0ULL);
static_assert(!uc::PHYSICAL_FOLD_VERIFIED);


// XDR-RDX-FOLD-TORK-PR34-010: exact fold, 6|7|8|9 reversible
// envelopes, BiDi/TriDi, Pi-family, interleaved Σ²/Σ³ PING.
namespace tork=swe::xdr_rdx_fold_tork;
static_assert(tork::AD_FOLDED==67600ULL);
static_assert(tork::CD_FOLDED==152100ULL);
static_assert(tork::AD_FOLDED!=tork::CD_FOLDED);
static_assert(tork::PING_HEAD(tork::HEAD_TORK));
static_assert(tork::PING_TAIL(tork::TAIL_TORK));
static_assert(tork::REVERSE(tork::FORWARD(tork::AD_FOLDED))==
              tork::AD_FOLDED);
static_assert(tork::FORWARD(tork::REVERSE(tork::CD_FOLDED))==
              tork::CD_FOLDED);
static_assert(tork::EQUAL(tork::BIDI(tork::BIDI(tork::INPUT_PAIR)),
                          tork::INPUT_PAIR));
static_assert(tork::EQUAL(tork::TRIDI(tork::TRIDI(
              tork::TRIDI(tork::VIEWS))),tork::VIEWS));
static_assert(tork::PI_DI==3ULL && tork::SIGMA_PI==false);
static_assert(tork::PI_SIGMA_WITNESS.distinct);
static_assert(tork::EQUAL(tork::TALON(tork::PI_SIGMA_WITNESS),
                          tork::INPUT_PAIR));
static_assert(tork::SIGMA_SIGMA());
static_assert(tork::SIGMA_SIGMA_SIGMA(64u));
static_assert(tork::PING_STRESS(128u));
static_assert(tork::CORRUPTION_REJECTED());
static_assert(!tork::EXTERNAL_APPROVAL_INFERRED);


// MINZA-DIRECT-PING-BIDI-PR34-011: skip NINZA intermediates for
// the frozen A--C hinge, validate in one step, then reversible BiDi.
namespace direct=swe::minza_direct_ping_bidi;
static_assert(direct::HINGE_VALID(direct::LARGE,direct::SMALL));
static_assert(direct::FOLD.valid);
static_assert(direct::FOLD.dot_cross_verified);
static_assert(direct::FOLD.length_preserved);
static_assert(direct::FOLD.x13==120 && direct::FOLD.y13==80);
static_assert(direct::FOLD.z2_169==46800);
static_assert(direct::FOLD.ad_folded==67600ULL &&
              direct::FOLD.cd_folded==152100ULL);
static_assert(direct::PING(direct::FOLD));
static_assert(direct::FOLD.ad_folded==tork::AD_FOLDED);
static_assert(direct::FOLD.cd_folded==tork::CD_FOLDED);
static_assert(direct::REPORT.bijective &&
              direct::REPORT.ping_after_restore);
static_assert(!direct::PING_PAIR(direct::REPORT.swapped));
static_assert(direct::PING_PAIR(direct::REPORT.restored));
static_assert(direct::HEAD_PING && direct::TAIL_PING);
static_assert(direct::REJECT_CORRUPTION());
static_assert(!direct::NINZA_REQUIRED);
static_assert(!direct::GENERAL_FOLD_PROVED);
static_assert(!direct::PHYSICAL_FOLD_PROVED);
static_assert(!direct::RELEASE_AUTHORIZED);


// MINZA-DIRECT-PING-TRIDI-PR34-012: distinct AC hinge third lane.
// Exact 400+900=1300, scaled by 169. C^3 = identity, BiDi kept.
namespace tridi=swe::minza_direct_ping_tridi;
static_assert(tridi::AC2==1300ULL);
static_assert(tridi::AC_FOLDED==219700ULL);
static_assert(tridi::AD2+tridi::CD2==tridi::AC2);
static_assert(tridi::AD_FOLDED+tridi::CD_FOLDED==
              tridi::AC_FOLDED);
static_assert(tridi::PING_CANONICAL(tridi::ORIGINAL));
static_assert(tridi::PING_SET(tridi::ROTATION_1));
static_assert(tridi::PING_SET(tridi::ROTATION_2));
static_assert(!tridi::PING_CANONICAL(tridi::ROTATION_1));
static_assert(!tridi::PING_CANONICAL(tridi::ROTATION_2));
static_assert(tridi::PING_CANONICAL(tridi::ROTATION_3));
static_assert(tridi::SAME(tridi::TRIDI_INVERSE(
              tridi::TRIDI(tridi::ORIGINAL)),tridi::ORIGINAL));
static_assert(tridi::BIDI_PRESERVED());
static_assert(tridi::REPORT.valid);
static_assert(tridi::REJECT_CORRUPTION());
static_assert(!tridi::NINZA_REQUIRED);
static_assert(!tridi::RELEASE_AUTHORIZED);


// THREE-GEOMETRIC-FOLDS-PR34-013: three DISTINCT active 90 degree hinges
// moving a connected downstream assembly, not label permutations.
namespace fold3=swe::three_geometric_folds;
static_assert(fold3::WITNESS.valid);
static_assert(fold3::FOLD_STEP(1u) && fold3::FOLD_STEP(2u) &&
              fold3::FOLD_STEP(3u));
static_assert(fold3::RIGHT_ANGLE_HINGE(1u) &&
              fold3::RIGHT_ANGLE_HINGE(2u) &&
              fold3::RIGHT_ANGLE_HINGE(3u));
static_assert(fold3::SEAMS_MATCH(0u)&&fold3::SEAMS_MATCH(1u)&&
              fold3::SEAMS_MATCH(2u)&&fold3::SEAMS_MATCH(3u));
static_assert(fold3::ALL_RIGID(0u)&&fold3::ALL_RIGID(3u));
static_assert(fold3::DOWNSTREAM_MOVED(1u)&&
              fold3::DOWNSTREAM_MOVED(2u)&&
              fold3::DOWNSTREAM_MOVED(3u));
static_assert(fold3::SUM_DOUBLE_AREA()==7200ULL);
static_assert(fold3::SUM_DOUBLE_AREA()==uc::SUM_NINE_DOUBLE_AREA());
static_assert(fold3::FREE_SEAM_COINCIDENT());
static_assert(fold3::SAME(fold3::CORNER_0,fold3::CORNER_4));
static_assert(!fold3::SEALED_SEAM_VERIFIED);
static_assert(!fold3::CLOSED_CUBE_VERIFIED);
static_assert(!fold3::PHYSICAL_PAPER_FOLD_VERIFIED);
static_assert(!fold3::RELEASE_AUTHORIZED);


// RECTANGLOID-PR34-014: 3 physical fold sources + 2 oriented end caps.
namespace prism=swe::rectangloid_primitive;
static_assert(prism::WITNESS.valid);
static_assert(prism::WALL_TRIANGLES==8u);
static_assert(prism::CAP_TRIANGLES==4u);
static_assert(prism::TRIANGLES==12u);
static_assert(prism::ALL_EDGES_PAIRED(prism::CLOSED));
static_assert(prism::MISSING_CAP_REJECTED());
static_assert(prism::VOLUME==13500);
static_assert(prism::SURFACE_AREA==4050);
static_assert(prism::SIGNED_VOLUME6(prism::CLOSED)==81000);
static_assert(prism::DOUBLE_AREA_TOTAL()==8100ULL);
static_assert(prism::VISIBLE_TRIANGLES(prism::CAMERA)==6u);
static_assert(prism::CULLED_TRIANGLES==6u);
static_assert(prism::CAP_0.source_area==
              prism::CAP_0.cap_area+prism::CAP_0.offcut_area);
static_assert(prism::CAP_1.source_area==
              prism::CAP_1.cap_area+prism::CAP_1.offcut_area);
static_assert(prism::DIARY_VIEW.fill==prism::Fill::Water);
static_assert(!prism::PHYSICAL_WATER_FILL_VERIFIED);
static_assert(!prism::FULL_Z_BUFFER_OCCLUSION_IMPLEMENTED);
static_assert(!prism::RELEASE_AUTHORIZED);

constexpr const char* test_sha = "0123456789abcdef0123456789abcdef01234567";
constexpr ApprovalClaim first{"synthetic-reviewer-1",test_sha,true};
constexpr ApprovalClaim second{"synthetic-reviewer-2",test_sha,true};
constexpr ApprovalClaim third{"synthetic-reviewer-3",test_sha,true};
static_assert(three_distinct_claims(first,second,third,test_sha));
static_assert(!three_distinct_claims(first,first,third,test_sha));
static_assert(!three_distinct_claims(first,second,
    ApprovalClaim{"synthetic-reviewer-3",test_sha,false},test_sha));

int main() {
    if(!verify_triforce()||!circle_of_sanity::inspect().all_consistent)return 1;
    for(const Record r : {euler,pi,intensity}) {
        const Audit a=recompute(r);
        if(!a.integer_matches||!a.remainder_in_range||!a.suffix_matches)return 2;
        if(a.exact_quotient*a.divisor+a.exact_remainder!=a.exact_product)return 3;
    }
    Record corrupted=pi;
    ++corrupted.expected_integer;
    if(verify(corrupted))return 4;
    if(!verify(euler)||!verify(pi)||!verify(intensity))return 5;
    if(!tork::SIGMA_SIGMA_SIGMA(4096u))return 6;
    if(!direct::PING(direct::FOLD)||
       !direct::REPORT.ping_after_restore)return 7;
    if(!tridi::VALIDATE().valid)return 8;
    if(!fold3::PING_THREE_FOLDS().valid)return 9;
    if(!prism::PING().valid)return 10;
    return 0;
}
