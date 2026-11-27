# Architecture Overview

## Layers

| Layer | Responsibility |
|---|---|
| LCD | Exact bounded rational quotient; independent header |
| REDUX | Exact reduction, work factorization, DAG analysis |
| ARIARI | Shared ratio core, reversible encoding and reconciliation |
| TOOLONG / GammaGamma | Optional arbitrary-precision arithmetic and tagged factorial differences |
| TALON / LF9WN9Z | Separate physical models with explicit assumptions |
| TRIFORCE / JSTAR | Source, arithmetic and sampled process provenance qualification |
| SILVERHAND | Serial union of independent native test targets |

```text
Source inputs -> arithmetic / geometry / model -> individual witnesses
             -> independent verification -> CI receipt -> release HOLD
```

The new bounded libraries use global functions or named type scopes rather than namespace declarations. Existing shared APIs such as `swe::coreao` remain namespaced. Type scopes prevent generic names such as `Status` and `Ratio` from colliding; namespace removal alone does not change runtime performance.

**Design rule:** Source algorithms and original operands are authoritative. Reports, indexes and renderings are derived. A reduced ratio does not preserve original operand scale unless a separate source witness is kept. A successful numerical comparison does not prove process provenance or release approval.

## See also

- [API Reference](API-Reference.md)
- [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)
- [Source Map](Source-Map.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: Getting Started](Getting-Started.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: Source Map](Source-Map.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [API Reference](API-Reference.md) | [GENSHIN](GENSHIN.md) | [Glossary](Glossary.md) | [Home](Home.md) | [Policy and Custody](Policy-and-Custody.md) | [Source Map](Source-Map.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
