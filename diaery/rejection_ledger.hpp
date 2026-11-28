/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Rejected manual selections are retained as provenance, not FREQ samples.
// Dynamic growth is supplied by the host; failures are explicit evidence gaps.
#ifndef SWE_DIAERY_REJECTION_LEDGER_HPP
#define SWE_DIAERY_REJECTION_LEDGER_HPP
namespace swe::diaery::reject {
using U64=unsigned long long;
enum class Reason : unsigned {
 Filtrex=1u, InvalidFrequencyDomain=2u, Memory=3u
};
struct Event {
 double x_inches,y_inches,z_inches;
 Reason reason;
};
struct Ledger {
 Event* events;
 U64 count,capacity,lost;
};
enum class Status { Recorded, CapacityRequired, Overflow };
[[nodiscard]] constexpr Status append(Ledger& l,Event e) noexcept {
 if(l.count==~0ULL)return Status::Overflow;
 if(!l.events||l.count>=l.capacity)return Status::CapacityRequired;
 l.events[l.count++]=e;
 return Status::Recorded;
}
[[nodiscard]] constexpr bool complete(const Ledger& l) noexcept {
 return l.lost==0ULL && l.count<=l.capacity && (!l.count||l.events);
}
} // namespace swe::diaery::reject
#endif
