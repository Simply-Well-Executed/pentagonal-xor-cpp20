// SPDX-License-Identifier: 0BSD
// ALONZ ACORN -> TREE: a reversible, byte-exact TRIPI boundary.
// Distinct from the frozen ALONZ stage-06 and the numeric Euler's Jig.
// No I/O, allocation, normalization, runtime network or external dependencies.
#ifndef SWE_ALONZ_ACORN_TREE_HPP
#define SWE_ALONZ_ACORN_TREE_HPP

namespace swe::alonz {
enum class Verdict : unsigned char {
    Reject, Hold, Accept
};

enum class JigEvidence : unsigned char {
    Undefined, Failed, IndependentlyVerified
};

// A witness is explicitly supplied by an independent, defined Jig verifier.
// This code does not claim to implement the exploratory byte-oriented Eulger's Jig.
struct Witness {
    JigEvidence jig = JigEvidence::Undefined;
    bool approved_protocol = false;
};

// CORE: source bytes. SHELL: bounded, exact envelope representation.
// TREE: only a fully validated candidate is permitted for later integration.
// The function does not mutate either source or envelope and never touches Git.
constexpr Verdict inspect(const unsigned char* core,
                          const unsigned char* shell,
                          unsigned long long length,
                          Witness witness) noexcept {
    if (length != 0ULL && (core == nullptr || shell == nullptr))
        return Verdict::Reject;
    for (unsigned long long i = 0ULL; i < length; ++i)
        if (core[i] != shell[i]) return Verdict::Reject;

    // Separate TRIPI checks: preserved bytes, specified Jig, chosen protocol.
    if (witness.jig == JigEvidence::Failed) return Verdict::Reject;
    if (witness.jig != JigEvidence::IndependentlyVerified ||
        !witness.approved_protocol) return Verdict::Hold;
    return Verdict::Accept;
}

constexpr unsigned char acorn[] = {0x41, 0x00, 0xFF, 0x5A};
constexpr unsigned char matching_shell[] = {0x41, 0x00, 0xFF, 0x5A};
constexpr unsigned char altered_shell[] = {0x41, 0x00, 0xFE, 0x5A};
constexpr Witness undefined_witness{};
constexpr Witness verified_witness{JigEvidence::IndependentlyVerified, true};
constexpr Witness failed_witness{JigEvidence::Failed, true};

static_assert(inspect(acorn, matching_shell, 4ULL, undefined_witness) ==
              Verdict::Hold);
static_assert(inspect(acorn, altered_shell, 4ULL, verified_witness) ==
              Verdict::Reject);
static_assert(inspect(acorn, matching_shell, 4ULL, failed_witness) ==
              Verdict::Reject);
static_assert(inspect(acorn, matching_shell, 4ULL, verified_witness) ==
              Verdict::Accept);
static_assert(inspect(nullptr, matching_shell, 4ULL, verified_witness) ==
              Verdict::Reject);
static_assert(inspect(nullptr, nullptr, 0ULL, undefined_witness) ==
              Verdict::Hold);
} // namespace swe::alonz
#endif // SWE_ALONZ_ACORN_TREE_HPP
