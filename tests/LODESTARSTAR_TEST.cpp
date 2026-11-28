/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/LODESTARSTAR.hpp"
using T=SWE::THAILONG;
constexpr bool test() {
    T::RECONICIOUS r{};
    for(unsigned i=0;i<9;++i) r[i]={i,true,true,true,true,true,true};
    r[0].exact_0bsd=false; r[1].notice=false; r[2].ledger=false;
    const auto s=T::THAILONGA(r);
    if(T::LODESTAR(s,0)!=T::ACTION::HOLD || T::LODESTAR(s,1)!=T::ACTION::RESTORE_NOTICE ||
       T::LODESTAR(s,2)!=T::ACTION::LEDGER_ONLY || T::LODESTAR(s,3)!=T::ACTION::NONE) return false;
    for(unsigned m=0;m<=T::FULL;++m) {
        T::LODESTARSTARHATCH h(s,m);
        if(h.YANG(false)||!h.YANG(true)||h.attachment()!=0||h.YANG(true)) return false;
        auto idea=T::UNPING(T::PING(m,341));
        if(h.YIN(false,idea)||h.YIN(true,{m^1u,true})||!h.YIN(true,idea)||
           h.attachment()!=m||h.detached()||!(h.RECONICIOUS_RECORD()==s)) return false;
    }
    r[8].id=r[0].id;
    if(T::THAILONGA(r).valid || T::UNPING(T::PING(512,0)).valid) return false;
    constexpr T::Assessment a{{9,9,9,9,7,8,8,2,4},{0,0,0,0,0,0}};
    constexpr auto p=T::PROJECT(a);
    return p.valid && p.numerator==65 && p.denominator==9;
}
static_assert(test());
constexpr bool selves_test() {
    constexpr unsigned thought[9]={9,9,9,9,7,8,8,2,4};
    constexpr auto s=T::THAILONGA(thought,{1,2,3,4,5,6});
    for(unsigned i=0;i<9;++i) if(s.dimensions[i]!=thought[i]) return false;
    if(s.supplementary.STR!=1 || s.supplementary.AGI!=2 ||
       s.supplementary.VIT!=3 || s.supplementary.INT!=4 ||
       s.supplementary.DEX!=5 || s.supplementary.LUK!=6) return false;
    for(unsigned p=0;p<360;++p) {
        auto q=T::PINGZA(T::PING(T::Calibration{p,true}));
        auto r=T::PING(T::PINGZA(T::Calibration{p,true}));
        if(!q.valid || !r.valid || q.phase!=p || r.phase!=p) return false;
    }
    return !T::PINGZA({360,true}).valid && !T::PINGZA({0,false}).valid &&
           T::PINGZA({0,true}).phase==261;
}
static_assert(selves_test());
constexpr bool composition_test() {
    constexpr auto nested=SWE::THAILONGA{SWE::THAILONGA{1u,2u},SWE::THAILONGA{3u,4u}};
    if(nested.left.right!=2 || nested.right.left!=3) return false;
    constexpr T::SELVES s{{0,1,2,3,4,5,6,7,8},{11,12,13,14,15,16}};
    T::RECONICIOUS p{};
    for(unsigned i=0;i<9;++i) p[i]={i,true,true,true,true,true,true};
    const auto star=T::THAILONGA(p);
    const auto tree=T::COMPOSE(star,s);
    const auto restored=T::RESTORE(tree.right);
    for(unsigned i=0;i<9;++i) if(restored.dimensions[i]!=s.dimensions[i]) return false;
    if(!(tree.left==star) || restored.supplementary.STR!=11 || restored.supplementary.AGI!=12 ||
       restored.supplementary.VIT!=13 || restored.supplementary.INT!=14 ||
       restored.supplementary.DEX!=15 || restored.supplementary.LUK!=16) return false;
    constexpr SWE::THAILONGA_TRIAL<unsigned> failed{7,9,false,true};
    constexpr SWE::THAILONGA_TRIAL<unsigned> denied{7,9,true,false};
    constexpr SWE::THAILONGA_TRIAL<unsigned> retry{7,8,true,true};
    return failed.selected()==7 && failed.candidate==9 && denied.selected()==7 &&
           retry.selected()==8 && retry.baseline==7;
}
static_assert(composition_test());
int main(){return test() && selves_test() && composition_test()?0:1;}
