// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#include "../shared/ariari_toolong.hpp"
int main() {
    using namespace swe;
    const toolong a{"10000000000000000000000000000000000000000"};
    const toolong b{"30000000000000000000000000000000000000"};
    const auto e=ariari::compose(deca_sigma::run(2.0),a,b,"3.2");
    if (!e.coreao_witness.passed() || !ariari::ariari(e.arithmetic,a,b)) return 1;
    if (e.arithmetic.core.a!=toolong{1000} || e.arithmetic.core.b!=toolong{3} ||
        e.arithmetic.redux_numerator!=toolong{997}) return 2;
    if (!e.gamma.exact() || e.gamma.magnitude!="4" || e.gamma.source!="3.2") return 3;
    const auto symbolic=ariari::compose(deca_sigma::run(2.0),a,b,"3.14159265358979323");
    if (symbolic.gamma.status!=gammagamma::Status::symbolic_only) return 4;
    auto bad=e.arithmetic; bad.core.common+=toolong{1};
    if (ariari::ariari(bad,a,b)) return 5;
    if (!ariari::ariari(ariari::thailonga(a,toolong{0}),a,toolong{0})) return 6;
    if (ariari::ariari(ariari::thailonga(toolong{0},b),toolong{0},b)) return 7;
    return 0;
}
