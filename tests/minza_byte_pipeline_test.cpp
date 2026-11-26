// SPDX-License-Identifier: 0BSD
// Separate host-side test: this source intentionally uses the existing STL API.
#include "../shared/minza_ninza_ping_unfold.hpp"

namespace pipeline = swe::sigma_penta_ping;
namespace unfold = swe::sigma_penta_ping::unfold;

int main() {
    pipeline::Input input{};
    for (unsigned lane = 0; lane < pipeline::loops; ++lane)
        for (unsigned field = 0; field < pipeline::fields; ++field)
            input.samples[lane].pixel[field] =
                static_cast<std::uint8_t>((lane * 29u + field * 47u) % 256u);
    pipeline::Report report{};
    if (!pipeline::execute(input, report)) return 1;
    const auto copies = unfold::unforge(report.ninza);
    const auto bytes = unfold::untwirl(report.ping, report.ninza);
    if (!copies.copies_match || !bytes.one_to_one_mapping ||
        !bytes.roundtrip_vs_primary || bytes.mismatched_vs_cached_restored)
        return 2;
    for (unsigned group = 0; group < 3u; ++group)
        for (unsigned depth = 0; depth < 3u; ++depth)
            if (report.minza[group].loop_indices[depth] != group * 3u + depth)
                return 3;
    auto mirror = report.ninza;
    mirror.mirrored[8][5] ^= 1u;
    if (unfold::unforge(mirror).mismatched_bytes != 1u) return 4;
    // Exhaust all 54 single-byte braid corruption positions.
    for (unsigned offset = 0; offset < pipeline::ping_bytes; ++offset) {
        auto bad = report.ping;
        bad.braid[offset] ^= 1u;
        const auto checked = unfold::untwirl(bad, report.ninza);
        if (checked.roundtrip_vs_primary ||
            checked.mismatched_vs_primary != 1u ||
            checked.mismatched_vs_cached_restored != 1u) return 5;
    }
    // Neither inspection modified its reference: the original still passes.
    if (!unfold::unforge(report.ninza).copies_match ||
        !unfold::untwirl(report.ping, report.ninza).roundtrip_vs_primary)
        return 6;
    return 0;
}
