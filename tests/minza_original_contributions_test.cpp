// SPDX-License-Identifier: 0BSD
// Regression adapter: include unchanged contributor originals.
#define main minza_scalar_original_entry
#include "../tools/coreao_scalar_zlib_rfc1950_original.cpp"
#undef main
#define main minza_quicksort_original_entry
#include "../tools/devils_advocate_quicksort_original.cpp"
#undef main
#include <zlib.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <vector>
int main(){
 using namespace core_audit;
 constexpr std::string_view source="Audit the evidence, not the assumption.";
 auto [frame,record]=ZlibRfc1950Engine::wrap(source);
 if(!record.valid())return 1;
 auto [payload,ok]=ZlibRfc1950Engine::inspect_zero_copy(frame);
 if(!ok.valid() || !std::equal(payload.begin(),payload.end(),source.begin(),source.end()))return 2;
 frame[2]^=1u;
 if(ZlibRfc1950Engine::inspect_zero_copy(frame).second.status!=AuditStatus::ERR_ADLER32_MISMATCH)return 3;
 auto [pseudo,_unused]=ZlibRfc1950Engine::wrap(source);
 std::array<Bytef,4096> decoded{};uLong decodedSize=decoded.size();
 if(::uncompress(decoded.data(),&decodedSize,pseudo.data(),pseudo.size())==Z_OK)return 4;
 std::vector<std::vector<std::int64_t>> cases={{},{1},{3,2,1,0},{2,2,2,2},{-100,-1,-5,8,7,9}};
 for(int n=0;n<2048;n+=17){
  std::vector<std::int64_t> v(n);
  for(int i=0;i<n;i++)v[i]=std::int64_t((i*113)%97)-48;
  cases.push_back(std::move(v));
 }
 for(auto sample:cases){
  auto expected=sample;
  std::sort(expected.begin(),expected.end());
  auto verdict=sort_audit::DevilsAdvocateQuicksort::sort(sample);
  if(!verdict.passed() || sample!=expected)return 5;
 }
 std::cout<<"PASS preserved originals: Scalar checksum, intentional non-DEFLATE, Quicksort vs std::sort\n";
 return 0;
}
