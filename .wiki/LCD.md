# LCD — Exact Quotient

Computes a canonical unsigned rational quotient `l = a/c` by cancelling the greatest common divisor. LCD is a project-specific name; it is not the conventional least common denominator of two fractions.

## Declaration

```cpp
using U = unsigned long long;
struct L { U a, c; };
[[nodiscard]] constexpr L LCD(U a, U c) noexcept;
```

| Input | Result |
|---|---|
| `c > 0` | Reduced numerator and denominator |
| `a == 0`, `c > 0` | `0/1` |
| `c == 0` | Original numerator with zero denominator; undefined quotient |

The implementation requires 64-bit `U`. There are no includes, namespaces, heap allocations or external calls. Global identifiers `U`, `L` and `LCD` can collide with unrelated headers.

## Example

```cpp
#include "LCD.hpp"
constexpr L value = LCD(10000, 30);
static_assert(value.a == 1000 && value.c == 3);
static_assert(LCD(1,0).c == 0);
```

## Qualification

`lcd_256_cycles_exact_quotient`: 256 × 256 operand pairs checked against an independent descending-divisor oracle, plus compile-time 64-bit boundary fixtures. Authored coverage, not a reported native test pass.

**Source:** [`shared/LCD.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/LCD.hpp), [`tests/LCD_TEST.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/LCD_TEST.cpp).

## See also

- [REDUX](REDUX.md)
- [ARIARI](ARIARI.md)
- [SILVERHAND](SILVERHAND.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: API Reference](API-Reference.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: REDUX](REDUX.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [ALONZ and METALOGRI](ALONZ-and-METALOGRI.md) | [API Reference](API-Reference.md) | [Getting Started](Getting-Started.md) | [Home](Home.md) | [REDUX](REDUX.md) | [TALON](TALON.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
