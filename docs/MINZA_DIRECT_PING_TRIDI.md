# MINZA → PING → TriDi | three original geometric edges

**EXPLORATORY | PR #34 | RELEASE=HOLD**

This additional lane follows the existing
`MINZA-DIRECT-PING-BIDI-PR34-011` operation without editing
the previous files, algorithms, or governance. It does not
claim to define TriDi canonically outside this experiment.

## Why a third lane exists

The source flat triangle ADC has `A=(0,0), D=(0,20), C=(30,20)`.
The previous 90-degree fold about AC demonstrated preservation
of AD and CD. The **unchanged hinge AC** is a third independent
geometric edge, not an arbitrarily synthesized second copy,
invented actor, or a replacement for the previous AIRGAP.

The three recorded squared lengths are:

| Edge and identity | Original squared | Folded scaled squared (×169) |
| --- | ---: | ---: |
| AD, HEAD | 400 | 67600 |
| CD, TAIL | 900 | 152100 |
| AC, HINGE | 1300 | 219700 |

Both `400+900=1300` and `67600+152100=219700` hold
because triangle ADC is right at D in this pinned geometry.
This is **not** a general statement about arbitrary triangles.
The direct 90° fold preserves the side lengths, with the
new vertical sign/orientation and physical folding unproved.

## TriDi mapping and inverse

Take `(AD,CD,AC)` with full source labels and lengths.
The proposed local `TriDi` is the cyclic permutation

`C(a,b,c)=(b,c,a)`; `C^3(a,b,c)=(a,b,c)`.

Each interim step has distinct, valid edge labels and intact
source values. A canonical-order PING succeeds only on the
original ordering or after all three turns. The inverse is
`C^-1(a,b,c)=(c,a,b)`. A bit flip, duplicate label, wrong
edge value or premature canonicalization must be rejected.

The earlier **BiDi** `(AD,CD)→(CD,AD)→(AD,CD)` remains
separately checked against the first two TriDi lanes.
The older TriDi example in `xdr_rdx_fold_tork.hpp` uses an
independent AIRGAP canary; it is NOT overwritten by this
new three-geometric-edge construction.

## Direct path and custody

The direct geometry pipeline is
**MINZA → PING → TriDi → PING**, without a NINZA state for
this explicitly pinned hinge and 90-degree fold. It reuses
the preceding exact rational projection and folded-length
witnesses, never infers numerical roots, and never promotes
a passing source check into a physical fold or general law.

Files are additive:
`shared/minza_direct_ping_tridi.hpp` and this document;
the native gate test, no-standard-library portability probe,
CMake install inventory and HEAD/TAIL source-file inventory
are updated. The original F1/F2/F3 source, BiDi, Q62
circle-fit and tork headers remain byte-for-byte unchanged.
The prior INCEPTION-GATE human acknowledgment requirement
is not bypassed. **RELEASE=HOLD. Chosen, never assumed.**
