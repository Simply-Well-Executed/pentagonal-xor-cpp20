<!-- SPDX-License-Identifier: 0BSD -->
# PentaPi lap 001 | guarded five-gate review + TCP/IPv4 TTL 255

**Status:** EXPLORATORY, native run result must be verified against its exact commit. **Release:** HOLD. **Source authority:** C11 record source and pinned [nine-record ledger](../pentadi/PentaDiC11BranchRecords.reference.tsv), prior four-lane QuadDi YAML, and the five-lane PentaDi scripts. **No auto-merge or self-editing of the test.**

## What PentaPi means in this engineering experiment

PentaPi is a **repeatable continuous-improvement review cycle**, not an actual new physical dimension or a compiler standard:

```text
                   (1) QuadDi qualified
                         /       \
          (5) TCP255   /         \ (2) ASM provenance
           and audit  /           \
                     \             /
        (4) profiler  \___________/ (3) exact C11 records
           safety

       REVIEW -> PROPOSE -> human decision -> NEW test revision
```

At every lap, tests gather evidence, grade independent gates and write proposed improvements into a **separate record**. They never alter their own source, original numeric records, test fixtures or release policy. Failure/HOLD are retained as evidence, not upgraded into PASS by repetition. The existing `RELEASE=HOLD` remains authoritative.

## Local TCP protocol: what TTL 255 means

[PowerShell IPv4/TCP transport qualifier](../pentadi/Invoke-PentaPiTcp255.ps1) uses the Windows operating system's TCP implementation. It does **not** purport to reimplement the entire TCP standard.

1. Opens a listener strictly on **127.0.0.1** and a transient operating-system-selected port.
2. Creates a separate IPv4 TCP client socket.
3. Sets `Socket.Ttl = 255` **before** connecting, and verifies the locally configured socket option.
4. Uses a bounded send/receive timeout of 2500 ms and a **two-byte, big-endian length prefix** for application-level framing. TCP is a byte stream, so it does not preserve independent application messages.
5. Sends the nine ASCII bytes `PENTAPI|1`, receives the complete message, echoes it, and checks exact byte-for-byte equality.
6. Closes and disposes the listener and both sockets, then writes a single local TSV receipt with a SHA-256 hash.

**TTL is part of IPv4, not TCP's application timeouts.** The eight-bit field has a maximum initial value of **255**, and intermediate routers decrement it. It is not a 255-year timer, a scheduled wait, a human lifespan statement, a TCP connection-retention setting or a promise of a destination. Because this test stays on loopback, it verifies the configured socket option and echo **but does not capture the outgoing packet's actual TTL on the wire**. A real network path or remote host is not contacted.

The project's TCP gate does **not** send data over the internet, connect to cloud services, maintain an always-on connection or open a public listener. Each qualification run creates its own bounded local listener and then terminates.

