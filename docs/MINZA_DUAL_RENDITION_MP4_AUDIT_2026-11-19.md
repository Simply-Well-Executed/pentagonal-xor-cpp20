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


## Codec-family expansion: SVT-AV1 LTO / PGO user-defined labels

**Protocol amendment:** `MINZA-SVT-BUILD-LABELS-20261119-v1`. **Status:** EXPLORATORY naming convention, not normative ITU-T codec identification. Added after the initial source inspection without altering its original results or protocol ID.

The owner proposes these labels for *SVT-AV1 encoder builds*:

| User-defined build label | SVT-AV1 build condition | On-disk bitstream codec |
| --- | --- | --- |
| `H.269` (experimental) | LTO enabled; PGO disabled | AV1 if standard SVT-AV1 output |
| `H.268` (experimental) | PGO enabled; LTO disabled | AV1 if standard SVT-AV1 output |
| `SVT-BASE` | Neither LTO nor PGO | AV1 if standard SVT-AV1 output |
| `SVT-LTO-PGO` | Both enabled | AV1 if standard SVT-AV1 output |

**Disambiguation:** upstream SVT-AV1 explicitly permits combining LTO and PGO; therefore the fourth row is a distinct combined profile, not silently `H.268` or `H.269`. A compiler's default LTO behavior must be explicitly inspected before claiming the PGO-only case. All experiment builds must enforce the owner's `-O2` policy in actual compiler invocation logs. LTO and PGO are compiler/linker optimization procedures, not codec specifications and not evidence of encryption.

**Codec-identification order:** file SHA-256 → valid MP4 `stsd` sample entry → codec configuration records → bounded parsing of the actual elementary bitstream → independently validated decoding → encoder provenance if available. For AV1-in-MP4, check `av01` and `av1C` plus AV1 open bitstream units; for AVC check `avc1`/`avc3` and `avcC`; for HEVC check `hvc1`/`hev1` and `hvcC`; for VVC check `vvc1`/`vvi1` and `vvcC` as appropriate. These codec names identify syntax families, not optimization recipes. If declared format and payload disagree, record a **mismatch requiring investigation**, not a speculative new codec identity.

**Retrospective limitation:** absent trusted compiler/encoder provenance, observing the compressed output alone does **not** establish that a binary was produced with LTO or PGO. Even an SVT-AV1 LTO/PGO executable cannot retroactively make an already-inspected AVC stream into AV1. Both supplied recordings still declare `avc1`; the user-defined H.268/H.269 labels are hypotheses or future comparison profiles, not observed properties of either recording.

**Reproducible comparison:** pin SVT-AV1 commit, platform/CPU, compiler and linker versions, full resolved flags, actual LTO and PGO settings, training video/corpus checksum for PGO, encoder CLI/preset/QP/CRF, input SHA-256, output stream SHA-256 and decoder test. Use identical input and encoder controls for all four variants. Compare encoding time, file size, decoded-frame hashes and objective metrics. A changed bitstream hash alone does not prove changed displayed scenes. Do not put third-party source recordings or frame crops into the public repository.

