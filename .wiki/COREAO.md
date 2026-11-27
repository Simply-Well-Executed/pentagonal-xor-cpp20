# COREAO — Read-only Trace Inspection

`swe::coreao::inspect(const swe::deca_sigma::Trace&)` checks the DecaSigma forward, reverse and Z-fold routes without mutating the input. It reports the maximum normalized residual and whether the trace is admissible.

```cpp
#include "coreao.hpp"
constexpr auto result = swe::coreao::inspect(swe::deca_sigma::run(2.0));
static_assert(result.passed());
```

`passed(tolerance)` requires admissibility, finite nonnegative tolerance and residual within tolerance. Invalid or undefined routes must not be treated as successful zero residuals. The optimization for equal finite nonzero operands returns exact zero, preserving other paths. The source-expression count is not a measured speedup.

**Source:** [`shared/coreao.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/coreao.hpp), [`docs/COREAO.md`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/COREAO.md), [`tests/redux_optimization_test.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/redux_optimization_test.cpp).

## See also

- [REDUX](REDUX.md)
- [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: TOOLONG and GammaGamma](TOOLONG-and-GammaGamma.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: ALONZ and METALOGRI](ALONZ-and-METALOGRI.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [API Reference](API-Reference.md) | [TOOLONG and GammaGamma](TOOLONG-and-GammaGamma.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
