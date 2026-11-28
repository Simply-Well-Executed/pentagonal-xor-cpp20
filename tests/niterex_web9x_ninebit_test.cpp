/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Scope: independently authored first-party source only; upstream reference rights unchanged.
*/
// SPDX-License-Identifier: 0BSD
// Cross-platform, STD-free C++20 NiteRex nine-bit contract checks.
#include "niterex_web9x_ninebit.hpp"

using swe::niterex::operation;
using swe::niterex::evaluate;

static_assert(swe::niterex::self_test());
static_assert(evaluate(operation::not_gate,13,10).value == 498U);
static_assert(evaluate(operation::ror_gate,13,10).value == 262U);
static_assert(evaluate(operation::add_gate,511,1).value == 0U);
static_assert(evaluate(operation::sub_gate,0,1).value == 511U);
static_assert(evaluate(operation::zero_gate,511,511).value == 0U);
static_assert(evaluate(operation::nop_gate,511,511).value == 511U);
static_assert(!evaluate(static_cast<operation>(255U),0U,0U).valid);
static_assert(!evaluate(operation::and_gate,0U,512U).valid);

int main() {
  if (!swe::niterex::self_test()) return 1;
  unsigned checks=0U;
  for (unsigned a=0U; a<=511U; ++a) {
    for (unsigned b=0U; b<=511U; ++b) {
      const auto x=evaluate(operation::xor_gate,a,b);
      const auto y=evaluate(operation::and_gate,a,b);
      const auto z=evaluate(operation::or_gate,a,b);
      if (!x.valid || !y.valid || !z.valid ||
          (x.value ^ y.value) != z.value ||
          (x.value ^ a) != b ||
          (y.value & x.value) != 0U) return 2;
      checks++;
    }
  }
  return checks==262144U ? 0 : 3;
}
