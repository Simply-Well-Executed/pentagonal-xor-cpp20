// SPDX-License-Identifier: 0BSD
// THAILONGA ACCESSIBLE ACTION DECK: visible Win32 standard BUTTON controls.
// No hotkey-only operation: every modeled native key action has an Invoke target.
#ifndef SWE_DIAERY_ACCESSIBLE_ACTIONS_HPP
#define SWE_DIAERY_ACCESSIBLE_ACTIONS_HPP
namespace swe::diaery::access {
enum class Kind : unsigned { Key, PresetTrace, CancelTrace, ConfirmTrace, BackspaceTrace };
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
 {"W WATER",'W',Kind::Key}, {"Y YANG",'Y',Kind::Key},
 {"C CUTAWAY",'C',Kind::Key}, {"B BUBBLE",'B',Kind::Key},
 {"+ ZOOM",187u,Kind::Key}, {"- ZOOM",189u,Kind::Key},
 {"NEXT STAGE",39u,Kind::Key}, {"PREV STAGE",37u,Kind::Key}
};
constexpr unsigned COUNT=sizeof(actions)/sizeof(actions[0]);
constexpr unsigned BUTTON_WIDTH=109u;
constexpr unsigned BUTTON_HEIGHT=48u;
constexpr unsigned BUTTON_GAP=7u;
constexpr unsigned COLUMNS=4u;
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
 return projection_buttons==10u&&COUNT>=40u;
}
} // namespace swe::diaery::access
#endif
