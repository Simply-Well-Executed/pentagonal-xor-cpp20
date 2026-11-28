/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// NiteRex / uni-cor-n-fold: independent native C++20 reference implementation.
// Modelled from user-supplied documented behavior; no MIT binary/source imported.
// Not a complete Web.9X renderer, browser, cryptographic verifier or OS emulator.
#ifndef SWE_NITEREX_UNICORN_FOLD_HPP
#define SWE_NITEREX_UNICORN_FOLD_HPP

namespace swe::niterex {
using count_t = decltype(sizeof(0));
inline constexpr unsigned word_mask = 511u;

enum class Op : unsigned { And, Or, Xor, Not, Add, Sub, Ror, Nop, Zero, Invalid };
struct Quest {
    const char* label;
    Op op;
    unsigned expected;
};
inline constexpr Quest quests[9] = {
    {"AND", Op::And, 8u}, {"OR", Op::Or, 15u},
    {"XOR", Op::Xor, 7u}, {"NOT", Op::Not, 498u},
    {"ADD", Op::Add, 23u}, {"SUB", Op::Sub, 3u},
    {"ROR", Op::Ror, 262u}, {"NOP", Op::Nop, 13u},
    {"ZERO", Op::Zero, 0u}
};
[[nodiscard]] constexpr bool equal(const char* a, const char* b) noexcept {
    if (!a || !b) return false;
    while (*a && *b) { if (*a++ != *b++) return false; }
    return *a == *b;
}
[[nodiscard]] constexpr bool parse_unsigned(const char* s, unsigned& out) noexcept {
    if (!s || !*s) return false;
    unsigned n = 0u;
    for (; *s; ++s) {
        if (*s < '0' || *s > '9') return false;
        unsigned digit = static_cast<unsigned>(*s - '0');
        if (n > (word_mask - digit) / 10u) return false;
        n = n * 10u + digit;
    }
    out = n;
    return true;
}
[[nodiscard]] constexpr Op parse_operation(const char* s) noexcept {
    for (const Quest& q : quests) {
        if (equal(s, q.label)) return q.op;
    }
    return Op::Invalid;
}
[[nodiscard]] constexpr unsigned apply(Op op, unsigned a, unsigned b) noexcept {
    a &= word_mask;
    b &= word_mask;
    switch (op) {
        case Op::And: return a & b;
        case Op::Or: return a | b;
        case Op::Xor: return a ^ b;
        case Op::Not: return (~a) & word_mask;
        case Op::Add: return (a + b) & word_mask;
        case Op::Sub: return (a - b) & word_mask;
        case Op::Ror: return ((a >> 1u) | ((a & 1u) << 8u)) & word_mask;
        case Op::Nop: return a;
        case Op::Zero: return 0u;
        default: return 0u;
    }
}
[[nodiscard]] constexpr bool verify_quests() noexcept {
    unsigned sum = 0u;
    for (const Quest& q : quests) {
        const unsigned result = apply(q.op, 13u, 10u);
        if (result != q.expected) return false;
        sum += result;
    }
    return sum == 829u;
}
static_assert(verify_quests(), "NiteRex nine quests must remain independent");

// Finite preview of the user's older Gaussian W/V model with width 1.05.
// Entries are rounded whole-number percentage fixtures, NOT a demographic
// classifier and not a substitute for preserving exact sample coordinates.
[[nodiscard]] constexpr unsigned strength(int w, int v, int cw, int cv) noexcept {
    const int dx = w - cw, dv = v - cv;
    const int d2 = dx * dx + dv * dv;
    switch (d2) {
        case 0: return 100u;
        case 1: return 64u;
        case 2: return 40u;
        case 4: return 16u;
        case 5: return 10u;
        case 8: return 3u;
        case 9: return 2u;
        case 10: return 1u;
        default: return 0u;
    }
}
[[nodiscard]] constexpr unsigned surface_a(int w, int v) noexcept {
    return strength(w,v,-1,1);
}
[[nodiscard]] constexpr unsigned surface_b(int w, int v) noexcept {
    return strength(w,v,1,-1);
}
static_assert(surface_a(-1,1) == 100u && surface_b(1,-1) == 100u);
static_assert(surface_a(0,0) == 40u && surface_b(0,0) == 40u);

struct Sink {
    void* context;
    bool (*write)(void*, const char*, count_t) noexcept;
};
inline bool emit(Sink sink, const char* s) noexcept {
    if (!sink.write || !s) return false;
    count_t n = 0u;
    while (s[n] != '\0') ++n;
    return sink.write(sink.context,s,n);
}
inline bool emit_number(Sink sink, unsigned value) noexcept {
    char digits[12]{};
    count_t index = sizeof(digits);
    do { digits[--index] = static_cast<char>('0' + value % 10u);
         value /= 10u; } while(value);
    return sink.write && sink.write(sink.context,digits+index,sizeof(digits)-index);
}
inline bool emit_signed(Sink sink, int value) noexcept {
    if (value < 0) return emit(sink,"-") && emit_number(sink,static_cast<unsigned>(-value));
    if (value > 0) return emit(sink,"+") && emit_number(sink,static_cast<unsigned>(value));
    return emit(sink,"0");
}

// Bounded offline XHTML 1.0 Strict document: no script, canvas, SVG,
// iframe, external stylesheets, dynamic downloads or executable web slots.
// Caller owns output bytes; a failed sink aborts the render immediately.
inline bool render_xhtml_strict(Sink s) noexcept {
    if (!emit(s,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" "
      "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
      "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
      "<head><title>Web.9X NiteRex uni-cor-n-fold reference</title>\n"
      "<meta http-equiv=\"Content-Type\" content=\"application/xhtml+xml; charset=UTF-8\" />\n"
      "<style type=\"text/css\"><![CDATA[\n"
      "body{margin:0;background:#101b2d;color:#edf5ff;font:16px/1.55 Arial,sans-serif}"
      ".shell{max-width:960px;margin:0 auto;padding:20px}"
      "h1{color:#d7f8ff}h2{color:#d2edff}"
      "p{max-width:830px}table{border-collapse:collapse;width:100%;margin:14px 0 28px}"
      "th,td{border:1px solid #466178;padding:9px;text-align:center}"
      "th{background:#24425b;color:#edf9ff}"
      "td{background:#172c41}.a{color:#76e9ef}.b{color:#ffbbdf}"
      ".meta{color:#abc8d8}.stack{font-weight:bold}"
      "a{color:#9ff1ec}:focus{outline:2px solid #edcc7c}"
      "]]></style></head><body>\n"
      "<div class=\"shell\"><h1>Web.9X / NiteRex / uni-cor-n-fold</h1>\n"
      "<p class=\"meta\">Offline XHTML 1.0 Strict reference generated by native ISO C++20. "
      "CORE preserved; neither A nor B represents a person, sex or gender.</p>\n"
      "<h2>Twenty-five independent W and V coordinates</h2>\n"
      "<p>W and V are integers from -2 through +2. Each cell retains "
      "separate A and B values. No equal-weight combination is calculated.</p>\n"
      "<table summary=\"Twenty-five signed W and V coordinates with two independent strengths\">\n"
      "<caption>W columns; V rows; A then B</caption><thead><tr>"
      "<th scope=\"col\">V / W</th>")) return false;
    for(int w=-2;w<=2;++w) {
        if(!emit(s,"<th scope=\"col\">") || !emit_signed(s,w) || !emit(s,"</th>")) return false;
    }
    if(!emit(s,"</tr></thead><tbody>")) return false;
    for(int v=2;v>=-2;--v) {
        if(!emit(s,"<tr><th scope=\"row\">") || !emit_signed(s,v) || !emit(s,"</th>")) return false;
        for(int w=-2;w<=2;++w) {
            if(!emit(s,"<td><span class=\"a\">A ")) ||
               !emit_number(s,surface_a(w,v)) ||
               !emit(s,"%</span> <span class=\"b\">B ") ||
               !emit_number(s,surface_b(w,v)) ||
               !emit(s,"%</span></td>")) return false;
        }
        if(!emit(s,"</tr>")) return false;
    }
    if(!emit(s,"</tbody></table>\n"
      "<h2>NiteRex nine-bit processor quests</h2>\n"
      "<p>A = 13; B = 10; word width = 9 bits. "
      "Each operation is separately evaluated, with no coin or entitlement claim.</p>\n"
      "<table summary=\"Nine processor operation results\"><caption>Nine reference operations</caption>"
      "<thead><tr><th scope=\"col\">Quest</th><th scope=\"col\">Result</th></tr></thead><tbody>")) return false;
    for(const Quest& q:quests) {
        if(!emit(s,"<tr><th scope=\"row\">") || !emit(s,q.label) ||
           !emit(s,"</th><td>") || !emit_number(s,apply(q.op,13u,10u)) ||
           !emit(s,"</td></tr>")) return false;
    }
    return emit(s,"</tbody></table>\n"
      "<p class=\"meta\">Nine-quest checksum = 829. Source of truth: independently computed C++20 "
      "operators; displayed tables are derived witnesses. No signatures are generated or "
      "verified. Prototype signed-manifest slot schema remains separately held for review.</p>\n"
      "<p><a href=\"#top\">Local document</a></p></div></body></html>\n");
}
} // namespace swe::niterex
#endif
