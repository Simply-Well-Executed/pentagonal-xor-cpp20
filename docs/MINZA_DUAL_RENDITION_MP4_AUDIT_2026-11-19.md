# MINZA dual-rendition MP4 inspection · 2026-11-19

**Status:** verified container/NAL observations + open visual questions. **Protocol ID:** `MINZA-DUAL-MP4-20261119-O2-v1`. **Branch:** `audit/minza-dual-rendition-2026-11-19`. Original videos and handwritten image notes are user-held, unmodified source material and are **not** distributed with this repository.

This report applies the user-named sequence **PADLOCK AND FLY → TOUGH SHIT! → THAILONGA** as three **research/audit stages**. These labels are not established cryptanalytic functions and do not imply a universal decryption method. The canonical Pentagonal XOR kernel, six-stage scheduler, and existing repository documents remain unchanged.

## Sources and exact media identities

| Field | Source A: user-supplied `ef9lUKuZwT81L9CN(2).mp4` | Source B: X player screen recording `2026-11-18 21-15-19(2).mp4` |
| --- | --- | --- |
| SHA-256 | `2ebc7763648c9c025ebc1c7d882c4300e2b9dba4fcae6260cf15291c2584ffb9` | `9b351e2f339e11b7bc2e10d9a377e4c72a9ed2ac1c3701951c935ab10a6dc0e3` |
| MP4 declared tracks | 1 `avc1` video, 1 `mp4a` audio | 1 `avc1` video, 1 `mp4a` audio |
| Media timescale | 3,000,000 ticks/s | 60 ticks/s |
| Video decoding-time sample duration | 100,100 ticks/sample | 1 tick/sample |
| Video sample count | 5,215 | 11,154 |
| Computed encoded rate | 3,000,000 / 100,100 = **30000/1001 ≈ 29.97003 samples/s** | 60/1 = **60 samples/s** |
| Track duration | ~174.01 s | 185.90 s |
| AVC NAL inventory | type 1: 5157; type 5: 58; type 6: 21; type 7: 58; type 8: 58 | type 1: 11109; type 5: 45; type 6: 11199; type 7: 1; type 8: 1 |
| SEI message kinds | type 5: 21 | type 0: 45; type 1: 11154 |
| Malformed sample/NAL length boundaries found by supplied parsers | 0 | 0 |

**Provenance:** Both local media files were read directly using the user-supplied standalone C++20 MP4/NAL/SEI inspectors. All three inspectors were **recompiled and rerun with `g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2`** on 2026-11-19. They were previously compiled at a different optimization level in an earlier audit; that historical result is preserved, but it is **not** the specified build protocol here. Existing parsers can still be hardened further against malformed MP4s; success on these files is not general parser-safety certification.

**Source A SEI clarification:** All 21 type-5 SEI messages expose UUID `dc45e9bd-e6d9-48b7-962c-d820d923eeef`; printable payload strings identify `x264 - core 164 r3095 baee400` and its encoder options. This is consistent with ordinary x264 unregistered encoder metadata, not a second encoded video track. Do not treat unregistered SEI bytes as encrypted content merely because they are not rendered as pixels.

## PADLOCK AND FLY: source custody and structural gates

1. Lock source paths, byte counts, SHA-256, recording origin, container structures, timescales, edit/presentation timelines, decoder provenance, and renderer settings *before* comparing results.
2. Inspect `ftyp/moov/trak/mdia/minf/stbl/stts/ctts/stsz/stsc/stco|co64/stsd/avcC/mdat` using validated, bounded offsets, including valid extended sizes. An MP4 track count is not equivalent to the number of *visibly distinct scenes*.
3. Parse AVC NAL units within each sample, respecting length-prefix width and emulation-prevention bytes. Treat SEI messages as typed metadata; record their actual payload formats. Examine standard protection markers with structurally valid box traversal rather than unrestricted raw substring searches.
4. Preserve the **two separately sourced recordings** as independent witnesses of their respective media encodings; neither is authenticated as the same underlying network-delivered bitrate/resolution rendition as the other.
5. Preserve raw handwritten front/back notes as images. The apparent `LRI`, `FSI`, `PDI`, `ZWJ`, and `ZWNJ` tokens refer to Unicode bidi isolation/join-control semantics, not established cryptographic keys. Transcription and directionality must be reviewed before defining any operators. Never silently discard or reorder graphemes, isolate initiators, or PDI closes.

## TOUGH SHIT!: falsifiable alternatives and tests

