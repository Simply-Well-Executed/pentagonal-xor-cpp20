// SPDX-License-Identifier: 0BSD
// PENTAPI: accessibility-oriented five-stage single-switch navigation.
// ROOT -> BRANCH -> TERMINAL -> NULL -> RESET -> ROOT.
// NULL is an inactive selection, never data deletion. No extra input required
// to exit a completed terminal action.
#ifndef SWE_DIAERY_PENTAPI_HPP
#define SWE_DIAERY_PENTAPI_HPP
#include "accessible_seals.hpp"
namespace swe::diaery::pentapi {
enum class Phase : unsigned { Root, Branch, Terminal, Null, Reset };
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
// Completing an action returns immediately to Root without requiring a
// second switch click. The transition visits NULL and RESET conceptually;
// the last action/result remains available to the audit, without clearing
// measurements, sample rows or input selections.
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
