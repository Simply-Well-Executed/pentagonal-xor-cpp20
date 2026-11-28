/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// THAILONGA ACCESSIBLE ACTION DECK: visible Win32 standard BUTTON controls.
// No hotkey-only operation: every modeled native key action has an Invoke target.
#ifndef SWE_DIAERY_ACCESSIBLE_ACTIONS_HPP
#define SWE_DIAERY_ACCESSIBLE_ACTIONS_HPP
namespace swe::diaery::access {
enum class Kind : unsigned { Key, PresetTrace, CancelTrace, ConfirmTrace, BackspaceTrace, TypeTrace, Rotate, KeyOtherView };
struct Action { const char* label; unsigned virtual_key; Kind kind; };
constexpr Action actions[]{
 {"D VIEW", 'D', Kind::Key},
 {"X - LEFT",37u,Kind::Key}, {"X + RIGHT",39u,Kind::Key},
 {"Y + UP",38u,Kind::Key}, {"Y - DOWN",40u,Kind::Key},
 {"Z + PGUP",33u,Kind::Key}, {"Z - PGDN",34u,Kind::Key},
 {"H FREQ", 'H',Kind::Key}, {"A ADD SAMPLE",'A',Kind::Key},
 {"C COPY AUDIT",'C',Kind::Key}, {"K STUDY GEAR",'K',Kind::Key},
 {"G GRID",'G',Kind::Key}, {"M MINT",'M',Kind::Key},
 {"P NEXT PROJ",'P',Kind::Key}, {"N CIRCLE BAND",'N',Kind::Key},
 {"J GAP CANARY",'J',Kind::Key}, {"T TRACE",'T',Kind::Key},
 {"F +1 FS",'F',Kind::Key}, {"S +1 SEC",'S',Kind::Key},
 {"R EDIT TRACE",'R',Kind::Key},
 {"{0} SOURCE",'0',Kind::Key}, {"{1} INTERCEPT",'1',Kind::Key},
 {"{2} IDENTITY",'2',Kind::Key}, {"{3} BASELINE",'3',Kind::Key},
 {"{4} MEAN",'4',Kind::Key}, {"{5} THRESH",'5',Kind::Key},
 {"{6} ROLLING",'6',Kind::Key}, {"{7} REGRESS",'7',Kind::Key},
 {"{8} CONFIDENCE",'8',Kind::Key}, {"{9} CIRCLE",'9',Kind::Key},
 {"TRACE Y=MX+B",0u,Kind::PresetTrace},
 {"TRACE Y=X",1u,Kind::PresetTrace},
 {"TRACE Y=0",2u,Kind::PresetTrace},
 {"ENTER / APPLY",13u,Kind::ConfirmTrace},
 {"ESC / CANCEL",27u,Kind::CancelTrace},
 {"BACKSPACE",8u,Kind::BackspaceTrace},
 {"TYPE Y",'Y',Kind::TypeTrace}, {"TYPE X",'X',Kind::TypeTrace},
 {"TYPE M",'M',Kind::TypeTrace}, {"TYPE B",'B',Kind::TypeTrace},
 {"TYPE =",'=',Kind::TypeTrace}, {"TYPE +",'+',Kind::TypeTrace},
 {"TYPE {",'{',Kind::TypeTrace}, {"TYPE }",'}',Kind::TypeTrace},
 {"TYPE 0",'0',Kind::TypeTrace}, {"TYPE 1",'1',Kind::TypeTrace},
 {"TYPE 2",'2',Kind::TypeTrace}, {"TYPE 3",'3',Kind::TypeTrace},
 {"TYPE 4",'4',Kind::TypeTrace}, {"TYPE 5",'5',Kind::TypeTrace},
 {"TYPE 6",'6',Kind::TypeTrace}, {"TYPE 7",'7',Kind::TypeTrace},
 {"TYPE 8",'8',Kind::TypeTrace}, {"TYPE 9",'9',Kind::TypeTrace},
 {"W WATER",'W',Kind::KeyOtherView}, {"Y YANG",'Y',Kind::KeyOtherView},
 {"C CUTAWAY",'C',Kind::KeyOtherView}, {"B BUBBLE",'B',Kind::KeyOtherView},
 {"+ ZOOM",187u,Kind::KeyOtherView}, {"- ZOOM",189u,Kind::KeyOtherView},
 {"NEXT STAGE",39u,Kind::KeyOtherView}, {"PREV STAGE",37u,Kind::KeyOtherView},
 {"YAW LEFT",0u,Kind::Rotate}, {"YAW RIGHT",1u,Kind::Rotate},
 {"PITCH UP",2u,Kind::Rotate}, {"PITCH DOWN",3u,Kind::Rotate}
};
constexpr unsigned COUNT=sizeof(actions)/sizeof(actions[0]);
constexpr unsigned BUTTON_WIDTH=109u;
constexpr unsigned BUTTON_HEIGHT=48u;
constexpr unsigned BUTTON_GAP=7u;
constexpr unsigned COLUMNS=6u;
constexpr unsigned ROWS=(COUNT+COLUMNS-1u)/COLUMNS;
[[nodiscard]] constexpr bool contract() noexcept {
 if(BUTTON_WIDTH<40u||BUTTON_HEIGHT<44u||BUTTON_GAP<4u)return false;
 unsigned projection_buttons=0u;
 for(unsigned i=0u;i<COUNT;++i){
   if(!actions[i].label||!actions[i].label[0])return false;
   if(actions[i].kind==Kind::Key&&
      actions[i].virtual_key>='0'&&actions[i].virtual_key<='9')
      ++projection_buttons;
 }
 return projection_buttons==10u&&COUNT>=55u;
}
} // namespace swe::diaery::access
#endif
