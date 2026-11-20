// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Exploratory exact-symbol ledger. A recognized token is not executable math.
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
    Unbalanced
};

enum class Provenance : unsigned char {
    UserEqualsNovember2026,
    CoreaoPlusStarNovember2026
};

struct Record {
    const char* spelling;
    unsigned length;
    Family family;
    Status status;
    Provenance provenance;
};

inline constexpr Record records[] = {
    {"=", 1u, Family::Equals, Status::Exploratory, Provenance::UserEqualsNovember2026},
    {";=", 2u, Family::Equals, Status::Exploratory, Provenance::UserEqualsNovember2026},
    {"[=", 2u, Family::Equals, Status::Exploratory, Provenance::UserEqualsNovember2026},
    {"]=", 2u, Family::Equals, Status::Exploratory, Provenance::UserEqualsNovember2026},
    {"++", 2u, Family::CoreaoPlusStar, Status::InterpretedNoncanonical, Provenance::CoreaoPlusStarNovember2026},
    {"+-", 2u, Family::CoreaoPlusStar, Status::InterpretedNoncanonical, Provenance::CoreaoPlusStarNovember2026},
    {"+\\", 2u, Family::CoreaoPlusStar, Status::Undefined, Provenance::CoreaoPlusStarNovember2026},
    {"+/", 2u, Family::CoreaoPlusStar, Status::Ambiguous, Provenance::CoreaoPlusStarNovember2026},
    {"+;", 2u, Family::CoreaoPlusStar, Status::Undefined, Provenance::CoreaoPlusStarNovember2026},
    {"+'", 2u, Family::CoreaoPlusStar, Status::Ambiguous, Provenance::CoreaoPlusStarNovember2026},
    {"+p", 2u, Family::CoreaoPlusStar, Status::Conditional, Provenance::CoreaoPlusStarNovember2026},
    {"+P", 2u, Family::CoreaoPlusStar, Status::Conditional, Provenance::CoreaoPlusStarNovember2026},
    {"+[", 2u, Family::CoreaoPlusStar, Status::Unbalanced, Provenance::CoreaoPlusStarNovember2026},
    {"+]", 2u, Family::CoreaoPlusStar, Status::Unbalanced, Provenance::CoreaoPlusStarNovember2026}
};

inline constexpr unsigned record_count =
    static_cast<unsigned>(sizeof(records) / sizeof(records[0]));

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

static_assert(record_count == 14u);
static_assert(unique_records());
static_assert(exact_ascii_records());
static_assert(lookup_exact("=", 1u)->status == Status::Exploratory);
static_assert(lookup_exact(";=", 2u)->status == Status::Exploratory);
static_assert(lookup_exact("[=", 2u)->status == Status::Exploratory);
static_assert(lookup_exact("]=", 2u)->status == Status::Exploratory);
static_assert(lookup_exact("+p", 2u) != lookup_exact("+P", 2u));
static_assert(lookup_exact("+[", 2u) != lookup_exact("+]", 2u));
static_assert(lookup_exact(";", 1u) == nullptr);
static_assert(lookup_exact("==", 2u) == nullptr);

} // namespace swe::quantum_operands
#endif
