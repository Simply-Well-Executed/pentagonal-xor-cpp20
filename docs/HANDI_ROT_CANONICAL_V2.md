# HANDI ROT16 / ROT10 canonical rotations, v2

**EXPLORATORY native C++20 display optimizer.** This optimizes only the ASCII 26-letter mechanical Caesar turns, not binary/word rotates, experiment counts, durations, output status codes or historical protocol identifiers.

## Canonical reduction

| Historical request | Equivalent action, same 26-letter alphabet | Applied in current code |
|---|---|---|
| Outer ROT42 | 42 mod 26 = 16 | ROT16 |
| Center ROT36 | 36 mod 26 = 10 | ROT10 |
| Outer ROT42 | 42 mod 26 = 16 | ROT16 |

Current projection: **ROT16 / ROT10 / ROT16**, with exactly three output letters. `SEX → ION`, `COREAO → SYH`, `YIN → OSD`; source bytes and suffix remain in their original source. A bounded 3-letter display is lossy and collision-prone, not encryption or identity.

## Source-wide audit and exclusions

- `shared/handi_rot_triad.hpp` uses already reduced `outer_shift=16` and `center_shift=10`. Its generic `rotate` helper accepts arbitrary shifts while reducing modulo 26 exactly once, then uses a single conditional wrap. No additional runtime loop is introduced.
- `tools/minza_permutation_contract.hpp` already reduces `n mod 26` and tests the historical user-requested ROT/EBG 1..256 test matrix. It is unchanged, since enumerating protocol inputs is not a costly algorithmic rotation and the historical study must remain comparable.
- `tools/minza_reverse_wrap_rot16.cpp` already uses explicit ROT16. It is unchanged. Other bit, byte, word, geometric, radio, temporal and protocol rotations are not automatically modulo 26; replacing their constants would change their semantics.
- The native Windows source-audit step scans active C++, C, PowerShell, Java, JavaScript, TypeScript, and XHTML files for unnormalized literal ROT42/ROT36 alphabetic instructions, excluding frozen Archive and license reference contents. It cannot prove all arbitrary dynamic expressions normalized, and is not a substitute for algorithm review.
- Existing published v8 document `docs/HANDI_SIGMAPI_COREAO_V8.md` remains historical. The v1 header is archived byte-for-byte at `Archive/Retired/shared/handi_rot_triad-v1.hpp.txt`, Git blob `a0dfb8717598a1cdcc9dc9760951c56f67e19878`. The optimized v2 source is a new explicit version, not a rewrite of that history.
- Native test verifies the projected output and full-turn equivalence; the prior test's failing `Visual Studio 17 2022` generator was incompatible with GitHub's current Windows 2025 / Visual Studio 2026 runner. CI now requests the Visual Studio 18 2026 generator. Results must be read from actual runs before claiming a PASS.

**Exit 42 is NOT an alphabet rotation** and remains unchanged. Source-only POSIX/GNU release HOLD remains unchanged and is not suppressed.

**Chosen, never assumed.**
