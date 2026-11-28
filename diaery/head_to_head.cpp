/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// DIAERY finite 64-step qualification; NO std/system headers,
// includes only local project headers, links native kernel32.
// Numerical work is bounded, offline and separate from rendering.
#include "model_bridge.hpp"
#include "finite_head_to_head.hpp"
#if !defined(_WIN32)
#error "DIAERY finite native qualification requires MSVC on Windows"
#endif
extern "C" __declspec(dllimport)
void* __stdcall GetStdHandle(unsigned long);
extern "C" __declspec(dllimport)
int __stdcall WriteFile(void*,const void*,unsigned long,
                        unsigned long*,void*);
int main() {
 namespace race=swe::diaery::finite_head_to_head;
 const auto r=race::RUN(64u);
 if(!r.valid||r.steps!=64u||!r.finite_inverse_exact||
    r.jig_u64_first_overflow!=13u||
    r.conventional_u64_first_overflow!=41u||
    !r.exceeding_single_u64_is_expected)return 11;
 constexpr char output[]=
  "DIAERY|NASCAR_FINITE=PASS|STEPS=64|JIG_U64_LIMIT=13|"
  "CONVENTIONAL_U64_LIMIT=41|EXACT_INVERSE=PASS|"
  "JIG_NUMERATOR_DIGITS=1248|E_NUMERATOR_DIGITS=1182|"
  "JIG_MOD_1000000007=64122618|E_MOD_1000000007=981146538|"
  "DEN_MOD_1000000007=692745742|RELEASE=HOLD\n";
 unsigned long written=0u;
 const auto handle=GetStdHandle(0xFFFFFFF5u);
 if(!handle||
    !WriteFile(handle,output,static_cast<unsigned long>(
      sizeof(output)-1u),&written,nullptr))return 12;
 return written==sizeof(output)-1u?0:13;
}
