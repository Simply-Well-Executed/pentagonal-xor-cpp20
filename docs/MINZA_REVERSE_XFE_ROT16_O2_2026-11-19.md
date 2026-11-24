# MINZA byte/bit mirror + XFE/EFX + ROT16 experiment

Experiment ID: MINZA-REV-XFE-ROT16-20261119-O2-v1
Status: completed, read-only source audit; exploratory transform, not established decryption.

## Sources, unmodified

- `ef9lUKuZwT81L9CN(2).mp4` (78,434,426 bytes): SHA-256 `2ebc7763648c9c025ebc1c7d882c4300e2b9dba4fcae6260cf15291c2584ffb9`.
- `2026-11-18 21-15-19(2).mp4` (54,255,103 bytes): SHA-256 `9b351e2f339e11b7bc2e10d9a377e4c72a9ed2ac1c3701951c935ab10a6dc0e3`.

## Operations

1. Reverse complete source file **byte order**. This does **not** reverse decoded video playback frames.
2. Construct exact literal `XFE` (hex 58 46 45) + reversed bytes + `EFX` (45 46 58). This is a private envelope, **not** a standard QuickTime atom container.
3. Scan for genuine FLV (header plus two correctly sized tags), AVI (RIFF AVI with HDLR/MOVI chunks), and MP4/MOV atom structure, rather than raw three-letter signatures alone.
4. On the unsuccessful wrapper, test ROT16 under alternative explicit definitions: Caesar alphabet shift +16 and -16; byte addition +16 and -16 modulo 256; 32-bit word half-swap (16-bit word rotation). Apply to wrapped payload only and also to full envelope.
5. Repeat steps 2-4 with the **entire bitstream reversed**, i.e., reverse byte order and reverse bit order within each byte. This is a separate interpretation of 'total mirror.'
6. For both variants, remove envelope and reverse the transform to confirm an exact byte-for-byte source restoration.

## Findings

- The wrapper was assembled correctly and could be fully reversed to the exact input for both videos.
- Both bit-mirror interpretations also restored their originals exactly.
- **No** tested transformed output contained a structurally valid FLV tag chain, AVI RIFF stream or parseable QuickTime MOV with a declared audio/video track. Some transformations produced accidental raw `FLV` or `RIFF` text sequences, but structural checks rejected them.
- FFprobe independently rejected the saved byte-reversal/wrap outputs and representative ROT16 outputs with `moov atom not found` / `Invalid data found when processing input`. The original media files parsed as H.264 video plus AAC audio.
- A failed signature/format test cannot exclude a novel secret format, an arbitrary steganographic payload, or content hidden in pixel values; it only establishes that these **specified transforms** did not reveal a standard container.

## Reproduction using PowerShell 7 (Windows, g++ C++20)

Place the C++ source beside the two input files, compile with **-O2**, and run:

```powershell
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2 .\minza_reverse_wrap_rot16.cpp -o .\minza_reverse_wrap_rot16.exe
.\minza_reverse_wrap_rot16.exe '.\ef9lUKuZwT81L9CN(2).mp4' '.\A' summary | Tee-Object .\A_results.txt
.\minza_reverse_wrap_rot16.exe '.\2026-11-18 21-15-19(2).mp4' '.\B' summary | Tee-Object .\B_results.txt
```

Use `write` instead of `summary` to create a small selection of full-length experimental output files (`_reverse_XFE_EFX.mov`, `_reverse_alpha_plus16_inside.mov`, `_reverse_byte_plus16_inside.mov`), preserving source media.

The program's structural MOV check deliberately requires a modern `ftyp` declaration and an audio/video `trak`; legacy QuickTime files can omit `ftyp`. This does not claim a complete MOV validator. The acronym `ROT16` does not denote one unambiguous binary format; listed variants are explicitly defined for reproducibility. For real playable MOV containers use correctly sized QuickTime atoms and valid track/sample tables; an empty `moov` or three-byte wrapper is not enough.

Source media is not included in this package. This experiment does not replace the earlier container/NAL audit. Any new frame-mirroring experiment should use a new protocol ID and decode/transform/render pixels without losing the source timeline.