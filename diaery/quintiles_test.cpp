/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "quintiles.hpp"
using namespace swe::diaery::quintiles;
constexpr auto simple=[]() constexpr {
 double values[10]{10.,1.,9.,2.,8.,3.,7.,4.,6.,5.};
 double scratch[10]{};
 const auto r=calculate({values,10ULL,10ULL},scratch,10ULL);
 return r.state==State::Ready && r.count==10ULL &&
   r.boundaries[0]==1. && r.boundaries[1]==2. &&
   r.boundaries[2]==4. && r.boundaries[3]==6. &&
   r.boundaries[4]==8. && r.boundaries[5]==10. &&
   values[0]==10.;
}();
static_assert(simple);
constexpr auto omitted=[]() constexpr {
 double values[2]{1.,2.};
 return calculate({values,2ULL,2ULL},nullptr,0ULL).state==State::MissingScratch;
}();
static_assert(omitted);
constexpr auto empty=calculate({nullptr,0ULL,0ULL},nullptr,0ULL);
static_assert(empty.state==State::NullNoData);
int main(){return 0;}
