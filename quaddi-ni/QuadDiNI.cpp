// SPDX-License-Identifier: 0BSD
// QUADDI-NI: native ISO C++20, no STL containers, offline TSV engine.
// Every source has its own ordinal. The six ordered states are exploratory.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <stdint.h>

static void hold(const char* why) {
    fprintf(stderr,"QUADDI|HOLD|%s\n",why);
    exit(1);
}
static bool finite_number(double v) { return v == v && v <= DBL_MAX && v >= -DBL_MAX; }
static bool next_line(FILE* f, char*& buf, size_t& cap) {
    size_t len=0; int ch=0;
    while ((ch=fgetc(f))!=EOF && ch!='\n') {
        if(len+1>=cap) {
            const size_t next=cap==0?128:cap*2;
            if(next<=cap) hold("line_allocation_overflow");
            char* fresh=new char[next];
            if(len) memcpy(fresh,buf,len);
            delete[] buf; buf=fresh; cap=next;
        }
        if(ch==0 || ch>127) hold("non_ASCII_input");
        buf[len++]=(char)ch;
    }
    if(ch==EOF && len==0) return false;
    if(len && buf[len-1]=='\r') --len;
    if(!cap) { cap=128; buf=new char[cap]; }
    buf[len]=0; return true;
}
static bool blank_or_comment(const char* s) {
    return s[0]==0 || s[0]=='#';
}
static bool safe_name(const char* s) {
    if(!*s) return false;
    for(const unsigned char* p=(const unsigned char*)s;*p;++p) {
        const unsigned char c=*p;
        if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||
             (c>='0'&&c<='9')||c=='_'||c=='-'||c=='.')) return false;
    }
    return true;
}
static double decimal(const char* s) {
    if(!*s) hold("empty_number");
    errno=0;
    char* tail=nullptr;
    const double v=strtod(s,&tail);
    if(tail==s || *tail!=0 || errno==ERANGE || !finite_number(v))
        hold("invalid_or_unrepresentable_number");
    return v;
}
static FILE* open_file(const char* path) {
    FILE* f=nullptr;
    if(fopen_s(&f,path,"rb")!=0 || !f) hold("source_file_missing");
    return f;
}
static unsigned long long count_rows(const char* path, bool is_notes) {
    FILE* f=open_file(path);
    char* buf=nullptr;size_t cap=0;unsigned long long count=0;
    while(next_line(f,buf,cap)) {
        if(blank_or_comment(buf)) continue;
        if(is_notes && !safe_name(buf)) hold("invalid_note_name");
        if(count==UINT64_MAX) hold("row_count_overflow");
        ++count;
    }
    delete[] buf;fclose(f);
    return count;
}
static double power_int(double x,int p) {
    // Defined zero-exponent empty product: t^0 = 1, including t = 0.
    double v=1.0,base=x;
    unsigned int n=(unsigned int)p;
    while(n) {
        if(n&1u)v*=base;
        n>>=1;
        if(n)base*=base;
        if(!finite_number(v)||!finite_number(base))hold("power_nonfinite");
    }
    return v;
}
static bool is_prime(uint64_t n) {
    if(n<2) return false;
    if(n%2==0) return n==2;
    for(uint64_t k=3;k<=n/k;k+=2) if(n%k==0) return false;
    return true;
}
static uint64_t prime_degree(double origin) {
    const double value=fabs(origin);
    // No invented prime when integral precision is already lost.
    if(value>9007199254740991.0) hold("prime_degree_not_exactly_representable");
    const uint64_t midpoint=(uint64_t)floor(value+0.5);
    for(uint64_t d=0;;++d) {
        const uint64_t low=d<=midpoint?midpoint-d:0;
        const uint64_t high=midpoint+d;
        const bool l=is_prime(low),h=is_prime(high);
        if(l||h) {
            if(l&&h && low!=high) return fabs(value-(double)low)<=fabs((double)high-value)?low:high;
            return l?low:high;
        }
        if(d>=UINT64_MAX-midpoint) hold("prime_search_overflow");
    }
}
struct Complex { double re,im; };
struct Witness { Complex state[6]; double gamma[6]; double closure; uint64_t degree; };
static Witness make_witness(double intercept,int power) {
    Witness w={};
    const double origin=power_int(intercept,power);
    w.degree=prime_degree(origin);
    double rr=0.0,ri=0.0;
    const double magnitude=pow(fabs(origin),1.0/(double)w.degree);
    if(!finite_number(magnitude)) hold("prime_root_nonfinite");
    if(origin<0.0 && w.degree==2) ri=magnitude;
    else rr=origin<0.0?-magnitude:magnitude;
    const double pi=3.141592653589793;
    w.state[0]={0,0};
    w.state[1]={0,-1};
    w.state[2]={rr,ri-1};
    w.state[3]={rr+pi,ri-1};
    w.state[4]={rr+pi,ri};
    w.state[5]={rr,ri};
    w.gamma[0]=0;
    for(int i=1;i<6;++i)
        w.gamma[i]=hypot(w.state[i].re-w.state[i-1].re,
                          w.state[i].im-w.state[i-1].im);
    w.closure=hypot(rr,ri);
    return w;
}
static void write_row(unsigned long long page,unsigned long long ordinal,
                      const char* name,int power,int state,double x,
                      double real,double imaginary,double centrix,
                      const Witness& w) {
    if(!finite_number(real)||!finite_number(imaginary))
        hold("computed_state_nonfinite");
    // Centrix is unavailable for slope 0, which remains a valid constant line.
    if(fprintf(stdout,
      "ROW\t%llu\t%llu\t%s\t%d\t%d\t%.17g\t%.17g\t%.17g\t",
      page,ordinal,name,power,state,x,real,imaginary)<0) hold("write_failure");
    if(finite_number(centrix)) printf("%.17g",centrix);
    else printf("NA");
    printf("\t%.17g\t%.17g\t%llu\n",w.gamma[state-1],w.closure,
           (unsigned long long)w.degree);
}
static void emit_samples(const char* samples,const char* name,
                         unsigned long long ordinal,unsigned long long note_count,
                         double slope,double intercept,int power) {
    const Witness w=make_witness(intercept,power);
    const double cx=slope==0.0?HUGE_VAL:-intercept/slope;
    FILE* f=open_file(samples);
    char* buf=nullptr;size_t cap=0;unsigned long long samples_seen=0;
    while(next_line(f,buf,cap)) {
        if(blank_or_comment(buf)) continue;
        const double x=decimal(buf);
        const double core=power_int(slope*x+intercept,power);
        for(int j=0;j<6;++j)
            write_row((unsigned long long)j+1,ordinal,name,power,j+1,x,
                      core+w.state[j].re,w.state[j].im,cx,w);
        write_row(6+note_count+ordinal,ordinal,name,power,6,x,
                  core+w.state[5].re,w.state[5].im,cx,w);
        ++samples_seen;
    }
    delete[] buf;fclose(f);
    if(!samples_seen) hold("empty_sample_file");
}
int main(int argc,char** argv) {
    if(argc!=4 && argc!=5) hold("usage_lines_samples_optional_power_notes");
    long p=0;
    if(argc==5) {
        if(!argv[3][0]||argv[3][0]=='+'||argv[3][0]=='-')hold("nonnegative_power_required");
        char* end=nullptr;errno=0;p=strtol(argv[3],&end,10);
        if(*end||errno==ERANGE||p<0||p>2147483647L)hold("power_not_representable");
    }
    const char* notes_path=argv[argc-1];
    const unsigned long long lines=count_rows(argv[1],false);
    const unsigned long long notes=count_rows(notes_path,true);
    if(!lines||lines>UINT64_MAX-6-notes) hold("page_count_overflow_or_no_lines");
    printf("META\t1\t6\t%llu\t%llu\t%llu\t%ld\n",
           lines,notes,6+lines+notes,p);
    FILE* nf=open_file(notes_path);
    char* note=nullptr;size_t note_cap=0;unsigned long long note_index=0;
    while(next_line(nf,note,note_cap)) {
        if(blank_or_comment(note)) continue;
        printf("NOTE\t%llu\t%s\n",7+note_index,note);++note_index;
    }
    delete[] note;fclose(nf);
    FILE* lf=open_file(argv[1]);
    char* line=nullptr;size_t line_cap=0;unsigned long long ordinal=0;
    while(next_line(lf,line,line_cap)) {
        if(blank_or_comment(line)) continue;
        char* one=strchr(line,'|');
        if(!one) hold("line_must_have_three_fields");
        *one++=0;
        char* two=strchr(one,'|');
        if(!two) hold("line_must_have_three_fields");
        *two++=0;
        if(strchr(two,'|')||!safe_name(line)) hold("invalid_line_name_or_fields");
        const double slope=decimal(one), intercept=decimal(two);
        ++ordinal;
        emit_samples(argv[2],line,ordinal,notes,slope,intercept,(int)p);
    }
    delete[] line;fclose(lf);
    printf("END\t%llu\t%llu\t%ld\tRELEASE=HOLD\n",lines,6+lines+notes,p);
    if(ferror(stdout)) hold("output_error");
    return 0;
}
