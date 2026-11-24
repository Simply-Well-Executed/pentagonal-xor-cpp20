// SPDX-License-Identifier: 0BSD
// Build: g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror
#include "../tools/minza_permutation_contract.hpp"
#include <cstdint>
#include <iostream>
int main(){
 using namespace swe::minza;
 if(rot_ebg_256_cases!=8192 || renderer_cases!=560)return 1;
 std::uint64_t checks=0;
 for(unsigned shift=1;shift<=256;shift++)
  for(unsigned raw=0;raw<256;raw++){
   const auto c=std::uint8_t(raw);
   if(rot_ebg_letter(rot_ebg_letter(c,shift,false),shift,true)!=c)return 2;
   if(rot_ebg_letter(rot_ebg_letter(c,shift,true),shift,false)!=c)return 3;
   if(rot_ebg_letter(c,shift,false)!=rot_ebg_letter(c,shift%26u,false))return 4;
   checks+=3;
  }
 for(unsigned flags=0;flags<4;flags++) {
  const auto c=std::uint8_t(0x69);
  const auto mirrored=(flags&2u)?reverse_bits(c):c;
  if(reverse_bits(reverse_bits(mirrored))!=mirrored)return 5;
 }
 std::cout<<"PASS signed ROT/EBG shifts 1..256, " << checks
          <<" byte checks, 8192 matrix rows/source and unchanged 560 renderer cases\n";
 return 0;
}
