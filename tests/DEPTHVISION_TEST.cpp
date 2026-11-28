/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/DEPTHVISION.hpp"
using D=SWE::DEPTHVISION;
constexpr D::Bytes l{"abc",3}, p{"notice:",7}, r{"notice:abc",10};
constexpr auto w=D::PEAKTHROUGH(l,r,p);
static_assert(w.state==D::State::ATTRIBUTION_PREFIX && w.compared==10);
static_assert(D::GLINT(l,l,p).state==D::State::IDENTICAL);
static_assert(D::GLINT(l,{"notice:abd",10},p).state==D::State::DIFFERENT);
static_assert(D::GLINT(l,{"ab",2},p).compared==0);
static_assert(D::GLINT({nullptr,1},r,p).state==D::State::HOLD);
static_assert(D::GLINT({nullptr,0},{nullptr,0},p).state==D::State::IDENTICAL);
static_assert(D::GLINT({"a\0b",3},{"a\0c",3},p).state==D::State::DIFFERENT);
static_assert(D::GLINT(l,{".abc",4},p).offset==0);
static_assert(D::GLINT(l,{"a.bc",4},p).offset==1);
static_assert(D::GLINT(l,{"abc.",4},p).offset==3);
static_assert(D::GLINT(l,{"a.bc",4},p).state==D::State::EXTRA_DOT_RIGHT);
static_assert(D::GLINT(l,{"a.bd",4},p).state==D::State::DIFFERENT);
static_assert(D::GLINT({"a.b",3},{"a..b",4},p).state==D::State::EXTRA_DOT_RIGHT);
constexpr auto prior=D::GLINT(l,{"ab",2},p);
constexpr auto recovery=D::THAILONGA(prior,l,r,p);
static_assert(recovery.prior.state==D::State::DIFFERENT &&
              recovery.observed.state==D::State::ATTRIBUTION_PREFIX);
int main(){
    char candidate[]="notice:abc";
    for(D::Size i=0;i<10;++i){
        const char old=candidate[i];candidate[i]='?';
        if(D::GLINT(l,{candidate,10},p).state!=D::State::DIFFERENT)return 1;
        candidate[i]=old;
    }
    return D::GLINT(l,{candidate,10},p).state==D::State::ATTRIBUTION_PREFIX?0:2;
}
