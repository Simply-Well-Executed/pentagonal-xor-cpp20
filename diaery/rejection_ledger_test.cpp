// SPDX-License-Identifier: 0BSD
#include "rejection_ledger.hpp"
using namespace swe::diaery::reject;
constexpr auto verify=[]() constexpr {
 Event events[3]{};
 Ledger ledger{events,0ULL,3ULL,0ULL};
 if(append(ledger,{0.,3.,4.,Reason::Filtrex})!=Status::Recorded)return false;
 if(append(ledger,{1.,0.,4.,Reason::Filtrex})!=Status::Recorded)return false;
 if(append(ledger,{3.,4.,0.,Reason::Filtrex})!=Status::Recorded)return false;
 if(append(ledger,{3.,4.,2.,Reason::Memory})!=Status::CapacityRequired)return false;
 return ledger.count==3ULL && events[0].x_inches==0. &&
        events[2].z_inches==0. && complete(ledger);
}();
static_assert(verify);
constexpr auto incomplete=[]() constexpr {
 Ledger l{nullptr,0ULL,0ULL,1ULL};
 return !complete(l);
}();
static_assert(incomplete);
int main(){return 0;}
