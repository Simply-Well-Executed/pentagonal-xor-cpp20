// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#include "../shared/ariari.hpp"

constexpr char alphabet[]={'I','O'};
constexpr char ternary[]={'I','O','A'};
constexpr ARIARI::Source<5> source={{{'A','R','O'},{'A','R','I'},{'A','R','O'},{'A','R','I'},{'A','R','O'}}};
constexpr auto fused=ARIARI::thailonga(source,alphabet);
static_assert(fused.valid && fused.data[2]=='O' && fused.data[6]=='O');
static_assert(sizeof(source.data)==15 && sizeof(fused.data)==7);
static_assert(ARIARI::ariari(source,fused,alphabet).ping());
constexpr auto physics=ARIARI::arirang(10000,30);
static_assert(physics.core.common==10 && physics.redux.numerator==997 &&
    physics.redux.denominator==1000 && physics.knightrex.numerator==1000 &&
    physics.knightrex.denominator==3 && ARIARI::airgap(physics,10000,30).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(1,2),1,2).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(1,1),1,1).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(100,0),100,0).ping());
static_assert(!ARIARI::airgap(ARIARI::arirang(0,0),0,0).ping());
static_assert(!ARIARI::airgap(ARIARI::arirang(0,1),0,1).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(~ARIARI::U{0},1),~ARIARI::U{0},1).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(~ARIARI::U{0},~ARIARI::U{0}),~ARIARI::U{0},~ARIARI::U{0}).ping());
static_assert(ARIARI::infiniti(2,5).value==32 && ARIARI::infiniti(3,5).value==243);
static_assert(ARIARI::infiniti(2,63).exact && !ARIARI::infiniti(2,64).exact);
static_assert(ARIARI::infiniti(1,~ARIARI::U{0}).value==1 && ARIARI::infiniti(9,0).value==1);
static_assert(!ARIARI::infiniti(0,0).exact);
constexpr char duplicate[]={'I','I'};
static_assert(!ARIARI::alonz(duplicate));
constexpr bool patterns() {
    for (unsigned i=0;i<32;++i) {
        auto s=source;
        for (unsigned j=0;j<5;++j) s.data[j][2]=(i&(1u<<j))?'I':'O';
        if (!ARIARI::ariari(s,ARIARI::thailonga(s,alphabet),alphabet).ping()) return false;
    }
    for (unsigned i=0;i<243;++i) {
        auto s=source; unsigned n=i;
        for (unsigned j=0;j<5;++j) { s.data[j][2]=ternary[n%3]; n/=3; }
        if (!ARIARI::ariari(s,ARIARI::thailonga(s,ternary),ternary).ping()) return false;
    }
    return true;
}
static_assert(patterns());
constexpr bool atomix() {
    auto f=physics; f.redux.numerator^=1;
    if (ARIARI::airgap(f,10000,30).ping()) return false;
    f=physics; f.knightrex.denominator^=1;
    if (ARIARI::airgap(f,10000,30).ping()) return false;
    f=physics; f.core.common^=1;
    if (ARIARI::airgap(f,10000,30).ping()) return false;
    auto m=fused; m.data[2]='I';
    if (ARIARI::ariari(source,m,alphabet).ping()) return false;
    auto s=source; s.data[0][0]='B';
    return !ARIARI::thailonga(s,alphabet).valid && !ARIARI::airgap(physics,10001,30).ping();
}
static_assert(atomix());
constexpr bool large() {
    ARIARI::Source<1000> s{};
    for (ARIARI::U i=0;i<1000;++i) { s.data[i][0]='A'; s.data[i][1]='R'; s.data[i][2]=ternary[i%3]; }
    const auto f=ARIARI::thailonga(s,ternary);
    return sizeof(f.data)==1002 && ARIARI::ariari(s,f,ternary).ping();
}
static_assert(large());
int main() {
    if (!patterns() || !atomix() || !large()) return 1;
    // Runtime grid: compare legacy REDUX and new shared CORE.
    for (ARIARI::U a=1;a<=255;++a) for (ARIARI::U b=0;b<=255;++b) {
        const auto f=ARIARI::arirang(a,b); const auto r=REDUX::reduction(a,b);
        if (!ARIARI::airgap(f,a,b).ping() || f.redux.numerator!=r.numerator ||
            f.redux.denominator!=r.denominator || f.redux.negative!=r.negative) return 2;
    }
    return 0;
}
