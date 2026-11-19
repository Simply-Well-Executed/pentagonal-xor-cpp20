// SPDX-License-Identifier: 0BSD
// Read-only media audit gates, not claims of secret video extraction.
#include <zlib.h>
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string_view>
#include <vector>
namespace gates {
constexpr unsigned L=9, C=6;
constexpr unsigned lane(unsigned col,unsigned pos){
 const unsigned s=(pos+col)%L;
 return (col%2==0)?s:(L-1-s);
}
bool positive_and_fault_zlib(){
 constexpr std::string_view source="FLV RIFF ftyp: synthetic POSITIVE-CONTROL data only.";
 uLong cap=compressBound(source.size());
 std::vector<Bytef> compressed(cap);
 if(compress2(compressed.data(),&cap,reinterpret_cast<const Bytef*>(source.data()),source.size(),Z_BEST_COMPRESSION)!=Z_OK)return false;
 compressed.resize(cap);
 std::array<Bytef,4096> out{};
 uLong outsize=out.size();
 if(uncompress(out.data(),&outsize,compressed.data(),compressed.size())!=Z_OK ||
    outsize!=source.size() || std::memcmp(out.data(),source.data(),outsize)!=0)return false;
 compressed.back()^=1u;outsize=out.size();
 return uncompress(out.data(),&outsize,compressed.data(),compressed.size())!=Z_OK;
}
bool rotl_inversion(){
 for(unsigned v=0;v<256;v++)for(int n=0;n<256;n++){
  const auto byte=static_cast<std::uint8_t>(v);
  const auto wide=static_cast<std::uint32_t>(v);
  if(std::rotr(std::rotl(byte,n),n)!=byte || std::rotr(std::rotl(wide,n),n)!=wide)return false;
 }
 return true;
}
bool ninza_ping(){
 std::array<std::uint8_t,L*C> original{},mirrored{},braid{},restored{};
 for(unsigned i=0;i<L*C;i++)original[i]=static_cast<std::uint8_t>(i*19u+3u);
 mirrored=original;
 for(unsigned c=0;c<C;c++)for(unsigned p=0;p<L;p++)
  braid[c*L+p]=original[lane(c,p)*C+c];
 std::array<bool,L*C> visited{};
 for(unsigned c=0;c<C;c++)for(unsigned p=0;p<L;p++){
  const unsigned offset=lane(c,p)*C+c;
  if(visited[offset])return false;
  visited[offset]=true;
  restored[offset]=braid[c*L+p];
 }
 if(restored!=original || mirrored!=original)return false;
 for(bool seen:visited)if(!seen)return false;
 mirrored[11]^=0x80u;
 return mirrored!=original && restored==original;
}
}
int main(){
 if(!gates::positive_and_fault_zlib())return 1;
 if(!gates::rotl_inversion())return 2;
 if(!gates::ninza_ping())return 3;
 std::cout<<"PASS YIN synthetic zlib; YING intentional checksum fault; YAN NINZA mismatch; YANG rotl inverse; PING 54-byte inverse\n";
 return 0;
}
