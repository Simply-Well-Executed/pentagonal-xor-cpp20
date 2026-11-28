/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
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
                  {zv,3ULL,3ULL},flags,false,0ULL,false,nullptr,nullptr,nullptr,{nullptr,0ULL,0ULL,0ULL}))return 1;
 if(w.used<200ULL)return 2;
 char small[10]{};
 audit::Writer bad{small,10ULL,0ULL,true};
 if(audit::emit(bad,frame,{xv,3ULL,3ULL},{yv,3ULL,3ULL},
                {zv,3ULL,3ULL},flags,false,0ULL,false,nullptr,nullptr,nullptr,{nullptr,0ULL,0ULL,0ULL}))return 3;
 return 0;
}
