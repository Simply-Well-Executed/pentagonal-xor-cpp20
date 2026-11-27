# ARIARI — Shared Core and Reconciliation

ARIARI derives REDUX and KNIGHTREX from a shared reduced core. The bounded implementation uses a single production GCD through REDUX and a separate binary GCD during verification.

For positive original `A` and remaining `B`, let `g=gcd(A,B)`, `a=A/g`, `b=B/g`. The source scale is retained in the core.

- REDUX: `(a-b)/a` (signed magnitude).
- KNIGHTREX: `a/b`.
- Closure: `R + 1/K = 1` for finite nonzero K.

## Methods

| Method | Description |
|---|---|
| `thailonga(A,B)` / `arirang(A,B)` | Compute the fused bounded result |
| `arirong(core,A,B)` | Reconcile against trusted source operands |
| `ariari(fusion,A,B)` / `airgap(fusion,A,B)` | Check ratio, source reconstruction and signed reduction |
| `decadakatana(A,B)` | Alias for the legacy tenfold gate |

```cpp
#include "ariari.hpp"
constexpr auto f = ARIARI::arirang(10000,30);
static_assert(f.knightrex.numerator == 1000);
static_assert(ARIARI::airgap(f,10000,30).ping());
```

Zero original is undefined. Zero remaining with positive original is elimination, not a finite speedup. The logical AIRGAP is not a physical network air gap, authentication or release authorization.

**Source:** [`shared/ariari.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/ariari.hpp), [`tests/ariari_test.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/ariari_test.cpp), [`docs/experiments/ARIRANG.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/experiments/ARIRANG.cpp).

## See also

- [REDUX](REDUX.md)
- [ALONZ and METALOGRI](ALONZ-and-METALOGRI.md)
- [TOOLONG and GammaGamma](TOOLONG-and-GammaGamma.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: REDUX](REDUX.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: TOOLONG and GammaGamma](TOOLONG-and-GammaGamma.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [ALONZ and METALOGRI](ALONZ-and-METALOGRI.md) | [API Reference](API-Reference.md) | [Home](Home.md) | [LCD](LCD.md) | [REDUX](REDUX.md) | [TOOLONG and GammaGamma](TOOLONG-and-GammaGamma.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
