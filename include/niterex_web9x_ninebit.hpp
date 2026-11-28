/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Native C++20 clean-room NiteRex nine-bit calculation core.
// User-authored Web9.X / NiteRex materials supplied as reference only.
// No upstream binary, XHTML script or MIT-licensed source has been copied.
#pragma once

namespace swe::niterex {
inline constexpr unsigned word_mask = 511U;

enum class operation : unsigned {
  and_gate, or_gate, xor_gate, not_gate, add_gate,
  sub_gate, ror_gate, nop_gate, zero_gate
};

struct result {
  unsigned value;
  bool valid;
};

[[nodiscard]] constexpr result evaluate(operation op,
                                         unsigned a,
                                         unsigned b) noexcept {
  if (a > word_mask || b > word_mask) {
    return {0U, false};
  }
  switch (op) {
    case operation::and_gate: return {a & b, true};
    case operation::or_gate:  return {a | b, true};
    case operation::xor_gate: return {a ^ b, true};
    case operation::not_gate: return {(~a) & word_mask, true};
    case operation::add_gate: return {(a + b) & word_mask, true};
    case operation::sub_gate: return {(a - b) & word_mask, true};
    case operation::ror_gate: return {((a >> 1U) | ((a & 1U) << 8U)) & word_mask, true};
    case operation::nop_gate: return {a, true};
    case operation::zero_gate:return {0U, true};
    default: return {0U, false};
  }
}

[[nodiscard]] constexpr const char* name(operation op) noexcept {
  switch (op) {
    case operation::and_gate: return "AND";
    case operation::or_gate:  return "OR";
    case operation::xor_gate: return "XOR";
    case operation::not_gate: return "NOT";
    case operation::add_gate: return "ADD";
    case operation::sub_gate: return "SUB";
    case operation::ror_gate: return "ROR";
    case operation::nop_gate: return "NOP";
    case operation::zero_gate:return "ZERO";
    default: return "INVALID";
  }
}

inline constexpr operation nine_quests[9] = {
  operation::and_gate, operation::or_gate, operation::xor_gate,
  operation::not_gate, operation::add_gate, operation::sub_gate,
  operation::ror_gate, operation::nop_gate, operation::zero_gate
};
inline constexpr unsigned expected[9] = {
  8U, 15U, 7U, 498U, 23U, 3U, 262U, 13U, 0U
};

[[nodiscard]] constexpr bool self_test() noexcept {
  unsigned checksum=0U;
  for (unsigned i=0U; i<9U; ++i) {
    const auto computed=evaluate(nine_quests[i],13U,10U);
    if (!computed.valid || computed.value != expected[i]) return false;
    checksum += computed.value;
  }
  return checksum == 829U;
}

static_assert(self_test(), "NITEREX|HOLD|nine_bit_reference_values_mismatch");
static_assert(!evaluate(operation::xor_gate,512U,0U).valid,
              "NITEREX|HOLD|out_of_bounds_operand_accepted");
static_assert(evaluate(operation::sub_gate,0U,1U).value == 511U,
              "NITEREX|HOLD|nine_bit_subtraction_wrap_mismatch");
} // namespace swe::niterex
