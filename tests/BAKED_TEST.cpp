/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/BAKED.hpp"
using B=SWE::BAKED;
constexpr B::REGION before[]={{0,4096,1,2,3},{4096,4096,1,4,3}};
constexpr B::REGION changed[]={{0,4096,1,2,3},{4096,4096,1,8,3}};
static_assert(B::PSYCHOZIZER(before,2,before,2)==0);
static_assert(B::PSYCHOZIZER(before,2,changed,2)==97);
static_assert(B::PSYCHOZIZER(before,2,changed,1)==97);
static_assert(B::PSYCHOZIZER(nullptr,0,nullptr,0)==97);
int main() {
    const B::DIAMONDTHREAD baseline;
    return baseline.valid() && baseline.CHECK()==0 ? 0 : 1;
}
