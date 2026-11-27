<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# LF9WN9Z — Native Physics ABI

A bounded integer tick model, distinct from TALON's aerodynamic simulation. Each state has five caller-owned `long long` elements: x, y, z in micrometres, vy and vz in micrometres per 10 ms tick.

```cpp
int lf9wn9z_reset(long long*, unsigned count, unsigned ceiling) noexcept;
int lf9wn9z_action(long long*, unsigned count, unsigned action) noexcept;
int lf9wn9z_step(long long*, unsigned count, unsigned ticks) noexcept;
```

The actual header declares C language linkage and Windows DLL import/export. The functions return 1 on success, 0 on rejection. Caller supplies valid writable storage; count must be five, ticks at most 10000, ceiling 0 or 1. Action IDs: 0 ZIPITUP, 1 ZIPITDOWN, 2 ZIPITOUT, 3 FUCKITIN.

The optimized batch stops at a stationary floor fixed point. The original 10000-tick jump fixture reaches that fixed point in 30 evaluated ticks. This is an operation-count reduction, not a measured wall-clock speedup. The independent literal tick oracle covers 7875 authored cases.

**Source:** [`docs/experiments/LF9WN9Z/LF9WN9Z.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/experiments/LF9WN9Z/LF9WN9Z.hpp), [`docs/experiments/LF9WN9Z/LF9WN9Z.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/experiments/LF9WN9Z/LF9WN9Z.cpp), [`tests/dakatana_physics_test.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/dakatana_physics_test.cpp).

## See also

- [TALON](TALON.md)
- [REDUX](REDUX.md)
- [SILVERHAND](SILVERHAND.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: GENSHIN](GENSHIN.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: TALON](TALON.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [API Reference](API-Reference.md) | [TALON](TALON.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