**Upstream references:**
- [SVT-AV1 project on GitLab (archived)](https://gitlab.com/AOMediaCodec/SVT-AV1)
- [Active SVT-AV1 upstream repository](https://github.com/AOMediaCodec/SVT-AV1)
- [Upstream LTO and PGO discussion](https://github.com/AOMediaCodec/SVT-AV1/blob/main/Docs/CommonQuestions.md)
- [AOMedia AV1 codec and bitstream specification](https://aomedia.org/specifications/av1/)

**Custody:** this amendment is newly exploratory; it leaves the original `MINZA-DUAL-MP4-20261119-O2-v1` evidence and the original media hashes unchanged. Distinguish user-defined build labels from standards and test results at every reporting layer.


## H.267: Doom9 community usage and official future-codec programme

**Separate protocol amendment:** `MINZA-H267-SOURCE-REVIEW-20261119-v1`. **Evidence level:** independently sourced terminology and ongoing research. This section does **not** modify the two video-file audits, change the codec identifiers already observed or redefine the prior LTO/PGO user labels.

The owner provided [Doom9's Forum](https://forum.doom9.org/) as a source for **H.267** video-codec discussion. The forum's archived threads contain discussion of H.267 as a prospective successor to H.266/VVC, **not** an umbrella name for all codecs discussed at Doom9:
- [Alliance for Open Media codecs, archived thread](https://forum.doom9.org/archive/index.php/t-172550-p-3.html) discusses experimental H.267 compression progress relative to VVC alongside AV-family encoder work.
- [Guesses on when AV2?, archived thread](https://forum.doom9.org/archive/index.php/t-182888.html) compares prospective H.267 timing against AV2, explicitly treating them as separate codec initiatives.
- [AOmedia next (AV2) codec, archived thread](https://forum.doom9.org/archive/index.php/t-184835.html) includes older estimates for H.267 timing; those projections are not certification or present-day publication status.

The strongest corroboration is primary: the [17 January 2025 ITU/ISO workshop programme](https://www.itu.int/en/ITU-T/Workshops-and-Seminars/2025/0117/Pages/programme.aspx) names a session **"Towards H.267/MPEG-Next: Current Status in JVET and Future Time Horizon"**. This establishes H.267 as a term genuinely used by video-coding standards experts for the next-generation effort, while **"Towards"** indicates development rather than completion.

The [ITU's July 28, 2026 Beyond-VVC Call for Proposals](https://www.itu.int/hub/2026/07/beyond-vvc-call-for-proposals-on-future-video-coding/) and [MPEG's August 12, 2026 meeting report](https://www.mpeg.org/155th-meeting-of-mpeg/) confirm a new standardization process, with **January 2027 evaluations** and **targeted first-version completion by end of 2029**. A historical Doom9 prediction of 2026/2027 ratification must not displace that newer, primary-source schedule.

### Source-to-claim boundaries

- **Supported:** H.267 is used as a prospective name for a next-generation **post-H.266/VVC** video coding effort. It is more than a made-up MINZA label.
- **Not supported:** all codecs discussed on Doom9 are collectively H.267. The forum covers H.264, H.265, H.266, AV1, AV2, many encoders and tuning forks as distinct things.
- **Not supported:** compiling SVT-AV1 using LTO or PGO turns its AV1 output into an H.267 format. The owner's separate experimental H.268/H.269 **build labels** continue to be recorded as labels, not normative identifiers.
- **Still verified for the two uploaded MP4s:** each declares `avc1` and has H.264-compatible SPS, PPS and NAL structure according to the cited local parsers. H.267 *terminology* cannot establish a second embedded codec stream, hidden image, encryption, or bitstream syntax that the file audit did not detect.
- **Further method:** if an alleged H.267 sample is supplied, preserve its source hash and exact research-model revision, parse it against that model's published syntax, cross-check with a compatible independently built decoder, and keep it out of the H.264/H.265/H.266/AV1 claims unless proven. No guesswork from file names or optimization flags.

Doom9 full-page retrieval was restricted in this audit; the forum-related findings are grounded in indexed thread text and corroborated with ITU/MPEG primary sources. No uninspected thread details are asserted.

## MINZA BiDi / TriDi-FLV / PiDi-AVI fall-down rescan

**New, separate experiment** `MINZA-CODEC-STACK-BIDI-TRIDI-PIDI-20261119-O2-v1`; **status:** native structural checks **verified** against both exact locally supplied files. This is an append-only amendment, not a revision to the earlier temporal report or the original video bytes.

### Hypotheses / meaning of the user-defined routes

- **Fall-down:** try H.269 → H.268 → H.267 → H.266 → H.265 → H.264, with a distinguishable state for **no declared track**, **unsupported/test not specified**, and **validated track**. H.269 (LTO-only SVT-AV1) and H.268 (PGO-only SVT-AV1) remain MINZA *build-profile aliases*, not standardized bitstream formats; for both, check the declared AV1 `av01` sample entry, but an AV1 stream alone cannot reveal its compiler flags.
- **BiDi:** enumerate structurally declared MP4 tracks in the `moov → trak → mdia → stbl → stsd` direction, then resolve every `stsc/stco|co64/stsz` sample back into a declared `mdat` range and verify all length-prefixed AVC NAL units. Bidirectional **consistency checking** is not a second video witness.
- **TriDi-FLV:** scan *all original file bytes*, including `mdat`, for the `FLV` header and validate version/flags, header length, first previous-tag-size, and at least two properly sized/type-constrained consecutive FLV tags. A bare `FLV` string is not a validated FLV movie.
- **PiDi-AVI:** scan *all original file bytes* for `RIFF` + `AVI `; validate little-endian RIFF chunk sizes and both `LIST hdrl` and `LIST movi`. A bare `RIFF` string is not a validated AVI.
- **Nested MP4 reconnaissance:** count plausible sized `ftyp` headers without treating a header candidate as proof of a second complete MP4 presentation.

The separate native inspector was compiled in C++20 with `g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2`. No Python, transcoding, visual interpolation, video pixel decoder, or writes to the original media were used. **Inspector source SHA-256:** `edb95f0c5dc6887ddc5ca0ca37be4666c0e7fb7e6f04c4f7d331eff1be66a250`. Keep this checksum paired with the locally supplied inspector `minza_codec_ladder.cpp`. The binary output is not a substitute for the source record.

### Direct measurements

| Metric | Source A `ef9lUKuZwT81L9CN(2).mp4` | Source B `2026-11-18 21-15-19(2).mp4` |
| --- | ---: | ---: |
| Original bytes | 78,434,426 | 54,255,103 |
| SHA-256 | `2ebc7763648c9c025ebc1c7d882c4300e2b9dba4fcae6260cf15291c2584ffb9` | `9b351e2f339e11b7bc2e10d9a377e4c72a9ed2ac1c3701951c935ab10a6dc0e3` |
| Declared video entries | 1 `avc1` | 1 `avc1` |
| Declared audio entries | 1 `mp4a` | 1 `mp4a` |
| AVCDecoderConfigurationRecord `avcC`, SPS/PPS checked | Valid | Valid |
| NAL length prefix | 4 bytes | 4 bytes |
| Video samples entirely within `mdat` | 5,215/5,215 | 11,154/11,154 |
| Video samples with exact NAL boundaries | 5,215/5,215 | 11,154/11,154 |
| Invalid MP4 box boundaries | 0 | 0 |
| Invalid AVC sample NAL boundaries | 0 | 0 |
| Raw `FLV` patterns anywhere | 5 | 2 |
| **Valid embedded FLV** | **0** | **0** |
| Raw `RIFF` patterns anywhere | 0 | 0 |
| **Valid embedded AVI** | **0** | **0** |
| Plausible `ftyp` box signatures | 1 (outer MP4) | 1 (outer MP4) |

| Fall-down candidate | Source A | Source B | Meaning |
| --- | --- | --- | --- |
| H.269 | No declared AV1 stream | No declared AV1 stream | User-defined LTO build alias; build provenance unavailable |
| H.268 | No declared AV1 stream | No declared AV1 stream | User-defined PGO build alias; build provenance unavailable |
| H.267 | Not testable against finalized syntax | Not testable against finalized syntax | Next-generation standardization effort; do not invent an ID/signature |
| H.266 | No declared VVC stream | No declared VVC stream | No `vvc1/vvi1` video entry |
| H.265 | No declared HEVC stream | No declared HEVC stream | No `hvc1/hev1` video entry |
| **H.264** | **Confirmed structural AVC** | **Confirmed structural AVC** | `avc1` + valid `avcC` + all bounded AVC NAL samples |

**H.264 NAL inventory, Source A:** type 1 = 5,157; type 5 = 58; type 6 = 21; type 7 = 58; type 8 = 58.

**H.264 NAL inventory, Source B:** type 1 = 11,109; type 5 = 45; type 6 = 11,199; type 7 = 1; type 8 = 1.

### Interpretation and remaining falsifiers

The results **do not support a conventionally multiplexed second video track, complete embedded FLV container, or complete embedded RIFF/AVI container in either original MP4** under the documented syntax checks. Seven total raw `FLV` byte patterns (5 + 2) did not form validated FLV streams. Container structure and H.264 AVC configuration agree; a purported alternate codec should produce a reproducible, independently decodable payload rather than only isolated magic bytes.

These checks **cannot** exclude encryption or steganography hidden in picture information, arbitrary raw data, fragmented/truncated internal containers, invalid deliberately obfuscated wrappers, unknown H.267 experimental syntax, or another rendition fetched by X. They do not provide an independent decoder for HEVC/VVC/AV1, detect all possible covert payloads, compare raster frames, or prove anything about an AI origin. The correct next experiment for the visual divergence is a frozen, independently decodable frame-pair comparison between the actual online inline and fullscreen stream URLs.

**Reproduction in PowerShell 7**, from the folder containing the original media and the C++ source:

```powershell
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2 .\minza_codec_ladder.cpp -o .\minza_codec_ladder.exe
Get-FileHash -Algorithm SHA256 'ef9lUKuZwT81L9CN(2).mp4','2026-11-18 21-15-19(2).mp4'
& .\minza_codec_ladder.exe 'ef9lUKuZwT81L9CN(2).mp4'
& .\minza_codec_ladder.exe '2026-11-18 21-15-19(2).mp4'
```

**Protocol custody:** Original source files remain unchanged. Any change to the search signatures, FLV/AVI validation thresholds, codec list, parser behavior, renderer, or media file produces a new experiment ID and retains this record as history. The labels BiDi, TriDi, and PiDi identify user-defined validation routes, not an industry-standard codec classification.


## Full byte mirror → XFE/EFX → ROT16: negative-control experiment

**Protocol:** `MINZA-REV-XFE-EFX-ROT16-20261119-v1`, separate from the original O2 codec and NAL inspection. **Status:** experiment completed; no additional playable video stream identified. This is a **byte-stream reversal**, distinct from an actual decoded-film time reversal or pixel-level horizontal mirror.

The requested sequence was carried out on local **copies of both videos**. Both source SHA-256 hashes remain the same as the original table. The whole MP4 payload's bytes were reversed, then wrapped with literal prefix `58 46 45` (`XFE`) and suffix `45 46 58` (`EFX`). Every generated candidate is precisely original input length plus six bytes. A reverse-the-reversal check recovered the original payload byte-for-byte for both sources.

| Candidate transformation, each source | Source A | Source B |
| --- | --- | --- |
| `XFE + reverse_bytes(MP4) + EFX` | invalid MOV, no `moov` | invalid MOV, no `moov` |
| `XFE + ROT16_alpha(reverse_bytes(MP4)) + EFX` | invalid MOV, no `moov` | invalid MOV, no `moov` |
| `ROT16_alpha(XFE + reverse_bytes(MP4) + EFX)` | invalid MOV, no `moov` | invalid MOV, no `moov` |
| `XFE + reverse_bits_per_byte(reverse_bytes(MP4)) + EFX` | invalid MOV, no `moov` | invalid MOV, no `moov` |
| `XFE + ROT16_alpha(reverse_bits_per_byte(reverse_bytes(MP4))) + EFX` | invalid MOV, no `moov` | invalid MOV, no `moov` |

**ROT16 convention:** uppercase and lowercase letters shifted **+16 modulo 26** independently, with nonalphabetic bytes unchanged. Both whole-wrapper and payload-only variants were tested because the scope was not specified. A second interpretation of **total mirroring** reversed the **eight bits of each byte** following full byte-order reversal. There were **10 generated candidates across 2 originals**, and all ten were rejected by locally installed `ffprobe 7.1.5` with `moov atom not found` and `Invalid data found when processing input`. The binary's original build optimization was not independently audited; **all new C++20 code was compiled at `-O2`**. Baseline controls were recognized as one H.264 video stream and one AAC audio stream in *each* original file.

**Structural boundary:** `XFE` and `EFX` are not sufficient standalone QuickTime header and footer structures. ISO BMFF/QuickTime atoms require a bounded size and four-character type (commonly `ftyp`, `moov`, `mdat`), so arbitrary 3-byte envelopes cannot restore reversed atom lengths, sample offsets, NAL lengths, or track tables. Reversing all bytes transforms the original `ftyp` signature into `pytf` at the opposite end. This experiment does not prove the absence of steganography in pixels; it specifically falsifies these attempted playable-track extraction operations.

**Custody:** The generated `.mov` failures and user-supplied video bytes are **not committed to the repository**. A reproducible standalone C++20 utility was compiled at `-O2` with stack protection, and source hashes were verified again before testing. Any subsequent change to the ROT16 definition, reversal granularity, wrapping rule or decoder requires a **new experiment ID** and should retain this negative result.

