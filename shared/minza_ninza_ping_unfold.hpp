/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Experimental read-only NINZA unforge and PING untwirl, C++20.
// No authentication, restoration of missing bytes, or source mutation.
#ifndef SWE_MINZA_NINZA_PING_UNFOLD_HPP
#define SWE_MINZA_NINZA_PING_UNFOLD_HPP
#include "sigma_penta_ping.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
namespace swe::sigma_penta_ping::unfold {
inline constexpr char protocol[] = "NINZA-PING-UNFORGE-UNTWIRL-v1";
struct Unforged {
    std::array<std::array<std::uint8_t, stripe_bytes>, loops> primary{};
    std::array<std::array<std::uint8_t, stripe_bytes>, loops> mirrored{};
    std::array<std::array<std::uint8_t, stripe_bytes>, loops> xor_difference{};
    unsigned mismatched_bytes{};
    bool copies_match{};
};
[[nodiscard]] constexpr Unforged unforge(const Ninza& source) noexcept {
    Unforged result{};
    result.primary=source.primary;
    result.mirrored=source.mirrored;
    for (unsigned n=0;n<loops;++n)
        for (unsigned c=0;c<stripe_bytes;++c) {
            result.xor_difference[n][c]=static_cast<std::uint8_t>(
                result.primary[n][c] ^ result.mirrored[n][c]);
            if (result.xor_difference[n][c]!=0u) ++result.mismatched_bytes;
        }
    result.copies_match=(result.mismatched_bytes==0u);
    return result;
}
struct Untwirled {
    std::array<std::array<std::uint8_t, stripe_bytes>,loops> restored{};
    std::array<std::array<unsigned,loops>,stripe_bytes> lane_by_column_position{};
    std::array<unsigned,ping_bytes> original_flat_offset{};
    unsigned mismatched_vs_primary{};
    unsigned mismatched_vs_cached_restored{};
    bool one_to_one_mapping{};
    bool roundtrip_vs_primary{};
};
[[nodiscard]] constexpr Untwirled untwirl(const Ping& braid,
                                           const Ninza& reference) noexcept {
    Untwirled result{};
    std::array<bool,ping_bytes> visited{};
    bool collision=false;
    for(unsigned col=0;col<stripe_bytes;++col)
        for(unsigned position=0;position<loops;++position) {
            const unsigned source_lane=lane(col,position);
            const unsigned flat=source_lane*stripe_bytes+col;
            const unsigned braided=col*loops+position;
            result.lane_by_column_position[col][position]=source_lane;
            result.original_flat_offset[braided]=flat;
            if (visited[flat]) collision=true;
            visited[flat]=true;
            result.restored[source_lane][col]=braid.braid[braided];
        }
    bool complete=true;
    for(bool seen:visited) complete=complete&&seen;
    result.one_to_one_mapping=!collision&&complete;
    for(unsigned n=0;n<loops;++n)
        for(unsigned c=0;c<stripe_bytes;++c) {
            const auto reconstructed=result.restored[n][c];
            if(reconstructed!=reference.primary[n][c])
                ++result.mismatched_vs_primary;
            if(reconstructed!=braid.restored[n*stripe_bytes+c])
                ++result.mismatched_vs_cached_restored;
        }
    result.roundtrip_vs_primary=result.one_to_one_mapping&&
                                 result.mismatched_vs_primary==0u;
    return result;
}
static_assert(lane(0,0)==0u && lane(1,0)==7u);
static_assert(lane(1,8)==8u && lane(5,8)==4u);
static_assert(ping_bytes==54u && loops*stripe_bytes==54u);
} // namespace swe::sigma_penta_ping::unfold
#endif // SWE_MINZA_NINZA_PING_UNFOLD_HPP
