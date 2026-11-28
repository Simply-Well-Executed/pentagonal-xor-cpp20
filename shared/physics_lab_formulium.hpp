// SPDX-License-Identifier: 0BSD
#pragma once
// PHYSICS-LAB/FORMULIUM-1: exact discrete experimental models, NOT legal decisions.
namespace SWE::PHYSICS_LAB::FORMULIUM {
using u64=unsigned long long;
struct LADDERDI final {
    static_assert(~0u==0xffffffffu);
    struct Fraction { unsigned n,d; };
    struct Standard { u64 left,right,denominator; bool equal,valid; };
    static constexpr Standard CROSS(Fraction l,Fraction r) noexcept {
        if(!l.d || !r.d) return {0,0,0,false,false};
        const u64 a=u64(l.n)*r.d,b=u64(r.n)*l.d;
        return {a,b,u64(l.d)*r.d,a==b,true};
    }
};
struct INTERROGATION final {
    static constexpr unsigned ROLL[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};
    struct Position { LADDERDI::Fraction value; unsigned id; bool valid; };
    static constexpr Position AT(unsigned c,unsigned i) noexcept {
        if(c>=6 || i>=3) return {{0,0},3,false};
        const unsigned id=ROLL[c][i];
        return {{unsigned(id==2),1},id,true};
    }
};
struct QUESTION final { u64 n,N,q; };
struct QUERY final { u64 correct,incorrect; };
struct INQUISITION final { u64 id; QUESTION requirement; };
struct JURY final {
    static constexpr bool VERIFY(QUESTION l,QUERY r) noexcept {
        return l.N!=0 && l.n<l.N && l.q!=0 && r.correct>=l.q && r.incorrect==0;
    }
};
struct TRIAL final {
    static constexpr u64 ANSWER(QUESTION l,QUERY r) noexcept { return l.n+JURY::VERIFY(l,r); }
};
using JUDGE=TRIAL;
struct DISCOVERY final {
    using FRACTION=LADDERDI::Fraction;
    static constexpr INQUISITION ARROW(u64 id,u64 n,u64 N,u64 q) noexcept { return {id,{n,N,q}}; }
    static constexpr u64 ASCEND(INQUISITION q,QUERY evidence) noexcept { return TRIAL::ANSWER(q.requirement,evidence); }
};
struct SILVERHAND_JUSTICE final {
    static constexpr unsigned EQUALITY=5;
    struct Witness { u64 l,r,c,e,shell,standard,changed; };
    static constexpr Witness COREAO(u64 l,u64 r,u64 c,u64 e) noexcept {
        const u64 x=l^(r^c); return {l,r,c,e,x,e|x,e&~x};
    }
};
using NONADMISSABLE_EVIDENCE=SILVERHAND_JUSTICE;
using HUNG_JURY=SILVERHAND_JUSTICE;
using TEFLON_DON=SILVERHAND_JUSTICE;
using ABATEMENT=SILVERHAND_JUSTICE;
using DOUBLE_JEPRODY=SILVERHAND_JUSTICE;
using CONVICTION=SILVERHAND_JUSTICE;
using INNOCENT_UNTIL_PROVEN_GUILTY=SILVERHAND_JUSTICE;
using FAIR_TRIAL=SILVERHAND_JUSTICE;
using RIGGED_TRIAL=SILVERHAND_JUSTICE;
struct EVIDENCE_ADMISSION final {
    struct Vector { double x,y; };
    static constexpr Vector ORDER(Vector v) noexcept { return {-v.y,v.x}; }
};
struct AREA_DEFENSE_CONSULTATION final {
    struct PUBLICOPINION { u64 support,oppose,abstain; };
    struct U { QUESTION left; QUERY right; };
    struct Posture { U source; u64 position; bool verified; SILVERHAND_JUSTICE::Witness witness; PUBLICOPINION airgap; };
    static constexpr Posture ASSEMBLE(U u,PUBLICOPINION p,u64 e) noexcept {
        const bool v=JURY::VERIFY(u.left,u.right);
        return {u,u.left.n+v,v,SILVERHAND_JUSTICE::COREAO(u.left.n,u.right.correct,u64(v),e),p};
    }
};
struct SECOND_WIND final {
    bool silence,coercion,voluntary;
    constexpr bool ADMIT() const noexcept { return !silence && !coercion && voluntary; }
};
struct LAST_RESORT final {
    static constexpr bool REVIEW(bool fear,bool proof) noexcept { return fear && !proof; }
};
using TAP_OUT=LAST_RESORT;
struct LEGALLY_STILL_ALIVE final {
    struct Truth { bool admissible,review,reject_inference; };
    static constexpr Truth VERIFY(SECOND_WIND s,bool fear,bool proof) noexcept {
        const bool r=LAST_RESORT::REVIEW(fear,proof); return {s.ADMIT(),r,r};
    }
};
struct LUCKY final {
    bool lawful,voluntary,informed;
    constexpr bool VALUE() const noexcept { return lawful && voluntary && informed; }
};
struct JUSTICE final {
    bool bias,misconduct;
    constexpr bool VALUE() const noexcept { return !bias && !misconduct; }
};
struct HONOR final {
    bool concern,evaluated;
    constexpr bool VALUE() const noexcept { return !concern || evaluated; }
};
struct LEGALY_IAM_DEAD final {
    static constexpr bool VERIFY(LUCKY l,JUSTICE j,HONOR h) noexcept { return l.VALUE() && j.VALUE() && h.VALUE(); }
};
using LEGALLY_IAM_ALIVE=LEGALY_IAM_DEAD;
struct LEGAL_SYSTEM final {
    struct Evidence {
        bool silence,coercion,voluntary,fear,proof,lawful,informed,bias,misconduct,concern,evaluated,confirmed;
    };
    struct Posture { Evidence source; bool truth,justice,honor,lucky,reject,stands; };
    static constexpr Posture ASSEMBLE(Evidence e) noexcept {
        const auto t=LEGALLY_STILL_ALIVE::VERIFY({e.silence,e.coercion,e.voluntary},e.fear,e.proof);
        const bool j=JUSTICE{e.bias,e.misconduct}.VALUE();
        const bool h=!e.concern || (e.evaluated && e.confirmed);
        const bool l=LUCKY{e.lawful,e.voluntary,e.informed}.VALUE();
        return {e,t.admissible,j,h,l,t.reject_inference,t.admissible&&j&&h&&l&&!t.reject_inference};
    }
    struct PERSIAN_JIG final { enum class DEKATUNG:unsigned { TRUTH,JUSTICE,HONOR,LUCKY }; };
    using FYRMID=PERSIAN_JIG;
};
struct COT final {
    bool truth,justice,honor,lucky,unsupported_inference;
    constexpr unsigned REVEAL() const noexcept {
        return unsigned(truth)|(unsigned(justice)<<1)|(unsigned(honor)<<2)|(unsigned(lucky)<<3)|(unsigned(unsupported_inference)<<4);
    }
};
using CASH=COT;
struct CASH_COLOR final {
    static constexpr unsigned LINES=13;
    enum class Status:unsigned { UNDETERMINED,SUPPORTED,CONTRADICTED,NEEDS_REVIEW };
    Status line[LINES]{};
};
struct JURY_NULLIFICATION final {
    static constexpr unsigned ROTATE(unsigned i) noexcept { return i<13?(i<4?i+9:i-4):13; }
    static constexpr unsigned RESTORE(unsigned i) noexcept { return i<13?(i<9?i+4:i-9):13; }
};
struct JURY_SAMPLING final {
    static constexpr unsigned SAMPLE(unsigned n) noexcept { return ((n%13)*9)%13; }
};
template<class EBC9> struct MOCK_TRIAL final {
    static constexpr unsigned SAMPLE(unsigned i) noexcept { return i<13?EBC9::TRANSFORM(i):13; }
};
template<class EBC9> struct MONGOLIAN_THE final {
    static constexpr bool BINDING=false;
    struct Development { unsigned source,output; bool valid,changed; };
    static constexpr Development DISCOVER(unsigned i) noexcept {
        const unsigned o=MOCK_TRIAL<EBC9>::SAMPLE(i); const bool v=i<13&&o<13;
        return {i,o,v,v&&i!=o};
    }
};
struct FWA_TRIANGLE final {
    enum class Node:unsigned { FRAUD,WASTE,ABUSE };
    struct Edge { Node left,right; };
    static constexpr Edge edges[3]={{Node::FRAUD,Node::WASTE},{Node::WASTE,Node::ABUSE},{Node::ABUSE,Node::FRAUD}};
};
struct GSADVANTAGE final {
    enum class Status:unsigned { UNDETERMINED,ALLEGED,CORROBORATED,CONTRADICTED,ADJUDICATED };
    enum class Burden:unsigned { INVESTIGATIVE,PREPONDERANCE,CLEAR_AND_CONVINCING,BEYOND_REASONABLE_DOUBT };
    enum class Jurisdiction:unsigned { UNDETERMINED,STATE,FEDERAL,CONCURRENT };
    struct Arrangement {
        Status fraud,waste,abuse;
        Burden required; Jurisdiction authority;
        bool source,authenticated,material,contradictions_reviewed;
        constexpr bool REVIEW_READY() const noexcept { return source&&authenticated&&material&&contradictions_reviewed&&authority!=Jurisdiction::UNDETERMINED; }
    };
};
struct LAW final {
    struct CAPITAL { static constexpr bool LIMITLESS=true; };
    struct TIME { static constexpr bool LIMITLESS=true; };
    struct TRUTH { bool factual; };
    struct EVIDENCE { bool present,provenance,admissibility,burden; };
    struct AUTHORITY { bool jurisdiction,timeliness,review; };
    struct REMEDY { bool available; };
    CAPITAL c; TIME t; TRUTH h; EVIDENCE e; AUTHORITY a; REMEDY r;
    constexpr unsigned REVEAL() const noexcept {
        return unsigned(!h.factual)|(unsigned(!e.present)<<1)|(unsigned(!e.provenance)<<2)|
        (unsigned(!e.admissibility)<<3)|(unsigned(!e.burden)<<4)|(unsigned(!a.jurisdiction)<<5)|
        (unsigned(!a.timeliness)<<6)|(unsigned(!a.review)<<7)|(unsigned(!r.available)<<8);
    }
};
using GREEK_CROSS=LAW;
struct RUN_THE_CLOCK final {
    using TOWER=LAW;
    static constexpr u64 REMAINING(u64 resources,u64 expenses) noexcept { return expenses>=resources?0:resources-expenses; }
};
template<unsigned N> struct ADVANTAGE_BIAS final {
    static_assert(N>0 && N<64);
    static constexpr u64 FULL=(1ULL<<N)-1;
    struct Posture { u64 original,unresolved,resolved; bool valid,review; };
    static constexpr Posture VERIFY(u64 u) noexcept {
        if(u&~FULL) return {FULL,u,0,false,true};
        return {FULL,u,FULL^u,true,u!=0};
    }
};
struct ACE final {
    static constexpr auto CONSULT(const LAW& c) noexcept { return ADVANTAGE_BIAS<9>::VERIFY(c.REVEAL()); }
};
struct NOBODY final {
    static constexpr auto CONSULT() noexcept { return ADVANTAGE_BIAS<9>::VERIFY(511); }
};
struct GREG final {
    static constexpr auto CONSULT(const LAW& c,bool capital,bool time) noexcept {
        return ADVANTAGE_BIAS<11>::VERIFY(u64(c.REVEAL())|(u64(capital)<<9)|(u64(time)<<10));
    }
};
}
