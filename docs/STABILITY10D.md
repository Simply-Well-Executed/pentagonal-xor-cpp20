<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# Stability-10D: PR #17 operator extraction

> **Document status: DUE FOR UPDATE (2026-11-19).** Verify claims against code and tests; sharpen active verbs; check copyright, project-mark language, accessibility, and 0BSD scope. This mark is not a failure verdict. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Status:** experimental 0BSD C++20 and Java 17 implementation, pending *this repository's* CI and review.

**Provenance:** [source PR #17](https://github.com/pcheek1991/faster-whisper-4d-abandonedware-uniunlicensd/pull/17), head `432a228f1dddbb2532de1bd1f439ee8ef34af4e9`. This is a reverse-repository of **explicitly attributable operators and tests**, not a claim that two repositories have identical histories. Existing Pentagonal XOR C11 parity, DecaSigma, TriPi and scalar COREAO modules remain unchanged.

## Canonical boundaries

- **CORE:** nine independently measured stereo float32 statistics, in order:
  `frame_count`, `mean_left`, `mean_right`, `rms_left`,
  `rms_right`, `peak_left`, `peak_right`,
  `zero_crossings_left`, `zero_crossings_right`.
- **Normalized ligands:** nine numbers from the exact mappings below.
- **Three interlocked pentagons:** cyclic five-input indices
  `[0,1,2,3,4]`, `[3,4,5,6,7]`, `[6,7,8,0,1]`.
  Each pair overlaps by exactly two ligands.
- **Canonical kernel:** `shared/pentagonal_xor_std_free.h`:
  `P5(h) = (1 - Π(1-2*h_i))/2`.
  Each ring calls **that** original kernel. No new competing XOR algorithm.
- **Six-byte output:** each ring score `p` quantizes to
  `word = floor(65535*p+0.5)`, then emits low byte and high byte.
  The exact byte sequence is `[word0_lo,word0_hi,word1_lo,word1_hi,word2_lo,word2_hi]`.
  This is *six bytes*, not nine.
- **Tenth scalar:** `Stability = 1 - (max(P0,P1,P2) - min(P0,P1,P2))`.
  It quantifies **ring-score agreement only**.

### Mapping from the original nine measurements

| Index | Normalization |
|---|---|
| 0 | `N/(N+1024)` |
| 1,2 | `(1+mean/(1+abs(mean)))/2` |
| 3,4 | `rms/(1+rms)` |
| 5,6 | `peak/(1+peak)` |
| 7,8 | `crossings/max(1,N-1)` |

All nine ligands must be finite in [0,1]. Frame count must be a positive exact integer. RMS and peak cannot be negative. Crossing counts must be integers between zero and `N-1`. The include-light C++ header additionally restricts exact counts to at most `2^53-1`, a declared binary64 integral-identity boundary; it refuses unsupported values rather than guessing.

This header never overwrites source values. On a failed input guard it does not write a fabricated score. It contains no tokenizer, trained weights, dynamic allocations or standard-library headers (apart from including this repository's original parity API).

## Three golden hexabytes

| Ligands | Three scores | Hexabyte | Stability |
|---|---|---|---|
| nine zeros | 0,0,0 | `000000000000` | 1 |
| nine ones | 1,1,1 | `ffffffffffff` | 1 |
| nine halves | 0.5,0.5,0.5 | `008000800080` | 1 |
| only ligand #3 is one | 1,1,0 | `ffffffff0000` | 0 |

Independent C++20 test `tests/stability10d_test.cpp` recomputes each ring as the sum of **16 odd-parity minterms** and checks thousands of deterministic inputs. It checks byte order, invalid numeric ranges, original source preservation, and the unchanged sixth-byte contract. The algebraic scores are not calibrated probabilities when the source features are correlated.

## Coreo stereo operator

`shared/coreo_stereo.hpp` exposes a **separate** frame projection from PR #17:
`[L(i),R(i)]` across `N` stereo frames becomes

```text
YIN(i)  = [-L(N-1-i), -R(N-1-i)]
YANG(i) = [-L(i), -R(i)]
QUAD(i) = [YIN-L,YIN-R,YANG-L,YANG-R]
```

The inverse projection reconstructs each sample from YANG. A read-only audit computes the **largest absolute mismatch**, including the Z-fold relationship `YIN(i)=YANG(N-1-i)`. The caller owns buffers; the source is immutable. Unlike the separate original DecaSigma COREAO algebra, this is an **audio-specific** mapping. It does not generate speech transcripts.

## AI / Whisper boundary: precise scope

PR #17 calls its experiment `Fastest-Whisper-10D`. The name does not establish model-free ASR.

**Proved capability:** numerical stereo waveform transformations, deterministic feature computation, an auditable six-byte Stability witness, and synthetic cross-platform arithmetic comparisons.

**Not proved:** recognition of arbitrary spoken words; language understanding; real ASR word error rate or benchmarks without a trained/acoustic language model; a security/authenticity guarantee; reconstruction of original inputs from a lossy six-byte witness.

The source PR removed its bundled Silero ONNX weight file and disabled implicit model downloads. Original third-party MIT Python/CTranslate2 code remains in its **own** source repository, and the new Pentagonal XOR release does not package it, the 59KB DUNGU-derived audio converter, recordings or model weights. These are scope and provenance exclusions, not a lossless transformation of those programs.

## Interface and tests

```cpp
#include <swe/stability10d.hpp>
double h[9] = {0,0,0,1,0,0,0,0,0};
swe::stability10d::Result witness{};
if (swe::stability10d::calculate(h, witness)) {
    char hex[13]{};
    swe::stability10d::hex(witness, hex); // ffffffff0000
}
```

Build with `cmake -S . -B build -DBUILD_TESTING=ON`, `cmake --build build --config Release`, `ctest --test-dir build -C Release --output-on-failure`. Installed consumers compile both added headers. Source and generated artifact SHA-256 evidence remain commit-scoped, not automatically transferable between repos.

**Release nomenclature:** this is a Pentagonal XOR **0.2** experimental addition, NOT a model-free transcription system or a release-quality Android speech recognizer.
