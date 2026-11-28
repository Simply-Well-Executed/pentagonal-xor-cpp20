<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# SILVERHAND — Serial Native Qualification

**Source revision included; native qualification pending.** SILVERHAND unites the build and test scheduling of twelve native executables; it does not fuse independent evidence into a single interchangeable result.

## Run order

| Order | CTest name |
|---:|---|
| 1 | `toolong_exact_arithmetic` |
| 2 | `gammagamma_pi_precision_boundaries` |
| 3 | `lcd_256_cycles_exact_quotient` |
| 4 | `redux_exact_reduction` |
| 5 | `redux_standalone_exact_reduction` |
| 6 | `redux_source_arithmetic_equivalence` |
| 7 | `ariari_fusion_exact_roundtrip` |
| 8 | `ariari_standalone_exact_roundtrip` |
| 9 | `ariari_toolong_gammagamma_composition` |
| 10 | `namespace_free_composition_65025` |
| 11 | `dakatana_physics_exact_batch` |
| 12 | `talon_256_cycles_terminal_velocity` |

## Procedure

Run the full `Invoke-TriforcePipeline.ps1` on the SILVERHAND source revision with a fresh external evidence directory. The precision configuration builds `SILVERHAND` through JSTAR with `--parallel 1`, `/m:1`, `/nr:false`. Do not invoke the diagnostic script as a bypass of a failed audit.

The script verifies the JSTAR receipt and exact CTest inventory, then executes each named test with `--parallel 1`, a 300-second per-test timeout and `--no-tests=error`. It records individual exit codes and durations, retains independent failures, and writes `SILVERHAND.json` and its SHA-256 digest to the CI log. Aggregate PASS requires all twelve tests. Release remains HOLD.

Serial scheduled workers do not imply one OS thread inside the compiler or linker. The anticipated benefit is simpler orchestration and deterministic diagnosis; a speedup must be measured against comparable runs, not assumed from the number of build targets.

**Source:** [`ci/Invoke-Silverhand.ps1`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/54bc903619417b35b97612b574cd32efdfa90a3a/ci/Invoke-Silverhand.ps1), [`ci/triforce/CMakeLists.txt`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/54bc903619417b35b97612b574cd32efdfa90a3a/ci/triforce/CMakeLists.txt), [`ci/Invoke-TriforcePipeline.ps1`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/54bc903619417b35b97612b574cd32efdfa90a3a/ci/Invoke-TriforcePipeline.ps1).

## See also

- [Getting Started](Getting-Started.md)
- [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)
- [Troubleshooting](Troubleshooting.md)
- [Release Status](Release-Status.md)

---
**Version:** [source revision `54bc9036`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/54bc903619417b35b97612b574cd32efdfa90a3a) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | Source included in documentation edition; native results not verified.

---

## Navigation

- [Previous: TALON](TALON.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [ALONZ and METALOGRI](ALONZ-and-METALOGRI.md) | [GENSHIN](GENSHIN.md) | [Getting Started](Getting-Started.md) | [Home](Home.md) | [LCD](LCD.md) | [LF9WN9Z](LF9WN9Z.md) | [REDUX](REDUX.md) | [Release Status](Release-Status.md) | [TALON](TALON.md) | [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
