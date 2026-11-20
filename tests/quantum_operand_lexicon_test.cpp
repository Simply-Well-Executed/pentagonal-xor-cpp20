#include "../shared/quantum_operand_lexicon.hpp"

namespace q = swe::quantum_operands;

static_assert(q::record_count == 22u);
static_assert(q::operand_count == 14u);
static_assert(q::mode_count == 3u);
static_assert(q::conceptual_mode_slots == 42u);
static_assert(q::unique_records());
static_assert(q::unique_mode_spellings());
static_assert(q::lookup_exact("+\\", 2u)->operand == q::Operand::ShiftDown);
static_assert(q::lookup_exact("+/", 2u)->operand == q::Operand::ShiftUp);
static_assert(q::lookup_exact("+\\", 2u)->mode == q::Mode::Gain);
static_assert(q::lookup_exact("+/", 2u)->mode == q::Mode::Gain);
static_assert(q::shift_association(q::Operand::ShiftUp) == q::ShiftAssociation::InverseSquareLaw);
static_assert(q::shift_association(q::Operand::ShiftDown) == q::ShiftAssociation::RootOfSquareTheorem);
static_assert(q::lookup_exact("+'", 2u)->status == q::Status::Ambiguous);
static_assert(q::lookup_exact("+;", 2u)->status == q::Status::Undefined);
static_assert(q::lookup_exact("+p", 2u)->status == q::Status::Conditional);
static_assert(q::lookup_exact("+P", 2u)->status == q::Status::Conditional);
static_assert(q::lookup_exact("++", 2u)->status == q::Status::InterpretedNoncanonical);
static_assert(q::lookup_exact("+-", 2u)->status == q::Status::InterpretedNoncanonical);
static_assert(q::lookup_exact("+[", 2u)->status == q::Status::Unbalanced);
static_assert(q::lookup_exact("+]", 2u)->status == q::Status::Unbalanced);
static_assert(q::lookup_exact("-p", 2u)->mode == q::Mode::Dispersion);
static_assert(q::lookup_exact("-P", 2u)->mode == q::Mode::Dispersion);
static_assert(q::lookup_exact("=p", 2u)->mode == q::Mode::Stability);
static_assert(q::lookup_exact("=P", 2u)->mode == q::Mode::Stability);
static_assert(q::lookup_exact("-/", 2u)->notation == q::Notation::Inferred);
static_assert(q::lookup_exact("=/", 2u)->notation == q::Notation::Inferred);
static_assert(q::lookup_exact("-\\", 2u)->notation == q::Notation::Inferred);
static_assert(q::lookup_exact("=\\", 2u)->notation == q::Notation::Inferred);
static_assert(q::lookup_exact("-/", 2u)->status == q::Status::ProposedNotation);
static_assert(q::lookup_exact("=\\", 2u)->status == q::Status::ProposedNotation);
static_assert(q::lookup_exact("= ", 2u) == nullptr);
static_assert(q::lookup_exact(" =", 2u) == nullptr);
static_assert(q::lookup_exact(";=", 1u) == nullptr);
static_assert(q::lookup_exact(";=", 2u)->family == q::Family::Equals);
static_assert(q::lookup_exact(";=", 2u)->mode == q::Mode::Unassigned);
static_assert(q::lookup_exact(";=", 2u)->provenance ==
              q::Provenance::UserEqualsNovember2026);
static_assert(q::lookup_exact("+p", 2u)->provenance ==
              q::Provenance::CoreaoPlusStarNovember2026);
static_assert(!q::evaluable(*q::lookup_exact("++", 2u)));

