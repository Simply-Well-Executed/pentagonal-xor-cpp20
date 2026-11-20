// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Exact exploratory syntax and three-mode operand slots, without math execution.
// Original spellings and their provenance are retained, including superseded readings.
#ifndef SWE_EXPLORATORY_QUANTUM_OPERAND_LEXICON_HPP
#define SWE_EXPLORATORY_QUANTUM_OPERAND_LEXICON_HPP

namespace swe::quantum_operands {

enum class Family : unsigned char {
    Equals,
    CoreaoPlusStar
};

enum class Status : unsigned char {
    Exploratory,
    InterpretedNoncanonical,
    Conditional,
    Ambiguous,
    Undefined,
    Unbalanced,
    ProposedNotation
};

enum class Provenance : unsigned char {
    UserEqualsNovember2026,
    CoreaoPlusStarNovember2026,
    UserModeNovember2026,
    InferredModeNotationNovember2026,
    UserPlusLiteralNovember2026
};

enum class Mode : unsigned char {
    Gain,
    Dispersion,
    Stability,
    Unassigned
};

enum class Operand : unsigned char {
    Equality,
    SemicolonEquality,
    OpenEquality,
    CloseEquality,
    Plus,
    Minus,
    ShiftDown,
    ShiftUp,
    Semicolon,
    Apostrophe,
    LowercaseP,
    UppercaseP,
    OpenBracket,
    CloseBracket
};

enum class Notation : unsigned char {
    Recovered,
    DirectUser,
    Inferred
};

enum class ShiftAssociation : unsigned char {
    None,
    InverseSquareLaw,
    RootOfSquareTheorem
};

struct Record {
    const char* spelling;
    unsigned length;
    Family family;
    Status status;
    Provenance provenance;
    Operand operand;
    Mode mode;
    Notation notation;
};

inline constexpr Record records[] = {
    {"=", 1u, Family::Equals, Status::Exploratory, Provenance::UserEqualsNovember2026,
        Operand::Equality, Mode::Unassigned, Notation::DirectUser},
    {";=", 2u, Family::Equals, Status::Exploratory, Provenance::UserEqualsNovember2026,
        Operand::SemicolonEquality, Mode::Unassigned, Notation::DirectUser},
    {"[=", 2u, Family::Equals, Status::Exploratory, Provenance::UserEqualsNovember2026,
        Operand::OpenEquality, Mode::Unassigned, Notation::DirectUser},
    {"]=", 2u, Family::Equals, Status::Exploratory, Provenance::UserEqualsNovember2026,
        Operand::CloseEquality, Mode::Unassigned, Notation::DirectUser},

    // Standalone + is an exact user-supplied literal, not shorthand for ++.
    {"+", 1u, Family::CoreaoPlusStar, Status::Exploratory,
        Provenance::UserPlusLiteralNovember2026, Operand::Plus, Mode::Unassigned, Notation::DirectUser},
    {"++", 2u, Family::CoreaoPlusStar, Status::InterpretedNoncanonical,
        Provenance::CoreaoPlusStarNovember2026, Operand::Plus, Mode::Gain, Notation::Recovered},
    {"+-", 2u, Family::CoreaoPlusStar, Status::InterpretedNoncanonical,
        Provenance::CoreaoPlusStarNovember2026, Operand::Minus, Mode::Gain, Notation::Recovered},
    // += is directly supplied notation; only the gain-prefix slot is classified.
    // No assignment, comparison, conservation, or numerical semantics are inferred.
    {"+=", 2u, Family::CoreaoPlusStar, Status::Exploratory,
        Provenance::UserPlusLiteralNovember2026, Operand::Equality, Mode::Gain, Notation::DirectUser},
    {"+\\", 2u, Family::CoreaoPlusStar, Status::Exploratory,
        Provenance::CoreaoPlusStarNovember2026, Operand::ShiftDown, Mode::Gain, Notation::DirectUser},
    {"+/", 2u, Family::CoreaoPlusStar, Status::Exploratory,
        Provenance::CoreaoPlusStarNovember2026, Operand::ShiftUp, Mode::Gain, Notation::DirectUser},
    {"+;", 2u, Family::CoreaoPlusStar, Status::Undefined,
        Provenance::CoreaoPlusStarNovember2026, Operand::Semicolon, Mode::Gain, Notation::Recovered},
    {"+'", 2u, Family::CoreaoPlusStar, Status::Ambiguous,
        Provenance::CoreaoPlusStarNovember2026, Operand::Apostrophe, Mode::Gain, Notation::Recovered},
    {"+p", 2u, Family::CoreaoPlusStar, Status::Conditional,
        Provenance::CoreaoPlusStarNovember2026, Operand::LowercaseP, Mode::Gain, Notation::DirectUser},
    {"+P", 2u, Family::CoreaoPlusStar, Status::Conditional,
        Provenance::CoreaoPlusStarNovember2026, Operand::UppercaseP, Mode::Gain, Notation::DirectUser},
    {"+[", 2u, Family::CoreaoPlusStar, Status::Unbalanced,
        Provenance::CoreaoPlusStarNovember2026, Operand::OpenBracket, Mode::Gain, Notation::Recovered},
    {"+]", 2u, Family::CoreaoPlusStar, Status::Unbalanced,
        Provenance::CoreaoPlusStarNovember2026, Operand::CloseBracket, Mode::Gain, Notation::Recovered},

    {"-p", 2u, Family::CoreaoPlusStar, Status::Exploratory,
        Provenance::UserModeNovember2026, Operand::LowercaseP, Mode::Dispersion, Notation::DirectUser},
    {"-P", 2u, Family::CoreaoPlusStar, Status::Exploratory,
        Provenance::UserModeNovember2026, Operand::UppercaseP, Mode::Dispersion, Notation::DirectUser},
    {"=p", 2u, Family::CoreaoPlusStar, Status::Exploratory,
        Provenance::UserModeNovember2026, Operand::LowercaseP, Mode::Stability, Notation::DirectUser},
    {"=P", 2u, Family::CoreaoPlusStar, Status::Exploratory,
        Provenance::UserModeNovember2026, Operand::UppercaseP, Mode::Stability, Notation::DirectUser},

    {"-/", 2u, Family::CoreaoPlusStar, Status::ProposedNotation,
        Provenance::InferredModeNotationNovember2026, Operand::ShiftUp, Mode::Dispersion, Notation::Inferred},
    {"=/", 2u, Family::CoreaoPlusStar, Status::ProposedNotation,
        Provenance::InferredModeNotationNovember2026, Operand::ShiftUp, Mode::Stability, Notation::Inferred},
    {"-\\", 2u, Family::CoreaoPlusStar, Status::ProposedNotation,
        Provenance::InferredModeNotationNovember2026, Operand::ShiftDown, Mode::Dispersion, Notation::Inferred},
    {"=\\", 2u, Family::CoreaoPlusStar, Status::ProposedNotation,
        Provenance::InferredModeNotationNovember2026, Operand::ShiftDown, Mode::Stability, Notation::Inferred}
};

inline constexpr unsigned record_count =
    static_cast<unsigned>(sizeof(records) / sizeof(records[0]));

inline constexpr unsigned operand_count = 14u;
inline constexpr unsigned mode_count = 3u;
inline constexpr unsigned conceptual_mode_slots = operand_count * mode_count;

constexpr bool identical(const char* a, const char* b,
                         unsigned length) noexcept {
    if (a == nullptr || b == nullptr) return false;
    for (unsigned i = 0u; i < length; ++i) {
        if (static_cast<unsigned char>(a[i]) !=
            static_cast<unsigned char>(b[i])) return false;
    }
    return true;
}

[[nodiscard]] constexpr const Record*
lookup_exact(const char* spelling, unsigned length) noexcept {
    if (spelling == nullptr || length == 0u || length > 2u)
        return nullptr;
    for (const auto& record : records) {
        if (record.length == length &&
            identical(spelling, record.spelling, length))
            return &record;
    }
    return nullptr;
}

[[nodiscard]] constexpr bool valid_mode(Mode mode) noexcept {
    return mode == Mode::Gain ||
           mode == Mode::Dispersion ||
           mode == Mode::Stability;
}

[[nodiscard]] constexpr bool valid_operand(Operand operand) noexcept {
    return static_cast<unsigned>(operand) < operand_count;
}

[[nodiscard]] constexpr const Record*
lookup_mode(Operand operand, Mode mode) noexcept {
    if (!valid_mode(mode) || !valid_operand(operand)) return nullptr;
    for (const auto& record : records) {
        if (record.operand == operand && record.mode == mode)
            return &record;
    }
    return nullptr;
}

struct ModeSlot {
    Operand operand;
    Mode mode;
    const Record* spelling;
    bool applicable;
};

[[nodiscard]] constexpr ModeSlot
mode_slot(Operand operand, Mode mode) noexcept {
    return {operand, mode, lookup_mode(operand, mode),
            valid_operand(operand) && valid_mode(mode)};
}

// Every source operand has all three semantic slots.
// A missing spelling is not a rejected mode, a numeric zero, or permission
// to fabricate a glyph. Inferred spellings must remain visibly inferred.
[[nodiscard]] constexpr ShiftAssociation
shift_association(Operand operand) noexcept {
    if (operand == Operand::ShiftUp)
        return ShiftAssociation::InverseSquareLaw;
    if (operand == Operand::ShiftDown)
        return ShiftAssociation::RootOfSquareTheorem;
    return ShiftAssociation::None;
}

[[nodiscard]] constexpr bool evaluable(const Record&) noexcept {
    return false;
}

constexpr bool unique_records() noexcept {
    for (unsigned i = 0u; i < record_count; ++i) {
        for (unsigned j = i + 1u; j < record_count; ++j) {
            if (records[i].length == records[j].length &&
                identical(records[i].spelling,
                          records[j].spelling,
                          records[i].length))
                return false;
        }
    }
    return true;
}

constexpr bool exact_ascii_records() noexcept {
    for (const auto& record : records) {
        if (record.length == 0u || record.length > 2u)
            return false;
        for (unsigned i = 0u; i < record.length; ++i) {
            const unsigned char ch =
                static_cast<unsigned char>(record.spelling[i]);
            if (ch < 0x21u || ch > 0x7Eu)
                return false;
        }
        if (record.spelling[record.length] != '\0')
            return false;
    }
    return true;
}

constexpr bool unique_mode_spellings() noexcept {
    for (unsigned i = 0u; i < record_count; ++i) {
        if (records[i].mode == Mode::Unassigned) continue;
        for (unsigned j = i + 1u; j < record_count; ++j) {
            if (records[i].operand == records[j].operand &&
                records[i].mode == records[j].mode)
                return false;
        }
    }
    return true;
}

static_assert(record_count == 24u);
static_assert(!valid_operand(static_cast<Operand>(255u)));
static_assert(!mode_slot(static_cast<Operand>(255u), Mode::Gain).applicable);
static_assert(conceptual_mode_slots == 42u);
static_assert(unique_records());
static_assert(unique_mode_spellings());
static_assert(exact_ascii_records());
static_assert(lookup_exact("=", 1u)->status == Status::Exploratory);
static_assert(lookup_exact(";=", 2u)->status == Status::Exploratory);
static_assert(lookup_exact("[=", 2u)->status == Status::Exploratory);
static_assert(lookup_exact("]=", 2u)->status == Status::Exploratory);
static_assert(lookup_exact("+p", 2u) != lookup_exact("+P", 2u));
static_assert(lookup_exact("+", 1u)->mode == Mode::Unassigned);
static_assert(lookup_exact("+", 1u)->operand == Operand::Plus);
static_assert(lookup_exact("+=", 2u)->mode == Mode::Gain);
static_assert(lookup_exact("+=", 2u)->operand == Operand::Equality);
static_assert(lookup_exact("+=", 2u)->notation == Notation::DirectUser);
static_assert(lookup_exact("+=", 2u)->status == Status::Exploratory);
static_assert(lookup_exact("+", 1u) != lookup_exact("++", 2u));
static_assert(lookup_mode(Operand::Equality, Mode::Gain) == lookup_exact("+=", 2u));
static_assert(lookup_mode(Operand::Plus, Mode::Gain) == lookup_exact("++", 2u));
static_assert(lookup_exact("+[", 2u) != lookup_exact("+]", 2u));
static_assert(lookup_exact("+/", 2u)->operand == Operand::ShiftUp);
static_assert(lookup_exact("+\\", 2u)->operand == Operand::ShiftDown);
static_assert(lookup_exact("-p", 2u)->mode == Mode::Dispersion);
static_assert(lookup_exact("=P", 2u)->mode == Mode::Stability);
static_assert(lookup_mode(Operand::ShiftUp, Mode::Gain) == lookup_exact("+/", 2u));
static_assert(lookup_mode(Operand::ShiftDown, Mode::Gain) == lookup_exact("+\\", 2u));
static_assert(lookup_mode(Operand::ShiftUp, Mode::Dispersion)->notation == Notation::Inferred);
static_assert(lookup_mode(Operand::ShiftDown, Mode::Stability)->notation == Notation::Inferred);
static_assert(mode_slot(Operand::SemicolonEquality, Mode::Gain).applicable);
static_assert(mode_slot(Operand::SemicolonEquality, Mode::Gain).spelling == nullptr);
static_assert(shift_association(Operand::ShiftUp) == ShiftAssociation::InverseSquareLaw);
static_assert(shift_association(Operand::ShiftDown) == ShiftAssociation::RootOfSquareTheorem);
static_assert(lookup_exact(";", 1u) == nullptr);
static_assert(lookup_exact("==", 2u) == nullptr);

} // namespace swe::quantum_operands
#endif
