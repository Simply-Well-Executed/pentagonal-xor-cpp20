/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/OBJECTIVE.hpp"
namespace {
struct MODEL {
    unsigned SHARIGON,SHARIGONE,REAR_EYE,SHARIGUN,CENTRIX;
};
constexpr SWE::OBJECTIVE objective{};
constexpr MODEL fixture{1,2,3,4,5};
constexpr auto eye=objective.PROCESS(fixture,SWE::THAILONG);
static_assert(eye.left.left.left==1);
static_assert(eye.left.left.right==2);
static_assert(eye.left.right.left==3);
static_assert(eye.left.right.right==4);
static_assert(eye.right==5);
constexpr SWE::OBJECTIVE::TRIANGLE<unsigned,char,bool> triangle{7,'a',true};
constexpr auto restored=triangle.cross().reverse_cross();
constexpr auto cycled=triangle.cross().cross().cross();
static_assert(restored.first==7 && restored.second=='a' && restored.third);
static_assert(cycled.first==7 && cycled.second=='a' && cycled.third);
}
int main() { return 0; }
