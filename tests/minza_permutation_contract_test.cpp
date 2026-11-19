// SPDX-License-Identifier: 0BSD
// Build: g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror tests/minza_permutation_contract_test.cpp -o minza_perm_test
#include "../tools/minza_permutation_contract.hpp"
#include <iostream>
int main(){
 std::size_t nonzero=0,qt_wrappers=0;
 for(const auto& c:swe::minza::renderer_matrix)
  if(c.use_native_yuv||c.use_chroma_metrics||c.use_short_ramp||c.use_pent_xor_fx)++nonzero;
 for(const auto& c:swe::minza::container_matrix)
  if(c.wrapper==swe::minza::Wrapper::quicktime_ftyp_mdat_control)++qt_wrappers;
 std::cout<<"renderer="<<swe::minza::renderer_cases<<" container="<<swe::minza::container_cases
          <<" nonzero_flags="<<nonzero<<" quicktime_wrappers="<<qt_wrappers<<'\n';
 return nonzero==525&&qt_wrappers==56?0:1;
}
