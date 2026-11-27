# TOOLONG and GammaGamma — Wide Arithmetic

Use the optional adapter when bounded 64-bit arithmetic is insufficient or an explicitly tagged GammaGamma decimal operand is required. TOOLONG is an existing arbitrary-precision library using STL and allocation; the adapter is not dependency-free.

```cpp
#include "ariari_toolong.hpp"
const swe::toolong a{10000}, b{30};
const auto e = ARIARI_TOOLONG::compose(
    swe::deca_sigma::run(2.0), a, b, "3.2");
```

The result contains a COREAO witness, wide arithmetic result, and separate GammaGamma result. GammaGamma evaluates factorial differences of explicitly separated decimal components. It does not certify work ratios, physical forces or a COREAO trace. Large factorial arguments can produce a symbolic result; retain source spelling and status.

The Q62 historical records and their exact quotient/remainder witnesses are separate from this general arithmetic adapter. Do not silently replace the recorded constants or reinterpret historical projections as a new rounding policy.

**Source:** [`shared/ariari_toolong.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/ariari_toolong.hpp), [`shared/toolong.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/toolong.hpp), [`shared/gammagamma.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/gammagamma.hpp), [`shared/toolong_q62_core.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/toolong_q62_core.hpp), [`tests/ariari_toolong_test.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/ariari_toolong_test.cpp).

## See also

- [ARIARI](ARIARI.md)
- [COREAO](COREAO.md)
- [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) · [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) · PR #337 draft; not merged or released.

---

## Navigation

Previous: [ARIARI](ARIARI.md) · [Index](Index.md) · [Home](Home.md) · Next: [COREAO](COREAO.md)

**Referenced by:** [API Reference](API-Reference.md) · [ARIARI](ARIARI.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
