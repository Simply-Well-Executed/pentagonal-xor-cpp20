/* SPDX-License-Identifier: 0BSD
 * BITFLYING-C-ABI-001: exactly the same header under C11 and ISO C++20.
 * Native C/C++ by-value owned data; no pointers to foreign memory, no heap.
 * 0..64-bit numerical payload; the 3072-bit C++ engine is a separate tier.
 * NOBIT=0 data bits; NOP=guarded identity. No speed or cryptographic claim.
 */
#ifndef SWE_BITFLYING_C_ABI_001_H
#define SWE_BITFLYING_C_ABI_001_H
#if defined(__cplusplus)
extern "C" {
static_assert(sizeof(unsigned long long)==8,"64-bit unsigned long long required");
#else
_Static_assert(sizeof(unsigned long long)==8,"64-bit unsigned long long required");
#endif

typedef unsigned long long bf_u64;
typedef unsigned int bf_u32;
typedef struct {
 bf_u64 value;
 bf_u32 width,tag,digest;
} bf_word;
typedef struct {
 bf_word result;
 int status;
} bf_result;
enum {
 BF_PASS=0, BF_REJECT_WIDTH=1, BF_REJECT_LOSS=2,
 BF_REJECT_GUARD=3, BF_REJECT_OVERFLOW=4,
 BF_REJECT_UNDERFLOW=5, BF_REJECT_PROVENANCE=6
};
static inline bf_u32 bf_mix(bf_u32 h,bf_u32 x) {
 return (h^x)*16777619u;
}
static inline bf_u32 bf_digest(bf_u64 value,bf_u32 width,bf_u32 tag){
 bf_u32 h=bf_mix(2166136261u,width);
 h=bf_mix(h,tag);
 h=bf_mix(h,(bf_u32)value);
 return bf_mix(h,(bf_u32)(value>>32u));
}
static inline int bf_fits(bf_u64 value,bf_u32 width){
 return width<=64u && (width==64u || (value>>width)==0ULL);
}
static inline bf_word bf_raw(bf_u64 value,bf_u32 width,bf_u32 tag){
 bf_word out={value,width,tag,bf_digest(value,width,tag)};
 return out;
}
static inline bf_result bf_fail(int status){
 bf_result r={ {0ULL,0u,0u,0u},status};
 return r;
}
static inline int bf_guarded(bf_word word){
 return bf_fits(word.value,word.width) &&
        word.digest==bf_digest(word.value,word.width,word.tag);
}
static inline bf_result bf_nbit(bf_u64 value,bf_u32 width,bf_u32 tag){
 if(width>64u)return bf_fail(BF_REJECT_WIDTH);
 if(!bf_fits(value,width))return bf_fail(BF_REJECT_LOSS);
 const bf_result r={bf_raw(value,width,tag),BF_PASS};
 return r;
}
static inline bf_result bf_nop(bf_word word){
 if(!bf_guarded(word))return bf_fail(BF_REJECT_GUARD);
 const bf_result r={word,BF_PASS};
 return r;
}
static inline bf_result bf_link(bf_word source,bf_u32 destination_bits){
 if(!bf_guarded(source))return bf_fail(BF_REJECT_GUARD);
 return bf_nbit(source.value,destination_bits,source.tag);
}
static inline int bf_equal(bf_word a,bf_word b){
 return bf_guarded(a)&&bf_guarded(b)&&
        a.value==b.value&&a.width==b.width&&a.tag==b.tag;
}
static inline int bf_compatible(bf_word a,bf_word b){
 return bf_guarded(a)&&bf_guarded(b)&&
        a.width==b.width&&a.tag==b.tag;
}
static inline bf_result bf_plus(bf_word a,bf_word b){
 if(!bf_guarded(a)||!bf_guarded(b))return bf_fail(BF_REJECT_GUARD);
 if(a.width!=b.width)return bf_fail(BF_REJECT_WIDTH);
 if(a.tag!=b.tag)return bf_fail(BF_REJECT_PROVENANCE);
 if(a.value>(~0ULL)-b.value)return bf_fail(BF_REJECT_OVERFLOW);
 return bf_nbit(a.value+b.value,a.width,a.tag);
}
static inline bf_result bf_minus(bf_word a,bf_word b){
 if(!bf_guarded(a)||!bf_guarded(b))return bf_fail(BF_REJECT_GUARD);
 if(a.width!=b.width)return bf_fail(BF_REJECT_WIDTH);
 if(a.tag!=b.tag)return bf_fail(BF_REJECT_PROVENANCE);
 if(a.value<b.value)return bf_fail(BF_REJECT_UNDERFLOW);
 return bf_nbit(a.value-b.value,a.width,a.tag);
}
static inline bf_result bf_bitand(bf_word a,bf_word b){
 if(!bf_guarded(a)||!bf_guarded(b))return bf_fail(BF_REJECT_GUARD);
 if(a.width!=b.width)return bf_fail(BF_REJECT_WIDTH);
 if(a.tag!=b.tag)return bf_fail(BF_REJECT_PROVENANCE);
 return bf_nbit(a.value&b.value,a.width,a.tag);
}
static inline bf_result bf_complement(bf_word a){
 if(!bf_guarded(a))return bf_fail(BF_REJECT_GUARD);
 const bf_u64 mask=(a.width==64u)?~0ULL:
   ((a.width==0u)?0ULL:((1ULL<<a.width)-1ULL));
 return bf_nbit((~a.value)&mask,a.width,a.tag);
}
/* "~=" is not a C/C++ operator: a named NONSTANDARD bitwise test. */
static inline int bf_tilde_equal_experimental(bf_word a,bf_word b){
 const bf_result inverse=bf_complement(b);
 return inverse.status==BF_PASS&&bf_equal(a,inverse.result);
}
#if defined(__cplusplus)
}
#endif
#endif
