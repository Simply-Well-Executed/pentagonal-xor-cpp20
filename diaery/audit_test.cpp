// SPDX-License-Identifier: 0BSD
#include "audit.hpp"
using namespace swe::diaery;
int main(){
 char output[8192]{};
 bool flags[10]{true,true,false,false,false,false,false,false,false,false};
 double xv[3]{1.,2.,3.},yv[3]{3.,4.,5.},zv[3]{2.,3.,4.};
 const auto frame=thailonga::build({2.,4.,3.,true,true,true},0ULL);
 audit::Writer w{output,8192ULL,0ULL,true};
 if(!audit::emit(w,frame,{xv,3ULL,3ULL},{yv,3ULL,3ULL},
                  {zv,3ULL,3ULL},flags,false,0ULL,false))return 1;
 if(w.used<200ULL)return 2;
 char small[10]{};
 audit::Writer bad{small,10ULL,0ULL,true};
 if(audit::emit(bad,frame,{xv,3ULL,3ULL},{yv,3ULL,3ULL},
                {zv,3ULL,3ULL},flags,false,0ULL,false))return 3;
 return 0;
}
