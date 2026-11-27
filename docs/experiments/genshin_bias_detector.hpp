// SPDX-License-Identifier: 0BSD
// Exploratory GENSHIN geometric BIAS detector, C++20, no includes.
// Input: registered integer contour samples AFTER four axis-aligned folds.
// Each sample retains occurrence ID and a bit for each applied reflection.
// No font acquisition, floating point, allocation, I/O or network calls.
namespace genshin_bias {
using I = long long;
using U = unsigned;
constexpr U capacity = 128, devices = 5;
constexpr I bound = 1000000;
struct Point { I x, y; };
struct Fold { U axis; I crease; }; // 0=x, 1=y; move greater side
struct Sample { Point p; U device, mask; };
struct Score { U total, inside; I numerator, denominator; };
struct Report {
    bool valid;
    U ties, peak;
    Point focus;
    Point origin[capacity];
    Score score[devices];
};
constexpr bool bounded(I x) { return x >= -bound && x <= bound; }
constexpr bool same(Point a, Point b) { return a.x == b.x && a.y == b.y; }
constexpr bool near(Point a, Point b, I r) {
    return a.x-b.x >= -r && a.x-b.x <= r &&
           a.y-b.y >= -r && a.y-b.y <= r;
}
constexpr bool undo(Sample s, const Fold (&f)[4], Point& p) {
    p=s.p;
    if (s.device>=devices || s.mask>=16 ||
        !bounded(p.x) || !bounded(p.y)) return false;
    for (U k=4; k!=0; --k) {
        U j=k-1;
        if (f[j].axis>1 || !bounded(f[j].crease)) return false;
        I& v=f[j].axis==0 ? p.x : p.y;
        if (v>f[j].crease) return false;
        if ((s.mask>>j)&1u) {
            if (v==f[j].crease) return false;
            v=2*f[j].crease-v;
        }
    }
    return true;
}
constexpr Report detect(const Sample* s, U n,
                        const Fold (&f)[4], I r) {
    Report out{};
    if (!s || n==0 || n>capacity || r<0 || r>bound) return out;
    for (U i=0; i<n; ++i) {
        if (!undo(s[i],f,out.origin[i])) return Report{};
        ++out.score[s[i].device].total;
    }
    out.valid=true;
    for (U i=0; i<n; ++i) {
        bool duplicate=false;
        for (U j=0; j<i; ++j)
            if (same(s[i].p,s[j].p)) duplicate=true;
        if (duplicate) continue;
        U hits=0;
        for (U j=0; j<n; ++j)
            if (near(s[i].p,s[j].p,r)) ++hits;
        if (hits>out.peak) {
            out.peak=hits; out.focus=s[i].p; out.ties=1;
        } else if (hits==out.peak) ++out.ties;
    }
    if (out.ties!=1) return out; // focus field not authoritative
    for (U i=0; i<n; ++i)
        if (near(s[i].p,out.focus,r)) ++out.score[s[i].device].inside;
    for (U d=0; d<devices; ++d) {
        Score& a=out.score[d];
        a.numerator=I(a.inside)*n-I(out.peak)*a.total;
        a.denominator=I(a.total)*n;
    }
    return out;
}
// Exact synthetic compile-time fixtures; not a crown or font experiment.
constexpr Fold test_folds[4]={{0,0},{1,0},{0,0},{1,0}};
constexpr Sample test_samples[]={
    {{-2,-2},0,1}, {{-2,-2},1,2}, {{-8,-8},1,0}
};
constexpr Report test_report=detect(test_samples,3,test_folds,0);
static_assert(test_report.valid && test_report.ties==1 && test_report.peak==2);
static_assert(same(test_report.origin[0],{2,-2}));
static_assert(same(test_report.origin[1],{-2,2}));
static_assert(test_report.score[0].numerator==1 &&
              test_report.score[0].denominator==3);
static_assert(test_report.score[1].numerator==-1 &&
              test_report.score[1].denominator==6);
constexpr Sample tie_samples[]={{{-2,-2},0,0},{{-8,-8},1,0}};
static_assert(detect(tie_samples,2,test_folds,0).ties==2);
constexpr Sample invalid_samples[]={{{0,-2},0,1}};
static_assert(!detect(invalid_samples,1,test_folds,0).valid);
} // namespace genshin_bias
