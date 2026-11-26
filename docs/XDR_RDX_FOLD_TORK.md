# XDR / RDX Fold-Tork PING | 6-7-8-9 (PR #34)

**EXPLORATORY | RELEASE=HOLD.**
New additive implementation in `shared/xdr_rdx_fold_tork.hpp`.
Never replace the original `u_circle_fit.hpp`, frozen F1/F2/F3,
SIGMA-E, SIGMA-F or four-pyramid sources to obtain a passing test.

## Source geometry (exact, not fitted)

The earlier MINZA→NINZA→PING 90-degree fold around the shared
hinge A=(0,0), C=(30,20), with D=(0,20), fixes the hinge while
moving D into 3D. X and Y coordinates use denominator 13; the
z-squared witness uses denominator 169 to avoid approximating
sqrt(13).

`X13=120`; `Y13=80`; `Z2_169=46800`.
Original squared distances `AD²=400` and `CD²=900`.
Since 13²=169:

- `AD_FOLDED=67600=169*400`, HEAD
- `CD_FOLDED=152100=169*900`, TAIL

The head and tail must remain distinct, **not** forced equal.

## Reversible 6|7|8|9 XDR and RDX envelopes

The names are experimental operator names, not a claim to
implement standard ONC-XDR serialization or an externally
defined RDX protocol. Each stage uses a documented unsigned
rotation followed/preceded by an XOR mask.

`XDR_s(v)=rotl64(v XOR mask_s,s)`;
`RDX_s(v)=rotr64(v,s) XOR mask_s`.
`RDX_s(XDR_s(v))=v` for s in 6,7,8,9.

- HEAD: `XDR6→XDR7→XDR8→XDR9` encodes AD;
  `RDX9→RDX8→RDX7→RDX6` decodes it.
- TAIL: `RDX9→RDX8→RDX7→RDX6` encodes CD;
  `XDR6→XDR7→XDR8→XDR9` decodes it.

The two enveloped records have role-specific integrity tags
and retain exact original values. Tags detect the specific
adversarial probes in this experiment, but are NOT
cryptographic authentication or data-integrity guarantees.

## Distinct BiDi/TriDi/Pi/Sigma lanes

The operator implementations here are **local, testable
adapters**, not a revision of any original symbolic definition:

- **BiDi**: reversible two-lane swap; `B(B(x))=x`.
- **TriDi**: three-lane cycle of HEAD, TAIL, explicit independent
  AIRGAP marker; `C(C(C(x)))=x`. Missing third data is
  not invented: this AIRGAP is a labeled local canary.
- **PiSigma**: record the two independent source branches as
  an ordered UNION; TALON separates them back without merging
  identity or changing witness order.
- **PiDi**: apply the previously user-given finite symbolic
  pi operation `PI_SYMBOL % 4 = 3` with
  `PI_SYMBOL=314159265358979323`. This scalar is a symbolic
  user-defined integer, NOT the mathematical irrational pi.
- **SigmaPi**: `!PI_SYMBOL = false` under C++ logical
  negation; preserved as **symbolic metadata**, not an
  approval or a failure claim.
- **SigmaSigma**: independently check head and tail
  source values and exact squared-length relations.
- **SigmaSigmaSigma**: require source, reversible traversal,
  Pi-family metadata, and roundtrip PING to all agree.

The original framework may have broader interpretations not
fully specified by its current source records. These adapters
do not claim completeness or canonical status.

## Interleaved "tork" stress and PING

Each reversible `COLLIDE` mixes both encoded lanes with
6/7/8/9 variable bit rotations and XOR, then `SEPARATE`
undoes exactly the same round, in reverse order. This
corresponds to the requested high-intensity interleaving
**without** any physical force, undefined races, or swapping
the original witness identities.

The PING checks round counts 6,7,8,9 and 64 at compile time
and rejects 0 and >4096. Runtime testing may use more
rounds, but no wall-clock benchmark or GPU workload is
claimed. Deliberate single-bit payload/tag mutation, a
role swap, and reversed provenance all fail closed.

The exercise is not a cryptographic collision search. A
reversible mixing transform cannot create stronger
security by throwing two integers together. Its value here
is a deterministic consistency and invertibility audit.

## Provenance

The header is include-free, standard-library-free, does not
modify existing source algorithms and introduces no network
traffic. The native and portable C++20 probe compile tests
and HEAD/TAIL SHA256 inventory explicitly reference it.
An existing **INCEPTION-GATE** earlier held PR #34 pending
human acknowledgments in issues #57 and #58; this commit
does **not** grant or simulate those acknowledgments.
Compiler or pipeline success is not presumed.

**RELEASE=HOLD | Chosen, never assumed.**
