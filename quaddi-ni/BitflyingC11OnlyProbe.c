/* SPDX-License-Identifier: 0BSD
 * POSITIVE with MSVC /TC /std:c11; EXPECTED NEGATIVE /TP /std:c++20.
 * _Generic is standardized C11 and is not an ISO C++20 operator.
 */
int main(void) {
 const int result=_Generic(9, int: 9, default: 0);
 return result==9 ? 0 : 1;
}
