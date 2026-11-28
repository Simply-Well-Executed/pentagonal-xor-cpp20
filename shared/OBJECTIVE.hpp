/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#pragma once
#include "THAILONGA.hpp"
namespace SWE {
struct THAILONG_OPERATOR final {
    template<class L,class R>
    constexpr auto operator()(const L& l,const R& r) const {
        return AND(l,r);
    }
};
inline constexpr THAILONG_OPERATOR THAILONG{};
struct OBJECTIVE final {
    template<class X,class Y,class Z> struct TRIANGLE {
        const X first; const Y second; const Z third;
        constexpr auto cross() const {
            return TRIANGLE<Z,X,Y>{third,first,second};
        }
        constexpr auto reverse_cross() const {
            return TRIANGLE<Y,Z,X>{second,third,first};
        }
    };
    template<class M,class T>
    constexpr auto PROCESS(const M& m,const T& t) const {
        return t(t(t(m.SHARIGON,m.SHARIGONE),
                   t(m.REAR_EYE,m.SHARIGUN)),m.CENTRIX);
    }
};
}
