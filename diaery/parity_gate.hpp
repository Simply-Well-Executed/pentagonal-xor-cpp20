/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Concurrence test: 1:1 state-space manifest equality between the XHTML
// source and the CANONICAL C++20 geometry. No ungrounded visual claims.
#ifndef SWE_DIAERY_PARITY_GATE_HPP
#define SWE_DIAERY_PARITY_GATE_HPP
#ifndef SWE_DIAERY_STRICT_PROFILE_HPP
#error "Load strict_profile.hpp first"
#endif
#ifndef SWE_DIAERY_MODEL_BRIDGE_HPP
#error "Load model_bridge.hpp first"
#endif
namespace swe::diaery::parity {
namespace p=swe::diaery::strict;
namespace model=swe::diaery::geometry;
[[nodiscard]] constexpr p::Builder manifest() noexcept {
    p::Builder b{};
    b.text("PARITY-1|folds=");b.num(model::form::FOLD_COUNT);
    b.text("|wall_triangles=");b.num(model::solid::WALL_TRIANGLES);
    b.text("|cap_triangles=");b.num(model::solid::CAP_TRIANGLES);
    b.text("|faces=");b.num(model::solid::TRIANGLES);
    b.text("|volume=");b.num(model::solid::VOLUME);
    b.text("|surface=");b.num(model::solid::SURFACE_AREA);
    b.text("|visible=");b.num(model::solid::VISIBLE_TRIANGLES(model::solid::CAMERA));
    b.text("|filled=");b.num(model::solid::DIARY_VIEW.fill==model::solid::Fill::Water?1:0);
    return b;
}
[[nodiscard]] constexpr p::Builder stage(unsigned s) noexcept {
    p::Builder b{};b.text("STAGE-");b.num(s);
    for(unsigned k=0;k<5u;++k){
        const unsigned panel=k<4u?k:3u;
        const auto pt=model::form::PROJECT({0,15*static_cast<long long>(k)},panel,s);
        b.chr('|');b.num(pt.y);b.chr(',');b.num(pt.z);
    }
    return b;
}
[[nodiscard]] constexpr bool concurrence(const p::Page& page) noexcept {
    if(!page.structurally_valid||!page.parity_seen)return false;
    const auto expected=manifest();
    if(!expected.ok)return false;
    bool foundManifest=false;
    bool foundStage[4]{};
    for(unsigned i=0;i<page.count;++i){
        const auto& line=page.lines[i];
        if(p::prefix(line.text,"PARITY-1|")){
            if(!p::same(line.text,expected.s))return false;
            foundManifest=true;
        }
        for(unsigned j=0;j<4;++j){
            p::Builder label{};label.text("STAGE-");label.num(j);label.chr('|');
            if(p::prefix(line.text,label.s)){
                const auto calculated=stage(j);
                if(!calculated.ok||!p::same(line.text,calculated.s))return false;
                foundStage[j]=true;
            }
        }
    }
    if(!foundManifest)return false;
    for(unsigned j=0;j<4;++j)if(!foundStage[j])return false;
    return model::solid::WITNESS.valid &&
           model::form::WITNESS.valid &&
           model::tridi::REPORT.valid &&
           model::bidi::REPORT.ping_after_restore;
}
static constexpr auto MANIFEST=manifest();
static_assert(MANIFEST.ok);
static_assert(p::same(MANIFEST.s,"PARITY-1|folds=3|wall_triangles=8|cap_triangles=4|faces=12|volume=13500|surface=4050|visible=6|filled=1"));
static constexpr auto STAGE_0=stage(0u);
static constexpr auto STAGE_1=stage(1u);
static constexpr auto STAGE_2=stage(2u);
static constexpr auto STAGE_3=stage(3u);
static_assert(p::same(STAGE_0.s,"STAGE-0|0,0|15,0|30,0|45,0|60,0"));
static_assert(p::same(STAGE_1.s,"STAGE-1|0,0|15,0|15,15|15,30|15,45"));
static_assert(p::same(STAGE_2.s,"STAGE-2|0,0|15,0|15,15|0,15|-15,15"));
static_assert(p::same(STAGE_3.s,"STAGE-3|0,0|15,0|15,15|0,15|0,0"));
}
#endif
