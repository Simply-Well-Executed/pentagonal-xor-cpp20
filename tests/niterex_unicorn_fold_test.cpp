/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Scope: independently authored first-party source only; upstream reference rights unchanged.
*/
// SPDX-License-Identifier: 0BSD
#include "../include/niterex_unicorn_fold.hpp"

namespace {
using swe::niterex::count_t;
struct Buffer {
    char bytes[18000]{};
    count_t used{0};
};
bool append(void* context, const char* p, count_t count) noexcept {
    if(!context || !p) return false;
    auto& b=*static_cast<Buffer*>(context);
    if(count>=sizeof(b.bytes)-b.used) return false;
    for(count_t i=0;i<count;++i) b.bytes[b.used+i]=p[i];
    b.used+=count;
    b.bytes[b.used]='\0';
    return true;
}
bool fail(void*,const char*,count_t) noexcept { return false; }
bool contains(const char* text,const char* pattern) noexcept {
    if(!text||!pattern) return false;
    for(;*text;++text) {
        const char* a=text;
        const char* b=pattern;
        while(*a&&*b&&*a==*b){++a;++b;}
        if(!*b) return true;
    }
    return false;
}
unsigned count_occurrences(const char* text,const char* pattern) noexcept {
    unsigned n=0;
    for(;*text;++text) {
        const char* a=text;
        const char* b=pattern;
        while(*a&&*b&&*a==*b){++a;++b;}
        if(!*b) ++n;
    }
    return n;
}
} // namespace
int main() {
    using namespace swe::niterex;
    static_assert(verify_quests());
    if(parse_operation("and")!=Op::Invalid ||
       parse_operation("AND")!=Op::And ||
       apply(Op::Ror,13u,0u)!=262u ||
       apply(Op::Not,0u,0u)!=511u ||
       apply(Op::Zero,511u,511u)!=0u ||
       apply(Op::Add,511u,1u)!=0u ||
       apply(Op::Sub,0u,1u)!=511u) return 1;
    unsigned value=123u;
    if(!parse_unsigned("0",value)||value!=0u ||
       !parse_unsigned("511",value)||value!=511u ||
       parse_unsigned("512",value) || parse_unsigned("-1",value) ||
       parse_unsigned("",value) || parse_unsigned("1.0",value) ||
       parse_unsigned("9x",value)) return 2;
    unsigned checksum=0;
    for(const Quest& q:quests) checksum+=apply(q.op,13u,10u);
    if(checksum!=829u) return 3;
    if(surface_a(-1,1)!=100 ||surface_b(1,-1)!=100 ||
       surface_a(0,0)!=40 ||surface_b(0,0)!=40) return 4;
    Buffer buffer{};
    if(!render_xhtml_strict({&buffer,append})) return 5;
    if(buffer.used<3900 || buffer.used>sizeof(buffer.bytes)-1 ||
       !contains(buffer.bytes,"XHTML 1.0 Strict//EN") ||
       !contains(buffer.bytes,"xmlns=\"http://www.w3.org/1999/xhtml\"") ||
       !contains(buffer.bytes,"Twenty-five independent W and V coordinates") ||
       !contains(buffer.bytes,"NiteRex nine-bit processor quests") ||
       !contains(buffer.bytes,"Nine-quest checksum = 829.") ||
       contains(buffer.bytes,"<canvas") ||
       contains(buffer.bytes,"<script") ||
       contains(buffer.bytes,"<svg") ||
       contains(buffer.bytes,"<iframe") ||
       !contains(buffer.bytes,"</html>")) return 6;
    if(count_occurrences(buffer.bytes,"<td><span class=\"a\">A ")!=25 ||
       count_occurrences(buffer.bytes,"<tr><th scope=\"row\">")!=14) return 7;
    if(render_xhtml_strict({nullptr,fail}) || render_xhtml_strict({nullptr,nullptr}))return 8;
    return 0;
}
