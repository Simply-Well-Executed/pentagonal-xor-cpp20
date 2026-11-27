// SPDX-License-Identifier: 0BSD
// GENSHIN/TriPi-v0: exact three-route audit of a BiDi occurrence ledger.
// Not a replacement for swe::tri_pi or HANDI/TriPi; no standard headers.
#ifndef GENSHIN_TRIPI_HPP
#define GENSHIN_TRIPI_HPP
struct genshin_tripi {
using U = unsigned;
using I = int;
static constexpr U absent = ~0u;
static constexpr U capacity = 256;
struct Source { U cp, expected_visual; };
struct Visual { U cp, logical; I delta; };
struct Report {
    bool forward, reverse, zfold;
    constexpr bool passed() const {
        return forward && reverse && zfold;
    }
};
static constexpr bool scalar(U c) {
    return c<=0x10FFFFu && !(c>=0xD800u && c<=0xDFFFu);
}
// expected_visual comes from a separately declared reference mapping;
// absent denotes caller-declared X9 omission, not deletion.
static constexpr Report inspect(const Source* s, U n,
                         const Visual* v, U m) {
    if (n>capacity || m>n || (!s && n) || (!v && m))
        return {};
    Report a{true,true,true};
    U rank[capacity]{}, expected[capacity]{};
    bool present[capacity]{}, seen[capacity]{};
    U kept=0;
    for (U i=0;i<n;++i) {
        if (!scalar(s[i].cp)) return {};
        U p=s[i].expected_visual;
        if (p==absent) continue;
        if (p>=m || present[p]) return {};
        present[p]=true;
        expected[p]=i;
        rank[i]=kept++;
        // YIN: forward comparison to declared reference.
        if (v[p].logical!=i || v[p].cp!=s[i].cp)
            a.forward=false;
    }
    if (kept!=m) return {};
    // YANG: reverse reconstruction and exact occurrence coverage.
    for (U p=0;p<m;++p) {
        U i=v[p].logical;
        if (i>=n || !scalar(v[p].cp)) {
            a.reverse=false; continue;
        }
        if (seen[i] || s[i].expected_visual==absent ||
            s[i].cp!=v[p].cp) a.reverse=false;
        seen[i]=true;
    }
    for (U i=0;i<n;++i)
        if (seen[i]!=(s[i].expected_visual!=absent))
            a.reverse=false;
    // AIRGAP: Z-fold traversal, alternating outside ends inward.
    // Exact inverse and per-occurrence displacement, then sum invariant.
    I sum=0;
    for (U k=0;k<m;++k) {
        U p=(k&1u) ? m-1-k/2 : k/2;
        U i=v[p].logical;
        if (i>=n || i!=expected[p] ||
            v[p].delta!=I(p)-I(rank[i])) {
            a.zfold=false; continue;
        }
        sum+=v[p].delta;
    }
    if (sum!=0) a.zfold=false;
    return a;
}
};

// Same frozen synthetic fixture values; evaluated after type completion.
static_assert([]() constexpr {
    // Synthetic resolved-level mixed-direction reference; not a font test.
    constexpr genshin_tripi::Source src[]={
        {0x41,0},{0x05D0,3},{0x05D1,2},{0x05D2,1},{0x42,4}
    };
    constexpr genshin_tripi::Visual good[]={
        {0x41,0,0},{0x05D2,3,-2},{0x05D1,2,0},{0x05D0,1,2},{0x42,4,0}
    };
    static_assert(genshin_tripi::inspect(src,5,good,5).passed());
    constexpr genshin_tripi::Visual wrong_order[]={
        {0x41,0,0},{0x05D0,1,0},{0x05D1,2,0},{0x05D2,3,0},{0x42,4,0}
    };
    static_assert(!genshin_tripi::inspect(src,5,wrong_order,5).forward);
    static_assert(genshin_tripi::inspect(src,5,wrong_order,5).reverse);
    static_assert(!genshin_tripi::inspect(src,5,wrong_order,5).zfold);
    constexpr genshin_tripi::Visual forged_delta[]={
        {0x41,0,0},{0x05D2,3,-1},{0x05D1,2,0},{0x05D0,1,1},{0x42,4,0}
    };
    static_assert(genshin_tripi::inspect(src,5,forged_delta,5).forward);
    static_assert(genshin_tripi::inspect(src,5,forged_delta,5).reverse);
    static_assert(!genshin_tripi::inspect(src,5,forged_delta,5).zfold);
    constexpr genshin_tripi::Visual duplicate[]={
        {0x41,0,0},{0x05D2,3,-2},{0x05D1,2,0},{0x05D2,3,0},{0x42,4,0}
    };
    static_assert(!genshin_tripi::inspect(src,5,duplicate,5).reverse);
    constexpr genshin_tripi::Source omitted[]={{0x41,0},{0x202A,genshin_tripi::absent},{0x42,1}};
    constexpr genshin_tripi::Visual retained[]={{0x41,0,0},{0x42,2,0}};
    static_assert(genshin_tripi::inspect(omitted,3,retained,2).passed());
    constexpr genshin_tripi::Source bad_ref[]={{0x41,0},{0x42,0}};
    static_assert(!genshin_tripi::inspect(bad_ref,2,retained,2).passed());
    return true;
}());
#endif
