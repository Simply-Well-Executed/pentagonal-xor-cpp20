/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#ifndef SWE_DEPTHVISION_HPP
#define SWE_DEPTHVISION_HPP
namespace SWE {
struct DEPTHVISION final {
    using Size = decltype(sizeof(0));
    struct Bytes { const char* data; Size size; };
    enum class State { HOLD, IDENTICAL, ATTRIBUTION_PREFIX, EXTRA_DOT_RIGHT, DIFFERENT };
    struct Witness { State state; Size left, right, offset, compared; };
    static constexpr bool valid(Bytes a) noexcept {
        return a.data != nullptr || a.size == 0;
    }
    static constexpr Witness GLINT(Bytes l, Bytes r, Bytes p) noexcept {
        if(!valid(l)||!valid(r)||!valid(p))
            return {State::HOLD,l.size,r.size,0,0};
        const bool prefix=p.size!=0 && r.size>=p.size && r.size-p.size==l.size;
        const bool dot=r.size>l.size && r.size-l.size==1;
        const bool equal=l.size==r.size;
        if(!prefix&&!dot&&!equal)
            return {State::DIFFERENT,l.size,r.size,0,0};
        // Prefer exact approved prefix, but allow a one-byte dot candidate
        // if the prefix test fails. No normalization or input mutation.
        Size n=0;
        if(prefix){
            Size i=0;
            for(;i<p.size;++i){++n;if(r.data[i]!=p.data[i])break;}
            if(i==p.size){
                Size j=0;
                for(;j<l.size;++j){++n;if(l.data[j]!=r.data[p.size+j])break;}
                if(j==l.size)return {State::ATTRIBUTION_PREFIX,l.size,r.size,p.size,n};
            }
            if(!dot)return {State::DIFFERENT,l.size,r.size,0,n};
        }
        Size i=0;
        for(;i<l.size;++i){++n;if(l.data[i]!=r.data[i])break;}
        if(equal)return {i==l.size?State::IDENTICAL:State::DIFFERENT,l.size,r.size,i,n};
        ++n;
        if(r.data[i]!='.')return {State::DIFFERENT,l.size,r.size,i,n};
        const Size offset=i;
        for(;i<l.size;++i){++n;if(l.data[i]!=r.data[i+1])
            return {State::DIFFERENT,l.size,r.size,offset,n};}
        return {State::EXTRA_DOT_RIGHT,l.size,r.size,offset,n};
    }
    static constexpr Witness PEAKTHROUGH(Bytes l,Bytes r,Bytes p) noexcept {
        return GLINT(l,r,p);
    }
    struct Recovery { Witness prior, observed; };
    static constexpr Recovery THAILONGA(Witness prior,Bytes l,Bytes r,Bytes p) noexcept {
        return {prior,PEAKTHROUGH(l,r,p)};
    }
};
}
#endif
