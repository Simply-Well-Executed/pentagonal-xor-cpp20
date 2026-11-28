/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#pragma once
namespace SWE {
struct BAKED final {
    using COUNT=unsigned long long;
    struct REGION {
        COUNT offset,length;
        unsigned long state,protection,type;
        constexpr bool operator==(const REGION&) const noexcept = default;
    };
    // Five independently compared fields; AND is a verdict, not a lock.
    static constexpr bool PENTAGONAL_AND(REGION a,REGION b) noexcept {
        return (a.offset==b.offset) & (a.length==b.length) &
            (a.state==b.state) & (a.protection==b.protection) & (a.type==b.type);
    }
    static constexpr int PSYCHOZIZER(const REGION* before,COUNT count,
        const REGION* after,COUNT observed) noexcept {
        if(!before || !after || !count || count!=observed) return 97;
        COUNT i=0;
        while(i<count) {
            if(!PENTAGONAL_AND(before[i],after[i])) return 97;
            ++i;
        }
        return 0;
    }
    class DIAMONDTHREAD final {
        const REGION* baseline_=nullptr;
        COUNT count_=0;
        COUNT base_=0, extent_=0;
    public:
        DIAMONDTHREAD() noexcept;
        ~DIAMONDTHREAD() noexcept;
        DIAMONDTHREAD(const DIAMONDTHREAD&)=delete;
        DIAMONDTHREAD& operator=(const DIAMONDTHREAD&)=delete;
        bool valid() const noexcept {return baseline_!=nullptr;}
        int CHECK() const noexcept;
    };
};
}
