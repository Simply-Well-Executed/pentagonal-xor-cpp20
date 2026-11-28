/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/WATCHGUARD.hpp"
using W=SWE::WATCHGUARD;
constexpr bool verify() {
    W w;
    if(w.REFLECT() || w.CONTINUE()) return false;
    if(!w.RECORD({9,true},{8,true},0) || w.failures()!=1 || w.latest().valid) return false;
    if(w.RECORD({0,true},{1,true},0) || !w.REFLECT() || w.ADAPT(false) ||
       !w.ADAPT(true) || !w.CONTINUE()) return false;
    if(!w.RECORD({8,true},{8,true},0) || !w.REFLECT() || !w.ADAPT(true) || !w.CONTINUE()) return false;
    return w.INTEGRITY() && w.attempts()==2 && w.successes()==1 && w.failures()==1;
}
static_assert(verify());
int main() {
    const auto a=SWE::WATCHGUARD_COUNTER();
    const auto b=SWE::WATCHGUARD_COUNTER();
    return verify() && W::VALID(a,b) ? 0 : 1;
}
