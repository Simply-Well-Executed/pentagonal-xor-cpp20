/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// AIRMAN/1 native, local, independent schematic viewer.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "../shared/THAILONGA3D.hpp"
#include "../shared/NULL0_TOPOLOGY.hpp"
#include "../shared/ATTENUATION.hpp"
using T=THAILONGA3D; using N=NULL0_TOPOLOGY; using A=ATTENUATION;
enum : int {Mode=101,Config,Mask,X,Y,Z,Apply,Rotate,Fold,Pair,Sign,Status,Reset,Dim,Time,Solar};
static HWND controls[16]{};
static int xyz[3]={8,10,12};
static unsigned selection=2047,rotation=0,chosen=0,view=0,signs=0,dimension=5,minute=0;
static bool reduced=true,fixed=false,solar=true;
static HWND control(int id){return controls[id-101];}
static constexpr COLORREF bg=RGB(16,29,49),ink=RGB(230,240,252),gold=RGB(241,204,120),
 blue=RGB(115,197,255),green=RGB(117,214,179),red=RGB(255,137,146),violet=RGB(194,161,255);
static void text(HDC dc,int x,int y,const wchar_t* s,COLORREF color=ink){
 SetTextColor(dc,color);TextOutW(dc,x,y,s,lstrlenW(s));
}
static void line(HDC dc,int x,int y,int a,int b,COLORREF color,int width=1){
 HPEN p=CreatePen(PS_SOLID,width,color);if(!p)return;
 auto old=SelectObject(dc,p);MoveToEx(dc,x,y,nullptr);LineTo(dc,a,b);SelectObject(dc,old);DeleteObject(p);
}
static void dot(HDC dc,int x,int y,int r,COLORREF color){
 HBRUSH b=CreateSolidBrush(color);if(!b)return;
 auto old=SelectObject(dc,b);auto pen=SelectObject(dc,GetStockObject(NULL_PEN));
 Ellipse(dc,x-r,y-r,x+r,y+r);SelectObject(dc,pen);SelectObject(dc,old);DeleteObject(b);
}
static void polygon(HDC dc,const POINT* p,int n,COLORREF color){
 HPEN pen=CreatePen(PS_SOLID,1,color);if(!pen)return;
 auto old=SelectObject(dc,pen);auto brush=SelectObject(dc,GetStockObject(NULL_BRUSH));
 Polygon(dc,p,n);SelectObject(dc,brush);SelectObject(dc,old);DeleteObject(pen);
}
static POINT project(T::Vector v,int cx,int cy){
 T::I x=v.x,z=v.z;for(unsigned i=0;i<rotation%4;++i){const auto tmp=x;x=-z;z=tmp;}
 return {LONG(cx+(x-z)*6),LONG(cy-v.y*9+(x+z)*3)};
}
static void geometry(HDC dc,int w,int h){
 const int x=w/2,y=h/2+16;
 const auto c=T::fold(xyz[0],xyz[1],xyz[2]);
 const T::Vector v{xyz[0],xyz[1],xyz[2]};
 for(int i=0;i<3;++i){
  T::Vector a{},b{};if(i==0){a.x=-16;b.x=16;}if(i==1){a.y=-16;b.y=16;}if(i==2){a.z=-16;b.z=16;}
  const auto p=project(a,x,y),q=project(b,x,y);
  const auto color=i==0?red:i==1?green:blue;
  line(dc,p.x,p.y,q.x,q.y,color);text(dc,q.x,q.y,i==0?L"X":i==1?L"Y":L"Z",color);
 }
 const auto p=project(v,x,y),q=project(c.primitive,x,y),r=reduced?q:p;
 line(dc,x,y,p.x,p.y,gold);line(dc,x,y,q.x,q.y,gold,3);dot(dc,p.x,p.y,5,ink);
 dot(dc,q.x,q.y,5,gold);dot(dc,r.x,r.y,8,green);
 text(dc,18,14,L"3D THAILONGA / original and reduced core");
 text(dc,18,37,L"Quarter-turn display; exact source unchanged",blue);
}
static void star(HDC dc,int x,int y,int r,bool inv,COLORREF color){
 const int p[5][2]={{0,-10},{6,8},{-10,-3},{10,-3},{-6,8}};
 POINT q[5]{};for(int i=0;i<5;++i)q[i]={x+p[i][0]*r/10,y+(inv?-1:1)*p[i][1]*r/10};
 polygon(dc,q,5,color);
}
static void hexmap(HDC dc,int w,int h){
 const int s=w/13<(h-75)/9?w/13:(h-75)/9;if(s<10)return;
 const int x0=(w-11*s)/2+s/2,y0=55;
 for(unsigned i=0;i<N::hexagons;++i){
  const unsigned row=i/11,col=i%11;const int x=x0+int(col)*s+int(row%2)*s/2,y=y0+int(row)*s;
  const int a=s/2,b=s/3;POINT p[6]={{x,y-a},{x+a,y-b},{x+a,y+b},{x,y+a},{x-a,y+b},{x-a,y-b}};
  const bool active=(selection&(1u<<col))!=0;const auto dim=RGB(77,92,110);
  polygon(dc,p,6,active?blue:dim);
  if(N::stars(i)==1)star(dc,x,y,a/2,false,active?gold:dim);
  else{star(dc,x-a/2,y,a/2,false,active?gold:dim);star(dc,x+a/2,y,a/2,true,active?blue:dim);if(active)dot(dc,x,y,2,violet);}
 }
 text(dc,18,14,L"HEX99 / 99 hexagons / 180 pentagrams / 360 XOR slots");
 text(dc,18,35,L"Mask chooses columns; violet = symbolic interface",blue);
}
static constexpr int ring[20][2]={{100,0},{95,31},{81,59},{59,81},{31,95},{0,100},
 {-31,95},{-59,81},{-81,59},{-95,31},{-100,0},{-95,-31},{-81,-59},{-59,-81},
 {-31,-95},{0,-100},{31,-95},{59,-81},{81,-59},{95,-31}};
