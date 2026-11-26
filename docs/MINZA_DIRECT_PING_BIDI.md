# MINZA → PING: direct geometric witness, then BiDi

**EXPLORATORY | RELEASE=HOLD | PR #34**

The established nine-triangle tessellation is the source. Strip
zero has large triangle `A=(0,0),B=(60,0),C=(30,20)` and its
small adjacent triangle `A=(0,0),D=(0,20),C=(30,20)`.
The common hinge is **AC**, not AD. The existing sources remain
unchanged.

## Why the NINZA intermediate can be skipped

For the single rigid 90-degree fold of vertex D about the fixed AC
hinge, no intermediate folded state is mathematically necessary.
Use the perpendicular decomposition of `v=D-A` against
`u=C-A`:

`D' = A + (u·v)/(u·u) * u + k*z`, with
`z² = det(u,v)²/(u·u)` and k perpendicular to the
original planar triangle.

For `u=(30,20)`, `v=(0,20)`:

`u·u=1300`; `u·v=400`; `det(u,v)=600`.

`D'=(120/13,80/13,+60/sqrt(13))`. The sign of z
chooses which side of the original plane receives the fold;
squared distance witnesses do not distinguish the sign.
The implementation carries `x13=120,y13=80,z2_169=46800`
with no approximate square root, pi or floating point.

Verify `dot²+cross²=axis²*|AD|²`, that the projected
coordinates lie on the hinge, and that both squared distances
remain unchanged. The direct witnesses are:

- HEAD: `AD_FOLDED=67600=13²*400`.
- TAIL: `CD_FOLDED=152100=13²*900`.

This **bypasses only an intermediary computation for this pinned
geometric sample**; it does not retire or redefine symbolic
NINZA elsewhere, and it does not verify a physical paper fold.

## BiDi without losing provenance

Construct the pair
`[HEAD(AD,67600), TAIL(CD,152100)]`. Apply the already
established local BiDi operation twice:

`BiDi(BiDi(pair)) = pair`.

The first swap is a reversible temporary traversal, not a
renaming or merger; PING only accepts canonical source roles
after restoration. Corrupting either folded length, forcing
them equal, or changing one source label fails closed.

Existing XDR/RDX HEAD/TAIL envelopes are checked, but
TriDi, PiDi, PiSigma, SigmaPi, SigmaSigmaSigma and
NINZA are **not required as intermediate operators** for
this direct MINZA→PING lane.

## Technical custody

- New include-free ISO C++20 header:
  `shared/minza_direct_ping_bidi.hpp`.
- Uses canonical exact coordinate witnesses from
  `shared/u_circle_fit.hpp`, and the existing reversible
  BiDi / envelope implementation from
  `shared/xdr_rdx_fold_tork.hpp`.
- Native + std-free test probes and the HEAD/TAIL file list
  include this new, separately sourced experiment.
- No change to the canonical three-side and four-side
  pyramid sources, original nine triangles, existing
  operators or Git history.
- No new network or runtime side effects.
- This is a special-case direct proof, not a universal
  hinge solver or official release authorization.

**RELEASE=HOLD. Chosen, never assumed.**
