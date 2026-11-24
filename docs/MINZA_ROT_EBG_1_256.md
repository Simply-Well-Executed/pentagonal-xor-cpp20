# MINZA: lilcaesearz ROT/EBG 1–256 audit

**Protocol:** `MINZA-LUZUROT-EBG-1-256-20261119-O2-v1`  
**Status:** measured bounded structure scan, no recovered auxiliary media tracks.  
**Source:** [pcheek1991/lilcaesearz, `main.ps1`](https://github.com/pcheek1991/lilcaesearz/blob/master/main.ps1). The source's class `LuZuRot` exposes `ROT13(+13)` and **`EBG13(-13)`**. `EBC` in the owner's prompt is recorded as the requested alias, **not** EBCDIC and not a third independently defined cipher.

The source algorithm normalizes the signed shift modulo **26**, rotates ASCII A–Z and a–z, and preserves all other characters. Its public C# input is a sanitized string and rejects embedded NUL characters. The user-supplied MP4 files contain arbitrary binary octets, so this audit ports **only the characterwise Caesar mapping** to unsigned bytes without passing whole media files through the C# text interface. The experiment does **not** claim to decrypt a codec or to use the source program's process-local signing-key machinery as a media key.

## Input custody

| Local file | Byte count | SHA-256 |
| --- | ---: | --- |
| `ef9lUKuZwT81L9CN(2).mp4` | 78,434,426 | `2ebc7763648c9c025ebc1c7d882c4300e2b9dba4fcae6260cf15291c2584ffb9` |
| `2026-11-18 21-15-19(2).mp4` | 54,255,103 | `9b351e2f339e11b7bc2e10d9a377e4c72a9ed2ac1c3701951c935ab10a6dc0e3` |

Source SHA-256 values were recomputed after testing; no source bytes were modified. The files and any third-party media are not committed to the public repository.

## Enumerated hypotheses per source

| Parameter | Cases |
| --- | ---: |
| Signed family | 2: ROT(+n), EBG(−n) |
| n | 256 integers from 1 through 256 |
| Mirror | 4: none, reverse bytes, reverse bits per byte, both |
| Rotation scope | 2: payload-only, entire wrapper |
| Wrapper | 2: literal `XFE+payload+EFX`, QuickTime `ftyp+mdat` data-only control |
| **Total** | **8,192 configurations/source** |

Both video files total **16,384 configuration ledger rows**. Because `n` is normalized mod 26, and because the wrapper/scope choices do not change the *payload* under a characterwise ASCII substitution, only **104 distinct full-payload scans/source** were necessary (26 normalized shifts × 4 mirrors). The actual virtual wrapper's first bytes were separately calculated for every named row. These are **virtual transformed views**, not 16,384 materialized movie files or individual decoder runs.

## Measured scan results

| Metric, summed over 104 unique payload views | Video A | OBS recording |
| --- | ---: | ---: |
| `FLV` ASCII signature hits | 512 | 267 |
| Structurally valid FLV headers | **0** | **0** |
| FLV candidates with two valid, bounded tags | **0** | **0** |
| Structurally valid AVI header candidates | **0** | **0** |
| Unique views with at least one `FLV` string | 103 | 97 |
| Untransformed `FLV` string count | 5 | 2 |

Across each source's 8,192 virtual wrapper rows, **2,120** retain a syntactically valid QuickTime *outer* `ftyp/mdat` control header. This is expected **by construction**, not evidence of newly extracted tracks: no `moov/trak` was supplied in the wrapper. Literal `XFE/EFX` are **not** QuickTime atom syntax.

**Conclusion:** none of the alphabetic ROT/EBG1–256, mirrored/unmirrored, scope/wrapper combinations produced an identified FLV or AVI stream through the bounded scanner. The 512/267 `FLV` signature totals are appearances in different transforms, many repetitions, not track counts. A full decoding/cryptographic-steganography exclusion was not conducted, so this is a negative result for **this algorithm and these validations**.

## Testing and implementation

- Existing [MINZA permutation contract](../tools/minza_permutation_contract.hpp) now includes `rot_ebg_letter(c,n,ebg)` and compile-time `ROT(+n)→EBG(−n)` roundtrips for each byte and n=1..256, while keeping prior `rot16_letter` behavior.
- [New verification test](../tests/minza_luzurot_ebg_256_test.cpp) checks forward/reverse shifts, 8,192-case matrix size and non-mutating byte transformations.
- A separate standalone full-file C++20 structural probe was compiled and run locally with **`-O2`**. The reproduction code, unit fixtures, and both **complete JSONL ledgers** were supplied to the user as the experiment attachment, rather than adding personal third-party media to this repository.
- The probe's **positive controls** detected a synthetic FLV file with two valid tags and a synthetic RIFF/AVI `LIST hdrl` header. A malformed FLV version was rejected. Unit controls also passed with `-O2 -fsanitize=address,undefined`. These checks improve validation of the test apparatus; they do not establish general parser safety.
- The operation order is **mirror source bytes/bits → signed Caesar substitution → wrap**, with optional rotation of the wrapper bytes. Changing this order or Caesar definition requires a new experiment ID; do not rewrite historical results.

For the original source's two standard APIs, `ROT13` and `EBG13` are mutually inverse but happen to produce the same shift on letters (since `+13 ≡ −13 mod 26`). For general n, EBG(−n) is the inverse of ROT(+n), while all 256 requested values cycle through the same 26 effective shifts.

**Provenance:** original media > SHA-256 > algorithm and exact case index > virtual octets > signature scan > structural validation > audit. `-O2` remains a build requirement; it is not a substitute for memory safety. No additional video tracks are inferred from character-string counts.

Related: [initial dual-rendition audit](MINZA_DUAL_RENDITION_MP4_AUDIT_2026-11-19.md), [temporal five-sample gates](MINZA_TEMPORAL_PENTAGONAL.md).
