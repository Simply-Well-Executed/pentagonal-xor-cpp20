// SPDX-License-Identifier: 0BSD
// DIAERY native, offline XHTML 1.0 Strict SUBSET viewer + 1:1 parity gate.
// No C++ standard library or system header is included. Geometry comes
// from the original repository, never reconstructed from XHTML markup.
#include "model_bridge.hpp"
#include "application_of_ymxb.hpp"
#include "dynamo_subject.hpp"
#include "trace_timeline.hpp"
#include "strict_profile.hpp"
#include "parity_gate.hpp"
#include "fusion_watchdog.hpp"
#include "fusion_qualification.hpp"
#include "fusion_generator.hpp"
namespace dia=swe::diaery;
static char g_input[16002]{};
static dia::strict::Page g_page{};
static bool g_loaded=false;

#if defined(_WIN32)
// Native Win32 ABI declarations only; no Win32 SDK header is included.
#define DIA_STDCALL __stdcall
#define DIA_IMPORT __declspec(dllimport)
using W32U=unsigned int;
using W32D=unsigned long;
using W32W=unsigned long long;
using W32L=long long;
using W32H=void*;
using W32CALLBACK=W32L (DIA_STDCALL*)(W32H,W32U,W32W,W32L);
struct W32POINT {long x,y;};
struct W32RECT {long left,top,right,bottom;};
struct W32PAINT {W32H hdc;int erase;W32RECT paint;int restore;int update;unsigned char reserved[32];};
struct W32MESSAGE {W32H hwnd;W32U message;W32W wparam;W32L lparam;W32D time;W32POINT point;W32D private_data;};
struct W32CLASS {
    W32U size,style;W32CALLBACK proc;int class_extra,window_extra;
    W32H instance,icon,cursor,background;
    const char* menu;const char* class_name;W32H small_icon;
};
extern "C" {
DIA_IMPORT W32H DIA_STDCALL GetModuleHandleA(const char*);
DIA_IMPORT unsigned short DIA_STDCALL RegisterClassExA(const W32CLASS*);
DIA_IMPORT W32H DIA_STDCALL CreateWindowExA(W32D,const char*,const char*,W32D,int,int,int,int,W32H,W32H,W32H,void*);
DIA_IMPORT int DIA_STDCALL ShowWindow(W32H,int);
DIA_IMPORT int DIA_STDCALL UpdateWindow(W32H);
DIA_IMPORT int DIA_STDCALL GetMessageA(W32MESSAGE*,W32H,W32U,W32U);
DIA_IMPORT int DIA_STDCALL TranslateMessage(const W32MESSAGE*);
DIA_IMPORT W32L DIA_STDCALL DispatchMessageA(const W32MESSAGE*);
DIA_IMPORT W32L DIA_STDCALL DefWindowProcA(W32H,W32U,W32W,W32L);
DIA_IMPORT void DIA_STDCALL PostQuitMessage(int);
DIA_IMPORT W32H DIA_STDCALL BeginPaint(W32H,W32PAINT*);
DIA_IMPORT int DIA_STDCALL EndPaint(W32H,const W32PAINT*);
DIA_IMPORT int DIA_STDCALL GetClientRect(W32H,W32RECT*);
DIA_IMPORT int DIA_STDCALL InvalidateRect(W32H,const W32RECT*,int);
DIA_IMPORT W32H DIA_STDCALL SetCapture(W32H);
DIA_IMPORT int DIA_STDCALL ReleaseCapture();
DIA_IMPORT int DIA_STDCALL FillRect(W32H,const W32RECT*,W32H);
DIA_IMPORT W32H DIA_STDCALL CreateSolidBrush(W32D);
DIA_IMPORT W32H DIA_STDCALL CreatePen(int,int,W32D);
DIA_IMPORT int DIA_STDCALL DeleteObject(W32H);
DIA_IMPORT W32H DIA_STDCALL SelectObject(W32H,W32H);
DIA_IMPORT int DIA_STDCALL MoveToEx(W32H,int,int,W32POINT*);
DIA_IMPORT int DIA_STDCALL LineTo(W32H,int,int);
DIA_IMPORT int DIA_STDCALL Polygon(W32H,const W32POINT*,int);
DIA_IMPORT W32D DIA_STDCALL SetTextColor(W32H,W32D);
DIA_IMPORT int DIA_STDCALL SetBkMode(W32H,int);
DIA_IMPORT int DIA_STDCALL TextOutA(W32H,int,int,const char*,int);
DIA_IMPORT W32H DIA_STDCALL CreateFileA(const char*,W32D,W32D,void*,W32D,W32D,W32H);
DIA_IMPORT int DIA_STDCALL ReadFile(W32H,void*,W32D,W32D*,void*);
DIA_IMPORT int DIA_STDCALL CloseHandle(W32H);
DIA_IMPORT int DIA_STDCALL DeleteFileA(const char*);
DIA_IMPORT W32H DIA_STDCALL GetStdHandle(W32D);
DIA_IMPORT int DIA_STDCALL WriteFile(W32H,const void*,W32D,W32D*,void*);
}
static constexpr W32D OPEN_EXISTING=3u;
static constexpr W32D FILE_READ=0x80000000u;
static constexpr W32D STDOUT_HANDLE=0xFFFFFFF5u;
static constexpr W32D WS_OVERLAPPEDWINDOW=0x00CF0000u;
static constexpr W32D WS_VISIBLE=0x10000000u;
static constexpr W32D WM_PAINT=0x000Fu,WM_DESTROY=0x0002u;
static constexpr W32D WM_KEYDOWN=0x0100u,WM_LBUTTONDOWN=0x0201u;
static constexpr W32D WM_LBUTTONUP=0x0202u,WM_MOUSEMOVE=0x0200u;
static int read_local(const char* path){
    W32H h=CreateFileA(path,FILE_READ,1u,nullptr,OPEN_EXISTING,0u,nullptr);
    if(h==reinterpret_cast<W32H>(-1))return -1;
    W32D n=0u;const int ok=ReadFile(h,g_input,16000u,&n,nullptr);
    CloseHandle(h);if(!ok||n==0u||n>=16000u)return -1;
    g_input[n]=0;return static_cast<int>(n);
}
static void print(const char* s){W32D written=0u;WriteFile(GetStdHandle(STDOUT_HANDLE),s,dia::strict::len(s),&written,nullptr);}

