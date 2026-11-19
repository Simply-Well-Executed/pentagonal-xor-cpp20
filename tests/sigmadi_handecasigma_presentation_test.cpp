// SPDX-License-Identifier: 0BSD
#include "../shared/sigmadi_handecasigma_presentation.hpp"
#include <iostream>
using namespace swe::sigma_penta_ping;
using namespace swe::han_presentation;
int main() {
    Input fixture{};
    for(unsigned n=0;n<loops;++n) {
        fixture.samples[n].pixel={0,64,128,192,255};
        fixture.source_tokens[n]=900u+n;
    }
    Report source{};
    if(!execute(fixture,source)) return 1;
    const View v=present(source);
    if(!v.nine_loop_source_valid || !v.presentation_only ||
       v.has_independent_authentication || v.relativismos_defined ||
       v.empathy_multiplier_measured || v.chirality.h11_available ||
       v.chirality.s11_available) return 2;
    for(unsigned n=0;n<loops;++n) {
        if(v.sigma[n].loop_index!=n ||
           v.sigma[n].twirl_reversed!=source.loop[n].route_reversed_before)
            return 3;
        const int sign=v.sigma[n].twirl_reversed?-1:1;
        if(v.sigma[n].q_polarity!=std::array<int,4>{sign,-sign,sign,-sign})
            return 4;
    }
    for(unsigned i=0;i<virtual_vietquang_count;++i) {
        const auto& lens=v.virtual_views[i];
        if(lens.index!=i || lens.trace_index!=i%9 ||
           lens.variation!=i/9 || lens.independent_evidence) return 5;
    }
    if(v.language_views[0].code[0]!='e' ||
       v.language_views[9].code[0]!='v' ||
       v.audit_names[8]!=eye_names[8]) return 6;
    const auto missing=inspect_chirality(Chirality{});
    if(missing.h11_available || missing.s11_available) return 7;
    const auto h=inspect_chirality(Chirality{76,72,true,false,0});
    if(!h.h11_available || h.s11_available || h.divisor<1.0 ||
       h.divisor>2.0 || h.accepted_turns!=148) return 8;
    const auto wrong_s10=inspect_chirality(Chirality{76,72,true,true,0.5});
    if(wrong_s10.s11_available) return 9;
    const auto empty=inspect_chirality(Chirality{0,0,true,false,0});
    if(empty.h11_available || empty.s11_available) return 10;
    std::cout << "PASS|HAN-PRESENTATION|osi_layer=6|eyes=9|languages=10|"
              << "virtual_lenses=108|nine_loop_indices=0..8|"
              << "s10_s11_uncomputed=1|independent_virtual_evidence=0\n";
    return 0;
}
