// SPDX-License-Identifier: 0BSD
#include "../shared/sigmadi_collision12.hpp"
#include <iostream>
#include <string_view>
using namespace swe::collision12;

constexpr Coordinate base{{
    "Simply-Well-Executed", "feature/vista-three-pass-9x9",
    "docs/SIGMA_PENTA_HAN_VISTA_BOARD.svg.js", "board",
    "build_board", "r", "result_handle", "svg_generation",
    "source-event:0", "board->commit", "v1", "declaration:0"
}};
constexpr Coordinate with(Axis axis, std::string_view value) {
    Coordinate c=base;
    c.axis[static_cast<unsigned>(axis)]=value;
    return c;
}
static_assert(valid(base));
static_assert(compare(base,base)==Relation::same_declaration);
static_assert(compare(base,with(Axis::declaration_instance,"declaration:1"))
              ==Relation::duplicate_binding);
static_assert(compare(base,with(Axis::lexical_scope,"nested_callback"))
              ==Relation::scoped_reuse);
static_assert(compare(base,with(Axis::revision,"v2"))
              ==Relation::changed_revision);
static_assert(compare(base,with(Axis::declared_type,"different_type"))
              ==Relation::inconsistent_declaration);
static_assert(compare(base,with(Axis::exact_symbol,"r2"))
              ==Relation::distinct);
static_assert(blocking(Relation::duplicate_binding));
static_assert(!blocking(Relation::scoped_reuse));
static_assert(dimension_count==12u);

int main() {
    Ledger<4> original{};
    const auto first=original.append(base);
    if(first.outcome!=Outcome::recorded || original.size!=1u) return 1;
    const auto repeat=original.append(base);
    if(repeat.outcome!=Outcome::repeated || original.size!=1u) return 2;
    const auto collision=original.append(
        with(Axis::declaration_instance,"declaration:1"));
    if(collision.outcome!=Outcome::blocked_collision ||
       collision.relation!=Relation::duplicate_binding ||
       original.size!=1u || collision.retained) return 3;
    const auto inconsistent=original.append(
        with(Axis::semantic_role,"unrelated_output"));
    if(inconsistent.outcome!=Outcome::blocked_collision ||
       inconsistent.relation!=Relation::inconsistent_declaration ||
       original.size!=1u) return 4;
    const auto scoped=original.append(with(Axis::lexical_scope,"inner_scope"));
    if(scoped.outcome!=Outcome::recorded_scoped || original.size!=2u) return 5;
    const auto revised=original.append(with(Axis::revision,"v2"));
    if(revised.outcome!=Outcome::recorded_revision || original.size!=3u) return 6;
    const auto distinct=original.append(with(Axis::exact_symbol,"s"));
    if(distinct.outcome!=Outcome::recorded || original.size!=4u) return 7;
    const auto overflow=original.append(with(Axis::exact_symbol,"t"));
    if(overflow.outcome!=Outcome::capacity_exceeded || original.size!=4u) return 8;
    auto invalid=base;
    invalid.axis[11]={};
    if(original.append(invalid).outcome!=Outcome::invalid ||
       original.size!=4u) return 9;

    // Visually similar Unicode symbols are NOT auto-collapsed.
    // Arabic آستر (U+0622 U+0633 U+062A U+0631) and ASCII "Aster"
    // remain different byte sequences and independent identities.
    const auto arabic=with(Axis::exact_symbol,
        "\xD8\xA2\xD8\xB3\xD8\xAA\xD8\xB1");
    const auto ascii=with(Axis::exact_symbol,"Aster");
    if(compare(arabic,ascii)!=Relation::distinct) return 10;
    if(compare(base,arabic)!=Relation::distinct) return 11;
    const auto width_change=with(Axis::exact_symbol,"R");
    if(compare(base,width_change)!=Relation::distinct) return 12;

    // Inspect is strictly observational: only append mutates the local
    // ledger on an explicitly admitted, non-colliding insertion.
    const std::size_t count=original.size;
    const auto receipt=original.inspect(
        with(Axis::declaration_instance,"declaration:8"));
    if(receipt.outcome!=Outcome::blocked_collision || original.size!=count)
        return 13;
    std::cout << "PASS|SIGMADI-COLLISION-12D-v1|axes=12|D11=declaration_instance"
              << "|duplicate_binding=blocked|same_scope_preserved=1"
              << "|scoped_reuse=allowed|unicode_exact=1|no_mutation_on_failure=1\n";
    return 0;
}
