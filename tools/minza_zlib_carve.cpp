// SPDX-License-Identifier: 0BSD
// C++20 -O2 read-only RFC1950/DEFLATE candidate scanner.
// Investigational: a valid header or embedded word is not itself a video track.
#include <zlib.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
namespace fs=std::filesystem;
using Byte=std::uint8_t;
struct Scan {
 std::uint64_t bytes=0, first78=0, valid78=0, validRfc=0, validDict=0;
 std::uint64_t attempts=0, streams=0, truncated=0, bad=0, suspectMedia=0;
 std::uint64_t cmfCases[8]{};
};
static auto read_file(const fs::path& p) {
 std::ifstream file(p, std::ios::binary|std::ios::ate);
 if(!file)throw std::runtime_error("open failed");
 const auto size=file.tellg();
 if(size<0 || std::uint64_t(size)>256ull*1024ull*1024ull)throw std::runtime_error("input size invalid");
 std::vector<Byte> x(static_cast<std::size_t>(size));
 file.seekg(0);
 if(!x.empty())file.read(reinterpret_cast<char*>(x.data()),std::streamsize(x.size()));
 if(!file && !x.empty())throw std::runtime_error("read failed");
 return x;
}
static bool header(Byte cmf,Byte flg){
 return ((cmf&15u)==8u) && (cmf>>4u)<=7u &&
        (((unsigned(cmf)<<8u)|flg)%31u==0u);
}
struct InflateInfo {
 bool success=false,limit=false;
 std::uint64_t input=0,output=0;
 std::array<Byte,32> prefix{};
};
static InflateInfo bounded_inflate(const Byte* data,std::size_t len){
 InflateInfo info{};
 z_stream z{};
 if(inflateInit(&z)!=Z_OK)return info;
 constexpr std::size_t IN_MAX=4u*1024u*1024u;
 constexpr std::size_t OUT_MAX=16u*1024u*1024u;
 std::array<Byte,65536> out{};
 std::size_t fed=0;
 for(int turn=0;turn<3000;++turn){
  if(z.avail_in==0 && fed<len && fed<IN_MAX){
   const std::size_t n=std::min({std::size_t(65536),len-fed,IN_MAX-fed});
   z.next_in=const_cast<Byte*>(data+fed);
   z.avail_in=static_cast<uInt>(n);
   fed+=n;
  }
  if(info.output>=OUT_MAX){info.limit=true;break;}
  const auto remaining=OUT_MAX-info.output;
  z.next_out=out.data();
  z.avail_out=static_cast<uInt>(std::min(remaining,out.size()));
  auto before=z.total_out;
  const int status=inflate(&z,Z_NO_FLUSH);
  const std::size_t produced=std::size_t(z.total_out-before);
  const auto start=info.output;
  for(std::size_t i=0;i<produced;i++)
   if(start+i<info.prefix.size())info.prefix[start+i]=out[i];
  info.output+=produced;
  if(status==Z_STREAM_END){info.success=true;break;}
  if(status!=Z_OK && status!=Z_BUF_ERROR)break;
  if(status==Z_BUF_ERROR && z.avail_in==0 && (fed>=len || fed>=IN_MAX)){
   if(fed>=IN_MAX)info.limit=true;
   break;
  }
  if(z.avail_in==0 && (fed>=len || fed>=IN_MAX)){
   if(fed>=IN_MAX)info.limit=true;
   break;
  }
  if(!produced && z.avail_in==0 && fed>=len)break;
 }
 info.input=z.total_in;
 inflateEnd(&z);
 return info;
}
static bool nestedMedia(const InflateInfo& r){
 const auto& p=r.prefix;
 if(r.output>=4 && p[0]=='F'&&p[1]=='L'&&p[2]=='V')return true;
 if(r.output>=12 && p[0]=='R'&&p[1]=='I'&&p[2]=='F'&&p[3]=='F')return true;
 if(r.output>=8 && p[4]=='f'&&p[5]=='t'&&p[6]=='y'&&p[7]=='p')return true;
 return false;
}
static void scan(const char* name,const fs::path& p){
 auto b=read_file(p);Scan s{};s.bytes=b.size();
 std::cout<<"SOURCE="<<name<<" SIZE="<<b.size()<<"\n";
 for(std::size_t i=0;i+1<b.size();i++){
  if(b[i]==0x78u)++s.first78;
  const Byte cmf=b[i],flg=b[i+1];
  if(!header(cmf,flg))continue;
  ++s.validRfc;++s.cmfCases[cmf>>4u];
  if(cmf==0x78u)++s.valid78;
  if(flg&0x20u){++s.validDict;continue;}
  ++s.attempts;
  auto result=bounded_inflate(b.data()+i,b.size()-i);
  if(result.success){
   ++s.streams;
   if(nestedMedia(result))++s.suspectMedia;
   std::cout<<"FOUND_ZLIB offset="<<i<<" consumed="<<result.input<<" uncompressed="
            <<result.output<<" decoded_media_magic="<<nestedMedia(result)<<"\n";
  }else if(result.limit)++s.truncated;
  else ++s.bad;
 }
 std::cout<<"SUMMARY SOURCE="<<name<<" first78="<<s.first78
          <<" candidate78_valid_header="<<s.valid78
          <<" candidate_all_CINFO_valid_header="<<s.validRfc
          <<" preset_dictionary="<<s.validDict
          <<" inflation_attempts="<<s.attempts
          <<" valid_checksum_complete_zlib_streams="<<s.streams
          <<" decoded_media_magic="<<s.suspectMedia
          <<" hit_resource_limit="<<s.truncated
          <<" rejected="<<s.bad<<"\n";
 for(unsigned i=0;i<8;i++)std::cout<<"CINFO"<<i<<"="<<s.cmfCases[i]<<" ";
 std::cout<<"\n";
}
static bool control(){
 const std::string source="FLV synthetic zlib positive control. Not an actual media stream.";
 uLong len=compressBound(source.size());std::vector<Byte> compressed(len);
 if(compress2(compressed.data(),&len,reinterpret_cast<const Byte*>(source.data()),
              source.size(),Z_BEST_COMPRESSION)!=Z_OK)return false;
 compressed.resize(len);
 auto decoded=bounded_inflate(compressed.data(),compressed.size());
 if(!decoded.success || decoded.output!=source.size() || !nestedMedia(decoded))return false;
 compressed.back()^=1u;
 if(bounded_inflate(compressed.data(),compressed.size()).success)return false;
 std::vector<Byte> pseudo{0x78u,0x01u};
 pseudo.insert(pseudo.end(),source.begin(),source.end());
 const auto ad=adler32(1,reinterpret_cast<const Byte*>(source.data()),source.size());
 for(int shift=24;shift>=0;shift-=8)pseudo.push_back(Byte((ad>>shift)&255u));
 if(bounded_inflate(pseudo.data(),pseudo.size()).success)return false;
 std::cout<<"CONTROL standard_zlib=PASS adler_tamper_rejected=PASS scalar_custom_envelope_not_DEFLATE=PASS\n";
 return true;
}
int main(int argc,char**argv){
 try{
  if(!control())return 2;
  if(argc!=3){std::cerr<<"Usage: minza_zlib_carve inputA.mp4 inputB.mp4\n";return 2;}
  scan("VIDEO_A",argv[1]);scan("OBS_B",argv[2]);return 0;
 }catch(const std::exception&e){std::cerr<<"ERROR "<<e.what()<<"\n";return 3;}
}
