<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# TRIFORCE and JSTAR — Evidence Contract

TRIFORCE uses independent arithmetic checks, source hashes, paired HEAD/TAIL stages and native witnesses. JSTAR observes Windows process ancestry and executable identity during native builds. JSTAR's 250 ms sampling is not a complete kernel trace.

| Evidence | Meaning |
|---|---|
| Build exit 0 | Build command returned success |
| JSTAR `SAMPLED_PASS` | No violations in the sampled process audit |
| JSTAR `HOLD` | Provenance incomplete or policy violation detected; not a malware verdict |
| CTest success | The named executable test passed |
| Release HOLD | No release authorization inferred |

Build success cannot substitute for JSTAR, nor can a source-only check substitute for native execution. Evidence belongs outside the source tree. Preserve the original receipts, source revision, exact test protocol and per-test identity.

**Source:** [`ci/Assert-TriforceHeadTail.ps1`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/ci/Assert-TriforceHeadTail.ps1), [`ci/Invoke-JstarProcessAudit.ps1`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/ci/Invoke-JstarProcessAudit.ps1), [`docs/JSTAR_PROCESS_PROVENANCE.md`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/JSTAR_PROCESS_PROVENANCE.md), [`docs/TOOLONG_TRIFORCE_Q62.md`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/TOOLONG_TRIFORCE_Q62.md).

## See also

- [SILVERHAND](SILVERHAND.md)
- [Troubleshooting](Troubleshooting.md)
- [Release Status](Release-Status.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: SILVERHAND](SILVERHAND.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: Troubleshooting](Troubleshooting.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [Architecture](Architecture.md) | [COREAO](COREAO.md) | [Home](Home.md) | [Policy and Custody](Policy-and-Custody.md) | [Release Status](Release-Status.md) | [SILVERHAND](SILVERHAND.md) | [TOOLONG and GammaGamma](TOOLONG-and-GammaGamma.md) | [Troubleshooting](Troubleshooting.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
