// SPDX-License-Identifier: 0BSD
// PR #32 ΣΣΣ / TRIFORCE: owner Q62 source records, with compile-time witnesses.
// Source: PR issue comments 7073177351, 7073580560, 7073578206.
// Original __uint128_t expressions remain in those comments; MSVC uses
// exact four-limb arithmetic for these fixed Q62 source words.
#ifndef SWE_TOOLONG_Q62_CORE_HPP
#define SWE_TOOLONG_Q62_CORE_HPP
namespace swe::toolong_q62 {
using u64 = unsigned long long;
static_assert(sizeof(u64) == 8, "64-bit unsigned long long required");
inline constexpr u64 multiplier = 100000000000000000ULL;
inline constexpr unsigned shift = 62u;
struct Record {
    const char* label;
    u64 source_word;
    u64 expected_integer;
};
inline constexpr Record euler{"Euler", 12535862302449814170ULL, 271828182845904523ULL};
inline constexpr Record pi{"Pi", 14488038916154245685ULL, 314159265358979323ULL};
inline constexpr Record intensity{"I", 315868046760674477ULL, 6849296450333523ULL};

// Independent constexpr calculation of (u128(word) * multiplier) >> 62
// using 32-bit limbs and 64-bit temporary products only.
[[nodiscard]] constexpr u64 multiply_shift62(u64 word) noexcept {
    constexpr u64 mask = 0xFFFFFFFFULL;
    const u64 a0 = word & mask, a1 = word >> 32u;
    const u64 b0 = multiplier & mask, b1 = multiplier >> 32u;
    const u64 p00 = a0 * b0;
    const u64 p01 = a0 * b1;
    const u64 p10 = a1 * b0;
    const u64 p11 = a1 * b1;
    const u64 middle = (p00 >> 32u) + (p01 & mask) + (p10 & mask);
    const u64 high = p11 + (p01 >> 32u) + (p10 >> 32u) +
                     (middle >> 32u);
    const u64 low = ((middle & mask) << 32u) | (p00 & mask);
    return (high << 2u) | (low >> 62u);
}
[[nodiscard]] constexpr bool matches(const Record& r) noexcept {
    return multiply_shift62(r.source_word) == r.expected_integer;
}
[[nodiscard]] constexpr bool suffix23(const Record& r) noexcept {
    return r.expected_integer % 100ULL == 23ULL;
}
[[nodiscard]] constexpr bool sigma_sigma_sigma() noexcept {
    return matches(euler) && matches(pi) && matches(intensity) &&
           suffix23(euler) && suffix23(pi) && suffix23(intensity);
}
static_assert(sigma_sigma_sigma(), "TRIFORCE: Q62 or shared suffix mismatch");
static_assert(euler.expected_integer == 271828182845904523ULL);
static_assert(pi.expected_integer == 314159265358979323ULL);
static_assert(intensity.expected_integer == 6849296450333523ULL);
static_assert(intensity.expected_integer < 10000000000000000ULL,
              "Intensity's integer output has 16 digits, not 18");

// Explicit human approval claims are NOT cryptographic authentication.
// An external trusted review system must verify IDs, SHA and signatures.
// These fields do not authorize a merge or release on their own.
struct ApprovalClaim {
    const char* reviewer_id;
    const char* reviewed_head_sha;
    bool explicit_yes;
};
[[nodiscard]] constexpr bool same_text(const char* a,const char* b) noexcept {
    if(!a || !b) return false;
    for(unsigned i=0u;;++i) {
        if(a[i]!=b[i])return false;
        if(a[i]=='\0')return true;
    }
}
[[nodiscard]] constexpr bool nonempty(const char* s) noexcept {
    return s && s[0]!='\0';
}
[[nodiscard]] constexpr bool is_commit_sha(const char* sha) noexcept {
    if(!sha)return false;
    for(unsigned i=0u;i<40u;++i) {
        const char c=sha[i];
        if(!((c>='0' && c<='9')||(c>='a' && c<='f')))return false;
    }
    return sha[40u]=='\0';
}
[[nodiscard]] constexpr bool three_distinct_claims(
    const ApprovalClaim& a, const ApprovalClaim& b,
    const ApprovalClaim& c, const char* expected_head) noexcept {
    return is_commit_sha(expected_head) &&
           a.explicit_yes && b.explicit_yes && c.explicit_yes &&
           nonempty(a.reviewer_id) && nonempty(b.reviewer_id) &&
           nonempty(c.reviewer_id) &&
           !same_text(a.reviewer_id,b.reviewer_id) &&
           !same_text(a.reviewer_id,c.reviewer_id) &&
           !same_text(b.reviewer_id,c.reviewer_id) &&
           same_text(a.reviewed_head_sha,expected_head) &&
           same_text(b.reviewed_head_sha,expected_head) &&
           same_text(c.reviewed_head_sha,expected_head);
}
// No identities, sign-offs or authenticated approvals are embedded here.
inline constexpr bool externally_authenticated_approval = false;
static_assert(!externally_authenticated_approval);
} // namespace swe::toolong_q62
#endif // SWE_TOOLONG_Q62_CORE_HPP
