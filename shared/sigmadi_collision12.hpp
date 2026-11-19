// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
//
// Experimental SIGMADI-COLLISION-12D-v1.
// "Dimension" means a provenance coordinate, NOT a physical dimension.
// Exactly zero-based D0..D11. D11 identifies the declaration instance.
// Read-only classification; no substitution, correction, aliasing or erasure.
#ifndef SWE_SIGMADI_COLLISION12_HPP
#define SWE_SIGMADI_COLLISION12_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace swe::collision12 {

inline constexpr std::string_view protocol =
    "SIGMADI-COLLISION-12D-v1";
inline constexpr std::size_t dimension_count = 12u;
enum class Axis : std::uint8_t {
    repository = 0, branch = 1, file = 2, name_space = 3,
    lexical_scope = 4, exact_symbol = 5, declared_type = 6,
    semantic_role = 7, source_provenance = 8, route = 9,
    revision = 10, declaration_instance = 11
};

inline constexpr std::array<std::string_view, dimension_count> axis_names {{
    "repository", "branch", "file", "namespace", "scope",
    "exact_symbol", "declared_type", "semantic_role",
    "source_provenance", "route", "revision", "declaration_instance"
}};

struct Coordinate {
    std::array<std::string_view, dimension_count> axis{};
    constexpr std::string_view get(Axis a) const noexcept {
        return axis[static_cast<unsigned>(a)];
    }
};

enum class Relation : std::uint8_t {
    distinct, same_declaration, scoped_reuse, duplicate_binding,
    inconsistent_declaration, changed_revision
};

constexpr bool equal(const Coordinate& x, const Coordinate& y) noexcept {
    for (std::size_t i=0; i<dimension_count; ++i)
        if (x.axis[i]!=y.axis[i]) return false;
    return true;
}

// Identity keys are source-language binding keys; exact UTF-8 bytes only.
// A source compiler (not this forensic adapter) decides what scope means.
// D0..D5 identify the binding; D11 is its explicit witness identity.
constexpr Relation compare(const Coordinate& a,
                           const Coordinate& b) noexcept {
    const bool common_location =
        a.get(Axis::repository)==b.get(Axis::repository) &&
        a.get(Axis::branch)==b.get(Axis::branch) &&
        a.get(Axis::file)==b.get(Axis::file) &&
        a.get(Axis::name_space)==b.get(Axis::name_space);
    if (!common_location) return Relation::distinct;
    const bool same_symbol =
        a.get(Axis::exact_symbol)==b.get(Axis::exact_symbol);
    if (!same_symbol) return Relation::distinct;
    if (a.get(Axis::lexical_scope)!=b.get(Axis::lexical_scope))
        return Relation::scoped_reuse;
    // A revision change is NOT silently comparable to the old revision;
    // old declarations remain independent historical witnesses.
    if (a.get(Axis::revision)!=b.get(Axis::revision))
        return Relation::changed_revision;
    if (a.get(Axis::declaration_instance)!=
        b.get(Axis::declaration_instance))
        return Relation::duplicate_binding;
    // One source declaration ID claims contradictory type/role/provenance
    // or route. That is evidence of a malformed/inconsistent witness.
    if (!equal(a,b)) return Relation::inconsistent_declaration;
    return Relation::same_declaration;
}
constexpr bool valid(const Coordinate& x) noexcept {
    for (const auto part:x.axis)
        if (part.empty()) return false;
    return true;
}
constexpr bool blocking(Relation r) noexcept {
    return r==Relation::duplicate_binding ||
           r==Relation::inconsistent_declaration;
}
constexpr std::string_view label(Relation r) noexcept {
    switch(r) {
    case Relation::distinct: return "DISTINCT";
    case Relation::same_declaration: return "SAME_DECLARATION";
    case Relation::scoped_reuse: return "SCOPED_REUSE";
    case Relation::duplicate_binding: return "DUPLICATE_BINDING";
    case Relation::inconsistent_declaration: return "INCONSISTENT_DECLARATION";
    case Relation::changed_revision: return "CHANGED_REVISION";
    }
    return "UNDEFINED";
}

enum class Outcome : std::uint8_t {
    recorded, repeated, recorded_scoped, recorded_revision,
    blocked_collision, invalid, capacity_exceeded
};
struct Receipt {
    Outcome outcome{Outcome::invalid};
    Relation relation{Relation::distinct};
    std::size_t matching_index{}; // 0-based; only meaningful when matched
    std::size_t recorded_count{};
    bool retained{};
};
// The store does not silently overwrite anything. Conflicting proposals
// are returned to the caller as receipts; callers retain the original
// evidence independently. String views borrow caller-owned storage.
// If long-term custody is needed, persist the bytes with separate ownership.
template<std::size_t Capacity>
struct Ledger {
    std::array<Coordinate, Capacity> records{};
    std::size_t size{};
    constexpr Receipt inspect(const Coordinate& candidate) const noexcept {
        if (!valid(candidate)) return {Outcome::invalid,Relation::distinct,0,size,false};
        bool scoped=false, revised=false;
        std::size_t scoped_index=0, revised_index=0;
        for (std::size_t i=0; i<size; ++i) {
            const Relation r=compare(records[i], candidate);
            if (blocking(r))
                return {Outcome::blocked_collision,r,i,size,false};
            if (r==Relation::same_declaration)
                return {Outcome::repeated,r,i,size,false};
            if (r==Relation::scoped_reuse && !scoped) {
                scoped=true; scoped_index=i;
            }
            if (r==Relation::changed_revision && !revised) {
                revised=true; revised_index=i;
            }
        }
        if(size==Capacity)
            return {Outcome::capacity_exceeded,Relation::distinct,0,size,false};
        if(scoped)
            return {Outcome::recorded_scoped,Relation::scoped_reuse,scoped_index,size+1,true};
        if(revised)
            return {Outcome::recorded_revision,Relation::changed_revision,revised_index,size+1,true};
        return {Outcome::recorded,Relation::distinct,0,size+1,true};
    }
    constexpr Receipt append(const Coordinate& candidate) noexcept {
        const Receipt receipt=inspect(candidate);
        if(receipt.retained) {
            records[size]=candidate;
            ++size;
        }
        return receipt;
    }
};

static_assert(dimension_count==12u);
static_assert(static_cast<unsigned>(Axis::declaration_instance)==11u);
static_assert(axis_names.size()==dimension_count);
} // namespace swe::collision12
#endif // SWE_SIGMADI_COLLISION12_HPP
