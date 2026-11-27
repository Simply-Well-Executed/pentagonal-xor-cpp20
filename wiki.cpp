// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// WIKI/1: native read-only Markdown navigation graph.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

struct WIKI {
    static constexpr int limit=128, name_size=96, byte_limit=262144;
    int n=0,index=-1,home=-1;
    bool edge[limit][limit]{};
    int prev[limit]{},next[limit]{};
    char name[limit][name_size]{};

    constexpr bool ring() const noexcept {
        if(n<2||n>limit||index<0||index>=n)return false;
        bool seen[limit]{};
        int p=index;
        for(int i=0;i<n;++i){
            if(p<0||p>=n||seen[p])return false;
            seen[p]=true;
            const int q=next[p];
            if(q<0||q>=n||prev[q]!=p||!edge[p][q])return false;
            p=q;
        }
        return p==index;
    }
    constexpr bool verify() const noexcept {
        if(!ring()||home<0||home>=n||home==index)return false;
        for(int i=0;i<n;++i){
            if(i!=index&&(!edge[index][i]||!edge[i][index]))return false;
            if(i!=home&&!edge[i][home])return false;
            for(int j=0;j<n;++j)
                if(edge[i][j]!=edge[j][i])return false;
        }
        return true;
    }
    constexpr unsigned links() const noexcept {
        unsigned count=0;
        for(int i=0;i<n;++i)for(int j=0;j<n;++j)
            if(i!=j&&edge[i][j])++count;
        return count;
    }
    // UNION: retain all edges, add reciprocal, central and ring edges.
    constexpr void unite() noexcept {
        if(n<2||n>limit||index<0||index>=n||home<0||home>=n)return;
        if(!ring())for(int i=0;i<n;++i){
            prev[i]=(i+n-1)%n;next[i]=(i+1)%n;
        }
        for(int i=0;i<n;++i){
            edge[i][index]=edge[index][i]=true;
            edge[i][home]=edge[home][i]=true;
            edge[i][next[i]]=edge[next[i]][i]=true;
            for(int j=0;j<n;++j)if(edge[i][j])edge[j][i]=true;
        }
    }
};

constexpr bool fixture() noexcept {
    WIKI w{};w.n=3;w.index=0;w.home=1;
    for(int i=0;i<3;++i){w.next[i]=(i+1)%3;w.prev[i]=(i+2)%3;}
    w.edge[0][1]=true;w.edge[1][2]=true;w.edge[2][0]=true;
    if(w.verify())return false;
    w.unite();
    if(!w.verify()||w.links()!=6)return false;
    w.edge[1][0]=false;
    if(w.verify())return false;
    w.unite();w.next[0]=0;
    if(w.verify())return false;
    w.unite();return w.verify()&&w.links()==6;
}
static_assert(fixture());

static WIKI wiki{};
static char bytes[WIKI::byte_limit+1]{};
static bool output_ok=true;

