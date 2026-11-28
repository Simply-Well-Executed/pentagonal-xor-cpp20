/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include "SWEATER.hpp"
namespace {
using S=SWE::SWEATER;
bool emit(const char* p,DWORD n) noexcept {
    DWORD written=0;
    HANDLE out=GetStdHandle(STD_ERROR_HANDLE);
    return out!=nullptr && out!=INVALID_HANDLE_VALUE &&
        WriteFile(out,p,n,&written,nullptr)!=0 && written==n;
}
template<unsigned N> bool literal(const char (&p)[N]) noexcept {
    return emit(p,N-1);
}
bool number(S::COUNT n) noexcept {
    char digits[20]; unsigned i=20;
    do { digits[--i]=static_cast<char>('0'+n%10); n/=10; } while(n);
    return emit(digits+i,20-i);
}
bool metric(S::METRIC m) noexcept {
    return m.valid?number(m.value):literal("NA");
}
bool change(S::CHANGE c) noexcept {
    if(!c.valid) return literal("NA");
    if(!literal(c.grew?"+":"")) return false;
    if(!c.grew && c.magnitude && !literal("-")) return false;
    return number(c.magnitude);
}
bool pair(S::METRIC a,S::METRIC b) noexcept {
    return metric(a)&&literal(",")&&metric(b)&&literal(",")&&change(S::GROWTH(a,b));
}
}
namespace SWE {
SWEATER::SAMPLE SWEATER::MEASURE() noexcept {
    SAMPLE s{};
    PROCESS_MEMORY_COUNTERS_EX m{};
    m.cb=sizeof(m);
    if(K32GetProcessMemoryInfo(GetCurrentProcess(),
        reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&m),sizeof(m))) {
        s.private_commit={m.PrivateUsage,true};
        s.working_set={m.WorkingSetSize,true};
        s.peak_working_set={m.PeakWorkingSetSize,true};
    }
    // Close the enumeration handle before measuring handles.
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
    if(snapshot!=INVALID_HANDLE_VALUE) {
        THREADENTRY32 entry{}; entry.dwSize=sizeof(entry);
        COUNT n=0;
        if(Thread32First(snapshot,&entry)) {
            do {
                if(entry.th32OwnerProcessID==GetCurrentProcessId()) ++n;
                entry.dwSize=sizeof(entry);
            } while(Thread32Next(snapshot,&entry));
            if(GetLastError()==ERROR_NO_MORE_FILES) s.threads={n,true};
        }
        CloseHandle(snapshot);
    }
    DWORD handles=0;
    if(GetProcessHandleCount(GetCurrentProcess(),&handles)) s.handles={handles,true};
    return s;
}
bool SWEATER::REPORT(const WITNESS& w) noexcept {
    const long long code=w.outcome;
    return literal("SWEATER/1 outcome=") &&
        (code>=0 || literal("-")) &&
        number(static_cast<COUNT>(code<0?-code:code)) &&
        literal(" ran=") && number(w.operation_ran?1:0) &&
        literal(" counter_valid=") && number(w.counter_valid?1:0) &&
        literal(" private_commit=") && pair(w.first.private_commit,w.last.private_commit) &&
        literal(" working_set=") && pair(w.first.working_set,w.last.working_set) &&
        literal(" peak_working_set=") && pair(w.first.peak_working_set,w.last.peak_working_set) &&
        literal(" handles=") && pair(w.first.handles,w.last.handles) &&
        literal(" threads=") && pair(w.first.threads,w.last.threads) && literal("\n");
}
}
