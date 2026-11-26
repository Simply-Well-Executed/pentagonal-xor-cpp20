// SPDX-License-Identifier: 0BSD
// SEAL OF SEALS OF SEALS: one root, one branch, one terminal page.
// Every action remains reachable; the fixed control AIRGAP never collapses.
#ifndef SWE_DIAERY_ACCESSIBLE_SEALS_HPP
#define SWE_DIAERY_ACCESSIBLE_SEALS_HPP
#include "accessible_actions.hpp"
namespace swe::diaery::seals {
constexpr unsigned ROOTS=2u,BRANCHES=2u,PAGES=5u,TERMINALS=4u;
constexpr unsigned VISIBLE_TERMINALS=TERMINALS;
constexpr unsigned COVERAGE=ROOTS*BRANCHES*PAGES*TERMINALS;
struct Selection {
 unsigned root,branch,page;
 bool expanded;
};
[[nodiscard]] constexpr bool valid(Selection s) noexcept {
 return s.root<ROOTS&&s.branch<BRANCHES&&s.page<PAGES;
}
[[nodiscard]] constexpr unsigned index(Selection s,unsigned terminal) noexcept {
 if(!valid(s)||terminal>=TERMINALS)return access::COUNT;
 return (((s.root*BRANCHES+s.branch)*PAGES+s.page)*TERMINALS)+terminal;
}
[[nodiscard]] constexpr bool visible(Selection s,unsigned action) noexcept {
 if(!s.expanded||action>=access::COUNT||!valid(s))return false;
 const unsigned first=index(s,0u);
 return action>=first&&action<first+TERMINALS;
}
[[nodiscard]] constexpr Selection choose_root(Selection s,unsigned root) noexcept {
 if(root>=ROOTS)return s;
 return {root,0u,0u,true};
}
[[nodiscard]] constexpr Selection choose_branch(Selection s,unsigned branch) noexcept {
 if(branch>=BRANCHES)return s;
 if(s.branch==branch && s.expanded)return {s.root,s.branch,s.page,false};
 return {s.root,branch,0u,true};
}
[[nodiscard]] constexpr Selection choose_page(Selection s,unsigned page) noexcept {
 if(page>=PAGES)return s;
 return {s.root,s.branch,page,true};
}
[[nodiscard]] constexpr bool contract() noexcept {
 if(COVERAGE<access::COUNT)return false;
 for(unsigned i=0;i<access::COUNT;++i){
   const unsigned p=i/TERMINALS;
   const Selection s{p/(BRANCHES*PAGES),(p/PAGES)%BRANCHES,p%PAGES,true};
   if(!valid(s)||!visible(s,i))return false;
   unsigned count=0u;
   for(unsigned j=0u;j<access::COUNT;++j)if(visible(s,j))++count;
   if(count>VISIBLE_TERMINALS)return false;
 }
 return true;
}
}
#endif
