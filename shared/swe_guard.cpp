// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// SWE Trust Boundary Guard: a non-cryptographic, five-channel C++20 PoC.
// Provenance: adapted from nvm-sh/nvm/.github/THREAT_MODEL.md.
// Pass/fail policy is ALL (AND); XOR is diagnostic only.

#include <array>
#include <bit>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <string_view>

namespace swe {
using Checks = std::array<bool, 5>;

constexpr bool alphabet(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
constexpr bool digit(char c) { return c >= '0' && c <= '9'; }
constexpr bool safe_char(char c) {
  return alphabet(c) || digit(c) || c == '_' || c == '-' || c == '.';
}
constexpr bool hexdigit(char c) {
  return digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

constexpr bool safe_relative_path(std::string_view s) {
  if (s.empty() || s.size() > 512 || s.front() == '/' || s.back() == '/') return false;
  std::size_t from = 0;
  for (std::size_t i = 0; i <= s.size(); ++i) {
    if (i == s.size() || s[i] == '/') {
      auto segment = s.substr(from, i - from);
      if (segment.empty() || segment == "." || segment == "..") return false;
      for (char c : segment) if (!safe_char(c)) return false;
      from = i + 1;
    }
  }
  return true;
}

constexpr bool safe_artifact_name(std::string_view s) {
  if (s.empty() || s.size() > 128 || s.front() == '.' || s.find("..") != s.npos) return false;
  for (char c : s) if (!safe_char(c)) return false;
  return true;
}
constexpr bool sha256_hex_syntax(std::string_view s) {
  if (s.size() != 64) return false;
  for (char c : s) if (!hexdigit(c)) return false;
  return true;
}
constexpr bool https_scheme(std::string_view url) {
  return url.starts_with("https://") && url.size() >= 9 && url.size() <= 2048;
}
constexpr bool trusted_https_authority(std::string_view url) {
  if (!https_scheme(url)) return false;
  auto start = std::string_view{"https://"}.size();
  auto end = url.find('/', start);
  if (end == url.npos) end = url.size();
  auto host = url.substr(start, end - start);
  // Deliberately strict: no userinfo, redirects, ports, IDN aliases,
  // percent-encoded hosts, query strings, fragments, or Unicode confusables.
  constexpr std::array<std::string_view, 4> hosts = {
    "simpwellx.com", "github.com", "raw.githubusercontent.com", "api.lovable.dev"
  };
  bool approved = false;
  for (auto candidate : hosts) approved |= host == candidate;
  if (!approved) return false;
  if (end == url.size()) return true;
  auto tail = url.substr(end + 1);
  return tail.empty() || safe_relative_path(tail);
}
constexpr Checks inspect(std::string_view url, std::string_view path,
                         std::string_view sha256_hex, std::string_view artifact) {
  return {https_scheme(url), trusted_https_authority(url),
          safe_relative_path(path), sha256_hex_syntax(sha256_hex),
          safe_artifact_name(artifact)};
}
constexpr bool conjunction(Checks v) {
  bool result = true;
  for (bool x : v) result &= x;
  return result;
}
constexpr bool disjunction(Checks v) {
  bool result = false;
  for (bool x : v) result |= x;
  return result;
}
constexpr bool exclusive_or(Checks v) {
  bool result = false;
  for (bool x : v) result ^= x;
  return result;
}
constexpr double fuzzy_xor_step(double prior, double h) {
  return prior * (1.0 - h) + (1.0 - prior) * h;
}
constexpr double fuzzy_xor(Checks v) {
  double x = 0.0;
  for (bool b : v) x = fuzzy_xor_step(x, b ? 1.0 : 0.0);
  return x;
}
static_assert(safe_relative_path("public/downloads/pentagonal-xor-emblem/color.svg"));
static_assert(!safe_relative_path("../.env"));
static_assert(!safe_relative_path("safe/%2e%2e/unsafe"));
static_assert(!safe_relative_path("safe\\..\\unsafe"));
static_assert(!safe_artifact_name("../result.zip"));
static_assert(!sha256_hex_syntax("not-a-hash"));
static_assert(trusted_https_authority("https://simpwellx.com/"));
static_assert(!trusted_https_authority("https://simpwellx.com@evil.example/"));
static_assert(!trusted_https_authority("https://evil.example/"));
static_assert(conjunction({true, true, true, true, true}));
static_assert(!conjunction({true, true, false, true, true}));
static_assert(exclusive_or({true, true, true, true, true}));
static_assert(exclusive_or({true, true, false, false, true}) &&
              !conjunction({true, true, false, false, true})); // XOR alone is UNSAFE.
static_assert(fuzzy_xor({true, true, true, true, true}) == 1.0);

bool self_test() {
  // Exhaustive proof of parity equivalence for all 32 Boolean states.
  for (unsigned mask = 0; mask < 32; ++mask) {
    Checks bits{};
    for (unsigned i = 0; i < 5; ++i) bits[i] = (mask & (1u << i)) != 0;
    if (exclusive_or(bits) != ((std::popcount(mask) & 1) != 0)) return false;
    if (conjunction(bits) != (mask == 31)) return false;
    if (fuzzy_xor(bits) != (exclusive_or(bits) ? 1.0 : 0.0)) return false;
  }
  constexpr std::string_view hash = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  if (!conjunction(inspect("https://simpwellx.com/", "public/downloads/color.svg",
                           hash, "artifact.tar.gz"))) return false;
  if (conjunction(inspect("https://simpwellx.com@evil.example/", "public/downloads/color.svg",
                           hash, "artifact.tar.gz"))) return false;
  if (conjunction(inspect("https://simpwellx.com/", "../private.env",
                           hash, "artifact.tar.gz"))) return false;
  if (conjunction(inspect("https://simpwellx.com/", "public/downloads/color.svg",
                           "broken", "artifact.tar.gz"))) return false;
  return true;
}
} // namespace swe

int main(int argc, char** argv) {
  if (argc == 2 && std::string_view{argv[1]} == "--self-test") {
    const bool ok = swe::self_test();
    std::cout << (ok ? "PASS: 32 parity states + negative inputs\n"
                     : "FAIL: self-test mismatch\n");
    return ok ? 0 : 1;
  }
  if (argc != 5) {
    std::cerr << "Usage: swe_guard <https-url> <relative-path> <sha256-hex> <artifact-name>\n"
                 "       swe_guard --self-test\n";
    return 2;
  }
  auto bits = swe::inspect(argv[1], argv[2], argv[3], argv[4]);
  constexpr std::array<std::string_view, 5> names = {
    "https_scheme", "trusted_authority", "canonical_path",
    "sha256_hex_syntax", "artifact_filename"
  };
  for (std::size_t i = 0; i < 5; ++i)
    std::cout << names[i] << '=' << (bits[i] ? "PASS" : "FAIL") << '\n';
  std::cout << "OR=" << swe::disjunction(bits)
            << " AND=" << swe::conjunction(bits)
            << " XOR_DIAGNOSTIC=" << swe::exclusive_or(bits) << '\n';
  // Never gate on parity; a 3-of-5 result can produce XOR=1 and still be unsafe.
  return swe::conjunction(bits) ? 0 : 1;
}
