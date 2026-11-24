// SPDX-License-Identifier: 0BSD
// User media remains unchanged. C++20 and -O2 optimized only.
// MINZA byte-reversed wrapper experiment, not a claim of decryption.
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

using Byte = std::uint8_t;
namespace fs = std::filesystem;
static Byte rot16_alpha(Byte c) {
    if (c >= 'A' && c <= 'Z') return Byte('A' + (c-'A'+16)%26);
    if (c >= 'a' && c <= 'z') return Byte('a' + (c-'a'+16)%26);
    return c;
}
static Byte reverse_bits(Byte v) {
    v = Byte(((v & 0xF0u) >> 4u) | ((v & 0x0Fu) << 4u));
    v = Byte(((v & 0xCCu) >> 2u) | ((v & 0x33u) << 2u));
    return Byte(((v & 0xAAu) >> 1u) | ((v & 0x55u) << 1u));
}
static std::vector<Byte> load(const fs::path& path) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) throw std::runtime_error("Cannot open input: "+path.string());
    auto end = in.tellg();
    if (end < 0 || static_cast<std::uint64_t>(end) > 512u*1024u*1024u)
        throw std::runtime_error("Input size invalid or >512 MiB");
    std::vector<Byte> bytes(static_cast<std::size_t>(end));
    in.seekg(0);
    if (!bytes.empty()) in.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!in && !bytes.empty()) throw std::runtime_error("Short read");
    return bytes;
}
static void save_wrapped(const fs::path& out, const std::vector<Byte>& payload, bool rot_payload, bool rot_whole, bool bit_mirror) {
    std::ofstream f(out, std::ios::binary | std::ios::trunc);
    if (!f) throw std::runtime_error("Unable to create: "+out.string());
    const std::array<Byte,3> prefix{'X','F','E'}, suffix{'E','F','X'};
    auto emit = [&](Byte b, bool onPayload) {
        if (bit_mirror && onPayload) b = reverse_bits(b);
        if ((rot_payload && onPayload) || rot_whole) b = rot16_alpha(b);
        f.put(static_cast<char>(b));
    };
    for (auto b: prefix) emit(b,false);
    for (auto b: payload) emit(b,true);
    for (auto b: suffix) emit(b,false);
    if (!f) throw std::runtime_error("Output write failure: "+out.string());
}
static void show8(const std::vector<Byte>& b, bool last) {
    constexpr char H[]="0123456789abcdef";
    if (b.empty()) return;
    std::size_t start=last?(b.size()>8?b.size()-8:0):0;
    for(std::size_t i=start;i<b.size() && i<start+8;i++) std::cout<<H[b[i]>>4]<<H[b[i]&15];
}
int main(int argc,char** argv) {
    if (argc!=4) { std::cerr << "usage: minza_reverse source.mp4 dest_dir prefix\n"; return 2; }
    try {
        fs::path input=argv[1],outdir=argv[2]; std::string prefix=argv[3];
        if(prefix.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-")!=std::string::npos)
            throw std::runtime_error("Unsafe output prefix");
        if(!fs::exists(outdir) || !fs::is_directory(outdir)) throw std::runtime_error("Output directory missing");
        const auto original=load(input);auto reversed=original;
        std::reverse(reversed.begin(),reversed.end());
        std::cout<<"SOURCE="<<input<<" bytes="<<original.size()<<" reversed-bytes="<<reversed.size()<<"\n";
        std::cout<<"original_first8=";show8(original,false);std::cout<<" original_last8=";show8(original,true);std::cout<<"\n";
        std::cout<<"reverse_first8=";show8(reversed,false);std::cout<<" reverse_last8=";show8(reversed,true);std::cout<<"\n";
        auto restored=reversed; std::reverse(restored.begin(),restored.end());
        if(restored!=original) throw std::runtime_error("Involution check failed");
        std::cout<<"full_byte_reversal_roundtrip=PASS\n";
        struct Mode {const char* filename;bool rp,rw,bm;};
        const Mode tests[]={
            {"reverse_XFE_EFX.mov",false,false,false},
            {"reverse_XFE_EFX_ROT16_payload.mov",true,false,false},
            {"reverse_XFE_EFX_ROT16_whole.mov",false,true,false},
            {"reverse_bits_XFE_EFX.mov",false,false,true},
            {"reverse_bits_XFE_EFX_ROT16_payload.mov",true,false,true}
        };
        for(const auto& mode:tests){
            fs::path dest=outdir/(prefix+"_"+mode.filename);
            save_wrapped(dest,reversed,mode.rp,mode.rw,mode.bm);
            if (fs::file_size(dest)!=original.size()+6) throw std::runtime_error("Unexpected output size");
            std::cout<<"OUTPUT="<<dest<<" bytes="<<fs::file_size(dest)<<" ROT16_alphabetical="<<(mode.rp?"payload":mode.rw?"whole":"off")<<" bit_reverse="<<(mode.bm?"on":"off")<<"\n";
        }
        return 0;
    } catch(const std::exception& e){ std::cerr<<"ERROR="<<e.what()<<"\n";return 3; }
}