| Hypothesis | Required comparison | Status |
| --- | --- | --- |
| A file contains two independently addressable video tracks | Structurally valid `trak` inventory | **Contradicted in both supplied files** |
| Source B has 60 presentation samples per second | Sample-table timescale/duration | **Verified** |
| Source A contains 29.97 encoded samples per second | Sample-table timescale/duration | **Verified** |
| A 30-ish-fps picture is repeated within a 60-fps capture | **Decoded** adjacent pixel-frame similarity, motion registration, tolerance distribution and even/odd phase, timestamp alignment | **Open** |
| X inline preview and fullscreen show different actual pictures | Capture both player modes from same URL with timecode, crop, stream URL/representation, player version, snapshot hashes | **Open** |
| One rendition contains hidden encrypted video | Identify a second bitstream, reproducible extraction map, valid codec syntax or authenticated ciphertext structure, and a demonstrable reconstruction | **Unsupported so far** |
| Picture changes result from AI editing | Original signed/raw source, encoder/upload history, reproducible content comparisons; frame rate alone is insufficient | **Open** |

**Important distinction:** zero byte-identical *compressed* neighboring samples does **not** prove zero duplicated **decoded** images. P-frame prediction, quantization, time-varying metadata and encoder state can alter payload bytes while the underlying picture remains nearly the same. Likewise, a filename, preview image or player-declared FPS is not proof of semantic uniqueness.

A screenshot crop, platform thumbnail, autoplay preview, full-screen scaling, adaptive bitrate, browser composition or time-offset mismatch are competing explanations for a preview/fullscreen difference. A higher FPS container can carry repeated, interpolated or genuinely new pictures; there is no universal rule that all AI-generated videos max out at 30 fps.

**Explicit experiment boundary:** no authenticated per-frame pixel decode of both exact sources with a locked decoder build was performed under this O2 audit. An earlier screen-recording pixel-hash list has unverified decoder provenance and must stay *provisional*. Do not retroactively relabel it `VERIFIED`.

## THAILONGA: non-destructive comparison and witness ledger

Use this read-only sequence:

`source bytes → source hash → MP4 boxes → declared streams → NAL/SEI → decoded presentation frames (future) → registered visual comparison (future) → findings → audit`.

If a two-sided visual display is built, label the views **SOURCE A** and **SOURCE B**, with individual checksums and timestamps. A same-source crop must instead say `SAME SOURCE / SECOND VIEW`. Display overlays/rotation/texture plates are derived views, not new originals. The existing MINZA five-sample temporal gates can provide descriptive similarity masks, but must not be interpreted as decryption or as an independent provenance witness.

A five-input XOR/parity reduction generally loses information and cannot invert an arbitrary unknown cipher. Authorized decryption is possible with proper keys and algorithms, but the existence of a legitimate plaintext-recipient does not imply ciphertext-only recovery is feasible. Likewise, Unicode bidirectional controls change **display order**, not underlying secret-key entropy.

## Reproduction on Windows PowerShell 7

Run from the directory containing the uploaded C++20 inspector sources and the MP4s. The commands **compile only with -O2** and do not mutate media:

```powershell
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2 'MINZA_MP4_Native_Sample_Audit(1).cpp' -o MINZA_MP4_Probe.exe
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2 'MINZA_H264_NAL_Sample_Audit(1).cpp' -o MINZA_AVC_Probe.exe
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2 'MINZA_H264_SEI_Native_Audit(1).cpp' -o MINZA_SEI_Probe.exe
Get-FileHash -Algorithm SHA256 'ef9lUKuZwT81L9CN(2).mp4','2026-11-18 21-15-19(2).mp4'
foreach ($media in @('ef9lUKuZwT81L9CN(2).mp4','2026-11-18 21-15-19(2).mp4')) {
  & .\MINZA_MP4_Probe.exe $media
  & .\MINZA_AVC_Probe.exe $media
  & .\MINZA_SEI_Probe.exe $media
}
```

**Build note:** `-O2` is an optimization setting, **not** a memory-safety policy or proof. Future parser hardening should use explicit integer-overflow/bounds checks, fuzzing, and optional `-fsanitize=address,undefined` builds **while retaining -O2**. The original files and audit stages are independently versioned.

## History conflict: previous 60 fps description

The user reports that an earlier conversation titled **Image inscription analysis** described `ef9lUKuZwT81L9CN` as 60 fps. An associated JSONL transcript was **not present among the files available to this audit**, so the historical wording and file identity have **not been verified**. Treat this as **NEEDS REVIEW**, not an assertion that the earlier statement never existed. The **current uploaded bytes**, identified by SHA-256 above, conclusively declare `30000/1001` encoded samples/s. A different export/transcode, earlier metadata, or earlier source attribution could explain a discrepancy; none is established.

**No mutation after analysis.** If a decoder, crop, tolerance, timestamp mapping, normalization or source file is changed, issue a new protocol ID, retain the earlier evidence and mark the two results non-comparable until separately validated.

Related repo methods: [MINZA temporal gate definitions](MINZA_TEMPORAL_PENTAGONAL.md), [provenance and projection rules](MINZA_PINWHEEL_ATLAS.md), [document-review register](DOCUMENT_REVIEW_REGISTER.md), and [0BSD scope](LICENSE_AND_MARKS.md). Third-party media rights remain with their holders.
