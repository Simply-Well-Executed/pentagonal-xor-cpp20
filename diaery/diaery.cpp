// SPDX-License-Identifier: 0BSD
// DIAERY native, offline XHTML 1.0 Strict SUBSET viewer + 1:1 parity gate.
// No C++ standard library or system header is included. Geometry comes
// from the original repository, never reconstructed from XHTML markup.
#include "model_bridge.hpp"
#include "application_of_ymxb.hpp"
#include "dynamo_subject.hpp"
#include "trace_timeline.hpp"
#include "filtrox.hpp"
#include "projections.hpp"
#include "thailonga.hpp"
#include "freq.hpp"
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
DIA_IMPORT W32H DIA_STDCALL GetProcessHeap();
DIA_IMPORT W32H DIA_STDCALL HeapAlloc(W32H,W32D,W32W);
DIA_IMPORT W32H DIA_STDCALL HeapReAlloc(W32H,W32D,W32H,W32W);
DIA_IMPORT int DIA_STDCALL HeapFree(W32H,W32D,W32H);
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
static constexpr W32D WM_KEYDOWN=0x0100u,WM_CHAR=0x0102u,WM_LBUTTONDOWN=0x0201u;
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
static bool g_study_gears=true;
static bool g_freq_view=false;
static swe::diaery::freq::Samples g_freq_x{};
static bool g_freq_memory_hold=false;
static swe::diaery::freq::Samples g_freq_y{};
static swe::diaery::freq::Samples g_freq_z{};
static unsigned g_freq_rejected=0u;
static bool g_projection_visible[10]{true,true,false,false,false,false,false,false,false,false};
static unsigned g_projection_selected=1u;
static bool g_orgasm_band_proposal=true;
static bool g_dynamo_mint=false;
static bool g_subunits=true;
static swe::diaery::trace::U64 g_elapsed_fs=0ULL;
static unsigned g_trace_index=0u;
static bool g_trace_editing=false,g_trace_rejected=false,g_skip_trace_open_char=false;
static char g_trace_input[16]{"Y=MX+B"};
static unsigned g_trace_used=6u;
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
    label(dc,left,537,"THREAD fs | DARN exact seconds+fs | YARN derived",grey);
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
    label(dc,475,530,"TRACE: bounded format",grey);
    label(dc,475,552,g_trace_input,thread);
    if(g_trace_editing)label(dc,475,568,"EDIT: type then Enter | Esc cancels",grey);
    else if(g_trace_rejected)label(dc,475,568,"HOLD: unknown trace expression",grey);
    if(line.valid && swe::diaery::research::included(swe::diaery::research::assess({nullptr,swe::diaery::research::Measurement::VaginalDepth,g_depth_inches,true}))){
        W32H trace_pen=CreatePen(0,1,rgb(108,172,174));
        old=SelectObject(dc,trace_pen);
        bool first=true;
        for(int step=0;step<=40;++step){
            // 2-second viewport, 1/20-second display sampling ONLY.
            const double seconds=static_cast<double>(step)/20.0;
            const auto value=tr::calculate(tr::parse(g_trace_input),seconds,line.slope,line.intercept);
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
    label(dc,475,585,"DARN SECONDS:",grey);label(dc,640,585,secbuf,thread);
    label(dc,475,607,"DARN REMAINDER fs:",grey);label(dc,640,607,rembuf,thread);
    const auto yarn_view=tr::make_yarn(g_elapsed_fs);
    char yarnbuf[24]{},sectorbuf[24]{};
    format_u64(yarnbuf,24u,yarn_view.shown_yarn_seconds);
    format_u64(sectorbuf,24u,static_cast<tr::U64>(yarn_view.sploon));
    label(dc,475,631,"YARN (derived sec):",grey);
    label(dc,640,631,yarn_view.status==tr::Status::Valid?yarnbuf:"HOLD",thread);
    label(dc,475,654,"IMPEREAL SPLOON /16:",grey);label(dc,680,654,sectorbuf,thread);
    label(dc,475,676,"YARN: +0.1; >0.6 carry; >=1 bonus",grey);
    const auto im=tr::n_minus_1663_pi_squared(yarn_view.sploon);
    char imbuf[24]{};
    if(im.status==tr::Status::Valid)decimal4(imbuf,24u,im.derived);
    label(dc,475,698,"n - (1663 * r):",grey);
    label(dc,690,698,im.status==tr::Status::Valid?imbuf:"HOLD",thread);
    label(dc,83,714,"F +1fs | S +1s | T cycle | R edit | G grid",grey);
    // Independent normalized marker ONLY. It is neither a physical
    // anatomy coordinate nor an observed orgasm probability.
    if(g_orgasm_band_proposal){
        const auto band=swe::diaery::projections::historical_female_orgasm_band;
        const auto state=swe::diaery::projections::band_qualification(band,false,false);
        label(dc,83,732,"PROJECTION: FEMALE ORGASM 41-42% | UNVERIFIED",thread);
        W32H guide=CreatePen(0,1,grey);
        W32H previous=SelectObject(dc,guide);
        MoveToEx(dc,83,754);LineTo(dc,403,754);
        SelectObject(dc,previous);DeleteObject(guide);
        const int begin=83+static_cast<int>(320u*band.lower_permyriad/10000u);
        const int end=83+static_cast<int>((320u*band.upper_permyriad+9999u)/10000u);
        W32H highlight=CreateSolidBrush(rgb(213,172,99));
        W32RECT strip{begin,746,end,762};
        FillRect(dc,&strip,highlight);DeleteObject(highlight);
        label(dc,83,769,"0%",grey);
        label(dc,383,769,"100%",grey);
        label(dc,475,754,state==swe::diaery::projections::Status::NullEvidence?
          "NULL: source/outcome not calibrated":"BAND READY",thread);
    }
    label(dc,83,680,"SECONDARY TIME AXIS: 0 to 2 seconds; exact counter never rounds to samples",grey);
}

// Grow as available, bounded only by host addressable memory and safety.
static bool ensure_freq_capacity(swe::diaery::freq::Samples& s){
    using swe::diaery::freq::U64;
    if(s.count<s.capacity)return true;
    const U64 next=s.capacity?(s.capacity<=~0ULL/2ULL?s.capacity*2ULL:0ULL):64ULL;
    if(next<=s.capacity || next>~0ULL/sizeof(double))return false;
    const U64 size=next*sizeof(double);
    W32H heap=GetProcessHeap();
    W32H buffer=s.values?HeapReAlloc(heap,0u,s.values,size):
                           HeapAlloc(heap,0u,size);
    if(!buffer)return false;
    s.values=static_cast<double*>(buffer);
    s.capacity=next;
    return true;
}
static double gear_length(double v){return swe::diaery::filtrox::clamp_to_study(v,swe::diaery::research::Measurement::PenileLength);}
static double gear_girth(double v){return swe::diaery::filtrox::clamp_to_study(v,swe::diaery::research::Measurement::PenileCircumference);}
static double gear_depth(double v){return swe::diaery::filtrox::clamp_to_study(v,swe::diaery::research::Measurement::VaginalDepth);}
// FREQ is a separate projectorial facet of THAILONGA. Each measurement
// has its own histogram. No actual research data are imported here.
static void paint_frequency_panel(W32H dc,int left,int top,
                                  const char* title,
                                  const swe::diaery::freq::Samples& samples,
                                  double selected_value,
                                  W32D color){
    namespace fq=swe::diaery::freq;
    const auto hist=fq::count_bins(samples);
    const auto fitted=fq::fit_bell(samples);
    const int width=288,height=230,base=top+height;
    label(dc,left,top-54,title,color);
    char counter[24]{};
    format_u64(counter,24u,static_cast<swe::diaery::trace::U64>(samples.count));
    label(dc,left,top-31,"MANUAL OBSERVATIONS:",rgb(166,180,196));
    label(dc,left+178,top-31,counter,color);
    // The count at the selected VALUE is a separate exact query.
    const auto exact=swe::diaery::freq::count_at_value(samples,selected_value);
    char valbuf[24]{},countbuf[24]{};
    decimal4(valbuf,24u,selected_value);
    format_u64(countbuf,24u,exact);
    label(dc,left,top-72,"COUNT AT VALUE:",rgb(166,180,196));
    label(dc,left+130,top-72,valbuf,color);
    label(dc,left+213,top-72,countbuf,color);
    W32H axes=CreatePen(0,1,rgb(125,146,163));
    W32H old=SelectObject(dc,axes);
    MoveToEx(dc,left,top,nullptr);LineTo(dc,left,base);
    LineTo(dc,left+width,base);
    SelectObject(dc,old);DeleteObject(axes);
    if(hist.state!=fq::State::Ready){
        label(dc,left+48,top+90,"FREQ: NULL / NO DATA",rgb(237,190,117));
        label(dc,left+35,top+117,"Press A to record X/Y/Z",rgb(159,176,194));
        return;
    }
    const swe::diaery::freq::U64 scale=hist.maximum_count?hist.maximum_count:1ULL;
    for(unsigned i=0;i<fq::BINS;++i){
        const int x=left+static_cast<int>(i)*18+2;
        const int bar=static_cast<int>(static_cast<double>(hist.bins[i])*static_cast<double>(height)/static_cast<double>(scale));
        if(bar<=0)continue;
        W32RECT rect{x,base-bar,x+14,base};
        W32H brush=CreateSolidBrush(color);
        FillRect(dc,&rect,brush);
        DeleteObject(brush);
    }
    char minbuf[24]{},maxbuf[24]{};
    decimal4(minbuf,24u,hist.minimum);
    decimal4(maxbuf,24u,hist.maximum);
    label(dc,left,base+9,minbuf,rgb(159,176,194));
    label(dc,left+225,base+9,maxbuf,rgb(159,176,194));
    format_u64(counter,24u,static_cast<swe::diaery::trace::U64>(scale));
    label(dc,left,top-12,"MAX COUNT:",rgb(159,176,194));
    label(dc,left+94,top-12,counter,rgb(221,217,164));
    if(fitted.state!=fq::State::Ready){
        label(dc,left,base+30,"BELL: NULL / insufficient variance or N",rgb(237,190,117));
        return;
    }
    // Gaussian overlay is an exploratory parametric fit on manual samples;
    // bars are empirical counts. No confidence interval is implied.
    W32H pen=CreatePen(0,2,rgb(232,211,142));
    old=SelectObject(dc,pen);
    bool first=true;
    for(unsigned step=0;step<=144u;++step){
        const double span=hist.maximum-hist.minimum;
        const double width=span>0.0?span/static_cast<double>(fq::BINS):0.0;
        const double value=hist.minimum+(static_cast<double>(step)+0.5)/144.0*span;
        const double bell=fq::expected_bin_count(fitted,value,width);
        int bh=static_cast<int>(bell*static_cast<double>(height)/static_cast<double>(scale)+0.5);
        if(bh<0)bh=0;if(bh>height)bh=height;
        const int gx=left+static_cast<int>(step)*2;
        const int gy=base-bh;
        if(first){MoveToEx(dc,gx,gy,nullptr);first=false;}
        else LineTo(dc,gx,gy);
    }
    SelectObject(dc,old);DeleteObject(pen);
    label(dc,left,base+30,"BELL: FITTED TO MANUAL VALUES",rgb(232,211,142));
}
static void paint_frequency(W32H dc,const W32RECT& rc){
    const W32D grey=rgb(159,176,194);
    label(dc,22,16,"THAILONGA | FREQ = COUNT AT VALUE | RELEASE=HOLD",rgb(95,212,202));
    label(dc,22,39,"FREQ HISTOGRAMS | X = observed VALUE | Y = COUNT | bin span derives from data",grey);
    label(dc,22,62,"Source: manual exploratory selections; NOT imported studies",grey);
    label(dc,22,87,"A: append current valid X/Y/Z values | H: graph view",grey);
    paint_frequency_panel(dc,46,175,"X / PENILE LENGTH",g_freq_x,g_length_inches,rgb(95,212,202));
    paint_frequency_panel(dc,445,175,"Y / PENILE CIRCUMFERENCE",g_freq_y,g_girth_inches,rgb(192,141,233));
    paint_frequency_panel(dc,844,175,"Z / VAGINAL DEPTH",g_freq_z,g_depth_inches,rgb(237,190,117));
    char rejected[24]{};
    format_u64(rejected,24u,static_cast<swe::diaery::trace::U64>(g_freq_rejected));
    label(dc,46,515,"REJECTED APPEND ATTEMPTS (NOT OBSERVATIONS):",grey);
    label(dc,460,515,rejected,rgb(237,190,117));
    label(dc,46,544,"FILTREX rejects zero, negative, missing and nonfinite values.",grey);
    label(dc,46,570,"FILTROX sample ranges are labels, not universal biological limits.",grey);
    label(dc,46,596,"The Gaussian is a fitted approximation, NOT original observations.",grey);
    label(dc,46,621,g_freq_memory_hold?"MEMORY HOLD: append denied, old records preserved":"Empty records remain NULL; no artificial record-count limit.",grey);
    label(dc,46,rc.bottom-46,"H: DYNTR0X/PROJECTIONS | D: DIAERY | FREQ source remains in session memory",grey);
}
static void paint_dynamo(W32H dc,const W32RECT& rc){
    const W32D grey=rgb(159,176,194),teal=rgb(95,212,202),gold=rgb(237,190,117),purple=rgb(192,141,233);
    label(dc,22,16,"THAILONGA | DYNTR0X + PROJECTIONS | native C++20 | HOLD",teal);
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
    // THAILONGA is the common source of truth for both rendered aspects.
    // Never infer paired scientific evidence from a selected graph coordinate.
    const auto shared=swe::diaery::thailonga::build(
        {g_length_inches,g_girth_inches,g_depth_inches,true,true,true},
         g_elapsed_fs);
    const auto line=shared.intercept;
    const bool filtrex_eligible=shared.eligible;
    label(dc,475,104,shared.eligible?
        "AM I VALID? INPUT YES":"AM I VALID? FILTREX HOLD",
        shared.eligible?teal:gold);
    label(dc,475,123,"RESEARCH: HOLD | UNKNOWN UNCERTAINTY",gold);
    namespace fx=swe::diaery::filtrox;
    namespace rs=swe::diaery::research;
    const auto x_check=shared.x.filtrox;
    const auto y_check=shared.y.filtrox;
    const auto z_check=shared.z.filtrox;
    const bool within=shared.within_sample_ranges;
    label(dc,475,218,within?"FILTROX: IN STUDY RANGES":"FILTROX: OUTSIDE SAMPLE / NULL",within?teal:gold);
    if(line.valid && filtrex_eligible && g_projection_visible[1]) {
        W32H pen=CreatePen(0,3,teal);old=SelectObject(dc,pen);
        MoveToEx(dc,plot_x(line.y_end.x,left,pix),plot_y(line.y_end.y,bottom,pix),nullptr);
        LineTo(dc,plot_x(line.x_end.x,left,pix),plot_y(line.x_end.y,bottom,pix));
        SelectObject(dc,old);DeleteObject(pen);
        label(dc,475,135,"APPLICATION: y = m*x + b",teal);
        text_value(dc,475,165,"m = -Y/X",line.slope,teal);
        text_value(dc,475,192,"b = Y (inches)",line.intercept,teal);
    } else if(!line.valid || !filtrex_eligible) {
        label(dc,475,135,"HOLD: invalid slope/intercept input",gold);
    }
    // Never plot invalid, non-finite or negative source coordinates.
    // The point is an unverified selection, not a real participant record.
    if(line.valid && filtrex_eligible && g_projection_visible[0]){
        const int sx=plot_x(g_length_inches,left,pix),sy=plot_y(g_girth_inches,bottom,pix);
        W32H marker=CreateSolidBrush(purple);
        W32RECT dot{sx-5,sy-5,sx+6,sy+6};FillRect(dc,&dot,marker);DeleteObject(marker);
        label(dc,sx+10,sy-12,"S (unverified selection)",purple);
    }
    // PROJECTIONS are transparent, independent overlays. Only observed
    // source and well-defined algebraic references are currently drawable.
    if(filtrex_eligible && line.valid){
        if(g_projection_visible[2]){
            W32H layer=CreatePen(0,1,rgb(150,151,215));
            W32H previous=SelectObject(dc,layer);
            MoveToEx(dc,left,bottom,nullptr);LineTo(dc,left+side,top);
            SelectObject(dc,previous);DeleteObject(layer);
        }
        if(g_projection_visible[3]){
            W32H layer=CreatePen(0,1,rgb(203,171,123));
            W32H previous=SelectObject(dc,layer);
            MoveToEx(dc,left,bottom-2,nullptr);LineTo(dc,left+side,bottom-2);
            SelectObject(dc,previous);DeleteObject(layer);
        }
    }
    const auto selected_projection=swe::diaery::projections::describe(g_projection_selected);
    const auto selected_status=shared.projection_status[g_projection_selected];
    label(dc,475,74,selected_projection.label,purple);
    label(dc,475,90,selected_status==swe::diaery::projections::Status::Ready?
        "PROJECTION: ALGEBRAIC / UNVERIFIED":
        "PROJECTION: NULL | EVIDENCE MISSING",selected_status==
        swe::diaery::projections::Status::Ready?grey:gold);
    // 41-42% is an unverified historical proposal. No calibrated
    // probability, anatomical axis or orgasm predictor is claimed.
    if(g_orgasm_band_proposal)
        label(dc,83,508,"ORGASM BAND 41-42%: EXPLORE / NULL (NO DEFINED OUTCOME)",gold);
    text_value(dc,475,255,"X length (in)",g_length_inches,grey);
    text_value(dc,475,282,"Y girth (in)",g_girth_inches,grey);
    text_value(dc,475,309,"Z depth (in)",g_depth_inches,gold);
    label(dc,475,337,
        x_check.position==fx::Position::BelowObserved?"X: BELOW HERBENICK RANGE":
        x_check.position==fx::Position::AboveObserved?"X: ABOVE HERBENICK RANGE":
        x_check.position==fx::Position::NullInput?"X: FILTREX EXCLUDED":"X: WITHIN SAMPLE",grey);
    label(dc,475,359,
        y_check.position==fx::Position::BelowObserved?"Y: BELOW HERBENICK RANGE":
        y_check.position==fx::Position::AboveObserved?"Y: ABOVE HERBENICK RANGE":
        y_check.position==fx::Position::NullInput?"Y: FILTREX EXCLUDED":"Y: WITHIN SAMPLE",grey);
    label(dc,475,381,
        z_check.position==fx::Position::BelowObserved?"Z: BELOW LLOYD RANGE":
        z_check.position==fx::Position::AboveObserved?"Z: ABOVE LLOYD RANGE":
        z_check.position==fx::Position::NullInput?"Z: FILTREX EXCLUDED":"Z: WITHIN SAMPLE",grey);
    label(dc,475,403,"Study extent is not a biological minimum",grey);
    label(dc,475,426,"NULL GUARD: RESEARCH HOLD",gold);
    label(dc,475,448,"SIGMA uncertainty: NULL (not zero)",grey);
    label(dc,475,470,g_dynamo_mint?"MINTING: ON (-0.03)":"MINTING: OFF",grey);
    if(g_dynamo_mint && line.valid && filtrex_eligible){
        // A second derived point only. The S source marker is unchanged.
        const double dx=swe::diaery::graph::minting(g_length_inches);
        const double dy=swe::diaery::graph::minting(g_girth_inches);
        W32H brush=CreateSolidBrush(gold);
        const int mx=plot_x(dx,left,pix),my=plot_y(dy,bottom,pix);
        W32RECT d{mx-3,my-3,mx+4,my+4};FillRect(dc,&d,brush);DeleteObject(brush);
    }
    label(dc,83,85,"Y (in)",grey);label(dc,285,489,"X (in) ->",grey);
    label(dc,475,494,g_study_gears?"GEARS: SAMPLE-RANGE (K)":"GEARS: EXPLORATORY (K)",grey);
    paint_time_plot(dc);
    label(dc,22,rc.bottom-70,"D: DIAERY/DYNTR0X | arrows X/Y | PgUp/PgDn Z | G grid | K study",grey);
    label(dc,22,rc.bottom-46,"H frequency | A add manual sample | P / 0..9 projections | N band",grey);
}

static void paint(W32H hwnd){
    W32PAINT ps{};W32H dc=BeginPaint(hwnd,&ps);
    W32RECT rc{};GetClientRect(hwnd,&rc);
    W32H bg=CreateSolidBrush(rgb(8,18,30));FillRect(dc,&rc,bg);DeleteObject(bg);
    SetBkMode(dc,1);
    if(g_dynamo){if(g_freq_view)paint_frequency(dc,rc);else paint_dynamo(dc,rc);EndPaint(hwnd,&ps);return;}
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
    if(msg==WM_DESTROY){
        W32H heap=GetProcessHeap();
        if(g_freq_x.values)HeapFree(heap,0u,g_freq_x.values);
        if(g_freq_y.values)HeapFree(heap,0u,g_freq_y.values);
        if(g_freq_z.values)HeapFree(heap,0u,g_freq_z.values);
        g_freq_x={};g_freq_y={};g_freq_z={};
        PostQuitMessage(0);return 0;
    }
    if(msg==WM_CHAR&&g_dynamo&&g_trace_editing){
        const unsigned chr=static_cast<unsigned>(w);
        if(g_skip_trace_open_char){g_skip_trace_open_char=false;return 0;}
        if(chr==13u){
            g_trace_editing=false;
            const int projection=swe::diaery::projections::parse(g_trace_input);
            if(projection>=0){
                g_projection_selected=static_cast<unsigned>(projection);
                g_projection_visible[g_projection_selected]=true;
                g_trace_rejected=false;
            }else{
                g_trace_rejected=swe::diaery::trace::parse(g_trace_input)==swe::diaery::trace::Pattern::Invalid;
            }
        } else if(chr==8u) {
            if(g_trace_used>0u)g_trace_input[--g_trace_used]=0;
        } else if(chr>=32u&&chr<=126u&&g_trace_used<15u){
            g_trace_input[g_trace_used++]=static_cast<char>(chr);
            g_trace_input[g_trace_used]=0;
        }
        InvalidateRect(hwnd,nullptr,0);return 0;
    }
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
            if(g_trace_editing){if(key==27u)g_trace_editing=false;InvalidateRect(hwnd,nullptr,0);return 0;}
            if(key=='R'){g_trace_editing=true;g_skip_trace_open_char=true;g_trace_used=0u;g_trace_input[0]=0;g_trace_rejected=false;InvalidateRect(hwnd,nullptr,0);return 0;}
            if(key==37u)g_length_inches-=0.05;
            if(key==39u)g_length_inches+=0.05;
            if(key==40u)g_girth_inches-=0.05;
            if(key==38u)g_girth_inches+=0.05;
            if(key==33u)g_depth_inches+=0.05;
            if(key==34u)g_depth_inches-=0.05;
            if(key=='H')g_freq_view=!g_freq_view;
            if(key=='A'){
                const auto current=swe::diaery::thailonga::build(
                    {g_length_inches,g_girth_inches,g_depth_inches,true,true,true},
                    g_elapsed_fs);
                // Transactional append: either all three measurements added
                // or none. An unverified manual XY/Z triple is not a
                // scientifically paired observation.
                if(current.eligible &&
                   swe::diaery::freq::admissible(g_length_inches)&&
                   swe::diaery::freq::admissible(g_girth_inches)&&
                   swe::diaery::freq::admissible(g_depth_inches)&&
                   ensure_freq_capacity(g_freq_x)&&
                   ensure_freq_capacity(g_freq_y)&&
                   ensure_freq_capacity(g_freq_z)){
                    // All three slots have room before mutating any count.
                    (void)swe::diaery::freq::append(g_freq_x,g_length_inches);
                    (void)swe::diaery::freq::append(g_freq_y,g_girth_inches);
                    (void)swe::diaery::freq::append(g_freq_z,g_depth_inches);
                }else{
                    if(current.eligible)g_freq_memory_hold=true;
                    ++g_freq_rejected;
                }
            }
            if(key=='K')g_study_gears=!g_study_gears;
            if(key=='P')g_projection_selected=(g_projection_selected+1u)%10u;
            if(key=='N')g_orgasm_band_proposal=!g_orgasm_band_proposal;
            if(key>='0'&&key<='9'){
                const unsigned slot=key-'0';
                g_projection_visible[slot]=!g_projection_visible[slot];
                g_projection_selected=slot;
            }
            if(key=='M')g_dynamo_mint=!g_dynamo_mint;
            if(key=='G')g_subunits=!g_subunits;
            if(key=='T'){
                g_trace_index=(g_trace_index+1u)%3u;
                const char* choices[3]{"Y=MX+B","Y=X","Y=0"};
                unsigned k=0u;
                while(choices[g_trace_index][k]&&k<15u){g_trace_input[k]=choices[g_trace_index][k];++k;}
                g_trace_input[k]=0;g_trace_used=k;g_trace_rejected=false;
            }
            if(key=='F'){
                const auto next=swe::diaery::trace::advance(g_elapsed_fs,1ULL);
                if(next.status==swe::diaery::trace::Status::Valid)g_elapsed_fs=next.fs;
            }
            if(key=='S'){
                const auto next=swe::diaery::trace::advance(g_elapsed_fs,swe::diaery::trace::FS_PER_SECOND);
                if(next.status==swe::diaery::trace::Status::Valid)g_elapsed_fs=next.fs;
            }
            if(g_study_gears){
                g_length_inches=gear_length(g_length_inches);
                g_girth_inches=gear_girth(g_girth_inches);
                g_depth_inches=gear_depth(g_depth_inches);
            }else{
                if(g_length_inches<0.0)g_length_inches=0.0;
                if(g_length_inches>12.0)g_length_inches=12.0;
                if(g_girth_inches<0.0)g_girth_inches=0.0;
                if(g_girth_inches>12.0)g_girth_inches=12.0;
                if(g_depth_inches<0.0)g_depth_inches=0.0;
                if(g_depth_inches>12.0)g_depth_inches=12.0;
            }
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
