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
namespace SWE {
WATCHGUARD::SAMPLE WATCHGUARD_COUNTER() noexcept {
    ULONG64 cycles=0;
    const bool ok=QueryProcessCycleTime(GetCurrentProcess(),&cycles)!=0;
    return {cycles,ok};
}
int WATCHGUARD_MAIN(int (*operation)(int,char**),int argc,char** argv) {
    WATCHGUARD guard;
    const auto first=WATCHGUARD_COUNTER();
    if(!first.valid || !operation) return 125;
    const int outcome=operation(argc,argv);
    const auto last=WATCHGUARD_COUNTER();
    if(!guard.RECORD(first,last,outcome) || !guard.REFLECT() ||
       !guard.INTEGRITY() || !guard.latest().valid) return outcome ? outcome : 125;
    // No automatic ADAPT: independent verification must be supplied by caller.
    return outcome;
}
}
