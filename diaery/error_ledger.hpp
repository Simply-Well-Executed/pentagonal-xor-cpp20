/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// DEARY error ledger: explicit uncertainty, fail-closed. RELEASE=HOLD.
#ifndef SWE_DIAERY_ERROR_LEDGER_HPP
#define SWE_DIAERY_ERROR_LEDGER_HPP
namespace swe::diaery::error {
enum class State { Valid, NullRecord, MissingComponent, Incomparable, Overflow, InvalidPhysicalModel };
enum class Metric { ErectLength, ErectCircumference, FlaccidLength, FlaccidCircumference, VaginalDepth };
struct Paper {
    Metric metric;
    // Nonnegative absolute upper bounds in integer micro-inches.
    // These MUST be genuine reported error bounds, not SD, SEM, or unrelated rates.
    unsigned long long bounds[8];
    unsigned count;
    bool all_reported;
    bool source_verified;
    bool non_mri;
    bool applicable;
};
struct Result { State state; unsigned long long numerator; unsigned long long denominator; };
constexpr unsigned long long LIMIT=~0ULL;
[[nodiscard]] constexpr Result sum_paper(const Paper& paper) noexcept {
    if(!paper.source_verified||!paper.applicable||
       (paper.metric==Metric::VaginalDepth && !paper.non_mri))
       return {State::NullRecord,0,1};
    if(!paper.all_reported||paper.count==0||paper.count>8)
       return {State::MissingComponent,0,1};
    unsigned long long sum=0;
    for(unsigned i=0;i<paper.count;++i) {
        if(paper.bounds[i]>LIMIT-sum)return {State::Overflow,0,1};
        sum+=paper.bounds[i];
    }
    return {State::Valid,sum,1};
}
[[nodiscard]] constexpr Result sigma_mean(const Paper* papers,unsigned count) noexcept {
    if(papers==nullptr||count==0)return {State::NullRecord,0,1};
    const Metric metric=papers[0].metric;
    unsigned long long total=0;
    for(unsigned i=0;i<count;++i) {
        if(papers[i].metric!=metric)return {State::Incomparable,0,1};
        const auto r=sum_paper(papers[i]);
        if(r.state!=State::Valid)return r;
        if(r.numerator>LIMIT-total)return {State::Overflow,0,1};
        total+=r.numerator;
    }
    // Rational total/count is retained exactly: no truncation or implicit rounding.
    return {State::Valid,total,count};
}
[[nodiscard]] constexpr bool admissible_depth(double inches,bool measured,bool non_mri) noexcept {
    return inches>0.0&&inches<=1000000000.0&&measured&&non_mri;
}
// The requested log(Euler-Jig / i^2) must remain HOLD:
// i^2=-1; a real logarithm of a negative real ratio is undefined,
// and Euler's Jig area cannot be used as a dimensionless error scale.
[[nodiscard]] constexpr State proposed_euler_jig_log_gate(bool wants_log_ratio) noexcept {
    return wants_log_ratio?State::InvalidPhysicalModel:State::Valid;
}
} // namespace swe::diaery::error
#endif
