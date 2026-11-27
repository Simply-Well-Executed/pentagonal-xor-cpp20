/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// PENTAPI: accessibility-oriented five-stage single-switch navigation.
// ROOT -> BRANCH -> TERMINAL -> NULL -> RESET -> ROOT.
// An invoked terminal action automatically returns to ROOT; retain evidence.
// NULL is an inactive selection, never data deletion. No extra input required
// to exit a completed terminal action.
#ifndef SWE_DIAERY_PENTAPI_HPP
#define SWE_DIAERY_PENTAPI_HPP
#include "accessible_seals.hpp"
namespace swe::diaery::pentapi {
enum class Phase : unsigned { Root, Branch, Terminal, XorAirgapOr, Null, Reset };
struct State {
 seals::Selection selection;
 Phase phase;
 unsigned last_action;
 unsigned long long completed;
 bool has_completed;
 bool last_action_succeeded;
};
[[nodiscard]] constexpr State initial() noexcept {
 return {{0u,0u,0u,false},Phase::Root,0u,0ULL,false,false};
}
[[nodiscard]] constexpr State root(State s,unsigned selected) noexcept {
 if(selected>=seals::ROOTS)return s;
 s.selection=seals::choose_root(s.selection,selected);
 s.selection.expanded=false; // terminal controls not yet exposed
 s.phase=Phase::Branch;
 return s;
}
[[nodiscard]] constexpr State branch(State s,unsigned selected) noexcept {
 if(s.phase!=Phase::Branch||selected>=seals::BRANCHES)return s;
 s.selection=seals::choose_branch(s.selection,selected);
 s.selection.page=0u;
 s.selection.expanded=true;
 s.phase=Phase::Terminal;
 return s;
}
[[nodiscard]] constexpr State page(State s,unsigned selected) noexcept {
 if(s.phase!=Phase::Terminal||selected>=seals::PAGES)return s;
 s.selection=seals::choose_page(s.selection,selected);
 return s;
}
[[nodiscard]] constexpr bool terminal_visible(State s,unsigned action) noexcept {
 return s.phase==Phase::Terminal && seals::visible(s.selection,action);
}
// A deliberately invoked terminal action returns automatically to ROOT.
// NULL and RESET affect navigation only; no additional activation is needed.
[[nodiscard]] constexpr State complete(State s,unsigned action,
                                         bool succeeded) noexcept {
 if(!terminal_visible(s,action))return s;
 s.phase=Phase::Null;
 s.selection.expanded=false;
 s.last_action=action;
 s.has_completed=true;
 s.last_action_succeeded=succeeded;
 if(s.completed!=~0ULL)++s.completed;
 s.phase=Phase::Reset;
 s.selection={0u,0u,0u,false};
 s.phase=Phase::Root;
 return s;
}
// Historical explicit-route API retained for source compatibility; complete()
// no longer produces XorAirgapOr. Not an active navigation requirement.
enum class Route : unsigned { ReleaseToRoot, ContinueBranch };
[[nodiscard]] constexpr State choose_or(State s,Route route) noexcept {
 if(s.phase!=Phase::XorAirgapOr)return s;
 if(route==Route::ContinueBranch){
    s.phase=Phase::Terminal;
    s.selection.expanded=true;
    return s;
 }
 // The ☯️ release is only the navigation selection, NOT measurements.
 s.phase=Phase::Null;
 s.selection.expanded=false;
 s.phase=Phase::Reset;
 s.selection={0u,0u,0u,false};
 s.phase=Phase::Root;
 return s;
}
[[nodiscard]] constexpr State reset(State s) noexcept {
 s.phase=Phase::Null;
 s.selection.expanded=false;
 s.phase=Phase::Reset;
 s.selection={0u,0u,0u,false};
 s.phase=Phase::Root;
 return s;
}
[[nodiscard]] constexpr bool invariant(State s) noexcept {
 return seals::valid(s.selection) &&
  (s.phase==Phase::Terminal?s.selection.expanded:!s.selection.expanded);
}
} // namespace swe::diaery::pentapi
#endif
