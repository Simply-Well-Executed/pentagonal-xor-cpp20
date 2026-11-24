// SPDX-License-Identifier: 0BSD
// SLIDINGTOOLONGA-001 | Exact decimal seed at x=0, any finite exponent text.
// Derived from DIAERY UNBOUND base-1e9 dynamic limbs, no STL/floating point.
// Full arbitrary-precision POLYNOMIAL SAMPLING is a separate qualification.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
static void hold(const char* reason){fprintf(stderr,"SLIDINGTOOLONGA|HOLD|%s\n",reason);exit(1);}
static constexpr uint64_t BASE=1000000000ULL;
struct Big {
 uint32_t* v=nullptr;size_t n=0,cap=0;
 Big()=default;Big(const Big&)=delete;Big& operator=(const Big&)=delete;
 ~Big(){free(v);}
};
static void reserve(Big& a,size_t n){
 if(n<=a.cap)return;
 if(n>SIZE_MAX/sizeof(uint32_t))hold("limb_overflow");
 size_t cap=a.cap?a.cap:4;
 while(cap<n){if(cap>SIZE_MAX/2){cap=n;break;}cap*=2;}
 void* p=realloc(a.v,cap*sizeof(uint32_t));
 if(!p)hold("limb_allocation");
 a.v=(uint32_t*)p;a.cap=cap;
}
static void seed(Big& a,uint32_t v){reserve(a,1);a.n=1;a.v[0]=v;}
static bool zero(const Big& a){return a.n==1&&a.v[0]==0;}
static void mul_small(Big& a,uint32_t m){
 if(zero(a)||m==0){seed(a,0);return;}
 if(a.n>SIZE_MAX-2)hold("capacity_overflow");
 reserve(a,a.n+2);
 uint64_t carry=0;
 for(size_t i=0;i<a.n;++i){
  const uint64_t v=(uint64_t)a.v[i]*m+carry;
  a.v[i]=(uint32_t)(v%BASE);carry=v/BASE;
 }
 while(carry){a.v[a.n++]=(uint32_t)(carry%BASE);carry/=BASE;}
}
static void add_digit(Big& a,uint32_t d){
 uint64_t carry=d;
 for(size_t i=0;i<a.n&&carry;++i){
  uint64_t v=(uint64_t)a.v[i]+carry;
  a.v[i]=(uint32_t)(v%BASE);carry=v/BASE;
 }
 if(carry){reserve(a,a.n+1);a.v[a.n++]=(uint32_t)carry;}
}
static void mult(Big& a,const Big& b){
 if(zero(a)||zero(b)){seed(a,0);return;}
 if(a.n>SIZE_MAX-b.n-1)hold("product_capacity");
 Big r;reserve(r,a.n+b.n+1);r.n=a.n+b.n+1;
 memset(r.v,0,r.n*sizeof(uint32_t));
 for(size_t i=0;i<a.n;++i){
  uint64_t carry=0;
  for(size_t j=0;j<b.n;++j){
   const size_t k=i+j;
   const uint64_t v=(uint64_t)a.v[i]*b.v[j]+r.v[k]+carry;
   r.v[k]=(uint32_t)(v%BASE);carry=v/BASE;
  }
  size_t k=i+b.n;
  while(carry){
   if(k>=r.n)hold("product_carry");
   const uint64_t v=(uint64_t)r.v[k]+carry;
   r.v[k]=(uint32_t)(v%BASE);carry=v/BASE;++k;
  }
 }
 while(r.n>1&&r.v[r.n-1]==0)--r.n;
 free(a.v);a.v=r.v;a.n=r.n;a.cap=r.cap;
 r.v=nullptr;r.n=0;r.cap=0;
}
static void clone(Big& a,const Big& b){
 reserve(a,b.n);a.n=b.n;
 memcpy(a.v,b.v,b.n*sizeof(uint32_t));
}
static size_t digit_count(const Big& a){
 if(a.n==0)hold("uninitialized_magnitude");
 size_t digits=(a.n-1)*9;
 uint32_t top=a.v[a.n-1];
 do{++digits;top/=10;}while(top);
 return digits;
}
static void print_big(const Big& a){
 printf("%u",a.v[a.n-1]);
 for(size_t i=a.n-1;i>0;--i)printf("%09u",a.v[i-1]);
}
static char* read_line(FILE* f,size_t& cap){
 size_t n=0;char* data=nullptr;
 int c=0;
 while((c=fgetc(f))!=EOF&&c!='\n'){
  if(c==0||c>127)hold("non_ascii");
  if(n+1>=cap){
   size_t next=cap?cap*2:128;
   if(next<=cap)hold("line_buffer_overflow");
   char* fresh=(char*)realloc(data,next);
   if(!fresh)hold("line_buffer_allocation");
   data=fresh;cap=next;
  }
  data[n++]=(char)c;
 }
 if(!n&&c==EOF){free(data);return nullptr;}
 if(n&&data[n-1]=='\r')--n;
 if(!data){data=(char*)malloc(1);if(!data)hold("line_empty_allocation");}
 data[n]=0;return data;
}
static bool safe_name(const char* s){
 if(!s||!*s)return false;
 for(const unsigned char* p=(const unsigned char*)s;*p;++p)
  if(!((*p>='A'&&*p<='Z')||(*p>='a'&&*p<='z')||
       (*p>='0'&&*p<='9')||*p=='_'||*p=='-'||*p=='.'))return false;
 return true;
}
struct Decimal {
 Big mantissa;size_t scale=0;int sign=0;
};
static void decimal(Decimal& d,const char* s){
 if(!s||!*s)hold("empty_decimal");
 int sign=1;if(*s=='-'){sign=-1;++s;}else if(*s=='+')++s;
 if(!*s)hold("empty_digits");
 seed(d.mantissa,0);bool seen=false,dot=false;
 for(;*s;++s){
  if(*s=='.'&&!dot){dot=true;continue;}
  if(*s<'0'||*s>'9')hold("bad_decimal");
  seen=true;mul_small(d.mantissa,10);add_digit(d.mantissa,(uint32_t)(*s-'0'));
  if(dot){if(d.scale==SIZE_MAX)hold("decimal_scale_overflow");++d.scale;}
 }
 if(!seen)hold("missing_decimal_digits");
 d.sign=zero(d.mantissa)?0:sign;
}
static bool any_nonzero(const char* digits){
 for(const char* p=digits;*p;++p)if(*p!='0')return true;
 return false;
}
static bool exponent_odd(const char* digits){
 size_t n=strlen(digits);return n && ((digits[n-1]-'0')&1);
}
static void half(char* digits){
 size_t n=strlen(digits);int rem=0;
 for(size_t i=0;i<n;++i){
  const int v=rem*10+(digits[i]-'0');
  digits[i]=(char)('0'+v/2);rem=v%2;
 }
}
static char* exponent_text(const char* input){
 if(!input||!*input)hold("empty_exponent");
 const size_t n=strlen(input);
 char* copy=(char*)malloc(n+1);
 if(!copy)hold("exponent_allocation");
 for(size_t i=0;i<n;++i){
  if(input[i]<'0'||input[i]>'9')hold("exponent_not_nonnegative_integer");
  copy[i]=input[i];
 }
 copy[n]=0;return copy;
}
static void power(const Decimal& d,const char* exp,Big& out,size_t& scale,int& sign){
 char* e=exponent_text(exp);
 sign=any_nonzero(e)?d.sign:1; // zero^0 is declared 1
 if(sign==0){seed(out,0);scale=0;free(e);return;}
 if(d.sign<0&&exponent_odd(e))sign=-1;
 else sign=1;
 Big base;clone(base,d.mantissa);seed(out,1);
 scale=0;size_t baseScale=d.scale;
 while(any_nonzero(e)){
  if(exponent_odd(e)){
   mult(out,base);
   if(scale>SIZE_MAX-baseScale)hold("result_scale_overflow");
   scale+=baseScale;
  }
  half(e);
  if(any_nonzero(e)){
   mult(base,base);
   if(baseScale>SIZE_MAX/2)hold("base_scale_overflow");
   baseScale*=2;
  }
 }
 free(e);
}
int main(int argc,char** argv){
 if(argc!=2&&argc!=3)hold("usage_lines_optional_exponent");
 const char* exponent=argc==3?argv[2]:"0";
 char* check=exponent_text(exponent);free(check);
 FILE* f=nullptr;
 if(fopen_s(&f,argv[1],"rb")!=0||!f)hold("lines_not_found");
 printf("META\tSLIDINGTOOLONGA-001\tPOWER=%s\tX=0\tEXACT_DECIMAL\n",exponent);
 unsigned long long count=0;
 for(;;){
  size_t cap=0;char* line=read_line(f,cap);
  if(!line)break;
  if(!line[0]||line[0]=='#'){free(line);continue;}
  char* slope=strchr(line,'|');
  if(!slope)hold("missing_fields");
  *slope++=0;char* intercept=strchr(slope,'|');
  if(!intercept)hold("missing_intercept");
  *intercept++=0;
  if(strchr(intercept,'|')||!safe_name(line))hold("unsafe_identity_or_fields");
  Decimal m,b;
  decimal(m,slope);decimal(b,intercept);
  Big out;size_t scale=0;int sign=0;
  power(b,exponent,out,scale,sign);
  if(count==ULLONG_MAX)hold("source_count_overflow");
  printf("SOURCE\t%llu\t%s\t%s\t%s\t%d\t%zu\t%zu\t",
    ++count,line,slope,intercept,sign,scale,digit_count(out));
  print_big(out);putchar('\n');free(line);
 }
 fclose(f);
 if(!count)hold("no_source_lines");
 printf("END\t%llu\tRELEASE=HOLD\n",count);
 if(ferror(stdout))hold("write_failure");
 return 0;
}