// Fusion output is CREATE_NEW: no silent overwrite of old sources.
// Buffered byte writes, no reinterpreted user XHTML as C++ syntax.
struct FusionSink {
 W32H handle;char buffer[4096]{};unsigned used=0u;bool ok=true;
 void flush() noexcept {
  if(!ok||used==0u)return;
  W32D written=0u;
  if(!WriteFile(handle,buffer,used,&written,nullptr)||written!=used)ok=false;
  used=0u;
 }
 void chr(char c) noexcept {
  if(!ok)return;
  if(used==sizeof(buffer))flush();
  if(ok)buffer[used++]=c;
 }
 void text(const char* s) noexcept {
  for(unsigned i=0u;s&&s[i];++i)chr(s[i]);
 }
};
static bool fusion_to_new_file(const char* output) noexcept {
 if(!output||!output[0])return false;
 W32H h=CreateFileA(output,0x40000000u,0u,nullptr,1u,0x80u,nullptr);
 if(h==reinterpret_cast<W32H>(-1))return false;
 FusionSink sink{h};
 const bool generated=dia::fusion::GENERATE(
     g_input,dia::strict::len(g_input),g_page,sink);
 sink.flush();const bool success=generated&&sink.ok;
 CloseHandle(h);
 if(!success)DeleteFileA(output);
 return success;
}
#else
extern "C" int open(const char*,int,...);
extern "C" long read(int,void*,unsigned long);
extern "C" int close(int);
extern "C" long write(int,const void*,unsigned long);
static int read_local(const char* path){
    const int fd=open(path,0);if(fd<0)return -1;
    const long n=read(fd,g_input,16000UL);close(fd);
    if(n<=0||n>=16000)return -1;
    g_input[n]=0;return static_cast<int>(n);
}
static void print(const char* s){write(1,s,dia::strict::len(s));}
#endif

static bool load_validate(const char* path) {
    const int n=read_local(path);
    if(n<0)return false;
    return dia::strict::parse(g_input,static_cast<unsigned>(n),g_page) &&
           dia::parity::concurrence(g_page);
}

