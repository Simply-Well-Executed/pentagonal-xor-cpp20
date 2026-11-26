// SPDX-License-Identifier: 0BSD
#include "pentapi.hpp"
using namespace swe::diaery::pentapi;
constexpr auto a=initial();
static_assert(invariant(a));
static_assert(a.phase==Phase::Root);
constexpr auto b=root(a,1u);
static_assert(b.phase==Phase::Branch&&!b.selection.expanded);
constexpr auto c=branch(b,1u);
static_assert(c.phase==Phase::Terminal&&c.selection.expanded);
constexpr auto d=page(c,4u);
static_assert(invariant(d));
constexpr unsigned leaf=seals::index(d.selection,0u);
static_assert(terminal_visible(d,leaf));
static_assert(!terminal_visible(d,0u));
constexpr auto e=complete(d,leaf,true);
static_assert(e.phase==Phase::XorAirgapOr&&!e.selection.expanded);
static_assert(e.completed==1ULL&&e.last_action==leaf);
static_assert(e.has_completed&&e.last_action_succeeded);
static_assert(invariant(e));
static_assert(choose_or(e,Route::ContinueBranch).selection.expanded);
static_assert(choose_or(e,Route::ReleaseToRoot).selection.expanded==false);
static_assert(choose_or(e,Route::ReleaseToRoot).phase==Phase::Root);
static_assert(choose_or(e,Route::ContinueBranch).phase==Phase::Terminal);
static_assert(terminal_visible(choose_or(e,Route::ContinueBranch),leaf));
static_assert(!terminal_visible(e,leaf));
static_assert(choose_or(d,Route::ReleaseToRoot).phase==Phase::Terminal);
static_assert(choose_or(e,Route::ReleaseToRoot).has_completed);
static_assert(choose_or(e,Route::ReleaseToRoot).last_action==leaf);
static_assert(complete(d,0u,true).phase==Phase::Terminal);
static_assert(reset(d).phase==Phase::Root);
static_assert(complete(d,leaf,false).last_action_succeeded==false);
static_assert(root(a,seals::ROOTS).phase==Phase::Root);
int main(){return 0;}
