// SPDX-License-Identifier: 0BSD
// Proposed geometric NINZA witness is scoped to the pinned AC fold.
#include "../shared/sigma_form_area_q62.hpp"
#include "../shared/right_triangle_f123_original.hpp"
#include "../shared/sigma_f_redaxualchoice.hpp"
#include "../shared/four_pyramid_reduxualchoice.hpp"
#include "../shared/u_circle_fit.hpp"
#include "../shared/xdr_rdx_fold_tork.hpp"
#include "../shared/minza_direct_ping_bidi.hpp"
#include "../shared/minza_direct_ping_tridi.hpp"
#include "../shared/three_geometric_folds.hpp"
#include "../shared/minza_topology_incidence.hpp"

namespace topology_design {
namespace grid = swe::u_circle_fit;
namespace fold = swe::minza_direct_ping_bidi;
namespace tork = swe::xdr_rdx_fold_tork;
using u64 = unsigned long long;

// A proposed geometric NINZA adapter, not the existing byte Ninza type.
struct NinzaFoldWitness {
    fold::DirectFold geometry;
    tork::Envelope head;
    tork::Envelope tail;
};
constexpr NinzaFoldWitness make_witness() noexcept {
    const auto f = fold::MINZA_TO_PING();
    return {f, tork::HEAD(f.ad_folded), tork::TAIL(f.cd_folded)};
}
constexpr bool ping(const NinzaFoldWitness& n) noexcept {
    return fold::PING(n.geometry) &&
        n.head.original.value == n.geometry.ad_folded &&
        n.tail.original.value == n.geometry.cd_folded &&
        tork::PING_HEAD(n.head) && tork::PING_TAIL(n.tail);
}
constexpr auto ninza = make_witness();
static_assert(ping(ninza));
static_assert(grid::SUM_NINE_DOUBLE_AREA() == 7200ULL);
static_assert(fold::FOLD.ad_folded == 169ULL * 400ULL);
static_assert(fold::FOLD.cd_folded == 169ULL * 900ULL);

// The source low-level API returns its input for invalid stages.
// This proposed public adapter rejects them and preserves the output.
constexpr bool stage_word(u64 value, unsigned stage, bool inverse,
                          u64& output) noexcept {
    if (stage < 6u || stage > 9u) return false;
    output = inverse ? tork::RDX(value, stage) : tork::XDR(value, stage);
    return true;
}
constexpr bool wildcard_roundtrips() noexcept {
    constexpr u64 values[] = {0ULL, ~0ULL, 1ULL << 63u,
        0x0123456789ABCDEFULL, tork::AD_FOLDED, tork::CD_FOLDED};
    for (unsigned stage = 6u; stage <= 9u; ++stage)
        for (u64 value : values) {
            if (tork::RDX(tork::XDR(value, stage), stage) != value ||
                tork::XDR(tork::RDX(value, stage), stage) != value)
                return false;
        }
    u64 untouched = 17ULL;
    return !stage_word(4ULL, 5u, false, untouched) && untouched == 17ULL &&
        !stage_word(4ULL, 10u, true, untouched) && untouched == 17ULL;
}
constexpr bool rejects_mutations() noexcept {
    auto geometry = ninza;
    geometry.geometry.ad_folded ^= 1ULL;
    auto payload = ninza;
    payload.head.encoded ^= 1ULL;
    auto tag = ninza;
    tag.tail.tag ^= 1ULL;
    auto role = ninza;
    role.head.original.source = tork::Source::CD;
    auto equalized = ninza;
    equalized.geometry.cd_folded = equalized.geometry.ad_folded;
    return !ping(geometry) && !ping(payload) && !ping(tag) &&
        !ping(role) && !ping(equalized) &&
        !tork::PING_HEAD(ninza.tail) && !tork::PING_TAIL(ninza.head);
}
static_assert(wildcard_roundtrips());
static_assert(rejects_mutations());
static_assert(tork::EQUAL(tork::BIDI(tork::BIDI(tork::INPUT_PAIR)),
                          tork::INPUT_PAIR));
static_assert(swe::minza_direct_ping_tridi::REPORT.valid);
static_assert(swe::three_geometric_folds::WITNESS.valid);
static_assert(!swe::three_geometric_folds::SEALED_SEAM_VERIFIED);
}

int main() {
    if (!topology_design::wildcard_roundtrips()) return 1;
    if (!topology_design::rejects_mutations()) return 2;
    if (!topology_design::ping(topology_design::make_witness())) return 3;
    if (!swe::minza_topology::base9_profile(swe::u_circle_fit::BASE9) ||
        !swe::minza_topology::negative_witnesses()) return 4;
    // Exhaust the source's finite accepted round-count domain at runtime.
    for (unsigned rounds = 1u; rounds <= 4096u; ++rounds)
        if (!swe::xdr_rdx_fold_tork::PING_STRESS(rounds)) return 5;
    if (swe::xdr_rdx_fold_tork::PING_STRESS(0u) ||
        swe::xdr_rdx_fold_tork::PING_STRESS(4097u)) return 6;
    return 0;
}
