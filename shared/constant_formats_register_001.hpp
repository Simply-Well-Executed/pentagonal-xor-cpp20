// SPDX-License-Identifier: 0BSD
// Source-backed renderings of existing canonical constants, NOT new authorities.
// Include the three named headers below before this include-free C++20 header.
#ifndef SWE_CONSTANT_FORMATS_REGISTER_001_HPP
#define SWE_CONSTANT_FORMATS_REGISTER_001_HPP
#ifndef SWE_TOOLONG_Q62_CORE_HPP
#error "include toolong_q62_core.hpp first"
#endif
#ifndef SWE_EULERS_JIG_CANONICAL_HPP
#error "include eulers_jig_canonical.hpp first"
#endif
#ifndef SWE_SLIDINGTOOLONG_HPP
#error "include slidingtoolong.hpp first"
#endif

namespace swe::constant_formats_register_001 {
using u64=unsigned long long;
using u32=unsigned int;
static_assert(sizeof(u64)==8&&sizeof(u32)==4&&sizeof(long)>=4);
struct Record {const char* name;u64 raw;unsigned decimal_scale;};
inline constexpr Record RECORDS[]{
 {"EULERS_JIG",swe::eulers_jig::ONE_TRIANGLE_X1E17,17u},
 {"EULERS_JIG_TRIANGLE_TOTAL",swe::eulers_jig::TOTAL_X1E12,12u},
 {"EULERS_JIG_TRIANGLE_COUNT",swe::eulers_jig::TRIANGLES,0u},
 {"SCALE_X1E17",swe::eulers_jig::SCALE_X1E17,0u},
 {"Q62_UNIT",1ULL<<swe::toolong_q62::shift,0u},
 {"Q62_SHIFT",swe::toolong_q62::shift,0u},
 {"EULER_SOURCE_Q62",swe::toolong_q62::euler.source_word,0u},
 {"PI_SOURCE_Q62",swe::toolong_q62::pi.source_word,0u},
 {"INTENSITY_SOURCE_Q62",swe::toolong_q62::intensity.source_word,0u},
 {"EULER_RESULT_X1E17",swe::toolong_q62::euler.expected_integer,17u},
 {"PI_RESULT_X1E17",swe::toolong_q62::pi.expected_integer,17u},
 {"INTENSITY_RESULT_X1E17",swe::toolong_q62::intensity.expected_integer,17u},
 {"Q64_FRACTION_ALL_ONES",swe::slidingtoolong::ONE_Q64_FRACTION,0u},
 {"MATH_E_Q64_FRACTION",swe::slidingtoolong::MATH_E.fraction,0u},
 {"MATH_PI_Q64_FRACTION",swe::slidingtoolong::MATH_PI.fraction,0u},
 {"INTENSITY_Q64_FRACTION",swe::slidingtoolong::INTENSITY_Q64.fraction,0u},
 {"SCALE_X1E18",swe::slidingtoolong::DECIMAL_SCALE_1E18,0u},
 {"MATH_E_RESULT_X1E18",swe::slidingtoolong::MATH_E_X1E18,18u},
 {"MATH_PI_RESULT_X1E18",swe::slidingtoolong::MATH_PI_X1E18,18u},
 {"INTENSITY_RESULT_X1E18",swe::slidingtoolong::INTENSITY_X1E18,18u},
 {"SIGMA_Q64_XOR_EXPLORATORY",swe::slidingtoolong::SIGMA_Q64_XOR,0u}
};
inline constexpr unsigned RECORD_COUNT=
 static_cast<unsigned>(sizeof(RECORDS)/sizeof(RECORDS[0]));
static_assert(RECORD_COUNT==21u);
inline constexpr u64 U32_MAX_VALUE=0xffffffffULL;
inline constexpr u64 SIGNED_LONG32_MAX=2147483647ULL;
inline constexpr u64 SIGNED_LONG64_MAX=9223372036854775807ULL;

// A true 32-bit destination exists only when its upper 32 bits are zero.
struct Word32Pair {u32 low,high;};
[[nodiscard]] constexpr Word32Pair as_two_32(u64 v) noexcept {
 return {static_cast<u32>(v&U32_MAX_VALUE),static_cast<u32>(v>>32u)};
}
[[nodiscard]] constexpr u64 from_two_32(Word32Pair w) noexcept {
 return (static_cast<u64>(w.high)<<32u)|w.low;
}
[[nodiscard]] constexpr bool fits_u32(u64 v) noexcept {
 return v<=U32_MAX_VALUE;
}
[[nodiscard]] constexpr bool fits_signed_long32(u64 v) noexcept {
 return v<=SIGNED_LONG32_MAX;
}
[[nodiscard]] constexpr bool fits_signed_long64(u64 v) noexcept {
 return v<=SIGNED_LONG64_MAX;
}
// Four signed long limbs in base 2^16 are legal on both LLP64 and LP64.
// This is a *multi-word* long representation, NOT a single long variable.
struct Long16x4 {long high3,high2,low1,low0;};
[[nodiscard]] constexpr Long16x4 as_four_long16(u64 v) noexcept {
 return {static_cast<long>((v>>48u)&65535ULL),
         static_cast<long>((v>>32u)&65535ULL),
         static_cast<long>((v>>16u)&65535ULL),
         static_cast<long>(v&65535ULL)};
}
[[nodiscard]] constexpr u64 from_four_long16(Long16x4 v) noexcept {
 return (static_cast<u64>(v.high3)<<48u)|
        (static_cast<u64>(v.high2)<<32u)|
        (static_cast<u64>(v.low1)<<16u)|
        static_cast<u64>(v.low0);
}
[[nodiscard]] constexpr bool legal_long16(Long16x4 w) noexcept {
 return w.high3>=0&&w.high3<=65535L&&w.high2>=0&&w.high2<=65535L&&
        w.low1>=0&&w.low1<=65535L&&w.low0>=0&&w.low0<=65535L;
}
[[nodiscard]] constexpr bool all_source_representations() noexcept {
 for(unsigned i=0;i<RECORD_COUNT;++i){
  const u64 v=RECORDS[i].raw;
  const Long16x4 word=as_four_long16(v);
  if(!legal_long16(word)||from_four_long16(word)!=v)return false;
  if(from_two_32(as_two_32(v))!=v)return false;
  if(fits_u32(v)!=(as_two_32(v).high==0u))return false;
 }
 return true;
}
static_assert(all_source_representations());
// Portable MSVC-compatible 128-bit intermediate / 64-bit result:
// use the existing four-limb Q62 source implementation, NOT a cast.
static_assert(swe::toolong_q62::multiply_shift62(
 swe::toolong_q62::euler.source_word)==swe::toolong_q62::euler.expected_integer);
static_assert(swe::toolong_q62::multiply_shift62(
 swe::toolong_q62::pi.source_word)==swe::toolong_q62::pi.expected_integer);
static_assert(swe::toolong_q62::multiply_shift62(
 swe::toolong_q62::intensity.source_word)==swe::toolong_q62::intensity.expected_integer);
static_assert(!fits_u32(swe::toolong_q62::intensity.expected_integer));
static_assert(fits_u32(swe::eulers_jig::TRIANGLES));
static_assert(swe::eulers_jig::ONE_TRIANGLE_X1E17==2864498081537971875ULL);

// This optional expression is a compiler-extension *alternative*.
// It is not the MSVC production path and is guarded by capability.
#if defined(__SIZEOF_INT128__)
using u128=unsigned __int128;
inline constexpr u64 optional_intensity_q62_128_to_64=
 static_cast<u64>((static_cast<u128>(
 swe::toolong_q62::intensity.source_word)*
 swe::toolong_q62::multiplier)>>swe::toolong_q62::shift);
static_assert(optional_intensity_q62_128_to_64==
 swe::toolong_q62::intensity.expected_integer);
#endif
} // namespace swe::constant_formats_register_001
#endif