#if defined(_WIN32)
static int g_stage=0,g_yaw=-30,g_pitch=22,g_zoom=5;
static bool g_drag=false,g_water=true,g_yang=true,g_bubble=true,g_cutaway=true;
static bool g_dynamo=false;
static double g_length_inches=4.0/2.54,g_girth_inches=6.1240,g_depth_inches=2.9000;
static bool g_dynamo_mint=false;
static bool g_subunits=true;
static swe::diaery::trace::U64 g_elapsed_fs=0ULL;
static unsigned g_trace_index=0u;
static int g_mouse_x=0,g_mouse_y=0;
static W32H g_window=nullptr;
// Rendering-only approximations; source and gate remain exact integers.
static double trig_sin(double a){
    // Polynomial after bounding to [-pi,pi] (angles in degrees).
    while(a>3.141592653589793)a-=6.283185307179586;
    while(a< -3.141592653589793)a+=6.283185307179586;
    const double a2=a*a;
    return a*(1-a2/6+a2*a2/120-a2*a2*a2/5040+
              a2*a2*a2*a2/362880-a2*a2*a2*a2*a2/39916800);
}
static double trig_cos(double a){return trig_sin(a+1.5707963267948966);}
static W32POINT screen(swe::rectangloid_primitive::Point3 p,int cx,int cy){
    const double yaw=static_cast<double>(g_yaw)*0.017453292519943295;
    const double pitch=static_cast<double>(g_pitch)*0.017453292519943295;
    const double x=static_cast<double>(p.x)-30.0;
    const double y=static_cast<double>(p.y)-7.5;
    const double z=static_cast<double>(p.z)-7.5;
    const double ax=trig_cos(yaw)*x-trig_sin(yaw)*y;
    const double ay=trig_sin(yaw)*x+trig_cos(yaw)*y;
    const double az=trig_cos(pitch)*z+trig_sin(pitch)*ay;
    return {static_cast<long>(cx+g_zoom*ax),static_cast<long>(cy-g_zoom*az)};
}
static W32D rgb(unsigned r,unsigned g,unsigned b){return r|(g<<8)|(b<<16);}
static void label(W32H dc,int x,int y,const char* s,W32D color){
    SetTextColor(dc,color);TextOutA(dc,x,y,s,dia::strict::len(s));
}
static void quad(W32H dc,swe::rectangloid_primitive::Quad face,
                 int cx,int cy,W32D color,bool outward_reverse){
    W32POINT a[4]{screen(face.a,cx,cy),screen(face.b,cx,cy),
                  screen(face.c,cx,cy),screen(face.d,cx,cy)};
    const long long cross2=static_cast<long long>(a[1].x-a[0].x)*(a[2].y-a[0].y)-
        static_cast<long long>(a[1].y-a[0].y)*(a[2].x-a[0].x);
    if(g_yang && g_stage>=3 && (outward_reverse?cross2>=0:cross2<=0))return;
    W32H brush=CreateSolidBrush(color);
    W32H old=SelectObject(dc,brush);
    Polygon(dc,a,4);SelectObject(dc,old);DeleteObject(brush);
}
// Native GDI DYNAMO: real line calculation, explicit source point,
// equal pixel/inch axes, separate unverified depth, NULL uncertainty HOLD.
// No change to XHTML parser, profile or fold viewer.
static void decimal4(char* out,unsigned capacity,double value) {
    if(capacity<12u)return;
    unsigned j=0u;
    if(value<0.){out[j++]='-';value=-value;}
    // Display-only rounding, nearest 0.0001; does NOT alter sources.
    const unsigned whole=static_cast<unsigned>(value);
    unsigned long long scaled=static_cast<unsigned long long>((value-static_cast<double>(whole))*10000.+0.5);
    unsigned integer=whole;
    if(scaled>=10000ULL){++integer;scaled-=10000ULL;}
    char reversed[12]{};unsigned n=0u;
    do{reversed[n++]=static_cast<char>('0'+integer%10u);integer/=10u;}while(integer&&n<10u);
    while(n&&j+1<capacity)out[j++]=reversed[--n];
    if(j+5>=capacity){out[j]=0;return;}
    out[j++]='.';
    out[j++]=static_cast<char>('0'+scaled/1000ULL%10ULL);
    out[j++]=static_cast<char>('0'+scaled/100ULL%10ULL);
    out[j++]=static_cast<char>('0'+scaled/10ULL%10ULL);
    out[j++]=static_cast<char>('0'+scaled%10ULL);
    out[j]=0;
}
static void text_value(W32H dc,int x,int y,const char* title,double v,W32D color){
    label(dc,x,y,title,color);
    char buffer[24]{};decimal4(buffer,24u,v);
    label(dc,x+188,y,buffer,color);
}
static int plot_x(double v,int left,int px_per_inch){return left+static_cast<int>(v*px_per_inch+0.5);}
static int plot_y(double v,int bottom,int px_per_inch){return bottom-static_cast<int>(v*px_per_inch+0.5);}
// Integer decimal formatting for exact time ledger. No fractional precision loss.
static void format_u64(char* out,unsigned capacity,swe::diaery::trace::U64 v){
    if(capacity==0u)return;
    char scratch[24]{};unsigned n=0u;
    do {scratch[n++]=static_cast<char>('0'+v%10ULL);v/=10ULL;} while(v&&n<24u);
    unsigned i=0u;
    while(n&&i+1u<capacity)out[i++]=scratch[--n];
    out[i]=0;
}
static void paint_time_plot(W32H dc) {
    namespace tr=swe::diaery::trace;
    const W32D grey=rgb(158,174,190),yarn=rgb(169,192,203),thread=rgb(221,217,164);
    const int left=83,right=443,top=552,bottom=665,width=360;
    label(dc,left,521,"TIME PLANE (not a physical clock)",grey);
    label(dc,left,537,"THREAD: femtoseconds | YARN: seconds",grey);
    W32H yarn_pen=CreatePen(0,1,yarn);
    W32H old=SelectObject(dc,yarn_pen);
    MoveToEx(dc,left,bottom,nullptr);LineTo(dc,right,bottom);
    for(int sec=0;sec<=2;++sec){
        const int px=left+sec*180;
        MoveToEx(dc,px,top,nullptr);LineTo(dc,px,bottom);
    }
    SelectObject(dc,old);DeleteObject(yarn_pen);
    // 1/4 second threads are dashed, but exact fs positions are integer.
    W32H thread_pen=CreatePen(0,1,rgb(83,98,107));
    old=SelectObject(dc,thread_pen);
    for(int q=1;q<8;++q){
        if(q%4==0)continue;
        const int px=left+q*45;
        for(int yy=top;yy<bottom;yy+=9){
            MoveToEx(dc,px,yy,nullptr);LineTo(dc,px,yy+4<bottom?yy+4:bottom);
        }
    }
    SelectObject(dc,old);DeleteObject(thread_pen);
    const tr::U64 window=2ULL*tr::FS_PER_SECOND;
    const tr::U64 local=g_elapsed_fs%window;
    const int cursor=left+static_cast<int>((local*360ULL)/window); // no overflow: local < 2e15
    W32H marker_pen=CreatePen(0,2,thread);
    old=SelectObject(dc,marker_pen);
    MoveToEx(dc,cursor,top,nullptr);LineTo(dc,cursor,bottom);
    SelectObject(dc,old);DeleteObject(marker_pen);
    // Controlled expression traces: source m/b are algebraic, not measurements.
    const auto line=swe::diaery::graph::application_of_ymxb(g_length_inches,g_girth_inches);
    const char* format[3]{"Y=MX+B","Y=X","Y=0"};
    label(dc,475,530,"TRACE: bounded format",grey);
    label(dc,475,552,format[g_trace_index],thread);
    if(line.valid){
        W32H trace_pen=CreatePen(0,1,rgb(108,172,174));
        old=SelectObject(dc,trace_pen);
        bool first=true;
        for(int step=0;step<=40;++step){
            // 2-second viewport, 1/20-second display sampling ONLY.
            const double seconds=static_cast<double>(step)/20.0;
            const auto value=tr::calculate(tr::parse(format[g_trace_index]),seconds,line.slope,line.intercept);
            if(value.status!=tr::Status::Valid){first=true;continue;}
            // Map y from -10 to +10 on this independent time panel.
            if(value.y< -10.||value.y>10.){first=true;continue;}
            const int px=left+step*9;
            const int py=top+static_cast<int>((10.0-value.y)*(bottom-top)/20.0+0.5);
            if(first){MoveToEx(dc,px,py,nullptr);first=false;}else LineTo(dc,px,py);
        }
        SelectObject(dc,old);DeleteObject(trace_pen);
    }
    const auto parts=tr::split_femtoseconds(g_elapsed_fs);
    char secbuf[24]{},rembuf[24]{};
    format_u64(secbuf,24u,parts.time.seconds);
    format_u64(rembuf,24u,parts.time.remainder_femtoseconds);
    label(dc,475,585,"SECONDS (exact):",grey);label(dc,640,585,secbuf,thread);
    label(dc,475,607,"REMAINDER fs:",grey);label(dc,640,607,rembuf,thread);
    label(dc,475,632,"F +1 fs | S +1 s | T trace | G grid",grey);
    label(dc,83,680,"SECONDARY TIME AXIS: 0 to 2 seconds; exact counter never rounds to samples",grey);
}