static bool starts(const char* p,const char* e,const char* s) noexcept {
    while(*s){if(p==e||*p!=*s)return false;++p;++s;}return true;
}
static const char* find(const char* p,const char* e,const char* s) noexcept {
    for(;p!=e;++p)if(starts(p,e,s))return p;
    return e;
}
static bool equal(const char* a,const char* b) noexcept {
    while(*a&&*a==*b){++a;++b;}return *a==*b;
}
static bool less(const char* a,const char* b) noexcept {
    while(*a&&*a==*b){++a;++b;}return *a<*b;
}
static int lookup(const char* p,const char* e) noexcept {
    for(int i=0;i<wiki.n;++i){
        const char* s=wiki.name[i];const char* q=p;
        while(q!=e&&*s&&*q==*s){++q;++s;}
        if(q==e&&!*s)return i;
    }
    return -1;
}
static void out(const char* s) noexcept {
    DWORD n=0,w=0;while(s[n])++n;
    if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),s,n,&w,nullptr)||w!=n)
        output_ok=false;
}
static void number(unsigned n) noexcept {
    char text[12]{};int i=11;
    do{text[--i]=char('0'+n%10);n/=10;}while(n);
    out(text+i);
}
static int hold(const char* reason) noexcept {
    out("WIKI|HOLD|");out(reason);out("\n");return 1;
}
static bool allowed(wchar_t c) noexcept {
    return (c>=L'A'&&c<=L'Z')||(c>=L'a'&&c<=L'z')||
           (c>=L'0'&&c<=L'9')||c==L'-'||c==L'_';
}
static bool inventory() noexcept {
    const DWORD a=GetFileAttributesW(L".wiki");
    if(a==INVALID_FILE_ATTRIBUTES||!(a&FILE_ATTRIBUTE_DIRECTORY)||
       (a&FILE_ATTRIBUTE_REPARSE_POINT))return false;
    WIN32_FIND_DATAW f{};
    HANDLE h=FindFirstFileW(L".wiki\\*",&f);
    if(h==INVALID_HANDLE_VALUE)return false;
    bool ok=true;
    do{
        const wchar_t* s=f.cFileName;int len=0;while(s[len])++len;
        if((len==1&&s[0]==L'.')||(len==2&&s[0]==L'.'&&s[1]==L'.'))continue;
        if(f.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT)){ok=false;break;}
        if(len<4||s[len-3]!=L'.'||s[len-2]!=L'm'||s[len-1]!=L'd')continue;
        if(wiki.n==WIKI::limit||len>=WIKI::name_size){ok=false;break;}
        for(int j=0;j<len-3;++j)if(!allowed(s[j]))ok=false;
        if(!ok)break;
        for(int j=0;j<len;++j)wiki.name[wiki.n][j]=char(s[j]);
        ++wiki.n;
    }while(FindNextFileW(h,&f));
    if(ok&&GetLastError()!=ERROR_NO_MORE_FILES)ok=false;
    FindClose(h);
    if(!ok||wiki.n<2)return false;
    for(int i=0;i<wiki.n;++i)for(int j=i+1;j<wiki.n;++j)
        if(less(wiki.name[j],wiki.name[i]))
            for(int k=0;k<WIKI::name_size;++k){
                const char c=wiki.name[i][k];wiki.name[i][k]=wiki.name[j][k];wiki.name[j][k]=c;
            }
    for(int i=0;i<wiki.n;++i){
        wiki.prev[i]=wiki.next[i]=-1;
        if(equal(wiki.name[i],"Index.md"))wiki.index=i;
        if(equal(wiki.name[i],"Home.md"))wiki.home=i;
    }
    return wiki.index>=0&&wiki.home>=0;
}
static bool read_page(int i,DWORD& size) noexcept {
    wchar_t path[WIKI::name_size+7]=L".wiki\\";
    int j=0;while(wiki.name[i][j]){path[6+j]=wchar_t(wiki.name[i][j]);++j;}
    HANDLE h=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,
                         FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
    if(h==INVALID_HANDLE_VALUE)return false;
    BY_HANDLE_FILE_INFORMATION info{};
    LARGE_INTEGER len{};
    bool ok=GetFileInformationByHandle(h,&info)!=0&&
            !(info.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))&&
            GetFileSizeEx(h,&len)!=0&&len.QuadPart>0&&len.QuadPart<=WIKI::byte_limit;
    if(ok)ok=ReadFile(h,bytes,DWORD(len.QuadPart),&size,nullptr)!=0&&size==DWORD(len.QuadPart);
    CloseHandle(h);
    if(!ok)return false;
    for(DWORD k=0;k<size;++k)if(!bytes[k])return false;
    bytes[size]=0;return true;
}
// Restricted inline-Markdown profile; ignore fenced code and HTTP(S).
static bool parse(int i,DWORD size) noexcept {
    const char* end=bytes+size;
    char fence=0;bool nav=false;
    for(const char* line=bytes;line!=end;){
        const char* e=find(line,end,"\n");
        const char* p=line;while(p!=e&&(*p==' '||*p=='\t'))++p;
        if(starts(p,e,"\x60\x60\x60")||starts(p,e,"~~~")){
            if(!fence)fence=*p;else if(fence==*p)fence=0;
        }else if(!fence){
            const bool navigation=starts(line,e,"Previous: [");
            if(navigation&&nav)return false;
            if(navigation)nav=true;
            const char* next_label=navigation?find(line,e,"Next: ["):e;
            int local=0;
            for(const char* m=find(line,e,"](");m!=e;){
                const char* a=m+2;const char* b=find(a,e,")");
                if(b==e)return false;
                if(!starts(a,b,"https://")&&!starts(a,b,"http://")){
                    if(starts(a,b,"./"))a+=2;
                    const int dest=lookup(a,b);
                    if(dest<0)return false;
                    wiki.edge[i][dest]=true;
                    if(navigation){
                        if(!local)wiki.prev[i]=dest;
                        if(next_label!=e&&m>next_label){
                            if(wiki.next[i]!=-1)return false;
                            wiki.next[i]=dest;
                        }
                    }
                    ++local;
                }
                m=find(b+1,e,"](");
            }
            if(navigation&&(wiki.prev[i]<0||wiki.next[i]<0))return false;
        }
        line=e==end?end:e+1;
    }
    return fence==0;
}
static void link(int i) noexcept {
    out("[");out(wiki.name[i]);out("](");out(wiki.name[i]);out(")");
}
static void plan() noexcept {
    wiki.unite();
    out("# Derived index\n");
    int i=wiki.index;
    for(int k=0;k<wiki.n;++k){out("- ");link(i);out("\n");i=wiki.next[i];}
    i=wiki.index;
    for(int k=0;k<wiki.n;++k){
        out("\n## ");out(wiki.name[i]);out("\n\nPrevious: ");link(wiki.prev[i]);
        out(" | ");link(wiki.index);out(" | ");link(wiki.home);
        out(" | Next: ");link(wiki.next[i]);out("\n\n");
        for(int j=0;j<wiki.n;++j)if(i!=j&&wiki.edge[i][j]){
            out("- ");link(j);out("\n");
        }
        i=wiki.next[i];
    }
}
int main(int argc,char** argv) {
    const bool generate=argc==2&&equal(argv[1],"--plan");
    if(argc!=1&&!generate)return hold("usage: wiki.exe [--plan]");
    if(!inventory())return hold("inventory");
    for(int i=0;i<wiki.n;++i){
        DWORD size=0;
        if(!read_page(i,size)||!parse(i,size)){
            out("WIKI|PAGE|");out(wiki.name[i]);out("\n");return hold("read_or_link");
        }
    }
    if(generate){
        plan();
        return wiki.verify()&&output_ok?0:1;
    }
    if(!wiki.verify())return hold("index_reciprocity_or_ring");
    out("WIKI|PASS|PAGES=");number(unsigned(wiki.n));
    out("|DIRECTED_LINKS=");number(wiki.links());
    out("|CLOSED_RING=1|RELEASE=HOLD\n");
    return output_ok?0:1;
}
