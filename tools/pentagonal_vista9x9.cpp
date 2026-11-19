// SPDX-License-Identifier: 0BSD
// Native C++20 VISTA 9x9 example: no input file mutation, no photo inference.
#include "../shared/pentagonal_vista_9x9.hpp"
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <iostream>

using namespace swe::pentagonal;
using namespace swe::pentagonal::vista9x9;

static bool parse_mask(const char* raw, std::uint32_t& output) {
    if (!raw || !*raw || *raw=='-' || *raw=='+') return false;
    errno=0;
    char* end=nullptr;
    const unsigned long long n=std::strtoull(raw,&end,0);
    if (errno || end==raw || !end || *end!='\0' || n>ligand_mask)
        return false;
    output=static_cast<std::uint32_t>(n);
    return true;
}
static const char* label(Stage s) {
    switch(s) {
        case Stage::xor_gate: return "XOR";
        case Stage::or_gate: return "OR";
        case Stage::and_gate: return "AND";
        case Stage::nop: return "NOP";
        case Stage::cor: return "COR";
        case Stage::flop: return "FLOP";
    }
    return "INVALID";
}
int main(int argc, char** argv) {
    if(argc!=3) {
        std::cerr << "usage: pentagonal_vista9x9 <observed30> <expected30>\n"
                     "Example: pentagonal_vista9x9 0x17 0x1f\n"
                     "Witness is unverified by default; this is a synthetic "
                     "controller demo, not proof of production deployment.\n";
        return 2;
    }
    std::uint32_t observed{},expected{};
    if(!parse_mask(argv[1],observed) || !parse_mask(argv[2],expected)) {
        std::cerr << "Invalid 30-bit input.\n";
        return 2;
    }
    std::array<Input,loop_count> source{};
    for(unsigned n=0;n<loop_count;++n) {
        source[n].observed=observed;
        source[n].expected=expected;
        source[n].source_token=0x900000u+n;
        source[n].witness_present=false;
    }
    const Report r=execute(source);
    std::cout << "SWE-VISTA-9X9-v1|EXPERIMENTAL|no production claim\n";
    for(unsigned n=0;n<loop_count;++n) {
        std::cout << "loop=" << (n+1u);
        for(unsigned g=0;g<gate_count;++g) {
            const Step& s=r.loops[n].gates[g];
            std::cout << "|" << label(s.stage) << "=" << static_cast<unsigned>(s.output);
        }
        for(unsigned p=0;p<vista_pass_count;++p) {
            const VistaPass& v=r.loops[n].views[p];
            std::cout << "|VISTA-" << static_cast<char>('A'+p)
                      << "=" << static_cast<unsigned>(v.structurally_consistent)
                      << ",rupture=" << v.cor.rupture
                      << ",witness_claimed=" << static_cast<unsigned>(v.witness_reported);
        }
        std::cout << "|loops_completed=" << r.loops[n].gates[5].loops_completed << "\n";
    }
    std::cout << "summary|gates=" << r.gate_ticks
              << "|vista=" << r.vista_ticks
              << "|total=" << r.extended_ticks
              << "|gate_reads=" << r.gate_observations
              << "|control_flow=" << static_cast<unsigned>(r.control_flow_consistent)
              << "|authenticated=0\n";
    return r.control_flow_consistent && r.extended_ticks==81u ? 0 : 1;
}
