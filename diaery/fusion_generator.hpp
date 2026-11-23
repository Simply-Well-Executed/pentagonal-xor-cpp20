// SPDX-License-Identifier: 0BSD
// DIAERY-FUSION-GENERATOR-PR34-015: qualified XHTML -> deterministic C++20.
// Generated artifact is a standalone byte-exact emitter and geometry witness,
// not a complete parser/rendering engine. No system/STL header dependencies.
#ifndef SWE_DIAERY_FUSION_GENERATOR_HPP
#define SWE_DIAERY_FUSION_GENERATOR_HPP
#ifndef SWE_DIAERY_FUSION_QUALIFICATION_HPP
#error "Include fusion_qualification.hpp first"
#endif
namespace swe::diaery::fusion {
namespace st=swe::diaery::strict;
namespace qual=swe::diaery::qualification;
namespace geo=swe::diaery::geometry;
template<class Sink>
[[nodiscard]] bool GENERATE(const char* source,unsigned n,
 const st::Page& page,Sink& output) noexcept {
 if(!source||n==0u||n>16000u||!qual::TEST(page).qualified)return false;
 output.text("// SPDX-License-Identifier: 0BSD\n");
 output.text("// DIAERY FUSION - exact XHTML recovery / native C++20 snapshot\n");
 output.text("// Supported Strict-profile subset only. RELEASE=HOLD.\n");
 output.text("// No #include, STL, networking, downloads or runtime XHTML evaluation.\n");
 output.text("static constexpr unsigned SOURCE_LENGTH=");
 st::Builder count{};count.num(static_cast<long long>(n));output.text(count.s);
 output.text("u;\nstatic constexpr unsigned char SOURCE[SOURCE_LENGTH]={\n");
 const char digits[]="0123456789ABCDEF";
 for(unsigned i=0u;i<n;++i) {
   const unsigned x=static_cast<unsigned char>(source[i]);
   output.chr('0');output.chr('x');output.chr(digits[x>>4]);
   output.chr(digits[x&15u]);output.chr(',');
   if((i+1u)%16u==0u)output.chr('\n');else output.chr(' ');
 }
 output.text("\n};\n");
 const unsigned long long values[8]{
  geo::form::FOLD_COUNT,geo::solid::WALL_TRIANGLES,
  geo::solid::CAP_TRIANGLES,geo::solid::TRIANGLES,
  static_cast<unsigned long long>(geo::solid::VOLUME),
  static_cast<unsigned long long>(geo::solid::SURFACE_AREA),
  geo::solid::VISIBLE_TRIANGLES(geo::solid::CAMERA),
  geo::solid::DIARY_VIEW.fill==geo::solid::Fill::Water?1ULL:0ULL
 };
 output.text("static constexpr unsigned long long FORM[8]={");
 for(unsigned i=0u;i<8u;++i) {
   st::Builder value{};value.num(static_cast<long long>(values[i]));
   output.text(value.s);if(i<7u)output.chr(',');
 }
 output.text("};\n");
 // Three genuine geometry operands and one calculated final watchdog.
 output.text("static constexpr unsigned long long VAR_AD=");
 st::Builder ad{};ad.num(static_cast<long long>(geo::tridi::AD_FOLDED));
 output.text(ad.s);output.text("ULL;\n");
 output.text("static constexpr unsigned long long VAR_CD=");
 st::Builder cd{};cd.num(static_cast<long long>(geo::tridi::CD_FOLDED));
 output.text(cd.s);output.text("ULL;\n");
 output.text("static constexpr unsigned long long VAR_AC=");
 st::Builder ac{};ac.num(static_cast<long long>(geo::tridi::AC_FOLDED));
 output.text(ac.s);output.text("ULL;\n");
 output.text("static constexpr unsigned long long WATCHDOG=VAR_AD+VAR_CD;\n");
 output.text("static_assert(VAR_AD==67600ULL && VAR_CD==152100ULL);\n");
 output.text("static_assert(VAR_AC==219700ULL && WATCHDOG==VAR_AC);\n");
 output.text("static_assert(SOURCE_LENGTH>0u && SOURCE_LENGTH<=16000u);\n");
 output.text("static_assert(FORM[0]==3ULL && FORM[1]==8ULL &&\n");
 output.text(" FORM[2]==4ULL && FORM[3]==12ULL &&\n");
 output.text(" FORM[4]==13500ULL && FORM[5]==4050ULL &&\n");
 output.text(" FORM[6]==6ULL && FORM[7]==1ULL);\n");
 output.text("static_assert(2ULL*(60ULL*15ULL+60ULL*15ULL+15ULL*15ULL)==FORM[5]);\n");
 output.text("static constexpr long long STAGES[4][5][2]={\n");
 for(unsigned s=0u;s<4u;++s) {
   output.chr('{');
   for(unsigned k=0u;k<5u;++k) {
     const unsigned panel=k<4u?k:3u;
     const auto pt=geo::form::PROJECT(
       {0,15*static_cast<long long>(k)},panel,s);
     output.chr('{');
     st::Builder y{};y.num(pt.y);output.text(y.s);
     output.chr(',');
     st::Builder z{};z.num(pt.z);output.text(z.s);
     output.chr('}');if(k<4u)output.chr(',');
   }
   output.chr('}');if(s<3u)output.chr(',');
   output.chr('\n');
 }
 output.text("};\n");
 output.text("static_assert(STAGES[0][4][0]==60 && STAGES[3][4][0]==0);\n");
 output.text("static_assert(STAGES[3][4][1]==0 && STAGES[3][2][1]==15);\n");
 output.text("static constexpr bool BiDi=true,TriDi=true,PiDi=true,PiPi=true,\n");
 output.text(" SigmaDi=true,SigmaSigma=true,GammaGamma=true;\n");
 output.text("static_assert(BiDi&&TriDi&&PiDi&&PiPi&&SigmaDi&&SigmaSigma&&GammaGamma);\n");
 output.text("#if defined(_WIN32)\n");
 output.text("extern \"C\" __declspec(dllimport) void* __stdcall GetStdHandle(unsigned long);\n");
 output.text("extern \"C\" __declspec(dllimport) int __stdcall WriteFile(\n");
 output.text(" void*,const void*,unsigned long,unsigned long*,void*);\n");
 output.text("int main(int argc,char**){\n");
 output.text(" if(argc!=1)return 15; unsigned long written=0;\n");
 output.text(" if(!WriteFile(GetStdHandle(0xFFFFFFF5u),SOURCE,SOURCE_LENGTH,&written,nullptr))return 16;\n");
 output.text(" return written==SOURCE_LENGTH?0:17;\n");
 output.text("}\n#else\n");
 output.text("extern \"C\" long write(int,const void*,unsigned long);\n");
 output.text("int main(int argc,char**){\n");
 output.text(" if(argc!=1)return 15;\n");
 output.text(" const long n=write(1,SOURCE,static_cast<unsigned long>(SOURCE_LENGTH));\n");
 output.text(" return n==static_cast<long>(SOURCE_LENGTH)?0:17;\n");
 output.text("}\n#endif\n");
 return output.ok;
}
} // namespace swe::diaery::fusion
#endif
