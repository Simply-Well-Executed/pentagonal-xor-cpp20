// SPDX-License-Identifier: 0BSD
// C++20 BiDi provenance: UAX #9 L2 on upstream resolved line levels.
// No includes. Not a complete Unicode Bidirectional Algorithm or renderer.
#ifndef GENSHIN_BIDI_HPP
#define GENSHIN_BIDI_HPP
struct genshin_bidi {
using U = unsigned;
using I = int;
static constexpr U capacity = 256;
static constexpr U absent = ~0u;
static constexpr U x9 = 255; // upstream removal marker, not a resolved level
struct Unit { U cp, level; };
struct Ledger {
    bool valid;
    U count;
    U visual_to_logical[capacity];
    U logical_to_visual[capacity];
    U retained_rank[capacity];
    I displacement[capacity];
};
static constexpr bool scalar(U c) {
    return c<=0x10FFFFu && !(c>=0xD800u && c<=0xDFFFu);
}
// Separate strict project check on raw logical stream, NOT soft-wrapped lines.
// No FSI direction resolution. Unmatched controls rejected by project policy.
static constexpr bool balanced(const U* s, U n) {
    if (!s && n) return false;
    U depth=0;
    for (U i=0;i<n;++i) {
        U c=s[i];
        if (!scalar(c)) return false;
        if (c==0x2066 || c==0x2067 || c==0x2068) {
            if (depth==125) return false;
            ++depth;
        } else if (c==0x2069) {
            if (!depth) return false;
            --depth;
        } else if (c==0x0A || c==0x0D || c==0x85 ||
                   c==0x2028 || c==0x2029 ||
                   (c>=0x1C && c<=0x1E)) {
            if (depth) return false;
        }
    }
    return depth==0;
}
// Input: one line AFTER level resolution and L1, with X9 omissions marked.
// Output: L2 positions, NOT shaped glyphs, grapheme clusters or screen pixels.
static constexpr Ledger project(const Unit* s, U n) {
    Ledger a{};
    if (n>capacity || (!s && n)) return a;
    U highest=0, lowest_odd=127;
    for (U i=0;i<capacity;++i) {
        a.visual_to_logical[i]=absent;
        a.logical_to_visual[i]=absent;
        a.retained_rank[i]=absent;
    }
    for (U i=0;i<n;++i) {
        if (!scalar(s[i].cp)) return Ledger{};
        U l=s[i].level;
        if (l==x9) continue;
        if (l>126) return Ledger{};
        a.retained_rank[i]=a.count;
        a.visual_to_logical[a.count++]=i;
        if (l>highest) highest=l;
        if ((l&1u) && l<lowest_odd) lowest_odd=l;
    }
    for (I l=I(highest);l>=I(lowest_odd);--l) {
        U i=0;
        while (i<a.count) {
            if (s[a.visual_to_logical[i]].level<U(l)) {
                ++i; continue;
            }
            U j=i;
            while (j<a.count &&
                   s[a.visual_to_logical[j]].level>=U(l)) ++j;
            U left=i,right=j-1;
            while (left<right) {
                U t=a.visual_to_logical[left];
                a.visual_to_logical[left++]=a.visual_to_logical[right];
                a.visual_to_logical[right--]=t;
            }
            i=j;
        }
    }
    for (U v=0;v<a.count;++v) {
        U i=a.visual_to_logical[v];
        a.logical_to_visual[i]=v;
        a.displacement[i]=I(v)-I(a.retained_rank[i]);
    }
    a.valid=true;
    return a;
}
static constexpr bool audit(const Ledger& a, U n) {
    if (!a.valid || n>capacity || a.count>n) return false;
    U kept=0;
    for (U i=0;i<n;++i) {
        U v=a.logical_to_visual[i];
        if (v==absent) {
            if (a.retained_rank[i]!=absent) return false;
            continue;
        }
        if (v>=a.count || a.visual_to_logical[v]!=i ||
            a.retained_rank[i]!=kept++ ||
            a.displacement[i]!=I(v)-I(a.retained_rank[i])) return false;
    }
    if (kept!=a.count) return false;
    for (U v=0;v<a.count;++v) {
        U i=a.visual_to_logical[v];
        if (i>=n || a.logical_to_visual[i]!=v) return false;
    }
    return true;
}
};

// Same frozen synthetic fixture values; evaluated after type completion.
static_assert([]() constexpr {
    constexpr genshin_bidi::Unit mixed[]={{0x41,0},{0x05D0,1},{0x05D1,1},
                            {0x05D2,1},{0x42,0}};
    constexpr auto m=genshin_bidi::project(mixed,5);
    static_assert(genshin_bidi::audit(m,5) && m.visual_to_logical[1]==3 &&
                  m.logical_to_visual[1]==3 && m.displacement[1]==2);
    static_assert(m.displacement[3]==-2);
    constexpr genshin_bidi::Unit nested[]={{0x41,0},{0x05D0,1},{0x31,2},
                             {0x32,2},{0x05D1,1},{0x42,0}};
    constexpr auto k=genshin_bidi::project(nested,6);
    static_assert(genshin_bidi::audit(k,6) && k.visual_to_logical[1]==4 &&
                  k.visual_to_logical[2]==2 && k.visual_to_logical[3]==3 &&
                  k.visual_to_logical[4]==1);
    constexpr genshin_bidi::Unit omitted[]={{0x41,0},{0x202A,genshin_bidi::x9},{0x42,0}};
    constexpr auto o=genshin_bidi::project(omitted,3);
    static_assert(genshin_bidi::audit(o,3) && o.count==2 && o.logical_to_visual[1]==genshin_bidi::absent &&
                  o.visual_to_logical[1]==2 && o.displacement[2]==0);
    constexpr genshin_bidi::U good[]={0x2066,0x2067,0x05D0,0x2069,0x200C,0x2069};
    constexpr genshin_bidi::U bad[]={0x2066,0x2069,0x2069,0x200C,0x200C,0x200C};
    constexpr genshin_bidi::U cross[]={0x2068,0x0A,0x2069};
    static_assert(genshin_bidi::balanced(good,6) && !genshin_bidi::balanced(bad,6) && !genshin_bidi::balanced(cross,3));
    constexpr genshin_bidi::Unit invalid[]={{0xD800,0}};
    static_assert(!genshin_bidi::project(invalid,1).valid);
    return true;
}());
#endif
