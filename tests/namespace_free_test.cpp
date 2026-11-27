// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#include "../shared/LCD.hpp"
#include "../shared/redux.hpp"
#include "../shared/ariari.hpp"
#include "../docs/experiments/genshin_bias_detector.hpp"
#include "../docs/experiments/genshin_bidi_provenance.hpp"
#include "../docs/experiments/genshin_tripi_provenance.hpp"
#include "../docs/experiments/genshin_zipit.hpp"

// One translation unit: prove that the separate type scopes coexist with LCD.
static_assert(LCD(15,7).a==15 && LCD(15,7).c==7);
static_assert(REDUX::reduction(15,7).numerator==8);
static_assert(ARIARI::airgap(ARIARI::arirang(15,7),15,7).ping());
static_assert(genshin_zipit::ZIPITDOWN(genshin_zipit::ZIPITDOWN())==genshin_zipit::CORE);
static_assert(genshin_bidi::balanced(nullptr,0));
static_assert(!genshin_bias::bounded(genshin_bias::bound+1));
static_assert(genshin_tripi::inspect(nullptr,0,nullptr,0).passed());

int main() {
    // Independent LCD and fused KNIGHTREX agreement on every positive pair.
    // Zero numerator/denominator semantics differ intentionally.
    for (U a=1;a<=255;++a) for (U c=1;c<=255;++c) {
        const L l=LCD(a,c);
        const auto f=ARIARI::arirang(a,c);
        if (l.a!=f.knightrex.numerator || l.c!=f.knightrex.denominator ||
            !ARIARI::airgap(f,a,c).ping()) return 1;
    }
    return 0;
}