int main() {
    for (unsigned i = 0u; i < q::record_count; ++i) {
        const auto& source = q::records[i];
        const auto* recovered = q::lookup_exact(source.spelling, source.length);
        if (recovered != &source || q::evaluable(source)) return 1;
        if (q::lookup_exact(source.spelling, source.length + 1u) != nullptr)
            return 2;
        if (q::valid_mode(source.mode) &&
            q::lookup_mode(source.operand, source.mode) != &source) return 3;
    }

    constexpr q::Operand operands[] = {
        q::Operand::Equality, q::Operand::SemicolonEquality,
        q::Operand::OpenEquality, q::Operand::CloseEquality,
        q::Operand::Plus, q::Operand::Minus, q::Operand::ShiftDown,
        q::Operand::ShiftUp, q::Operand::Semicolon, q::Operand::Apostrophe,
        q::Operand::LowercaseP, q::Operand::UppercaseP,
        q::Operand::OpenBracket, q::Operand::CloseBracket
    };
    constexpr q::Mode modes[] = {
        q::Mode::Gain, q::Mode::Dispersion, q::Mode::Stability
    };
    unsigned conceptual = 0u;
    for (const auto operand : operands) {
        for (const auto mode : modes) {
            const auto slot = q::mode_slot(operand, mode);
            if (!slot.applicable || slot.operand != operand ||
                slot.mode != mode) return 4;
            if (slot.spelling != nullptr &&
                (slot.spelling->operand != operand ||
                 slot.spelling->mode != mode)) return 5;
            ++conceptual;
        }
    }
    if (conceptual != q::conceptual_mode_slots) return 6;
    if (q::mode_slot(q::Operand::ShiftUp,q::Mode::Unassigned).applicable)
        return 7;
    if (q::mode_slot(q::Operand::SemicolonEquality,q::Mode::Dispersion).spelling)
        return 8;

    constexpr char unrecognized[][3] = {
        "+=", "+?", "==", "=;", ";", "[", "]", "+",
        "p+", "P+", "=]", "=[", ";+", "-=", "-?", "P="
    };
    for (const auto& spelling : unrecognized) {
        unsigned len = 0u;
        while (len < 3u && spelling[len] != '\0') ++len;
        if (q::lookup_exact(spelling, len) != nullptr) return 9;
    }

    constexpr char fullwidth_equals_utf8[] = "\xEF\xBC\x9D";
    if (q::lookup_exact(fullwidth_equals_utf8, 3u) != nullptr) return 10;
    if (q::lookup_exact(nullptr, 0u) != nullptr) return 11;
    if (q::lookup_exact(";", 1u) != nullptr) return 12;
    if (q::lookup_exact("+p", 2u) == q::lookup_exact("+P", 2u)) return 13;
    if (q::lookup_exact("-p", 2u) == q::lookup_exact("-P", 2u)) return 14;
    if (q::lookup_exact("=p", 2u) == q::lookup_exact("=P", 2u)) return 15;
    if (q::lookup_exact("+[", 2u) == q::lookup_exact("+]", 2u)) return 16;

    if (q::lookup_mode(q::Operand::ShiftUp, q::Mode::Gain) != q::lookup_exact("+/",2u))
        return 17;
    if (q::lookup_mode(q::Operand::ShiftUp, q::Mode::Dispersion) != q::lookup_exact("-/",2u))
        return 18;
    if (q::lookup_mode(q::Operand::ShiftUp, q::Mode::Stability) != q::lookup_exact("=/",2u))
        return 19;
    if (q::lookup_mode(q::Operand::ShiftDown, q::Mode::Gain) != q::lookup_exact("+\\",2u))
        return 20;
    if (q::lookup_mode(q::Operand::ShiftDown, q::Mode::Dispersion) != q::lookup_exact("-\\",2u))
        return 21;
    if (q::lookup_mode(q::Operand::ShiftDown, q::Mode::Stability) != q::lookup_exact("=\\",2u))
        return 22;
    if (q::lookup_mode(q::Operand::LowercaseP, q::Mode::Stability) != q::lookup_exact("=p",2u))
        return 23;
    if (q::lookup_mode(q::Operand::UppercaseP, q::Mode::Dispersion) != q::lookup_exact("-P",2u))
        return 24;
    return 0;
}
