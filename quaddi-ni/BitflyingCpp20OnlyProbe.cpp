// SPDX-License-Identifier: 0BSD
// POSITIVE with MSVC /TP /std:c++20; EXPECTED NEGATIVE /TC /std:c11.
// Templates and consteval are C++ features, not valid ISO C11 syntax.
template<unsigned N>
consteval unsigned bitflying_cpp20_only() noexcept { return N+1u; }
static_assert(bitflying_cpp20_only<9u>()==10u);
int main() { return bitflying_cpp20_only<9u>()==10u ? 0 : 1; }
