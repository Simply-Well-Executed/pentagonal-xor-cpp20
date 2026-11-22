// SPDX-License-Identifier: 0BSD
// Compile and execute with native MSVC C++20 /O2, not clang-cl.
#include "../shared/toolong_q62.hpp"
using namespace swe::toolong_q62;

static_assert(sigma_sigma_sigma());
static_assert(circle_of_sanity::centrix.all_consistent);
static_assert(!circle_of_sanity::centrix.release_authorized);
static_assert(euler.expected_integer % 100ULL == 23ULL);
static_assert(pi.expected_integer % 100ULL == 23ULL);
static_assert(intensity.expected_integer % 100ULL == 23ULL);
static_assert(!externally_authenticated_approval);
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
