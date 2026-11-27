# ALONZ and METALOGRI — Reversible Encoding

The draft ARIARI encoding is separate from the established ALONZ acorn-tree feature. It accepts an alphabet of distinct bytes and a finite compile-time token count `N`. Each token must consist of `A`, `R` and one alphabet member.

```text
ARO ARI ARO ARI ARO -> AROIOIO -> ARO ARI ARO ARI ARO
```

`ARIARI::thailonga` stores the prefix once and keeps every suffix in order. `arirong` restores the sequence; `ariari` verifies against the original.

Original payload: `3N` bytes. Encoded payload: `N+2` bytes. Metadata is excluded from both counts. This is not general-purpose compression.

`ARIARI::infiniti(alphabet,depth)` uses checked exponentiation by squaring. It returns the exact finite cardinality when representable and preserves the operands with `exact=false` otherwise. It does not allocate every possible configuration or represent an actually infinite memory.

**Qualification:** 32 binary and 243 ternary sequences of length five, a 1000-token fixture and mutation rejection are authored in the ARIARI tests.

**Source:** [`shared/ariari.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/ariari.hpp), [`tests/ariari_test.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/ariari_test.cpp), [`shared/alonz_acorn_tree.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/alonz_acorn_tree.hpp).

## See also

- [ARIARI](ARIARI.md)
- [LCD](LCD.md)
- [SILVERHAND](SILVERHAND.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: COREAO](COREAO.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: GENSHIN](GENSHIN.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [API Reference](API-Reference.md) | [ARIARI](ARIARI.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
