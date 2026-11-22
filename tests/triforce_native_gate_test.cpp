// SPDX-License-Identifier: 0BSD
// Compile and execute with native MSVC C++20 /O2, not clang-cl.
#include "../shared/toolong_q62.hpp"
#include "../shared/sigma_form_geometry.hpp"
#include "../shared/sigma_form_area_q62.hpp"
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
    return 0;
}
