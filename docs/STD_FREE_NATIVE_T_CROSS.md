# STD-free native C++20 XOR engine: T-CROSS +/-DIFF

**Status: CANDIDATE SOURCE COMMITTED; COMPILE, LINK, RUNTIME AND WHOLE-REPOSITORY VERIFICATION PENDING.**

This record applies to the *separate throughput byte-stream engine*. It does not silently alter the established five-input parity-probability algorithm, other C++ public header interfaces, Git history or previously frozen T-CROSS line counts.

## Frozen boundaries

- **Public:** `pxor::FixedBuffer<T,N>`, `ByteSlice`, and `StringSlice` are explicitly native C++20 ownership/borrow types.
- **Private:** `pxor::ObfuscatedData<N>` holds cipher bytes privately. Its encoded bytes are a compile-time-obfuscated literal; they are not cryptographic secrets.
- **Shared:** `ConstByteSlice` is a *read-only borrowed view*, not a cross-process shared-memory segment, a C# managed heap, atomicity or OS permission. `EngineAuditor` has intentional access to a limited ciphertext witness.
- **Scope cleanup:** `with_decrypted` accepts a void-returning callback and wipes its two local buffers with volatile writes in a destructor, including on ordinary C++ exception unwinding where enabled. This is best effort and cannot erase compiler register copies, retained pointers, untrusted callback snapshots or other replicas.
- **Language level:** strict ISO C++20, no `if consteval` (C++23), standard-library `std::is_constant_evaluated`, `std::span`, `std::views`, compiler-specific `__builtin_memcpy`, or other `std::` APIs in the original four throughput source files, now extended by three Suite source files.
- **System boundary:** Windows uses native Win32 headers/functions, and Linux uses POSIX/Linux headers/functions (`write`, `clock_gettime`, `sched_setaffinity`, `setpriority`, `mmap`). These are external **system API calls**. The source being STD-free is **not** proof of a freestanding executable, absence of CRT/libc, absent binary imports, or a zero-CALL machine-code program.

## T-CROSS source mapping

| Component | - Previous candidate | + STD-free candidate | Evidence |
| --- | --- | --- | --- |
| `include/pxor_memory.hpp` | `<cstddef>`, `<cstdint>`, `std::size_t` aliases | Compiler-width checked `unsigned char`, `int`, `long long`, `decltype(sizeof(0))`; no standard includes | Source inspected |
| `include/pentagonal_xor.hpp` | `<array>`, `<span>`, `<ranges>`, `<cstring>`, `std::is_constant_evaluated` | `FixedBuffer`, `ByteSlice`, manual lazy cursor, constexpr `pack_u64_le` / `unpack_u64_le`, runtime-only scoped-wipe callback | Source inspected |
| `src/main.cpp` | `vector`, `unique_ptr`, `chrono`, `from_chars`, `printf` | Fixed 16 MiB storage; native Win32/POSIX output, clock, bounded decimal parsing; native OS page mapping only for opt-in larger sizes | Source inspected |
| `tests/pentagonal_xor_throughput_test.cpp` | `vector`, `array`, `span`, `type_traits`, `puts` | Built-in arrays, compile-time pure XOR/LE tests, 14 runtime tail and alignment tests, read/write borrow checks | Source inspected |
| CMake installation | Only `pentagonal_xor.hpp` installed | Both `pentagonal_xor.hpp` and `pxor_memory.hpp` installed to `include/swe` | Source inspected |
| Binary standard-library linkage | Not separately inspected | No claim; actual import table and runtime dependencies remain to be verified | **NOT TESTED** |

## Explicit semantic / compatibility notes

