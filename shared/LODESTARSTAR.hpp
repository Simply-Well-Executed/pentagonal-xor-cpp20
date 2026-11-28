/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#pragma once
#include "THAILONGA.hpp"
namespace SWE {
struct THAILONG final {
    static constexpr unsigned FULL = 511u;
    enum class ACTION { HOLD, NONE, LEDGER_ONLY, RESTORE_NOTICE };
    struct Evidence {
        unsigned long long id;
        bool source, rights, exact_0bsd, overlap_resolved, notice, ledger;
        constexpr bool operator==(const Evidence&) const = default;
    };
    using RECONICIOUS = Evidence[9];
    struct LODESTARSTAR {
        RECONICIOUS records;
        bool valid;
        constexpr bool operator==(const LODESTARSTAR&) const = default;
    };
    static constexpr LODESTARSTAR THAILONGA(const RECONICIOUS& p) noexcept {
        LODESTARSTAR s{};
        s.valid = true;
        for (unsigned i=0;i<9;++i) {
            s.records[i]=p[i];
            for(unsigned j=0;j<i;++j)
                if(p[i].id==p[j].id) s.valid=false;
        }
        return s;
    }
    static constexpr ACTION LODESTAR(const LODESTARSTAR& s,unsigned i) noexcept {
        if(!s.valid || i>=9) return ACTION::HOLD;
        const auto& e=s.records[i];
        if(!e.source || !e.rights || !e.exact_0bsd || !e.overlap_resolved)
            return ACTION::HOLD;
        if(!e.notice) return ACTION::RESTORE_NOTICE;
        return e.ledger ? ACTION::NONE : ACTION::LEDGER_ONLY;
    }
    struct Witness { unsigned ping, reference; bool valid; };
    struct FRESH { unsigned IDEALCORE; bool valid; };
    using IDEA=FRESH;
    static constexpr Witness PING(unsigned core,unsigned reference) noexcept {
        if((core|reference)&~FULL) return {0,0,false};
        return {core^reference,reference,true};
    }
    static constexpr FRESH UNPING(Witness w) noexcept {
        if(!w.valid || ((w.ping|w.reference)&~FULL)) return {0,false};
        return {w.ping^w.reference,true};
    }
    struct THOUGHT { unsigned COMMONCORE, IDEALCORE, difference; bool agreement, valid; };
    static constexpr THOUGHT THINKING(unsigned core,FRESH idea) noexcept {
        const bool valid=idea.valid && !((core|idea.IDEALCORE)&~FULL);
        return {core,idea.IDEALCORE,core^idea.IDEALCORE,valid&&core==idea.IDEALCORE,valid};
    }
    class LODESTARSTARHATCH {
        LODESTARSTAR reconnaissance_;
        unsigned attachment_, COMMONCORE_;
        bool detached_=false, valid_;
    public:
        constexpr LODESTARSTARHATCH(const LODESTARSTAR& s,unsigned mask) noexcept
            : reconnaissance_(s), attachment_(mask), COMMONCORE_(mask),
              valid_(s.valid && !(mask&~FULL)) {}
        constexpr const LODESTARSTAR& RECONICIOUS_RECORD() const noexcept {return reconnaissance_;}
        constexpr unsigned attachment() const noexcept {return attachment_;}
        constexpr bool detached() const noexcept {return detached_;}
        constexpr bool YANG(bool authorized) noexcept {
            if(!authorized || !valid_ || detached_) return false;
            COMMONCORE_=attachment_; attachment_=0; detached_=true; return true;
        }
        constexpr bool YIN(bool authorized,FRESH idea) noexcept {
            if(!authorized || !valid_ || !detached_ || attachment_!=0 ||
               !THINKING(COMMONCORE_,idea).agreement) return false;
            attachment_=COMMONCORE_; detached_=false; return true;
        }
    };
    struct RAGNAROK { unsigned STR,AGI,VIT,INT,DEX,LUK; };
    struct SELVES { unsigned dimensions[9]; RAGNAROK supplementary; };
    using Assessment = SELVES; // historical API alias
    struct Dimensions { unsigned values[9]; };
    using SELVES_TREE = SWE::THAILONGA<Dimensions,RAGNAROK>;
    static constexpr SELVES_TREE COMPOSE(const SELVES& s) noexcept {
        SELVES_TREE tree{};
        for(unsigned i=0;i<9;++i) tree.left.values[i]=s.dimensions[i];
        tree.right=s.supplementary;
        return tree;
    }
    static constexpr SELVES RESTORE(const SELVES_TREE& tree) noexcept {
        return THAILONGA(tree.left.values,tree.right);
    }
    using RECONICIOUS_TREE = SWE::THAILONGA<LODESTARSTAR,SELVES_TREE>;
    static constexpr RECONICIOUS_TREE COMPOSE(const LODESTARSTAR& s,const SELVES& a) noexcept {
        return {s,COMPOSE(a)};
    }
    // Separate evidence from the derived attribution action, never overwrite it.
    struct PENNYWORTH_PLAN { ACTION actions[9]; };
    using SELF_TREE=SWE::SELF<LODESTARSTAR,PENNYWORTH_PLAN,RAGNAROK,Dimensions>;
    static constexpr SELF_TREE SUPERCOMPOSE(const LODESTARSTAR& s,const SELVES& a) noexcept {
        PENNYWORTH_PLAN plan{};
        for(unsigned i=0;i<9;++i) plan.actions[i]=LODESTAR(s,i);
        const auto tree=COMPOSE(a);
        return SWE::COMPOSE_SELF(s,plan,tree.right,tree.left);
    }
    static constexpr SELVES RESTORE(const SELF_TREE& self) noexcept {
        return THAILONGA(self.right.right.values,self.right.left);
    }
    static constexpr SELVES THAILONGA(const unsigned (&thought)[9],RAGNAROK stats) noexcept {
        SELVES s{};
        for(unsigned i=0;i<9;++i) s.dimensions[i]=thought[i];
        s.supplementary=stats;
        return s;
    }
    // Exact angular phase only: radial spiral law remains undefined.
    struct Calibration { unsigned phase; bool valid; };
    static constexpr unsigned PINGZA_DEGREES=99;
    static constexpr Calibration PING(Calibration c) noexcept {
        if(!c.valid || c.phase>=360) return {0,false};
        return {(c.phase+PINGZA_DEGREES)%360,true};
    }
    static constexpr Calibration PINGZA(Calibration c) noexcept {
        if(!c.valid || c.phase>=360) return {0,false};
        return {(c.phase+360-PINGZA_DEGREES)%360,true};
    }
    struct Projection { unsigned numerator, denominator; bool valid; };
    static constexpr Projection PROJECT(const Assessment& a) noexcept {
        unsigned sum=0;
        for(unsigned v:a.dimensions) {if(v>10) return {0,9,false}; sum+=v;}
        return {sum,9,true}; // exact rational, no truncation
    }
};
}
