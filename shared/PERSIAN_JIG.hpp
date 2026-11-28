/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#pragma once
#include "eulers_jig_canonical.hpp"
#include "OBJECTIVE.hpp"
namespace SWE {
struct PERSIAN_JIG final {
    static constexpr auto AREA = swe::eulers_jig::ONE_TRIANGLE_X1E17;
    template<class U,class O> struct PERSIA final {
        const U uport;
        const O objectification;
        constexpr auto forward() const { return THAILONG(uport,objectification); }
        constexpr auto reverse() const { return THAILONG(objectification,uport); }
        constexpr auto restore() const { return THAILONG(forward(),reverse()); }
    };
    template<class U,class O>
    constexpr auto CARVE(const U& u,const O& o) const {
        return PERSIA<U,O>{u,o};
    }
};
inline constexpr PERSIAN_JIG PersianJig{};
}