static constexpr unsigned counts[3]={64,97,20};
static void orbit(HDC dc,int x,int y,unsigned n,bool active){
 POINT p[97]{};const unsigned full=n/20;
 dot(dc,x,y,18,active?RGB(42,100,157):RGB(31,68,108));
 for(unsigned k=0;k<n;++k){
  const int r=28+int(k/20)*13;const unsigned j=(k%20+rotation*5+k/20)%20;
  p[k]={x+r*ring[j][0]/100,y+r*ring[j][1]/135};
 }
 for(unsigned g=0;g<full;++g)for(unsigned j=0;j<5;++j){
  const unsigned a=g*20+10+j,b=g*20+(j==4&&!fixed?15:15+j);
  line(dc,p[a].x,p[a].y,p[b].x,p[b].y,!fixed&&(j==0||j==4)?red:green);
 }
 for(unsigned k=0;k<n;++k)dot(dc,p[k].x,p[k].y,3,k>=full*20?RGB(166,177,191):k%20<10?gold:blue);
}
static void triforce(HDC dc,int w,int h){
 const int ax=w/2,ay=h/4,lx=w/4,rx=3*w/4,by=3*h/4-16;
 line(dc,ax,ay,lx,by,violet);line(dc,lx,by,rx,by,violet);line(dc,rx,by,ax,ay,violet);
 orbit(dc,lx,by,64,chosen==0);orbit(dc,rx,by,97,chosen==1);orbit(dc,ax,ay,20,chosen==2);
 text(dc,lx-16,by-82,L"D64",gold);text(dc,rx-16,by-96,L"D97",blue);
 text(dc,ax-16,ay-46,L"D20",green);dot(dc,ax,h/2,23,RGB(66,47,95));
 text(dc,ax-21,h/2-8,L"AERIS",violet);
 text(dc,14,14,L"TRIFORCE / D64 -> D97 -> D20");text(dc,14,35,L"Symbolic routes, not physical orbit propagation",blue);
}
static void jig(HDC dc,int w,int h){
 const auto b=A::de_radicalize(64,100,8,10);
 const A::SignedJig j[4]={b.pp,b.nn,b.pn,b.np};const wchar_t* names[4]={L"+8,+10",L"-8,-10",L"+8,-10",L"-8,+10"};
 text(dc,16,15,L"Signed Euler jig / 4 retained orientations");
 for(int k=0;k<4;++k){
  wchar_t out[100]{};wsprintfW(out,L"%s   correction %ld   compensated %ld",names[k],long(j[k].correction),long(j[k].compensated));
  const int x=w/4+(k%2)*w/2,y=h/3+(k/2)*h/3;
  dot(dc,x,y,12,unsigned(k)==signs?gold:blue);text(dc,x-95,y+20,out,unsigned(k)==signs?gold:ink);
 }
 text(dc,16,h-28,L"Dimensionless results; no dBi measurement",blue);
}
static void stateview(HDC dc,int w,int h){
 const auto b=A::de_radicalize(64,100,8,10);
 const A::SignedJig j[4]={b.pp,b.nn,b.pn,b.np};const auto a=j[signs];
 const long long values[11]={a.gain_root,a.requested_root,-36,a.correction,a.compensated,64,100,a.divisor,a.remainder,a.gain_root<0?-1:1,a.requested_root<0?-1:1};
 const wchar_t* names[11]={L"Gain root",L"Requested root",L"Loss",L"Correction",L"Compensated",L"Gain square",L"Requested square",L"Divisor",L"Remainder",L"Gain sign",L"Requested sign"};
 const int cx=w/2,cy=h/2+15;const int r=120;
 text(dc,16,16,L"5D-11D state-coordinate projection (not independent spatial axes)");
 for(unsigned i=0;i<dimension;++i){
  const unsigned j0=(i*20/dimension+rotation*5)%20;
  const int x=cx+r*ring[j0][0]/100,y=cy+r*ring[j0][1]/100;
  line(dc,cx,cy,x,y,i<5?gold:blue);dot(dc,x,y,4,i<5?gold:blue);
  wchar_t s[64]{};wsprintfW(s,L"D%u %s: %ld",i+1,names[i],long(values[i]));
  text(dc,x-50,y+8,s);
 }
 dot(dc,cx,cy,5,violet);
}
static void energyview(HDC dc,int w,int h){
 const auto e=N::energy(minute,solar);wchar_t s[100]{};
 wsprintfW(s,L"T=%u min / %s / %s",minute,solar?L"Solar + storage":L"Storage only",minute%90<54?L"Sunlight":L"Eclipse");
 text(dc,16,16,L"Illustrative energy laboratory");text(dc,16,45,s,blue);
 const int x=50,y=h/2-25,bar=w-100;
 line(dc,x,y,x+bar,y,RGB(76,93,116),25);
 line(dc,x,y,x+int(e.watt_minutes)*bar/6000,y,green,25);
 wsprintfW(s,L"Stored energy: %u / 60 Wh (exact watt-minute ratio)",e.watt_minutes);
 text(dc,x,y+35,s,gold);
 text(dc,16,h-50,L"100 Wh capacity; 50 W load; 80 W generation during sunlight",blue);
 text(dc,16,h-27,L"90-minute cycle; 36-minute eclipse; not a spacecraft prediction",blue);
}
static void update(HWND win){
 wchar_t s[256]{};const auto c=T::fold(xyz[0],xyz[1],xyz[2]);
 const auto a=T::AERIS({xyz[0],xyz[1],xyz[2]},c);
 if(view==0)wsprintfW(s,L"XYZ (%d,%d,%d) / core (%ld,%ld,%ld) / scale %lu / AERIS %s",
 xyz[0],xyz[1],xyz[2],long(c.primitive.x),long(c.primitive.y),long(c.primitive.z),static_cast<unsigned long>(c.scale),a.ping?L"PASS":L"FAIL");
 else if(view==1)wsprintfW(s,L"Mask %u / %u active pentagrams / %u XOR slots",selection,N::active_stars(selection),2u*N::active_stars(selection));
 else if(view==2)wsprintfW(s,L"D%u / %u complete groups / %u reserve / pairing %s",counts[chosen],counts[chosen]/20,counts[chosen]%20,fixed?L"PASS":L"OVERLAP");
 else if(view==3)wsprintfW(s,L"Signed branch %u of 4; all four retained",signs+1);
 else if(view==4)wsprintfW(s,L"%u state coordinates / %u nonempty subsets / %u planes",dimension,(1u<<dimension)-1,dimension*(dimension-1)/2);
 else wsprintfW(s,L"Elapsed %u min / %u watt-minutes stored",minute,N::energy(minute,solar).watt_minutes);
 SetWindowTextW(control(Status),s);InvalidateRect(win,nullptr,TRUE);
}
static HWND make(HWND win,int id,const wchar_t* cls,const wchar_t* title,DWORD style,int x,int y,int w,int h){
 HWND c=CreateWindowW(cls,title,WS_CHILD|WS_VISIBLE|style,x,y,w,h,win,reinterpret_cast<HMENU>(INT_PTR(id)),GetModuleHandleW(nullptr),nullptr);
 if(c){controls[id-101]=c;SendMessageW(c,WM_SETFONT,WPARAM(GetStockObject(DEFAULT_GUI_FONT)),TRUE);}return c;
}
static void combo(HWND c,const wchar_t* s){SendMessageW(c,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(s));}
static LRESULT CALLBACK proc(HWND win,UINT msg,WPARAM wp,LPARAM lp){
 if(msg==WM_CREATE){
  auto m=make(win,Mode,L"COMBOBOX",L"",WS_TABSTOP|CBS_DROPDOWNLIST,12,10,190,160);
  combo(m,L"3D THAILONGA");combo(m,L"HEX99 starchart");combo(m,L"TRIFORCE orbits");combo(m,L"Signed Euler jig");combo(m,L"5D-11D state chart");combo(m,L"Energy laboratory");SendMessageW(m,CB_SETCURSEL,0,0);
  auto c=make(win,Config,L"COMBOBOX",L"",WS_TABSTOP|CBS_DROPDOWNLIST,210,10,115,120);
  combo(c,L"D64");combo(c,L"D97");combo(c,L"D20");SendMessageW(c,CB_SETCURSEL,0,0);
  make(win,Mask,L"EDIT",L"2047",WS_TABSTOP|WS_BORDER|ES_NUMBER,335,10,65,24);
  make(win,X,L"EDIT",L"8",WS_TABSTOP|WS_BORDER,410,10,55,24);
  make(win,Y,L"EDIT",L"10",WS_TABSTOP|WS_BORDER,475,10,55,24);
  make(win,Z,L"EDIT",L"12",WS_TABSTOP|WS_BORDER,540,10,55,24);
  make(win,Apply,L"BUTTON",L"Apply mask / XYZ",WS_TABSTOP,605,10,145,26);
  make(win,Rotate,L"BUTTON",L"Rotate 90",WS_TABSTOP,12,48,115,30);
  make(win,Fold,L"BUTTON",L"Original / core",WS_TABSTOP,137,48,130,30);
  make(win,Pair,L"BUTTON",L"Toggle pairing",WS_TABSTOP,277,48,130,30);
  make(win,Sign,L"BUTTON",L"Next signed branch",WS_TABSTOP,417,48,150,30);
  make(win,Reset,L"BUTTON",L"Reset",WS_TABSTOP,577,48,85,30);
  make(win,Dim,L"BUTTON",L"Next dimension",WS_TABSTOP,12,86,130,28);
  make(win,Time,L"BUTTON",L"Time +10 min",WS_TABSTOP,152,86,130,28);
  make(win,Solar,L"BUTTON",L"Toggle solar",WS_TABSTOP,292,86,130,28);
  make(win,Status,L"STATIC",L"",SS_LEFT,12,119,750,35);
  const wchar_t* captions[4]={L"Mask",L"X",L"Y",L"Z"};
  const int positions[4]={335,410,475,540};
  for(int i=0;i<4;++i){
   HWND label=CreateWindowW(L"STATIC",captions[i],WS_CHILD|WS_VISIBLE,positions[i],35,60,12,win,nullptr,GetModuleHandleW(nullptr),nullptr);
   if(label)SendMessageW(label,WM_SETFONT,WPARAM(GetStockObject(DEFAULT_GUI_FONT)),TRUE);
  }
  for(HWND v:controls)if(!v)return -1;
  update(win);return 0;
 }
 if(msg==WM_COMMAND){
  const int id=LOWORD(wp);const unsigned notification=HIWORD(wp);
  if(id==Mode&&notification==CBN_SELCHANGE)view=unsigned(SendMessageW(control(Mode),CB_GETCURSEL,0,0));
  else if(id==Config&&notification==CBN_SELCHANGE)chosen=unsigned(SendMessageW(control(Config),CB_GETCURSEL,0,0));
  else if(notification==BN_CLICKED){
   if(id==Apply){
    BOOL good=FALSE;const UINT m=GetDlgItemInt(win,Mask,&good,FALSE);
    if(!good||m>N::mask){SetWindowTextW(control(Status),L"Invalid mask: use 0..2047");return 0;}
    int v[3]{};for(int i=0;i<3;++i){const UINT raw=GetDlgItemInt(win,X+i,&good,TRUE);v[i]=int(raw);if(!good||v[i]<-16||v[i]>16){SetWindowTextW(control(Status),L"Invalid XYZ: use signed -16..16");return 0;}}
    selection=m;for(int i=0;i<3;++i)xyz[i]=v[i];
   }else if(id==Rotate)rotation=(rotation+1)%4;
   else if(id==Fold)reduced=!reduced;
   else if(id==Pair)fixed=!fixed;
   else if(id==Sign)signs=(signs+1)%4;
   else if(id==Dim)dimension=dimension==11?5:dimension+1;
   else if(id==Time)minute=minute==180?0:minute+10;
   else if(id==Solar)solar=!solar;
   else if(id==Reset){xyz[0]=8;xyz[1]=10;xyz[2]=12;selection=2047;rotation=chosen=view=signs=0;fixed=false;reduced=true;dimension=5;minute=0;solar=true;
    SetDlgItemInt(win,Mask,selection,FALSE);for(int i=0;i<3;++i)SetDlgItemInt(win,X+i,UINT(xyz[i]),TRUE);
    SendMessageW(control(Mode),CB_SETCURSEL,0,0);SendMessageW(control(Config),CB_SETCURSEL,0,0);
   }else return 0;
  }else return 0;
  update(win);return 0;
 }
 if(msg==WM_CTLCOLORSTATIC){
  HDC dc=reinterpret_cast<HDC>(wp);SetTextColor(dc,ink);SetBkMode(dc,TRANSPARENT);
  return reinterpret_cast<LRESULT>(GetStockObject(NULL_BRUSH));
 }
 if(msg==WM_PAINT){
  PAINTSTRUCT ps{};HDC dc=BeginPaint(win,&ps);RECT r{};GetClientRect(win,&r);
  HBRUSH b=CreateSolidBrush(bg);if(b){FillRect(dc,&r,b);DeleteObject(b);}SetBkMode(dc,TRANSPARENT);
  auto font=SelectObject(dc,GetStockObject(DEFAULT_GUI_FONT));SetViewportOrgEx(dc,0,160,nullptr);
  const int w=int(r.right),h=int(r.bottom)-160;
  if(view==0)geometry(dc,w,h);else if(view==1)hexmap(dc,w,h);else if(view==2)triforce(dc,w,h);else if(view==3)jig(dc,w,h);else if(view==4)stateview(dc,w,h);else energyview(dc,w,h);
  SelectObject(dc,font);EndPaint(win,&ps);return 0;
 }
 if(msg==WM_DESTROY){PostQuitMessage(0);return 0;}return DefWindowProcW(win,msg,wp,lp);
}
int main(){
 HINSTANCE instance=GetModuleHandleW(nullptr);WNDCLASSW c{};c.lpfnWndProc=proc;c.hInstance=instance;c.lpszClassName=L"SWE_AIRMAN";c.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));
 if(!RegisterClassW(&c))return 1;
 HWND w=CreateWindowW(c.lpszClassName,L"AIRMAN | THAILONGA / AERIS",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,800,730,nullptr,nullptr,instance,nullptr);
 if(!w)return 2;ShowWindow(w,SW_SHOW);UpdateWindow(w);
 MSG m{};BOOL result=0;while((result=GetMessageW(&m,nullptr,0,0))>0){if(!IsDialogMessageW(w,&m)){TranslateMessage(&m);DispatchMessageW(&m);}}
 return result<0?3:0;
}