References: [IETF RFC 791](https://www.rfc-editor.org/rfc/rfc791) and [Microsoft Socket.Ttl property](https://learn.microsoft.com/en-us/dotnet/api/system.net.sockets.socket.ttl).

## Five independent audit gates

| Gate | Condition for PASS | What it does NOT claim |
|---|---|---|
| 1. QuadDi prerequisite | Existing `needs: quaddi-concurrence` gate and reusable PentaDi workflow `workflow_call`; the upstream job actually completed success before the downstream job starts | A fifth language is automatically compatible with the first four |
| 2. C11 / ASM | Real MSVC `/FA` C11 assembly listing plus independently assembled MASM reference; no executable delivered | That the assembly was emitted by Borland Delphi 7 |
| 3. Record correctness | Nine clean C11 pipe records equal a single canonical TSV, with **5 valid** and **4 invalid** signed rel8 displacements | Arbitrary inputs are trusted; no run without checks |
| 4. Profiler safety | Original `keep.pass` saved unchanged; qualified `BuildSpeedProfiler.pas` linked into runner; range/overflow/I/O checks retained for qualification | User's proposed `{$R-}`, `{$Q-}`, `{$I-}` are automatically safe |
| 5. TCP + custody | TTL 255 loopback echo receipt; every pinned source SHA-256 unchanged during the lap | Network-wide TTL capture, permanent CPU/memory state, or years |

The [PentaPi lap grader](../pentadi/Invoke-PentaPiLapAudit.ps1) emits a new `PentaPi_lap_001.tsv` and `PentaPi_next_lap_proposals.md` in runner temporary storage. Suggestions require a separately authorized, reviewed code change and fresh CI. No recursive automatic code rewriting, no inference that an unchanged result must become policy, no release authorization.

## Reproducible pipeline

The existing [four-lane QuadDi workflow](../.github/workflows/quaddi-ni-oracle-cpp20.yml) runs before the reusable [PentaDi fifth-lane job](../.github/workflows/pentadi-delphi7-asm.yml). After C11 has compiled and run its branch-record generator, the fifth-lane job runs the TTL 255 transport fixture and the five-gate PentaPi ledger.

The **Delphi 7 proprietary compiler is not fetched or installed from an archive**. A lawfully provisioned SHA-256-pinned DCC32 remains an optional future qualification. The original `keep.pass` is historical source data, not an installed compiler. Its build-speed flags are preserved as a proposal; they do not disable safety in these tests.

**Executed engineering qualification: PASS (preflight only), with explicit Delphi HOLD.** Code commit `51d999e5d3338439bae7599d95143d26410de038` passed both the [QuadDi/PentaDi/PentaPi workflow #45325162042](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45325162042) and [DIAERY regression #45325161989](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45325161989). The five-gate job printed the following actual source-derived proof:

```text
PENTADI_PROFILER|HISTORICAL_KEEP_PASS=SEALED|SAFETY_OVERRIDE=WIRED|TIMING=MEASURE_ONLY|RELEASE=HOLD
PENTADI|C11_TO_ASM=PASS|COMPILER=MSVC|OPTIMIZATION=O2|PRODUCER=C11_NOT_DELPHI7|RELEASE=HOLD
PENTAPI_TCP|LOOPBACK=PASS|IPV4_TTL=255|SET_BEFORE_CONNECT=PASS|TCP_FRAMING=PASS|ECHO=PASS|ON_WIRE_TTL=NOT_TESTED|NO_EXTERNAL_NETWORK=PASS|EVIDENCE_SHA256=604f3b3ea0fa1225967974d342fa8b9e5776a6a67a94e4ad64a3f2d7fc193f2a|RELEASE=HOLD
PENTAPI_GATE|01_QUADDI_DEPENDENCY|PASS
PENTAPI_GATE|02_ASM_PROVENANCE|PASS
PENTAPI_GATE|03_C11_RECORD_INTEGRITY|PASS
PENTAPI_GATE|04_PROFILER_CUSTODY|PASS
PENTAPI_GATE|05_TCP255_AND_NO_MUTATION|PASS
PENTAPI|LAP=001|PRECHECK=5_OF_5|PROFILER=WIRED|TCP_INITIAL_IPV4_TTL=255|DELPHI7_COMPILER=HOLD|NO_AUTO_MUTATION=PASS|LEDGER_SHA256=df577758fab806b004a79f9ceff4b40683fda8770187cf5bcbec4040daf6520a|RELEASE=HOLD
```

The native `.asm` file was generated by **MSVC C11 `/FA`** and remains a runner evidence output; its SHA-256 was `57ea91bfc02e77461eadf83d2c0aed901974aabee76a05c4d9543f577d3f67f1`. Neither this audit nor the green workflow proves Delphi 7 compilation, on-wire TTL after routing, or executable performance acceleration. **Delphi 7 DCC32 remains HOLD.** This evidence-update document commit occurs after the tested code commit; it does not rewrite earlier compiled test results.

**RELEASE=HOLD, draft PR #95.**
