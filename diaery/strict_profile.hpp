/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// DIAERY strict-profile XML scanner: no library includes, no allocation.
// Accepts ONLY the specified small, structurally safe XHTML 1.0 Strict
// sublanguage. NOT a general XML 1.0 or complete W3C DTD validator.
// All accepted pages are limited to exact XHTML Strict declarations,
// html/head/title/body, and plain ASCII h1/h2/p paragraphs.
#ifndef SWE_DIAERY_STRICT_PROFILE_HPP
#define SWE_DIAERY_STRICT_PROFILE_HPP
namespace swe::diaery::strict {
using u64=unsigned long long;
struct Line {char tag;char text[238];};
struct Page {
    char title[238];Line lines[36];unsigned count=0u;
    bool parity_seen=false;bool stage_seen[4]{};
    bool structurally_valid=false;
};
[[nodiscard]] constexpr unsigned len(const char *p) noexcept {
    if(!p)return 0u;
    unsigned n=0u;
    while(p[n])++n;
    return n;
}
[[nodiscard]] constexpr bool same(const char* a,const char* b) noexcept {
    if(!a||!b)return false;
    for(unsigned i=0;;++i){if(a[i]!=b[i])return false;if(!a[i])return true;}
}
[[nodiscard]] constexpr bool prefix(const char* a,const char* b) noexcept {
    if(!a||!b)return false;
    for(unsigned i=0;b[i];++i)if(a[i]!=b[i])return false;
    return true;
}
struct Builder {
    char s[256]{};unsigned n=0u;bool ok=true;
    constexpr void chr(char c) noexcept {
        if(n+1u>=sizeof(s)){ok=false;return;}
        s[n++]=c;s[n]=0;
    }
    constexpr void text(const char* p) noexcept {
        for(unsigned i=0;p&&p[i];++i)chr(p[i]);
    }
    constexpr void num(long long v) noexcept {
        if(v<0){chr('-');v=-v;}
        char tmp[24]{};unsigned i=0u;
        do{tmp[i++]=static_cast<char>('0'+v%10);v/=10;}while(v&&i<24u);
        while(i)chr(tmp[--i]);
    }
};
struct Scanner {
    const char* p;unsigned remaining;
    constexpr void space() noexcept {
        while(remaining && (*p==' '||*p=='\r'||*p=='\n'||*p=='\t')){++p;--remaining;}
    }
    constexpr bool take(const char* literal) noexcept {
        const unsigned n=len(literal);
        if(n>remaining)return false;
        for(unsigned i=0;i<n;++i)if(p[i]!=literal[i])return false;
        p+=n;remaining-=n;return true;
    }
    constexpr bool text(char* out,unsigned capacity) noexcept {
        unsigned n=0u;
        while(remaining && *p!='<'){
            const unsigned char c=static_cast<unsigned char>(*p);
            // Fail closed: no entity parsing, CDATA, controls or UTF-8 in POC.
            if(c<0x20u||c>0x7eu||c=='&'||n+1u>=capacity)return false;
            out[n++]=*p++;--remaining;
        }
        if(!n)return false;
        out[n]=0;
        return true;
    }
};
inline constexpr const char* XML="<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
inline constexpr const char* DTD="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">";
inline constexpr const char* ROOT="<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">";
[[nodiscard]] constexpr bool parse(const char* data,unsigned size,Page& page) noexcept {
    page=Page{};
    if(!data||size==0u||size>16000u)return false;
    Scanner s{data,size};
    // No bytes before XML declaration; no external entity handling.
    if(!s.take(XML))return false;
    s.space();
    if(!s.take(DTD))return false;
    s.space();
    if(!s.take(ROOT))return false;
    s.space();
    if(!s.take("<head>"))return false;
    if(!s.take("<title>"))return false;
    if(!s.text(page.title,sizeof(page.title)))return false;
    if(!s.take("</title>"))return false;
    if(!s.take("</head>"))return false;
    s.space();
    if(!s.take("<body>"))return false;
    s.space();
    bool titleSeen=false;
    while(s.remaining&&!prefix(s.p,"</body>")){
        if(page.count>=36u)return false;
        const char* closer=nullptr;char tag=0;
        if(s.take("<h1>")){closer="</h1>";tag='1';if(titleSeen)return false;titleSeen=true;}
        else if(s.take("<h2>")){closer="</h2>";tag='2';if(!titleSeen)return false;}
        else if(s.take("<p>")){closer="</p>";tag='p';if(!titleSeen)return false;}
        else return false;
        Line& line=page.lines[page.count];line.tag=tag;
        if(!s.text(line.text,sizeof(line.text)))return false;
        if(!s.take(closer))return false;
        if(prefix(line.text,"PARITY-1|")){
            if(tag!='p'||page.parity_seen)return false;
            page.parity_seen=true;
        }
        for(unsigned stage=0;stage<4;++stage){
            Builder b{};b.text("STAGE-");b.num(stage);b.chr('|');
            if(prefix(line.text,b.s)){
                if(tag!='p'||page.stage_seen[stage])return false;
                page.stage_seen[stage]=true;
            }
        }
        ++page.count;s.space();
    }
    if(!titleSeen||!s.take("</body>"))return false;
    s.space();if(!s.take("</html>"))return false;
    s.space();if(s.remaining)return false;
    page.structurally_valid=true;return true;
}
} // namespace swe::diaery::strict
#endif
