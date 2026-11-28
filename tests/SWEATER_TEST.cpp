/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/SWEATER.hpp"
using S=SWE::SWEATER;
static_assert(S::GROWTH({10,true},{30,true}).magnitude==20);
static_assert(S::GROWTH({10,true},{30,true}).grew);
static_assert(!S::GROWTH({30,true},{10,true}).grew);
static_assert(S::GROWTH({30,true},{10,true}).magnitude==20);
static_assert(!S::GROWTH({0,false},{10,true}).valid);
static_assert(S::GROWTH({~0ULL,true},{0,true}).magnitude==~0ULL);
static_assert(S::HIGHWATER({20,true},{10,true}).value==20);
int main() {
    const auto a=S::MEASURE(); const auto b=S::MEASURE();
    const bool valid=a.private_commit.valid && b.private_commit.valid &&
        a.working_set.valid && b.working_set.valid &&
        a.peak_working_set.valid && b.peak_working_set.valid &&
        a.handles.valid && b.handles.valid && a.threads.valid && b.threads.valid;
    return valid && S::REPORT({a,b,0,true,true}) ? 0 : 1;
}