1. **Exact seeded vs periodic LUT remains different:** `apply_pentagonal_xor(ByteSlice, seed)` uses finite-width `P_(seed+i)`; `fast_xor_buffer(ByteSlice)` repeats 256 compile-time key bytes. The first 256 positions for seed 1 coincide, but offsets after 255 are not interchangeable.
2. **Endian and aliasing:** Pack/unpack explicitly shifts individual bytes, so unaligned input does not invoke unaligned 64-bit pointer dereferences, strict-aliasing violations or endian-dependent whole-word byte swaps. Compilers may or may not optimize the eight-byte loop into a particular instruction sequence.
3. **Library-free headers are not automatically a faster executable:** Multiple shifts and stores can be slower than optimized `memcpy`; full benchmark comparisons require the same CPU, payload, thermal state, affinity, and repetition counts.
4. **Earlier C++ API compatibility:** The preceding `std::span<std::byte>` facade and `std::views`-based sequence were replaced by raw borrowed byte views and `PentagonalSequence::Cursor`. This is an intentional API change for the *new exploratory throughput module*, **not** proof that historical downstream consumers remain source-compatible.
5. **No allocation/library silence:** The default 16 MiB BSS buffer exists before `main` but is explicitly first-written only after the affinity request. Larger buffers use Win32 `VirtualAlloc` or POSIX `mmap` and are released. This may still involve platform dynamic libraries and does not guarantee NUMA placement.
6. **Non-standard platform APIs remain by necessity:** `#include <windows.h>` and Linux/POSIX system headers are the only platform includes in the throughput header. This is *standard-library-free source*, not a claim of "no external calls" or "no system libraries."

## POSIX T-CROSS gate execution

```sh
sh ci/t-cross-posix.sh gate-no-std-throughput
sh ci/t-cross-posix.sh gate-no-std-repository
sh ci/t-cross-posix.sh build-cpp20
sh ci/t-cross-posix.sh test-ctest
sh ci/t-cross-posix.sh emit-diff-yaml > .iron-t-cross-diff.yml
```

The first gate scans seven named throughput/Suite source files: the original four plus `include/pentagonal_suite.hpp`, `src/suite_main.cpp`, and `tests/pentagonal_suite_test.cpp`. The separate `gate-suite-no-comments` rejects comment syntax in the three Suite files. The second inventories **all tracked C/C++ source paths**, and must report a failure whenever older modules still use a standard-library API. For example, the unchanged `pentagonal_xor_cpp20.cpp` still uses `std::stod`, `std::iostream`-related streams and standard exceptions. Other header and test source files also remain to be migrated individually. No deletion or silent reclassification is authorized to make the gate pass.

`Taskfile.yml` includes both gates in the default set and retains a Git-derived YAML +/-DIFF exporter. The current strict default is expected to block at the repository-wide audit unless those existing dependencies are removed and measured. Targeted CMake/CTest commands can be invoked independently for testing.

## Acceptance

- **Source check:** No `std::`, C/C++ standard headers or C standard library calls in the seven named native throughput/Suite source files, based on submitted source changes. The POSIX check itself has not been independently run.
- **C++20 compile:** Must be run with GCC/Clang `-std=c++20` and MSVC `/std:c++20`, with extensions OFF. NOT TESTED in this PR checkout.
- **CTest:** Preserve original parity tests; add and execute new constexpr packing, 14 length/alignment checks, stream divergence, ciphertext access, scoped cleanup tests. NOT TESTED in this PR checkout.
- **Binary/runtime:** Inspect direct imports of both `pentagonal_xor_cli` and `pentagonal_suite_cli`, standard library/CRT dependencies, memory mappings, machine code and behavior on native Windows and POSIX hosts. NOT TESTED.
- **Whole repository:** NOT COMPLIANT yet. The original parity CLI and other historical C++ sources use `std::`. The gate must remain red until they are individually migrated and validated.
- **Release state:** PR #19 stays DRAFT and UNMERGED. No source audit substitutes for compiled test evidence.

**Status policy:** absence of `std::` tokens in selected source is not a substitute for a successful linker/import-table audit. Existing code's use of standard libraries is not erased by this new module.

## Additional Pentagonal Suite scope

The new [Pentagonal Suite operation protocol](PENTAGONAL_SUITE.md) is the current source contract for XOR, OR, AND, NAND, NOP, NIL, NAN, COR and VISTA. Its three new C++20 files are STD-free and comment-free on source inspection. The existing POSIX source gate now checks seven new C++20 files, and the binary-import gate separately examines the original throughput CLI and new Suite CLI. This does not change the status of older repository files: whole-repository STD-free compliance and runtime-library freedom remain unproven or blocked. The exact source delta is kept in [IRON T-CROSS](IRON_T_CROSS_REVIEW.md) with a frozen Git comparison, and the local YAML exporter derives future Git counts automatically.
