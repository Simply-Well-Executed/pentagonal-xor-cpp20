// SPDX-License-Identifier: 0BSD
// ESTABLISHED SOURCE SNAPSHOT: F1/F2/F3 Q62 witness as of PR #34.
// Prior canonical source remains shared/sigma_form_area_q62.hpp.
// NEVER replace this baseline to make a proposed fold pass.
// Revisions need new IDs with old source/commit retained.
#ifndef SWE_RIGHT_TRIANGLE_F123_ORIGINAL_HPP
#define SWE_RIGHT_TRIANGLE_F123_ORIGINAL_HPP
namespace swe::right_triangle_f123_original {
using u64 = unsigned long long;
inline constexpr const char* source_commit =
    "65a86d5ecd474efb41a7694d52b656e43636ba3c";
inline constexpr const char* source_file =
    "shared/sigma_form_area_q62.hpp";
inline constexpr const char* source_blob_sha =
    "414f894bcdb5726e4b7a569c43239f01676ee5c6";
inline constexpr u64 Q = 1ULL << 62u;
// Declared normalized synthetic coordinates; NOT photograph measurements.
inline constexpr u64 x = Q / 2ULL;
inline constexpr u64 y = Q / 2ULL;
inline constexpr u64 z = Q;
inline constexpr u64 oblique_double_area = 3ULL * Q / 4ULL;
// F1 = x*z/2, F2 = y*z/2, F3 = q/2.
// These are frozen Q62 outputs, not a new arithmetic implementation.
inline constexpr u64 F1_Q62 = 1152921504606846976ULL;
inline constexpr u64 F2_Q62 = 1152921504606846976ULL;
inline constexpr u64 F3_Q62 = 1729382256910270464ULL;
// The separate right-triangle base must not be called F1/F2/F3.
inline constexpr u64 BASE_Q62 = 576460752303423488ULL;
inline constexpr u64 LATERAL_Q62 = 4035225266123964416ULL;
inline constexpr u64 TOTAL_Q62 = 4611686018427387904ULL;
static_assert(sizeof(u64) == 8);
static_assert(x == Q / 2ULL && y == Q / 2ULL && z == Q);
static_assert(oblique_double_area == 3ULL * Q / 4ULL);
static_assert(F1_Q62 == Q / 4ULL);
static_assert(F2_Q62 == Q / 4ULL);
static_assert(F3_Q62 == 3ULL * Q / 8ULL);
static_assert(BASE_Q62 == Q / 8ULL);
static_assert(F1_Q62 + F2_Q62 + F3_Q62 == LATERAL_Q62);
static_assert(BASE_Q62 + LATERAL_Q62 == TOTAL_Q62);
} // namespace swe::right_triangle_f123_original
#endif // SWE_RIGHT_TRIANGLE_F123_ORIGINAL_HPP
