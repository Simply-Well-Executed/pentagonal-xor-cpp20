<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# TALON — Terminal Velocity Model

TALON deforms a parametric figure-eight into a circle; the physical model treats the circle as the cross-section of a sphere. The deformation does not generate energy.

## Model specification

| Parameter | Value |
|---|---:|
| Mass | 0.009 kg |
| Radius | 0.01 m |
| Gravity | 9.80665 m/s² |
| Air density | 1.225 kg/m³ |
| Assumed drag coefficient | 0.47 |
| Frozen experiment | 256 advances of 4 m |

Quadratic drag: `k = rho * Cd * pi * r² / 2`. Terminal velocity: `sqrt(m*g/k)`. For an initially stationary sphere falling distance h in this idealized atmosphere: `v²/vt² = 1 - exp(-h/length)`, where `length = vt²/(2*g)`.

The model neglects atmospheric variation, buoyancy and changes in drag coefficient. Infinite height is a mathematical limit, not a physical drop from space. Numerical results use binary64 approximations.

```cpp
#include "TALON.hpp"
constexpr auto r = TALON::run();
static_assert(r.passed && r.completed == 256);
```

`TALON::thailonga` incorporates exact LCD distance normalization and numerical energy quantities. `ariari` checks energy closure; `airgap` checks analytical versus RK4 results and the speed envelope; `redux` reports dissipated fraction. These are local physical-model methods, not a replacement for ARIARI's exact rational API.

Independent numerical evaluation gave 31.23954 m/s terminal speed and 4.39159 J terminal kinetic energy. Tolerances: 2e-7 normalized squared speed and 1e-10 J closure. Native qualification remains separate.

**Source:** [`shared/TALON.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/TALON.hpp), [`tests/TALON_TEST.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/TALON_TEST.cpp).

## See also

- [LF9WN9Z](LF9WN9Z.md)
- [LCD](LCD.md)
- [SILVERHAND](SILVERHAND.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: LF9WN9Z](LF9WN9Z.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: SILVERHAND](SILVERHAND.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [API Reference](API-Reference.md) | [Home](Home.md) | [LF9WN9Z](LF9WN9Z.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
