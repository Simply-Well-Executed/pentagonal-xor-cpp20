<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# REDUX — Exact Reduction and Work

REDUX measures reduction relative to original work: `R = (A-B)/A`. Negative results indicate expansion. All rational results use signed magnitude rather than integer truncation.

## Methods

| Method | Contract |
|---|---|
| `fraction(n,d,negative)` | Reduced signed-magnitude rational; zero denominator is a failure status |
| `reduction(original,optimized)` | Exact reduction; zero original is undefined |
| `dakatana(original,optimized)` | Tenfold threshold; zero remaining is elimination |
| `factorize(lanes,common,unique_total)` | Checked reuse of genuinely equivalent common work |
| `analyze(tasks,n)` | Deterministic topological analysis; maximum 32 tasks |
| `time_reduction(schedule)` | Work reduction relative to the critical path |
| `ideal_speedup(schedule)` | Work divided by critical path; not measured elapsed speedup |

```cpp
#include "redux.hpp"
constexpr auto r = REDUX::reduction(10000,30);
static_assert(r.numerator == 997 && r.denominator == 1000);
```

Reject invalid dependencies, cycles and overflow. No task execution, thread spawning or network access occurs in the DAG analysis. The fixed capacity is part of the supported domain, not a namespace artifact.

**Source:** [`shared/redux.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/redux.hpp), [`tests/redux_test.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/redux_test.cpp), [`docs/experiments/REDUX.C`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/experiments/REDUX.C).

## See also

- [LCD](LCD.md)
- [ARIARI](ARIARI.md)
- [SILVERHAND](SILVERHAND.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: LCD](LCD.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: ARIARI](ARIARI.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [API Reference](API-Reference.md) | [ARIARI](ARIARI.md) | [COREAO](COREAO.md) | [LCD](LCD.md) | [LF9WN9Z](LF9WN9Z.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
