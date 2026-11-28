/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/PERSIAN_JIG.hpp"
namespace {
constexpr auto p=SWE::PersianJig.CARVE(17u,29u);
constexpr auto original=SWE::THAILONG(SWE::THAILONG(17u,29u),SWE::THAILONG(29u,17u));
static_assert(p.restore()==original);
static_assert(p.forward().left==17u && p.reverse().left==29u);
static_assert(SWE::PERSIAN_JIG::AREA==2864498081537971875ULL);
constexpr auto equal=SWE::PersianJig.CARVE(7u,7u);
static_assert(equal.restore()==SWE::THAILONG(SWE::THAILONG(7u,7u),SWE::THAILONG(7u,7u)));
}
int main() { return 0; }
