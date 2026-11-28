// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
#include "../shared/minza_centrix_light_cxx20.hpp"
#include <cmath>
#include <iostream>
using namespace swe::minza::centrix;
constexpr bool attenuation_parity() {
 for(unsigned selected=0;selected<tile_count;++selected)
  for(unsigned target=0;target<tile_count;++target) {
   double weight=1.0;
   for(unsigned d=0;d<distance(selected,target);++d)weight*=neighborhood_decay;
   if(weight!=decay_weights[distance(selected,target)])return false;
   constexpr double exposures[]={0.0,0.25,1.0,2.5,3.0};
   for(double exposure:exposures) {
    const double reference=clamp(clamp(exposure,minimum_exposure,maximum_exposure)*(0.40+0.60*weight),0.07,2.70);
    if(radiance(selected,target,exposure)!=reference)return false;
   }
  }
 return true;
}
static_assert(attenuation_parity());
int main(){
 if(!attenuation_parity())return 7;
 for(unsigned selected=0;selected<tile_count;++selected){
  if(mirror(mirror(selected))!=selected)return 1;
  for(unsigned target=0;target<tile_count;++target){
    for(double exposure:{0.25,1.0,2.5}){
      const double front=radiance(selected,target,exposure);
      const double rear=radiance(mirror(selected),mirror(target),exposure);
      if(std::abs(front-rear)>1e-12)return 2;
      if(front<0.07 || front>2.7)return 3;
    }
    if(radiance(selected,target,0.25)>radiance(selected,target,1.0))return 4;
    if(radiance(selected,target,1.0)>radiance(selected,target,2.5))return 5;
  }
 }
 if(radiance(9,4,1)!=0 || radiance(1,9,1)!=0 || mirror(9)!=9)return 6;
 std::cout << "PASS: all 81 front/rear mappings, 3 R10 exposures, single-light symmetry and bounds\n";
 return 0;
}
