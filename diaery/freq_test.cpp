/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "freq.hpp"
using namespace swe::diaery::freq;
constexpr Samples empty{nullptr,0ULL,0ULL};
static_assert(count_bins(empty).state==State::NullNoData);
static_assert(fit_bell(empty).state==State::NullNoData);
static_assert(!admissible(0.)&&!admissible(-1.)&&!admissible(1.0e10));
constexpr auto test=[]() constexpr {
 double raw[5]{1.,1.,2.,3.,8.};
 Samples s{raw,5ULL,5ULL};
 const auto histogram=count_bins(s);
 const auto bell=fit_bell(s);
 return histogram.state==State::Ready &&
        histogram.total==5ULL &&
        histogram.minimum==1. &&
        histogram.maximum==8. &&
        count_at_value(s,1.)==2ULL &&
        count_at_value(s,8.)==1ULL &&
        bell.state==State::Ready &&
        expected_bin_count(bell,2.,(histogram.maximum-histogram.minimum)/BINS)>0.;
}();
static_assert(test);
constexpr auto no_cap=[]() constexpr {
 double values[130]{};
 Samples s{values,0ULL,130ULL};
 for(unsigned i=0u;i<130u;++i)
     if(append(s,i%2u?2.:3.)!=State::Ready)return false;
 return s.count==130ULL&&count_bins(s).total==130ULL&&
        count_at_value(s,3.)==65ULL;
}();
static_assert(no_cap);
constexpr auto full=[]() constexpr {
 double one[]{1.};
 Samples s{one,1ULL,1ULL};
 return append(s,2.)==State::CapacityRequired&&s.count==1ULL;
}();
static_assert(full);
constexpr auto no_rows=[]() constexpr {
 Samples s{nullptr,0ULL,0ULL};
 return append(s,1.)==State::CapacityRequired&&s.count==0ULL;
}();
static_assert(no_rows);
constexpr auto repeated=[]() constexpr {
 double values[3]{2.,2.,2.};
 return fit_bell({values,3ULL,3ULL}).state==State::NullZeroVariance;
}();
static_assert(repeated);
static_assert(negative_exp_approx(0.)==1.);
int main(){return 0;}
