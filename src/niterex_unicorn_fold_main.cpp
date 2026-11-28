/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Scope: independently authored first-party source only; upstream reference rights unchanged.
*/
// SPDX-License-Identifier: 0BSD
// NiteRex: native stdout application. It GENERATES XHTML; it is not a browser.
// No project-authored shell, JavaScript, C++ standard-library headers or network I/O.
#include "../include/niterex_unicorn_fold.hpp"

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN 1
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX 1
#  endif
#  include <windows.h>
#elif defined(__linux__) && defined(__x86_64__)
// Raw Linux AMD64 system calls instead of project-authored POSIX functions.
// Process startup is linked by the selected compiler and remains toolchain-HOLD.
#else
#  error "NiteRex native CLI currently supports Win32 and Linux AMD64 only"
#endif

namespace {
using swe::niterex::count_t;
bool write_stream(void* context, const char* data, count_t size) noexcept {
    const bool error = (context != nullptr);
#if defined(_WIN32)
    HANDLE h = GetStdHandle(error ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    if (h == nullptr || h == INVALID_HANDLE_VALUE) return false;
    while(size) {
        DWORD n = 0;
        const DWORD chunk = size > 0x7fffffffu ? 0x7fffffffu : static_cast<DWORD>(size);
        if (!WriteFile(h,data,chunk,&n,nullptr) || n==0) return false;
        data+=n; size-=n;
    }
    return true;
#else
    const long fd=error ? 2L : 1L;
    while(size) {
        long n;
        __asm__ __volatile__("syscall" : "=a"(n)
              : "a"(1L), "D"(fd), "S"(data), "d"(size)
              : "rcx", "r11", "memory");
        if(n<=0) return false;
        data+=n; size-=static_cast<count_t>(n);
    }
    return true;
#endif
}
bool say(const char* s,bool error=false) noexcept {
    return swe::niterex::emit({error ? reinterpret_cast<void*>(1) : nullptr,write_stream},s);
}
bool number(unsigned x) noexcept {
    return swe::niterex::emit_number({nullptr,write_stream},x);
}
} // namespace

int main(int argc,char** argv) {
    using namespace swe::niterex;
    if(argc==2 && equal(argv[1],"--self-test")) {
        if(!verify_quests()) { (void)say("NITEREX|HOLD|nine_quests_mismatch\n",true);return 2; }
        return say("NITEREX|PASS|NINE_QUESTS|MASK=511|CHECKSUM=829|XHTML_1_0_STRICT\n")?0:3;
    }
    if(argc==2 && equal(argv[1],"--xhtml")) {
        if(!render_xhtml_strict({nullptr,write_stream})) {
            (void)say("NITEREX|HOLD|xhtml_output_failed\n",true);return 3;
        }
        return 0;
    }
    if((argc==4 || argc==5) && equal(argv[1],"--core")) {
        const Op op=parse_operation(argv[2]);
        unsigned a=0,b=0;
        if(op==Op::Invalid || !parse_unsigned(argv[3],a) ||
           (argc==5 && !parse_unsigned(argv[4],b))) {
            (void)say("NITEREX|HOLD|invalid_9_bit_operator_or_operand\n",true);
            return 2;
        }
        if(!number(apply(op,a,b)) || !say("\n")) return 3;
        return 0;
    }
    (void)say("Usage: niterex_unicorn_fold --self-test | --xhtml | --core OP A [B]\n",true);
    return 2;
}
