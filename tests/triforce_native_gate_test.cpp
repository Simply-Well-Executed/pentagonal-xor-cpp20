// SPDX-License-Identifier: 0BSD
// Compile and execute with native MSVC C++20 /O2, not clang-cl.
#include "../shared/toolong_q62.hpp"
#include "../shared/sigma_form_geometry.hpp"
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
