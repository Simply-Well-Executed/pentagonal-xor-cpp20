// SPDX-License-Identifier: 0BSD
#include "freq.hpp"
using namespace swe::diaery::freq;
constexpr Samples empty{};
static_assert(count_bins(empty).state==State::NullNoData);
static_assert(fit_bell(empty).state==State::NullNoData);
static_assert(!admissible(0.)&&!admissible(-1.)&&!admissible(13.));
static_assert(bin_index(0.75)==1u&&bin_index(12.)==15u);
constexpr auto prepared=[]() constexpr {
 Samples s{};
 append(s,1.0);append(s,1.0);append(s,2.0);append(s,3.0);
 return s;
}();
static_assert(prepared.count==4u);
static_assert(count_at_value(prepared,1.0)==2u);
static_assert(count_bins(prepared).total==4u);
static_assert(count_bins(prepared).bins[1]==2u);
static_assert(count_bins(prepared).bins[2]==1u);
static_assert(fit_bell(prepared).state==State::Ready);
static_assert(fit_bell(prepared).mean==1.75);
static_assert(fit_bell(prepared).sample_variance>0.91);
static_assert(fit_bell(prepared).sample_variance<0.92);
static_assert(expected_bin_count(fit_bell(prepared),1.75)>0.0);
constexpr auto repeated=[]() constexpr {
 Samples s{};append(s,2.0);append(s,2.0);append(s,2.0);return s;
}();
static_assert(fit_bell(repeated).state==State::NullZeroVariance);
constexpr auto missing=[]() constexpr {
 Samples s{};append(s,0.);return s;
}();
static_assert(missing.count==0u);
static_assert(negative_exp_approx(0.0)==1.0);
static_assert(negative_exp_approx(50.0)>=0.0);
int main(){return 0;}
