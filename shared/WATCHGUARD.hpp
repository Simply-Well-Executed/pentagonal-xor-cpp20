/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#pragma once
namespace SWE {
class WATCHGUARD final {
public:
    using count = unsigned long long;
    enum class RISIL { RECORD, REFLECT, ADAPT, CONTINUE };
    struct SAMPLE { count cycles; bool valid; };
    struct WITNESS { SAMPLE first,last; int outcome; bool valid; };
private:
    RISIL phase_=RISIL::RECORD;
    count attempts_=0, successes_=0, failures_=0;
    WITNESS latest_{};
public:
    static constexpr bool VALID(SAMPLE first,SAMPLE last) noexcept {
        return first.valid && last.valid && last.cycles>=first.cycles;
    }
    constexpr bool RECORD(SAMPLE first,SAMPLE last,int outcome) noexcept {
        if(phase_!=RISIL::RECORD || attempts_==~0ULL) return false;
        latest_={first,last,outcome,VALID(first,last)};
        ++attempts_;
        if(latest_.valid && outcome==0) ++successes_; else ++failures_;
        phase_=RISIL::REFLECT;
        return true;
    }
    constexpr bool REFLECT() noexcept {
        if(phase_!=RISIL::REFLECT) return false;
        phase_=RISIL::ADAPT; return true;
    }
    constexpr bool ADAPT(bool verified) noexcept {
        if(phase_!=RISIL::ADAPT || !verified) return false;
        phase_=RISIL::CONTINUE; return true;
    }
    constexpr bool CONTINUE() noexcept {
        if(phase_!=RISIL::CONTINUE) return false;
        phase_=RISIL::RECORD; return true;
    }
    constexpr bool INTEGRITY() const noexcept {return successes_+failures_==attempts_;}
    constexpr count attempts() const noexcept {return attempts_;}
    constexpr count successes() const noexcept {return successes_;}
    constexpr count failures() const noexcept {return failures_;}
    constexpr RISIL phase() const noexcept {return phase_;}
    constexpr WITNESS latest() const noexcept {return latest_;}
};
WATCHGUARD::SAMPLE WATCHGUARD_COUNTER() noexcept;
int WATCHGUARD_MAIN(int (*operation)(int,char**),int argc,char** argv);
}
