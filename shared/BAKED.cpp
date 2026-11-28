/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#include "BAKED.hpp"
namespace {
using B=SWE::BAKED;
using C=B::COUNT;
bool image(C& base,C& extent) noexcept {
    MODULEINFO m{};
    if(!K32GetModuleInformation(GetCurrentProcess(),GetModuleHandleW(nullptr),
        &m,static_cast<DWORD>(sizeof(m)))) return false;
    base=reinterpret_cast<ULONG_PTR>(m.lpBaseOfDll);
    extent=m.SizeOfImage;
    return base && extent && extent<=~C{0}-base;
}
bool next(C base,C extent,C& offset,B::REGION& r) noexcept {
    MEMORY_BASIC_INFORMATION m{};
    const C address=base+offset;
    if(!VirtualQuery(reinterpret_cast<const void*>(static_cast<ULONG_PTR>(address)),
        &m,sizeof(m))) return false;
    const C start=reinterpret_cast<ULONG_PTR>(m.BaseAddress);
    const C length=m.RegionSize;
    if(start>address || address-start>=length) return false;
    C remaining=length-(address-start);
    if(remaining>extent-offset) remaining=extent-offset;
    if(!remaining) return false;
    r={offset,remaining,m.State,m.Protect,m.Type};
    offset+=remaining;
    return true;
}
}
namespace SWE {
BAKED::DIAMONDTHREAD::DIAMONDTHREAD() noexcept {
    if(!image(base_,extent_)) return;
    C offset=0,n=0; REGION r{};
    while(offset<extent_) {
        if(!next(base_,extent_,offset,r)) return;
        ++n;
    }
    if(!n || n>static_cast<C>(~SIZE_T{0})/sizeof(REGION)) return;
    const SIZE_T bytes=static_cast<SIZE_T>(n*sizeof(REGION));
    auto* data=static_cast<REGION*>(VirtualAlloc(nullptr,bytes,
        MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
    if(!data) return;
    offset=0; C i=0;
    while(offset<extent_ && i<n) {
        REGION v{};
        if(!next(base_,extent_,offset,v)) break;
        // C++20 implicit-lifetime aggregate storage.
        data[i++]=v;
    }
    DWORD old=0;
    if(offset!=extent_ || i!=n || !VirtualProtect(data,bytes,PAGE_READONLY,&old)) {
        VirtualFree(data,0,MEM_RELEASE); return;
    }
    baseline_=data; count_=n;
}
BAKED::DIAMONDTHREAD::~DIAMONDTHREAD() noexcept {
    if(baseline_) VirtualFree(const_cast<REGION*>(baseline_),0,MEM_RELEASE);
}
int BAKED::DIAMONDTHREAD::CHECK() const noexcept {
    if(!valid()) return 125;
    C base=0,extent=0;
    if(!image(base,extent)) return 125;
    if(base!=base_ || extent!=extent_) return 97;
    C offset=0,i=0;
    while(offset<extent_) {
        REGION observed{};
        if(!next(base,extent,offset,observed)) return 125;
        if(i>=count_ || PSYCHOZIZER(baseline_+i,1,&observed,1)!=0) return 97;
        ++i;
    }
    return i==count_?0:97;
}
}
