// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek.
// Read-only MINZA configuration enumeration; no video decoding or decryption.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
namespace swe::minza {
enum class ColorMode : std::uint8_t { mono, gray, c16, full16, fullgray };
enum class DitherMode : std::uint8_t { none, ordered2, ordered4, ordered8, random, fstein, pentxor };
enum class Mirror : std::uint8_t { original, reverse_bytes, reverse_bits, reverse_bytes_and_bits };
enum class Rotation : std::uint8_t { none, rot16_forward, rot16_backward,
  ebc16_le_plus16_provisional, ebc16_le_minus16_provisional,
  ebc16_be_plus16_provisional, ebc16_be_minus16_provisional };
enum class Scope : std::uint8_t { payload_only, entire_wrapped_file };
enum class Wrapper : std::uint8_t { xfe_efx_literal, quicktime_ftyp_mdat_control };
struct RenderConfig {
  bool use_native_yuv, use_chroma_metrics, use_short_ramp, use_pent_xor_fx;
  ColorMode color;
  DitherMode dither;
};
struct ContainerConfig { Mirror mirror; Rotation rotation; Scope scope; Wrapper wrapper; };
inline constexpr std::array<std::string_view,5> color_names{
 "mono", "gray", "16", "full16", "fullgray"};
inline constexpr std::array<std::string_view,7> dither_names{
 "none", "ordered2", "ordered4", "ordered8", "random", "fstein", "pentxor"};
inline constexpr std::array<std::string_view,4> mirror_names{
 "original", "reverse_bytes", "reverse_bits", "reverse_bytes_and_bits"};
inline constexpr std::array<std::string_view,7> rotation_names{
 "none", "rot16_forward", "rot16_backward",
 "ebc16_le_plus16_provisional", "ebc16_le_minus16_provisional",
 "ebc16_be_plus16_provisional", "ebc16_be_minus16_provisional"};
inline constexpr std::array<std::string_view,2> scope_names{
 "payload_only", "entire_wrapped_file"};
inline constexpr std::array<std::string_view,2> wrapper_names{
 "XFE_EFX_LITERAL", "QUICKTIME_FTYP_MDAT_CONTROL"};
inline constexpr std::size_t renderer_cases=16u*5u*7u;
inline constexpr std::size_t container_cases=4u*7u*2u*2u;
[[nodiscard]] constexpr auto all_renderer_cases() {
 std::array<RenderConfig,renderer_cases> a{};
 std::size_t i=0;
 for(unsigned flags=0;flags<16;flags++)
  for(unsigned color=0;color<5;color++)
   for(unsigned dither=0;dither<7;dither++)
    a[i++]={bool(flags&1u),bool(flags&2u),bool(flags&4u),bool(flags&8u),
            static_cast<ColorMode>(color),static_cast<DitherMode>(dither)};
 return a;
}
[[nodiscard]] constexpr auto all_container_cases() {
 std::array<ContainerConfig,container_cases> a{};
 std::size_t i=0;
 for(unsigned mirror=0;mirror<4;mirror++)
  for(unsigned rotation=0;rotation<7;rotation++)
   for(unsigned scope=0;scope<2;scope++)
    for(unsigned wrapper=0;wrapper<2;wrapper++)
     a[i++]={static_cast<Mirror>(mirror),static_cast<Rotation>(rotation),
             static_cast<Scope>(scope),static_cast<Wrapper>(wrapper)};
 return a;
}
inline constexpr auto renderer_matrix=all_renderer_cases();
inline constexpr auto container_matrix=all_container_cases();
[[nodiscard]] constexpr std::uint8_t rot16_letter(std::uint8_t c, bool forward){
 unsigned shift=forward?16u:10u;
 if(c>='A'&&c<='Z')return std::uint8_t('A'+(c-'A'+shift)%26u);
 if(c>='a'&&c<='z')return std::uint8_t('a'+(c-'a'+shift)%26u);
 return c;
}
[[nodiscard]] constexpr std::uint16_t trial_word_shift(std::uint16_t word,bool forward){
 return std::uint16_t(forward?unsigned(word)+16u:unsigned(word)-16u);
}
[[nodiscard]] constexpr std::uint8_t reverse_bits(std::uint8_t b){
 b=std::uint8_t((b>>4u)|(b<<4u));
 b=std::uint8_t(((b&0xCCu)>>2u)|((b&0x33u)<<2u));
 return std::uint8_t(((b&0xAAu)>>1u)|((b&0x55u)<<1u));
}
[[nodiscard]] consteval bool roundtrip_ok() {
 for(unsigned c=0;c<256;c++){
  auto b=std::uint8_t(c);
  if(rot16_letter(rot16_letter(b,true),false)!=b)return false;
  if(reverse_bits(reverse_bits(b))!=b)return false;
 }
 for(unsigned w=0;w<65536;w++){
  auto b=std::uint16_t(w);
  if(trial_word_shift(trial_word_shift(b,true),false)!=b)return false;
 }
 return true;
}
static_assert(renderer_matrix.size()==560);
static_assert(container_matrix.size()==112);
static_assert(roundtrip_ok());
} // namespace swe::minza
