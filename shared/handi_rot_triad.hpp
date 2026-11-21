// SPDX-License-Identifier: 0BSD
// HANDI-ROT16-CENTER10-TRIAD-v2: minimal mod-26 ASCII display projection.
// Prior v1 is archived unchanged; no canonical identity changes.
#ifndef SWE_HANDI_ROT_TRIAD_HPP
#define SWE_HANDI_ROT_TRIAD_HPP
namespace swe::handi::rot_triad {
inline constexpr unsigned alphabet_size=26u;
inline constexpr unsigned outer_shift=16u;
inline constexpr unsigned center_shift=10u;
struct Projection {
    char text[4]{};
    unsigned discarded_suffix{};
    bool valid{};
};
[[nodiscard]] constexpr char rotate(char c,unsigned rotation) noexcept {
    // General callers still accept any number of turns; normalize once.
    // display() calls with already-normalized constants 16 and 10.
    const unsigned shift=rotation%alphabet_size;
    char base='\0';
    if(c>='A' && c<='Z') base='A';
    if(c>='a' && c<='z') base='a';
    if(!base) return '\0';
    const unsigned advanced=static_cast<unsigned>(c-base)+shift;
    return static_cast<char>(base+(advanced>=alphabet_size ?
                                   advanced-alphabet_size : advanced));
}
[[nodiscard]] constexpr Projection display(const char* source,
                                            unsigned length) noexcept {
    Projection r{};
    if(source==nullptr || length<3u) return r;
    // Read exactly three ASCII letters. No later source bytes are touched.
    r.text[0]=rotate(source[0],outer_shift);
    r.text[1]=rotate(source[1],center_shift);
    r.text[2]=rotate(source[2],outer_shift);
    if(!r.text[0] || !r.text[1] || !r.text[2]) return Projection{};
    r.text[3]='\0';
    r.discarded_suffix=length-3u;
    r.valid=true;
    return r;
}
static_assert(outer_shift<alphabet_size && center_shift<alphabet_size);
static_assert(rotate('S',outer_shift)=='I' &&
              rotate('E',center_shift)=='O' &&
              rotate('X',outer_shift)=='N');
constexpr auto example=display("SEX",3u);
static_assert(example.valid && example.text[0]=='I' &&
              example.text[1]=='O' && example.text[2]=='N');
} // namespace swe::handi::rot_triad
#endif
