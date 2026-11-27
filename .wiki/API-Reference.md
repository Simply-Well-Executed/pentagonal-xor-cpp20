# API Reference

## Libraries

| Component | Primary interface | Header | Dependencies |
|---|---|---|---|
| [LCD](LCD.md) | `LCD(U,U)` | [`shared/LCD.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/LCD.hpp) | None |
| [REDUX](REDUX.md) | `REDUX::reduction`, `REDUX::analyze` | [`shared/redux.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/redux.hpp) | None |
| [ARIARI](ARIARI.md) | `ARIARI::arirang`, `ARIARI::airgap` | [`shared/ariari.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/ariari.hpp) | REDUX |
| [TOOLONG and GammaGamma](TOOLONG-and-GammaGamma.md) | `ARIARI_TOOLONG::compose` | [`shared/ariari_toolong.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/ariari_toolong.hpp) | Existing wide libraries, STL |
| [COREAO](COREAO.md) | `swe::coreao::inspect` | [`shared/coreao.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/coreao.hpp) | TriPi / DecaSigma |
| [ALONZ and METALOGRI](ALONZ-and-METALOGRI.md) | `ARIARI::thailonga`, `ARIARI::infiniti` | [`shared/ariari.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/ariari.hpp) | REDUX header |
| [GENSHIN](GENSHIN.md) | `genshin_zipit`, `genshin_bidi`, `genshin_tripi` | [`docs/experiments/genshin_zipit.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/experiments/genshin_zipit.hpp) | See individual headers |
| [LF9WN9Z](LF9WN9Z.md) | `lf9wn9z_reset`, `lf9wn9z_action`, `lf9wn9z_step` | [`docs/experiments/LF9WN9Z/LF9WN9Z.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/experiments/LF9WN9Z/LF9WN9Z.hpp) | Windows DLL ABI |
| [TALON](TALON.md) | `TALON::run` | [`shared/TALON.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/shared/TALON.hpp) | LCD |

All interfaces here describe the pinned draft revision, not a compatibility promise. Read the header for complete declarations and validate with the matching tests.

## See also

- [Getting Started](Getting-Started.md)
- [Architecture](Architecture.md)
- [Release Status](Release-Status.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: Source Map](Source-Map.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: LCD](LCD.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [Architecture](Architecture.md) | [Glossary](Glossary.md) | [Home](Home.md) | [Source Map](Source-Map.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
