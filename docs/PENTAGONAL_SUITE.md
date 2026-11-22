# Pentagonal Suite Engine: ISO C++20 operation protocol

**Historical shell instructions:** POSIX commands and shell behavior described below are preserved as historical protocol/evidence. The 17 wrappers are removed from active CI; links target archived text. See the [discarded-files registry](../discardedfiles.md). Retained task names invoke the native fixed transcript and exit 42 / **RELEASE=HOLD**, without performing these former operations.

**Status: EXPLORATORY source committed in draft PR #19.** All new suite source files contain zero C/C++ standard-library includes, zero `std::` references and zero code comments. Do not apply this status automatically to the existing parity, legacy tools, whole repository or final linked binaries.

## Source and authority

- [`include/pentagonal_suite.hpp`](../include/pentagonal_suite.hpp): additive source of the nine suite operations. Includes the existing [`pentagonal_xor.hpp`](../include/pentagonal_xor.hpp) and reuses its original pentagonal function, 256-byte keystream LUT, exact XOR, fast XOR and little-endian pack/unpack.
- [`tests/pentagonal_suite_test.cpp`](../tests/pentagonal_suite_test.cpp): independent source-level and constexpr checks for seeded and periodic mode results, 14 buffer lengths, unaligned offsets, NAN tails, bounded COR/VISTA behavior and integer-limit traversal.
- [`src/suite_main.cpp`](../src/suite_main.cpp): separate native Windows/Linux demo and timed suite pipeline. The existing throughput benchmark CLI and five-feature parity CLI are preserved.
- [`Taskfile.yml`](../Taskfile.yml) and [`ci/t-cross-posix.sh`](../Archive/Retired/ci/t-cross-posix.sh.txt): automatic Git-derived ±DIFF YAML exporter and fail-closed STD-free/comment-free validation gates.
- **Identity:** These are separate bytewise operations. None is a replacement for the established five-input odd-parity probability calculation.

## Explicit operation matrix

| Name | Entry point | Mathematical operation | Stream |
| --- | --- | --- | --- |
| PENTAGONAL-XOR | `apply_pentagonal_xor` / `fast_xor_buffer` | `B ^ K`; reversible when same stream repeats | Exact seeded / separate 256-byte periodic LUT |
| PENTAGONAL-OR | `apply_pentagonal_or` / `fast_or_buffer` | `B \| K` | Exact seeded / periodic |
| PENTAGONAL-AND | `apply_pentagonal_and` / `fast_and_buffer` | `B & K` | Exact seeded / periodic |
| PENTAGONAL-NAND | `apply_pentagonal_nand` / `fast_nand_buffer` | `~(B & K)` truncated to 8 bits | Exact seeded / periodic |
| PENTAGONAL-NOP | `apply_pentagonal_nop` / `fast_nop_buffer` | Leave buffer untouched | No key read, no writes |
| PENTAGONAL-NIL | `apply_pentagonal_nil` / `fast_nil_buffer` | Fill output with zero bytes | No key read |
| PENTAGONAL-NAN | `apply_pentagonal_nan` | Store quiet-NaN bit patterns in little-endian 8-byte units | Indexed `P_n` masked to 51 payload bits |
| PENTAGONAL-COR | `PentagonalCor` | Stateful generated pentagonal sequence with optional count | Index supplied by cursor |
| PENTAGONAL-VISTA | `PentagonalVista` | Non-owning lazy counted sequence with `operator[]`, iterator and subvista | Index supplied by view |

**Key-stream provenance matters.** The exact seeded byte stream at offset `i` derives `P_(seed+i)`. Fast LUT operations repeat the 256 keys for `P_1..P_256`, indexed `i & 255`; after index 255, the streams generally differ. Fast and seeded forms are **not interchangeable** in an existing encoded record. A transform is not an authenticated cipher or a substitute for cryptography.

## NAN contract and data safety

Each complete eight-byte block is the **little-endian integer bit pattern** `0x7FF8000000000000 | (P_n & 0x0007FFFFFFFFFFFF)`. This is a binary64 quiet-NaN encoding if interpreted on a conforming IEEE-754 64-bit floating-point implementation. The code does not alias, cast or dereference these integer bytes as a `double`.

The final 1–7 bytes of a nonmultiple-of-eight output are set deterministically to `0xFF`. They are not a complete NaN value. `is_pentagonal_nan(bits)` tests whether a bit pattern has the expected *quiet-NaN tag*, **not** whether the payload could only have originated from this generator. `extract_nan_payload` retrieves the low 51 bits and is not an inverse pentagonal mapping.

NaN boxing is a data-format demonstration, **not** a cryptographic integrity seal or a general-purpose floating-point payload transport guarantee. Some hardware and software normalize NaN payloads when floating-point operations touch them.

## COR/VISTA boundary behavior

`PentagonalCor(start,count)` tracks **remaining count**, rather than calculating `start+count`, and `done()` signals exhaustion. `try_next(out)` preserves `out` on exhaustion. When `current` reaches the maximum representable unsigned 64-bit index, it emits that index once and then stops rather than silently wrapping to zero. `next()` yields zero when exhausted, so callers requiring a distinct exhaustion signal should use `try_next`.

`PentagonalVista(start,count)` clamps its count at the 64-bit index boundary, avoids incrementing an iterator beyond its final element, and returns an empty subvista for an out-of-range request. Indexed access `operator[]` requires `offset < size()`; the view does not own, allocate or lock memory. The resulting pentagonal values remain finite-width `u64` arithmetic and may wrap *mathematically* at large indices even when the cursor itself is bounded.

## Execution and performance expectations

```sh
sh ci/t-cross-posix.sh gate-no-std-throughput
sh ci/t-cross-posix.sh gate-suite-no-comments
sh ci/t-cross-posix.sh build-cpp20
sh ci/t-cross-posix.sh test-ctest
sh ci/t-cross-posix.sh emit-diff-yaml > .iron-t-cross-diff.yml
```

The local native suite demo executes a 16 MiB sequence of **NOP → XOR → OR → AND → NAND**, takes a monotonic time measurement, computes a witness before further mutations, writes two NAN blocks, then applies NIL. It requests moderate priority and best-effort CPU affinity, not Windows time-critical priority. These calls do not guarantee CPU residency, NUMA locality, constant-time execution, or any particular speedup. NOP is an O(1) pass-through, **not a constant-duration full-buffer memory access**. Ordinary NIL zero-fill does not guarantee secure erasure if a compiler can prove the result is unused.

CMake adds `pentagonal_suite_test` and `pentagonal_suite_cli` alongside the existing targets, and installs the suite header. No GitHub Actions workflow is introduced. All suite code is written in ISO C++20 with no standard headers or `std::` names. Windows/Linux system APIs and possible C/POSIX runtime dependencies remain to be audited in linked binaries. The strict whole-repository STD-free gate still detects older standard-library code.

**T-CROSS acceptance remains HOLD until the actual candidate has been built and run.** A green source scanner is not a compiler pass. A passing CTest suite is not a zero-runtime-import or zero-CALL proof. The PR must remain draft and unmerged until the relevant gate outcomes are recorded.