static void paint_dynamo(W32H dc,const W32RECT& rc){
    const W32D grey=rgb(159,176,194),teal=rgb(95,212,202),gold=rgb(237,190,117),purple=rgb(192,141,233);
    label(dc,22,16,"DIAERY | APPLICATION OF Y=MX+B | native C++20 | RELEASE=HOLD",teal);
    label(dc,22,39,"X horizontal = penile length (in); Y vertical = circumference (in)",grey);
    label(dc,22,60,"Z separate = vaginal depth (in), unverified; no physiological inference",grey);
    const int left=83,top=110,side=360,pix=30,bottom=top+side;
    // X MAJOR and Y MAJOR are each one physical inch in their own axis.
    // Same 1px stroke width; subunits are lighter and monochrome dashed.
    W32H grid_pen=CreatePen(0,1,rgb(77,90,105));
    W32H old=SelectObject(dc,grid_pen);
    for(int n=0;n<=12;++n){
        MoveToEx(dc,left+pix*n,top,nullptr);LineTo(dc,left+pix*n,bottom);
        MoveToEx(dc,left,bottom-pix*n,nullptr);LineTo(dc,left+side,bottom-pix*n);
    }
    SelectObject(dc,old);DeleteObject(grid_pen);
    if(g_subunits){
        W32H sub_pen=CreatePen(0,1,rgb(41,49,57));
        old=SelectObject(dc,sub_pen);
        // Compute raster positions directly from rational integer indices;
        // never add approximate floating 1/3 values to the source axis.
        for(unsigned quarter=1u;quarter<48u;++quarter){
            if(swe::diaery::trace::x_is_major(quarter))continue;
            const int xx=left+static_cast<int>((quarter*pix+2u)/4u);
            for(int yy=top;yy<bottom;yy+=10){
                MoveToEx(dc,xx,yy,nullptr);LineTo(dc,xx,yy+5<bottom?yy+5:bottom);
            }
        }
        for(unsigned third=1u;third<36u;++third){
            if(swe::diaery::trace::y_is_major(third))continue;
            const int yy=bottom-static_cast<int>((third*pix+1u)/3u);
            for(int xx=left;xx<left+side;xx+=10){
                MoveToEx(dc,xx,yy,nullptr);LineTo(dc,xx+5<left+side?xx+5:left+side,yy);
            }
        }
        SelectObject(dc,old);DeleteObject(sub_pen);
    }
    W32H axis=CreatePen(0,2,grey);old=SelectObject(dc,axis);
    MoveToEx(dc,left,bottom,nullptr);LineTo(dc,left+side,bottom);
    MoveToEx(dc,left,bottom,nullptr);LineTo(dc,left,top);
    SelectObject(dc,old);DeleteObject(axis);
    const auto line=swe::diaery::graph::application_of_ymxb(g_length_inches,g_girth_inches);
    if(line.valid) {
        W32H pen=CreatePen(0,3,teal);old=SelectObject(dc,pen);
        MoveToEx(dc,plot_x(line.y_end.x,left,pix),plot_y(line.y_end.y,bottom,pix),nullptr);
        LineTo(dc,plot_x(line.x_end.x,left,pix),plot_y(line.x_end.y,bottom,pix));
        SelectObject(dc,old);DeleteObject(pen);
        label(dc,475,135,"APPLICATION: y = m*x + b",teal);
        text_value(dc,475,165,"m = -Y/X",line.slope,teal);
        text_value(dc,475,192,"b = Y (inches)",line.intercept,teal);
    } else {
        label(dc,475,135,"HOLD: invalid slope/intercept input",gold);
    }
    // XY subject marker is distinct from derived intercept endpoints.
    const int sx=plot_x(g_length_inches,left,pix),sy=plot_y(g_girth_inches,bottom,pix);
    W32H marker=CreateSolidBrush(purple);
    W32RECT dot{sx-5,sy-5,sx+6,sy+6};FillRect(dc,&dot,marker);DeleteObject(marker);
    label(dc,sx+10,sy-12,"S (source selection)",purple);
    text_value(dc,475,255,"X length (in)",g_length_inches,grey);
    text_value(dc,475,282,"Y girth (in)",g_girth_inches,grey);
    text_value(dc,475,309,"Z depth (in)",g_depth_inches,gold);
    label(dc,475,345,"NULL GUARD: RESEARCH HOLD",gold);
    label(dc,475,369,"No paired subject or valid error budget verified",grey);
    label(dc,475,392,"SIGMA uncertainty: NULL (not zero)",grey);
    label(dc,475,420,"Derived MINTING is display-only",grey);
    label(dc,475,443,g_dynamo_mint?"MINTING: ON (-0.03)":"MINTING: OFF",grey);
    if(g_dynamo_mint){
        // A second derived point only. The S source marker is unchanged.
        const double dx=swe::diaery::graph::minting(g_length_inches);
        const double dy=swe::diaery::graph::minting(g_girth_inches);
        W32H brush=CreateSolidBrush(gold);
        const int mx=plot_x(dx,left,pix),my=plot_y(dy,bottom,pix);
        W32RECT d{mx-3,my-3,mx+4,my+4};FillRect(dc,&d,brush);DeleteObject(brush);
    }
    label(dc,83,85,"Y (in)",grey);label(dc,285,489,"X (in) ->",grey);
    label(dc,475,477,g_subunits?"GRID: X /4 ; Y /3":"GRID: MAJOR ONLY",grey);
    paint_time_plot(dc);
    label(dc,22,rc.bottom-70,"D: DIAERY/DYNAMO | arrows: X/Y | PgUp/PgDn: Z | G: subunits",grey);
    label(dc,22,rc.bottom-46,"M: MINTING | F: +1fs | S: +1s | T: trace | source remains intact",grey);
}

