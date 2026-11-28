/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#pragma once
namespace SWE {
struct SWEATER final {
    using COUNT=unsigned long long;
    struct METRIC { COUNT value; bool valid; };
    struct SAMPLE {
        METRIC private_commit, working_set, peak_working_set, handles, threads;
    };
    struct CHANGE { COUNT magnitude; bool grew; bool valid; };
    static constexpr CHANGE GROWTH(METRIC a,METRIC b) noexcept {
        if(!a.valid || !b.valid) return {};
        return {b.value>=a.value ? b.value-a.value : a.value-b.value,
                b.value>a.value,true};
    }
    static constexpr METRIC HIGHWATER(METRIC a,METRIC b) noexcept {
        if(!a.valid) return b;
        if(!b.valid) return a;
        return a.value>=b.value?a:b;
    }
    struct WITNESS {
        SAMPLE first,last;
        int outcome;
        bool operation_ran;
        bool counter_valid;
    };
    static SAMPLE MEASURE() noexcept;
    static bool REPORT(const WITNESS&) noexcept;
};
}
