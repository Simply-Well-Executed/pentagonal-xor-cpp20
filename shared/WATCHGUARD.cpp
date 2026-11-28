/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "WATCHGUARD.hpp"
#include "SWEATER.hpp"
#include "BAKED.hpp"
namespace SWE {
WATCHGUARD::SAMPLE WATCHGUARD_COUNTER() noexcept {
    ULONG64 cycles=0;
    const bool ok=QueryProcessCycleTime(GetCurrentProcess(),&cycles)!=0;
    return {cycles,ok};
}
int WATCHGUARD_MAIN(int (*operation)(int,char**),int argc,char** argv) {
    WATCHGUARD guard;
    const BAKED::DIAMONDTHREAD diamond;
    const int initial=diamond.CHECK();
    const auto before=SWEATER::MEASURE();
    const auto first=WATCHGUARD_COUNTER();
    if(initial!=0 || !first.valid || !operation) {
        const auto after=SWEATER::MEASURE();
        const int rejected=initial!=0?initial:125;
        (void)SWEATER::REPORT({before,after,rejected,false,false});
        return rejected;
    }
    const int outcome=operation(argc,argv);
    const auto last=WATCHGUARD_COUNTER();
    const int baked=diamond.CHECK();
    const auto after=SWEATER::MEASURE();
    const bool valid=guard.RECORD(first,last,outcome) && guard.REFLECT() &&
        guard.INTEGRITY() && guard.latest().valid;
    const bool reported=SWEATER::REPORT({before,after,outcome,true,valid});
    // No automatic ADAPT: independent verification must be supplied by caller.
    // Separate marker: never relabel the original SWEATER outcome.
    if(baked!=0) {
        const char* message=baked==97?"BAKED/1 status=97 changed\n":"BAKED/1 status=125 unavailable\n";
        DWORD written=0;
        const DWORD length=baked==97?26u:31u;
        (void)WriteFile(GetStdHandle(STD_ERROR_HANDLE),message,length,&written,nullptr);
        return baked;
    }
    return valid && reported ? outcome : (outcome ? outcome : 125);
}
}