static void paint(W32H hwnd){
    W32PAINT ps{};W32H dc=BeginPaint(hwnd,&ps);
    W32RECT rc{};GetClientRect(hwnd,&rc);
    W32H bg=CreateSolidBrush(rgb(8,18,30));FillRect(dc,&rc,bg);DeleteObject(bg);
    SetBkMode(dc,1);
    if(g_dynamo){paint_dynamo(dc,rc);EndPaint(hwnd,&ps);return;}
    label(dc,22,15,"DIAERY | XHTML 1.0 STRICT PROFILE | 1:1-PARITY-GATE: PASS",rgb(122,225,211));
    label(dc,22,37,"Native ISO C++20 geometry; no remote content, no script execution",rgb(209,199,173));
    unsigned row=0u;
    for(unsigned i=0;i<g_page.count && row<20u;++i){
        if(dia::strict::prefix(g_page.lines[i].text,"PARITY-")||
           dia::strict::prefix(g_page.lines[i].text,"STAGE-"))continue;
        label(dc,18,90+static_cast<int>(row)*24,g_page.lines[i].text,
              g_page.lines[i].tag=='p'?rgb(180,196,209):rgb(243,209,154));
        ++row;
    }
    label(dc,20,rc.bottom-65,"Left/Right: folds | drag: rotate | +/-: zoom | W: water | Y: YANG cull | C: cutaway | B: bubble",rgb(164,207,225));
    label(dc,20,rc.bottom-41,"Capped surface only at stage 4+. Physical sealing and water fill are NOT verified.",rgb(207,168,125));
    int centerX=rc.right>900?rc.right-320:rc.right/2;
    int centerY=rc.bottom>720?rc.bottom/2:rc.bottom/2+80;
    if(g_bubble){
        W32H pen=CreatePen(0,2,rgb(44,137,162));
        W32H prev=SelectObject(dc,pen);
        // A 2D enclosing circle motif: illustrative, not a 3D sphere renderer.
        for(int i=0;i<120;++i){
            const double a=(static_cast<double>(i)*6.283185307179586)/120.;
            const int x=centerX+static_cast<int>(245*trig_cos(a));
            const int y=centerY+static_cast<int>(245*trig_sin(a));
            if(i==0)MoveToEx(dc,x,y,nullptr);else LineTo(dc,x,y);
        }
        SelectObject(dc,prev);DeleteObject(pen);
    }
    const unsigned effective=static_cast<unsigned>(g_stage<3?g_stage:3);
    const bool capped=g_stage>=4;
    for(unsigned panel=0u;panel<4u;++panel){
        const long long ly=15LL*panel,hy=ly+15;
        auto pa=swe::three_geometric_folds::PROJECT({0,ly},panel,effective);
        auto pb=swe::three_geometric_folds::PROJECT({60,ly},panel,effective);
        auto pc=swe::three_geometric_folds::PROJECT({60,hy},panel,effective);
        auto pd=swe::three_geometric_folds::PROJECT({0,hy},panel,effective);
        swe::rectangloid_primitive::Quad q{{pa.x,pa.y,pa.z},{pb.x,pb.y,pb.z},
                                           {pc.x,pc.y,pc.z},{pd.x,pd.y,pd.z}};
        W32D colors[4]{rgb(107,155,173),rgb(117,191,184),rgb(167,152,202),rgb(205,166,116)};
        quad(dc,q,centerX,centerY,colors[panel],true);
    }
    if(capped){
        // Only the model at stage 3 is topologically capped.
        if(g_water && g_cutaway){
            quad(dc,swe::rectangloid_primitive::END_CAP(30),centerX,centerY,rgb(45,149,191),false);
        }
        if(!g_cutaway)quad(dc,swe::rectangloid_primitive::END_CAP(0),centerX,centerY,rgb(183,199,200),false);
        quad(dc,swe::rectangloid_primitive::END_CAP(60),centerX,centerY,rgb(200,196,171),false);
        if(g_water)label(dc,centerX-110,centerY+216,"Water: visual selection",rgb(96,206,237));
        if(g_yang)label(dc,centerX-110,centerY+236,"YANG: face filter selected",rgb(235,192,125));
    }
    const char* stages[6]{"FLAT","90deg hinge 1","90deg hinge 2","90deg hinge 3","TWO CAPS","VISUAL FUSION"};
    label(dc,centerX-110,centerY-235,stages[g_stage],rgb(243,212,155));
    EndPaint(hwnd,&ps);
}
static int mouse_component(W32L lp,bool upper){
    const unsigned value=static_cast<unsigned>(upper?(lp>>16):(lp&65535LL))&65535u;
    return static_cast<int>(static_cast<short>(value));
}
static W32L DIA_STDCALL window_proc(W32H hwnd,W32U msg,W32W w,W32L l){
    if(msg==WM_PAINT){paint(hwnd);return 0;}
    if(msg==WM_DESTROY){PostQuitMessage(0);return 0;}
    if(msg==WM_LBUTTONDOWN&&g_dynamo)return 0;
    if(msg==WM_MOUSEMOVE&&g_dynamo)return 0;
    if(msg==WM_LBUTTONDOWN){g_drag=true;g_mouse_x=mouse_component(l,false);g_mouse_y=mouse_component(l,true);SetCapture(hwnd);return 0;}
    if(msg==WM_LBUTTONUP){g_drag=false;ReleaseCapture();return 0;}
    if(msg==WM_MOUSEMOVE&&g_drag){
        const int x=mouse_component(l,false),y=mouse_component(l,true);
        g_yaw+=x-g_mouse_x;g_pitch+=y-g_mouse_y;
        if(g_pitch>80)g_pitch=80;if(g_pitch< -80)g_pitch=-80;
        g_mouse_x=x;g_mouse_y=y;InvalidateRect(hwnd,nullptr,0);return 0;
    }
    if(msg==WM_KEYDOWN){
        const unsigned key=static_cast<unsigned>(w);
        if(key=='D'){g_dynamo=!g_dynamo;InvalidateRect(hwnd,nullptr,0);return 0;}
        if(g_dynamo){
            if(key==37u&&g_length_inches>4.0/2.54)g_length_inches-=0.05;
            if(key==39u&&g_length_inches<26.0/2.54)g_length_inches+=0.05;
            if(key==40u&&g_girth_inches>3.0/2.54)g_girth_inches-=0.05;
            if(key==38u&&g_girth_inches<19.0/2.54)g_girth_inches+=0.05;
            if(key==33u&&g_depth_inches<6.0)g_depth_inches+=0.05;
            if(key==34u&&g_depth_inches>0.0)g_depth_inches-=0.05;
            if(key=='M')g_dynamo_mint=!g_dynamo_mint;
            if(key=='G')g_subunits=!g_subunits;
            if(key=='T')g_trace_index=(g_trace_index+1u)%3u;
            if(key=='F'){
                const auto next=swe::diaery::trace::advance(g_elapsed_fs,1ULL);
                if(next.status==swe::diaery::trace::Status::Valid)g_elapsed_fs=next.fs;
            }
            if(key=='S'){
                const auto next=swe::diaery::trace::advance(g_elapsed_fs,swe::diaery::trace::FS_PER_SECOND);
                if(next.status==swe::diaery::trace::Status::Valid)g_elapsed_fs=next.fs;
            }
            if(g_length_inches<4.0/2.54)g_length_inches=4.0/2.54;
            if(g_length_inches>26.0/2.54)g_length_inches=26.0/2.54;
            if(g_girth_inches<3.0/2.54)g_girth_inches=3.0/2.54;
            if(g_girth_inches>19.0/2.54)g_girth_inches=19.0/2.54;
            if(g_depth_inches<0.0)g_depth_inches=0.0;
            if(g_depth_inches>6.0)g_depth_inches=6.0;
            InvalidateRect(hwnd,nullptr,0);return 0;
        }
        if(key==39u&&g_stage<5)++g_stage;
        if(key==37u&&g_stage>0)--g_stage;
        if((key==187u||key==107u)&&g_zoom<10)++g_zoom;
        if((key==189u||key==109u)&&g_zoom>2)--g_zoom;
        if(key=='W')g_water=!g_water;
        if(key=='Y')g_yang=!g_yang;
        if(key=='C')g_cutaway=!g_cutaway;
        if(key=='B')g_bubble=!g_bubble;
        InvalidateRect(hwnd,nullptr,0);return 0;
    }
    return DefWindowProcA(hwnd,msg,w,l);
}
static int show_native(){
    W32H instance=GetModuleHandleA(nullptr);
    W32CLASS cls{};cls.size=sizeof(cls);cls.proc=window_proc;cls.instance=instance;
    cls.class_name="SWE_DIAERY_STRICT";
    if(!RegisterClassExA(&cls))return 30;
    g_window=CreateWindowExA(0u,cls.class_name,"DIAERY | XHTML Strict Geometry Inspector",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,60,50,1300,900,nullptr,nullptr,instance,nullptr);
    if(!g_window)return 31;
    ShowWindow(g_window,1);UpdateWindow(g_window);
    W32MESSAGE msg{};while(GetMessageA(&msg,nullptr,0u,0u)>0){
        TranslateMessage(&msg);DispatchMessageA(&msg);
    }
    return 0;
}
#endif
int main(int argc,char** argv){
    const bool check=argc>=3&&dia::strict::same(argv[1],"--check");
    const bool reject=argc>=3&&dia::strict::same(argv[1],"--reject");
    const bool qualify=argc>=3&&dia::strict::same(argv[1],"--qualify");
    const bool dynamo=argc>=2&&dia::strict::same(argv[1],"--dynamo");
    const bool fusion=argc==4&&dia::strict::same(argv[1],"--fusion");
#if defined(_WIN32)
    const char* path=(check||reject||qualify||fusion)?argv[2]:(dynamo?(argc>=3?argv[2]:"diaery\\index.xhtml"):(argc>1?argv[1]:"diaery\\index.xhtml"));
#else
    const char* path=(check||reject||qualify||fusion)?argv[2]:(argc>1?argv[1]:"diaery/index.xhtml");
#endif
    g_loaded=load_validate(path);
    if(reject){
        if(g_loaded){print("DIAERY|NEGATIVE_GATE_FAILED\n");return 17;}
        print("DIAERY|NEGATIVE_REJECTED\n");return 0;
    }
    if(!g_loaded){print("DIAERY|XHTML_STRICT_OR_PARITY_HOLD\n");return 12;}
    if(check){print("DIAERY|1:1-PARITY-GATE=PASS|XHTML_STRICT_PROFILE=PASS|RELEASE=HOLD\n");return 0;}
    if(qualify||fusion) {
        const auto results=dia::qualification::TEST(g_page);
        if(!results.qualified){print("DIAERY|FUSION_QUALIFICATION=HOLD\n");return 18;}
        if(qualify) {
          print("DIAERY|FUSION_QUALIFICATION=PASS|BiDi=1|TriDi=1|PiDi=1|PiPi=SYMBOLIC|SigmaDi=1|SigmaSigma=1|GammaGamma=BOUNDED|WATCHDOG=PASS|RELEASE=HOLD\n");
          return 0;
        }
#if defined(_WIN32)
        if(!fusion_to_new_file(argv[3])) {
          print("DIAERY|FUSION_CREATE_NEW_FAILED\n");return 19;
        }
        print("DIAERY|FUSION_SOURCE_CREATED|QUALIFICATION=PASS|RELEASE=HOLD\n");
        return 0;
#else
        print("DIAERY|FUSION_OUTPUT_REQUIRES_NATIVE_WINDOWS\n");
        return 20;
#endif
    }
#if defined(_WIN32)
    if(dynamo)g_dynamo=true;
    return show_native();
#else
    print("DIAERY|PARITY_PASS|Native window is Windows-only; use --check on POSIX\n");return 0;
#endif
}
