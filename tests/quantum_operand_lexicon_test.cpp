#include "../shared/quantum_operand_lexicon.hpp"

namespace q = swe::quantum_operands;

static_assert(q::record_count == 14u);
static_assert(q::lookup_exact("+\\", 2u)->status == q::Status::Undefined);
static_assert(q::lookup_exact("+/", 2u)->status == q::Status::Ambiguous);
static_assert(q::lookup_exact("+'", 2u)->status == q::Status::Ambiguous);
static_assert(q::lookup_exact("+;", 2u)->status == q::Status::Undefined);
static_assert(q::lookup_exact("+p", 2u)->status == q::Status::Conditional);
static_assert(q::lookup_exact("+P", 2u)->status == q::Status::Conditional);
static_assert(q::lookup_exact("++", 2u)->status == q::Status::InterpretedNoncanonical);
static_assert(q::lookup_exact("+-", 2u)->status == q::Status::InterpretedNoncanonical);
static_assert(q::lookup_exact("+[", 2u)->status == q::Status::Unbalanced);
static_assert(q::lookup_exact("+]", 2u)->status == q::Status::Unbalanced);
static_assert(q::lookup_exact("= ", 2u) == nullptr);
static_assert(q::lookup_exact(" =", 2u) == nullptr);
static_assert(q::lookup_exact(";=", 1u) == nullptr);
static_assert(q::lookup_exact(";=", 2u)->family == q::Family::Equals);
static_assert(q::lookup_exact(";=", 2u)->provenance ==
              q::Provenance::UserEqualsNovember2026);
static_assert(q::lookup_exact("+p", 2u)->provenance ==
              q::Provenance::CoreaoPlusStarNovember2026);
static_assert(!q::evaluable(*q::lookup_exact("++", 2u)));

int main() {
    for (unsigned i = 0u; i < q::record_count; ++i) {
        const auto& source = q::records[i];
        const auto* recovered =
            q::lookup_exact(source.spelling, source.length);
        if (recovered != &source || q::evaluable(source)) return 1;
        if (q::lookup_exact(source.spelling, source.length + 1u) != nullptr)
            return 2;
    }

    constexpr char unrecognized[][3] = {
        "+=", "+?", "==", "=;", ";", "[", "]", "+",
        "p+", "P+", "=]", "=[", ";+"
    };
    for (const auto& spelling : unrecognized) {
        unsigned len = 0u;
        while (len < 3u && spelling[len] != '\0') ++len;
        if (q::lookup_exact(spelling, len) != nullptr) return 3;
    }

    constexpr char fullwidth_equals_utf8[] = "\xEF\xBC\x9D";
    if (q::lookup_exact(fullwidth_equals_utf8, 3u) != nullptr) return 4;
    if (q::lookup_exact(nullptr, 0u) != nullptr) return 5;
    if (q::lookup_exact(";", 1u) != nullptr) return 6;
    if (q::lookup_exact("+p", 2u) == q::lookup_exact("+P", 2u)) return 7;
    if (q::lookup_exact("+[", 2u) == q::lookup_exact("+]", 2u)) return 8;

    const auto* equal = q::lookup_exact("=", 1u);
    const auto* semi_equal = q::lookup_exact(";=", 2u);
    const auto* bracket_equal = q::lookup_exact("[=", 2u);
    const auto* close_equal = q::lookup_exact("]=", 2u);
    if (!equal || !semi_equal || !bracket_equal || !close_equal) return 9;
    if (equal->status != q::Status::Exploratory ||
        semi_equal->status != q::Status::Exploratory ||
        bracket_equal->status != q::Status::Exploratory ||
        close_equal->status != q::Status::Exploratory) return 10;
    return 0;
}
