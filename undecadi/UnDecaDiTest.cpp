// SPDX-License-Identifier: 0BSD
// ISO C++20 native no <std> dependencies. Directly tests MASM object.
extern "C" unsigned int UnDecaDi(unsigned int);
int main() {
  for (unsigned int i = 0; i <= 255u; ++i) {
    const unsigned int result=UnDecaDi(i);
    if (result != (0x96u ^ (0x69u ^ i)) ||
        (0x69u ^ (0x96u ^ result)) != i) return 10;
  }
  const unsigned int a=UnDecaDi(0xDBu);
  const unsigned int b=UnDecaDi(0xDFu);
  if (a == b || ((a | 0x24u) != (b | 0x24u))) return 20;
  return 0;
}
