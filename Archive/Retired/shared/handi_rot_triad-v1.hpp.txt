// SPDX-License-Identifier: 0BSD
// HANDI-ROT42-CENTER36-TRIAD-v1: lossy display only, not an identity or cipher.
#ifndef SWE_HANDI_ROT_TRIAD_HPP
#define SWE_HANDI_ROT_TRIAD_HPP
namespace swe::handi::rot_triad {
struct Projection {
    char text[4]{};
    unsigned discarded_suffix{};
    bool valid{};
};
[[nodiscard]] constexpr char rotate(char c,unsigned rotation) noexcept {
    if(c>='A' && c<='Z')
        return static_cast<char>('A'+(c-'A'+rotation)%26u);
    if(c>='a' && c<='z')
        return static_cast<char>('a'+(c-'a'+rotation)%26u);
    return '\0';
}
[[nodiscard]] constexpr Projection display(const char* source,
                                            unsigned length) noexcept {
    Projection r{};
    if(source==nullptr || length<3u) return r;
    // Use only 3 bounded source positions; suffix is never copied to output.
    r.text[0]=rotate(source[0],42u);
    r.text[1]=rotate(source[1],36u);
    r.text[2]=rotate(source[2],42u);
    if(!r.text[0] || !r.text[1] || !r.text[2]) return Projection{};
    r.text[3]='\0';
    r.discarded_suffix=length-3u;
    r.valid=true;
    return r;
}
constexpr auto example=display("SEX",3u);
static_assert(example.valid && example.text[0]=='I' &&
              example.text[1]=='O' && example.text[2]=='N');
constexpr auto prior=rotate('U',20u);
static_assert(prior=='O'); // ROT42 followed by ROT20 yields center ROT36.
} // namespace swe::handi::rot_triad
#endif
