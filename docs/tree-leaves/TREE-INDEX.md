# TREE-DIFF index | PR #103 focus branch

Status: derived evidence index, **RELEASE=HOLD**, 2026-11-24. This page is not a copy of all GitHub Actions logs or artifacts, nor a claim of merge readiness.

## Trunk and branches

Trunk: [main](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/tree/main). The seven open branches retain independent source custody. PR #95 is stacked on PR #34, not directly on main.

| PR | Historical source custody leaf | Relationship |
|---|---|---|
| 34 | [JSTAR](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/feat/jstar-msvc-process-provenance-20261121/docs/tree-leaves/PR-34-LEAF.md) | Open, base main |
| 95 | [QUADDI-NI](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/feat/quaddi-ni-oracle-cpp20-20261123/docs/tree-leaves/PR-95-LEAF.md) | Open, base PR 34 branch |
| 97 | [MINZA](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/audit/minza-dual-rendition-2026-11-19/docs/tree-leaves/PR-97-LEAF.md) | Open, base main |
| 99 | [Oracle evidence](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/evidence/jstar-oracle27-20261121/docs/tree-leaves/PR-99-LEAF.md) | Open, base main |
| 100 | [JDK diagnostic](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/experiment/oracle-jdk27-vcxproj-diagnostics-20261121/docs/tree-leaves/PR-100-LEAF.md) | Open, base main |
| 101 | [Operand lexicon](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/feat/exploratory-quantum-operand-lexicon-20261119/docs/tree-leaves/PR-101-LEAF.md) | Open, base main |
| 103 | [Euler](PR-103-LEAF.md) | Selected open focus, base main |

Merged historical sources: [PR #98](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/98) and [PR #102](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/102); preserve original source commits and review threads as history. Merged does not mean deleted.

## Audit and research focus

- Old PR #103 source snapshot `01ebcaf6fd3a6402fbaefe103d7b5ffb3e2d249d`, Git tree `5d330dc6092e33741739f5a1526e3b5b82b2ab45`.
- Frozen historical nine-rung results copied as a **derived TSV**: [PR-103-LADDERDI-WITNESSES.tsv](PR-103-LADDERDI-WITNESSES.tsv), sourced from [GitHub Actions run 45444022883](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45444022883).
- Historical result: 5 successful jobs (NonaDi, EnneaDi, DecaDi, UnDecaDi, HenDecaDi); 4 failed jobs (HeptaDi, OctaDi, DoDecaDi, TriDecaDi).
- [PR-PARITY failure](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45444022839) includes missing `diaery/Invoke-ParityGate.ps1` and TRIFORCE source/configuration failure. These must be resolved from authoritative source, never bypassed with stubs or made into policy.
- EULER-Q64-v3 truncated value retained through immutable Git ancestry; EULER-Q64-v4 implements nearest-even in new commits. Native `__uint128_t` is not ISO C++20 on MSVC, so compatibility remains unresolved.
- No uploaded GitHub Actions artifacts were returned for the inspected #103 LADDERDI and PR-PARITY runs. Logs and run metadata are linked, but ZIP bytes have not been duplicated.
- A commit snapshot is a Git copy of historical source content, not a recreated external runtime artifact.
- Source, runnable binaries, evidence, derived reports, consent, release, and branch ancestry remain separate namespaces.

No destructive prune, squash, forced branch rewriting, automatic approval or merge was performed. Chosen, never assumed.
