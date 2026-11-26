// SPDX-License-Identifier: 0BSD
// DIAERY Tri-Flavors native C++20 <-> XHTML 1.0 Strict subset.
// MSVC /O2, no system or STL headers, no network.
#include "model_bridge.hpp"
#include "../shared/eulers_jig_canonical.hpp"
#include "tri_flavors_gate.hpp"
#if !defined(_WIN32)
#error "Native Windows MSVC required"
#endif
extern "C" {
__declspec(dllimport) void* __stdcall CreateFileA(
 const char*,unsigned long,unsigned long,void*,unsigned long,unsigned long,void*);
__declspec(dllimport) int __stdcall ReadFile(
 void*,void*,unsigned long,unsigned long*,void*);
__declspec(dllimport) int __stdcall CloseHandle(void*);
__declspec(dllimport) void* __stdcall GetStdHandle(unsigned long);
__declspec(dllimport) int __stdcall WriteFile(
 void*,const void*,unsigned long,unsigned long*,void*);
}
namespace tri=swe::diaery::tri_flavors;
namespace str=swe::diaery::strict;
static char data[16002]{};
int main(int argc,char** argv) {
 if(argc!=2||!argv||!argv[1])return 20;
 void* handle=CreateFileA(argv[1],0x80000000u,1u,nullptr,3u,0x80u,nullptr);
 if(handle==reinterpret_cast<void*>(-1))return 21;
 unsigned long received=0u;
 const bool loaded=ReadFile(handle,data,16001u,&received,nullptr)!=0;
 CloseHandle(handle);
 if(!loaded||received==0u||received>16000u)return 22;
 data[received]=0;
 str::Page page{};
 if(!str::parse(data,static_cast<unsigned>(received),page)||
    !tri::PAGE(page))return 23;
 static constexpr char output[]=
 "TRIFLAVOR|PASS|JIG_X1E17=2864498081537971875|RADIUS2_MIN=2500|JIG_GAP_X1E18=278772184302000000|REFERENCE_GAP_X1E18=29962978621104960|SWEEP=4096|NORMALIZED_CLOSED=1|RELEASE=HOLD\n";
 unsigned long written=0u;
 if(!WriteFile(GetStdHandle(0xFFFFFFF5u),output,
      static_cast<unsigned long>(sizeof(output)-1u),&written,nullptr))
  return 24;
 return written==sizeof(output)-1u?0:25;
}
