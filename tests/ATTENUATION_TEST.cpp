/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/ATTENUATION.hpp"

using A = ATTENUATION;
using C = A::Cohort;
constexpr A::Count start{103,C::classified};
constexpr A::Count mixed_end{167,C::pennyworth};
constexpr A::Count actual_end{200,C::classified};
constexpr auto mixed=A::evaluate(start,100,mixed_end);
static_assert(mixed.gain==64 && mixed.shortfall==36);
static_assert(mixed.bridge==-3 && mixed.attenuated==61);
static_assert(mixed.cross_through && !mixed.comparable && !mixed.transfer.valid);
static_assert(A::audit(mixed,start,100,mixed_end));
constexpr auto actual=A::evaluate(start,100,actual_end);
static_assert(actual.gain==97 && actual.shortfall==3 && actual.attenuated==94);
static_assert(actual.comparable && actual.transfer.valid && actual.cross_through);
static_assert(actual.transfer.numerator==97 && actual.transfer.denominator==100);
static_assert(A::audit(actual,start,100,actual_end));
constexpr auto exact=A::evaluate(start,100,{203,C::classified});
static_assert(!exact.cross_through && exact.shortfall==0 && exact.attenuated==97);
static_assert(!A::evaluate(start,0,start).transfer.valid);
static_assert(!A::evaluate({1,C::unknown},1,{2,C::unknown}).comparable);
constexpr A::U M=~A::U{0};
static_assert(A::evaluate({M,C::other},0,{0,C::other}).attenuated==-8589934590LL);
static_assert(A::evaluate({0,C::other},M,{M,C::other}).attenuated==8589934590LL);
static_assert(A::bridge_symbol==0x2261 && A::cross_symbol==0x2260);
static_assert(A::bridge_utf8[0]==0xE2 && A::bridge_utf8[1]==0x89 &&
              A::bridge_utf8[2]==0xA1 && A::bridge_utf8[3]==0);
static_assert(A::cross_utf8[0]==0xE2 && A::cross_utf8[1]==0x89 &&
              A::cross_utf8[2]==0xA0 && A::cross_utf8[3]==0);
constexpr bool corrupt() {
    auto w=actual; ++w.attenuated;
    if(A::audit(w,start,100,actual_end))return false;
    w=actual; w.comparable=false;
    if(A::audit(w,start,100,actual_end))return false;
    w=actual; w.cross_through=false;
    if(A::audit(w,start,100,actual_end))return false;
    return !A::audit(actual,start,100,mixed_end);
}
static_assert(corrupt());
// === arithmetic identity; ==== exact quotient; ==== reverse factor witness.
constexpr auto jig=A::EULERS_JIG(64,100,8,10);
static_assert(jig.valid && jig.loss==-36);
static_assert(jig.divisor==18 && jig.remainder==0 && jig.correction==2);
static_assert(jig.correction==10-8 && jig.correction*jig.divisor==-jig.loss);
static_assert(jig.compensated==-34);
static_assert(!A::EULERS_JIG(64,100,7,10).valid);
static_assert(!A::EULERS_JIG(0,0,0,0).valid);
static_assert(A::EULERS_JIG(100,64,10,8).compensated==34);
static_assert(A::EULERS_JIG(64,64,8,8).compensated==0);
static_assert(A::EULERS_JIG(0,100,0,10).compensated==-90);
static_assert(A::EULERS_JIG(4294836225U,0,65535,0).valid);
static_assert(!A::EULERS_JIG(0,0,M,M).valid);
constexpr auto branches=A::de_radicalize(64,100,8,10);
static_assert(branches.pp.valid && branches.pp.correction==2 && branches.pp.compensated==-34);
static_assert(branches.nn.valid && branches.nn.correction==-2 && branches.nn.compensated==-38);
static_assert(branches.pn.valid && branches.pn.correction==-18 && branches.pn.compensated==-54);
static_assert(branches.np.valid && branches.np.correction==18 && branches.np.compensated==-18);
static_assert(!A::de_radicalize(64,64,8,8).pn.valid);
static_assert(!A::de_radicalize(0,0,0,0).pp.valid);
static_assert(A::de_radicalize(0,100,0,10).pp.compensated==
              A::de_radicalize(0,100,0,10).np.compensated);
static_assert(!A::de_radicalize(64,100,7,10).pp.valid);
int main() {
    for(A::U x=0;x<64;++x)
        for(A::U y=0;y<64;++y)
            for(unsigned s=0;s<4;++s) {
                const auto j=A::oriented(x*x,y*y,x,y,(s&1u)!=0,(s&2u)!=0);
                const A::I a=(s&1u)?-A::I(x):A::I(x);
                const A::I b=(s&2u)?-A::I(y):A::I(y);
                if(a+b==0) { if(j.valid)return 1; continue; }
                if(!j.valid || j.remainder || j.correction!=b-a ||
                   j.correction*j.divisor!=A::I(y)*y-A::I(x)*x ||
                   j.compensated!=A::I(x)*x-A::I(y)*y+b-a)return 1;
            }

    for(A::U x=0;x<64;++x)
        for(A::U y=0;y<64;++y) {
            const auto j=A::EULERS_JIG(x*x,y*y,x,y);
            if(x+y==0) { if(j.valid)return 1; continue; }
            if(!j.valid || j.remainder ||
               j.correction!=A::I(y)-A::I(x) ||
               j.compensated!=A::I(x)*x-A::I(y)*y+A::I(y)-A::I(x))
                return 1;
        }

    for(A::U b=0;b<32;++b)
        for(A::U r=0;r<32;++r)
            for(A::U o=0;o<32;++o) {
                const A::Count x{b,C::classified}, y{o,C::classified};
                const auto w=A::evaluate(x,r,y);
                if(!A::audit(w,x,r,y) ||
                   w.gain+w.shortfall!=A::I(r) ||
                   w.attenuated!=A::I(o)+A::I(r)-2*A::I(b))
                    return 1;
            }
    return 0;
}
