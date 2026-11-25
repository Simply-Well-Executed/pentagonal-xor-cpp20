// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// REDNECKENGINEERED MODEL-BIDI | EXPLORATORY | RELEASE=HOLD
// Preserve the user's MODEL->BAKE->BITMAP->COOK->CUBEMAP->DIFZ->PING
// ->FINALIZEDMODELREADYFORPUBLISHING order, and audit it in reverse.
// This is evidence traversal, not an invented inverse of lossy baking.
// Source tokens are external witness IDs, never computed from a model here.
#ifndef SWE_REDNECKENGINEERED_MODEL_BIDI_HPP
#define SWE_REDNECKENGINEERED_MODEL_BIDI_HPP
#include <array>
#include <cstdint>
namespace swe::redneckengineered::model_bidi {
using u64=std::uint64_t;
enum class Stage : unsigned {
    MODEL,BAKE,BITMAP,COOK,CUBEMAP,DIFZ,PING,
    FINALIZEDMODELREADYFORPUBLISHING
};
inline constexpr unsigned stage_count=8u;
inline constexpr std::array<Stage,stage_count> forward_order{
    Stage::MODEL,Stage::BAKE,Stage::BITMAP,Stage::COOK,
    Stage::CUBEMAP,Stage::DIFZ,Stage::PING,
    Stage::FINALIZEDMODELREADYFORPUBLISHING
};
struct Witness {
    Stage stage{};
    u64 input_token{};
    u64 output_token{};
    bool recorded{};
    bool independently_qualified{};
};
struct Trace {
    std::array<Witness,stage_count> stages{};
    bool forward_ping{};
    bool backward_ping{};
    bool original_token_traced{};
    bool publish_authorized{}; // May never be inferred from traversal alone.
};
[[nodiscard]] constexpr bool forward_ping(const Trace& trace) noexcept {
    for(unsigned i=0;i<stage_count;++i) {
        const Witness& w=trace.stages[i];
        if(w.stage!=forward_order[i] || !w.recorded ||
           w.input_token==0u || w.output_token==0u)
            return false;
        if(i!=0u && trace.stages[i-1u].output_token!=w.input_token)
            return false;
    }
    return true;
}
// Reverse traversal checks the stored chain of witnesses. It does not
// reconstruct a model from a cubemap, undo a bake, or declare DIFZ invertible.
[[nodiscard]] constexpr bool backward_ping(const Trace& trace) noexcept {
    for(unsigned i=stage_count;i>0u;--i) {
        const Witness& w=trace.stages[i-1u];
        if(w.stage!=forward_order[i-1u] || !w.recorded ||
           w.input_token==0u || w.output_token==0u)
            return false;
        if(i<stage_count && w.output_token!=trace.stages[i].input_token)
            return false;
    }
    return true;
}
[[nodiscard]] constexpr bool individual_ping(const Trace& trace,
                                             Stage stage) noexcept {
    const unsigned i=static_cast<unsigned>(stage);
    return i<stage_count && trace.stages[i].stage==stage &&
           trace.stages[i].recorded &&
           trace.stages[i].independently_qualified;
}
[[nodiscard]] constexpr bool all_individual_pings(const Trace& trace) noexcept {
    for(unsigned i=0u;i<stage_count;++i)
        if(!individual_ping(trace,forward_order[i]))return false;
    return true;
}
[[nodiscard]] constexpr Trace bidi(const Trace& source) noexcept {
    Trace result=source;
    result.forward_ping=forward_ping(source);
    result.backward_ping=backward_ping(source);
    result.original_token_traced=result.forward_ping&&result.backward_ping&&
        source.stages[0u].input_token!=0u;
    // These synthetic witness IDs do not prove model processing or authorize
    // publishing. No source-defined DIFZ, asset baker, or renderer connected.
    result.publish_authorized=false;
    return result;
}
static_assert(forward_order[0]==Stage::MODEL);
static_assert(forward_order[1]==Stage::BAKE);
static_assert(forward_order[2]==Stage::BITMAP);
static_assert(forward_order[3]==Stage::COOK);
static_assert(forward_order[4]==Stage::CUBEMAP);
static_assert(forward_order[5]==Stage::DIFZ);
static_assert(forward_order[6]==Stage::PING);
static_assert(forward_order[7]==Stage::FINALIZEDMODELREADYFORPUBLISHING);
static_assert(stage_count==8u);
} // namespace swe::redneckengineered::model_bidi
#endif
