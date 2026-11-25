// SPDX-License-Identifier: 0BSD
// PentaDi: four independent LadderDi decisions + shared NULL witness.
// EXPLORATORY, source-preserving. NULL is absence of qualification, not PASS.
#ifndef SWE_DIAERY_PENTADI_HPP
#define SWE_DIAERY_PENTADI_HPP
namespace swe::diaery::pentadi {
enum class State { Null, Pass, Fail };
enum class Stage { Rung, Excite, Excited, Attest };
struct Four {
    State rung;
    State excite;
    State excited;
    State attest;
};
struct Result { State common_null; State status; unsigned pass_count; };
[[nodiscard]] constexpr Result evaluate(Four stages) noexcept {
    const State v[4]{stages.rung,stages.excite,stages.excited,stages.attest};
    unsigned count=0u;
    bool failed=false,missing=false;
    for(unsigned i=0;i<4u;++i){
        if(v[i]==State::Fail)failed=true;
        else if(v[i]==State::Null)missing=true;
        else ++count;
    }
    return {State::Null,failed?State::Fail:(missing?State::Null:State::Pass),count};
}
[[nodiscard]] constexpr bool promoted(const Result& r) noexcept {
    return r.common_null==State::Null&&r.status==State::Pass&&r.pass_count==4u;
}
// Shared NULL does not get counted as a PASS; it remains explicitly present.
} // namespace swe::diaery::pentadi
#endif
