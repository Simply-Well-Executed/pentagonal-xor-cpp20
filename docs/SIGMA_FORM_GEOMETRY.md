# SIGMA-FORM-EXPLORATORY-PR34-001

<!-- SPDX-License-Identifier: 0BSD -->

**EXPLORATORY mathematical adapter; RELEASE=HOLD.**
No historical Q62 record, approval, protocol, or source is replaced.

## Source authority

`shared/toolong_q62_core.hpp` retains the exact operator
`Q(w) = floor((w * 100000000000000000)/2^62)`.

| Witness | Canonical source word | Exact expected integer |
| --- | --- | --- |
| Euler | 12535862302449814170 | 271828182845904523 |
| Pi | 14488038916154245685 | 314159265358979323 |
| Intensity | 315868046760674477 | 6849296450333523 |

All outputs end in `23`; that is a limited suffix-consistency witness,
not shared physical origin or cryptographic authentication. The 16-digit
Intensity output corresponds to `0.06849296450333523 W/m^2` when divided
by `10^17`, not to an imaginary number in the mathematical sense.

The requested `__uint128_t` expression is a compiler extension, not ISO
C++20 and not available to native MSVC. The canonical source's
include-free four-limb integer arithmetic is left unchanged. A
compiler-gated `__uint128_t` cross-check is included in the native
test code for environments supporting it, without requiring it on MSVC.

## Proposed forms

This module mathematically models the nine supplied photographs; it
does NOT recover 3D coordinates, physical fabric dynamics, light intensity,
or calibrated camera properties from image pixels.

| Form | Function | Equation |
| --- | --- | --- |
| Rigid square pyramid | `square_pyramid_z` | `z=h*(1-max(abs(x-cx),abs(y-cy))/b)` |
| Veil edge | `quadratic_bezier` | `(1-t)^2*P0+2*t*(1-t)*P1+t^2*P2` |
| Fabric pleat | `pleat_x` | `center+amplitude*sin(frequency*y+phase)` |
| Fabric billow | `damped_wave` | `A0*exp(-kappa*s)*sin(k*s-omega*t+phase)` |
| Pose | `two_link_arm` | two angular segments with a relative elbow angle |
| Diffuse light | `lambert` | `Ia+rho*E*max(0,n dot l)` |
| Projection | `pinhole_project` | `(fx*x/z+cx,fy*y/z+cy)` |
| Intensity sample | `inverse_square_intensity` | `P*exp(-alpha*omega)/(4*pi*omega^2)` |

The geometry module has **no includes or STL dependencies**, is designed
for native C++20 `/O2` and provides `constexpr` checks. The
trigonometric and exponential series are bounded approximations,
not certified accuracy bounds: sine accepts input phases within
64 radians and attenuation accepts exponents in `[0,16]`.
Each function returns a `valid` flag, rejecting unsupported domains.
Lambert requires unit-length vectors; pinhole projection requires
positive depth. The independent geometric `geometry_pi` double is
not the authoritative Q62 Pi source word.

## Boundary and evidence

- **HEAD:** the existing full-source SHA-256 inventory and exact Q62
  BigInteger checks preserve the source and now require the new files.
- **AIRGAP:** `circle_of_sanity::centrix` still computes only the
  original local read-only arithmetic/suffix consistency. It is not
  a fourth triangle or a physical network disconnection.
- **TAIL:** the original paired source gate runs after each controlled
  operation. The native `triforce_native_gate` additionally tests
  the exploratory geometry and deliberately invalid inputs.

Numerical sample agreement is **not** photographic similarity scoring.
No camera calibration, segmentation, cloth inversion, independent
measurement, authentication, or image renderer has been implemented.
The pre-existing unrelated TOOLONG `third` declaration collision
and all licensing/CI HOLDs remain independent and unresolved.

**No auto-approval, source rewrite, merge, or release. Failure never
becomes policy; chosen, never assumed.**


## Three SIGMA projected areas + independent AIRGAP A

**Protocol:** SIGMA-AREA-BCD-AIRGAP-001. Canonical C++20 header:
`shared/sigma_form_area_q62.hpp`. The `circle_of_sanity::centrix`
Q62 arithmetic gate is left intact. This module defines a separate
geometric area check, not another owner or fourth triangle.

- **AIRGAP A:** `base*altitude/2`, reference triangular pyramid silhouette.
  This is **not** the full 3-D surface area of a square pyramid.
- **SIGMA B:** quadratic Bézier segment plus its closing chord,
  `abs(det(P1-P0,P2-P0))/3`.
- **SIGMA C:** constant-width pleated strip's *projected* area,
  `width*height`; the folded 3-D fabric surface area can differ.
- **SIGMA D:** variable-width billowing strip, integrated using
  exact trapezoids over ordered position/width samples.

Input coordinates are unsigned Q62 integers with `unit=2^62`; areas
are unsigned Q62 squared-coordinate representations:
`raw=floor(area*2^62)`. The exported `decimal17` function computes
`floor(raw*10^17/2^62)` using 64x64->128 four-limb multiplication
with no includes, no STL and no floating-point input. The expression
matches the integer-scaling *style* of the canonical Q62 source
records but these geometric source coordinates are newly declared,
distinct and synthetic. Numeric photograph-derived meters or pixel
coordinates are NOT available.

### Explicit synthetic normalized test, not measured photograph areas

| Lane | Declared normalized inputs | Q62 area | decimal17 |
| --- | --- | ---: | ---: |
| A | triangle base 1, altitude 1 | 2305843009213693952 | 50000000000000000 |
| B | Bezier (0,0), (0.5,1), (1,0) | 1537228672809129301 | 33333333333333333 |
| C | strip width 0.25, height 1 | 1152921504606846976 | 25000000000000000 |
| D | widths 0.5,0.75,0.5 at 0,0.5,1 | 2882303761517117440 | 62500000000000000 |

These map to 0.5, approximately 1/3, 0.25 and 0.625
**normalized area units** respectively. They are not square metres
and are not inferred from a person's appearance, garment physics
or pyramid photograph. In the same 2-D coordinate system,
the reference ratios are exactly represented as the Q62 rational
pairs `B/A`, `C/A`, and `D/A`, with no forced equality.
Without common camera geometry, known scale and perspective, those
projected ratios must not be interpreted as physical 3-D surface-area
ratios.

All three SIGMA lanes require individually valid inputs. The AIRGAP A
must have positive independently supplied area. `assemble()` returns
false for incomplete inputs and always reports physical calibration
as unverified. An internal consistency pass cannot authorize a
release, authenticate a photograph, waive CI HOLD, or substitute for
external validation. Static assertions in the native gate and
STD-free probe cover these inputs and negative controls.


## SIGMA-FORM-FOLD-PR34-002: two independent paths and B/C/D fold sequence

This is an **additive exploratory protocol**, retaining all earlier
SIGMA-AREA-BCD-AIRGAP-001 source and witnessed history. It does not rename
or replace the original Q62 Euler/Pi/Intensity `circle_of_sanity::centrix`.

**Names:** `SigmaA` is a new square-base surface-area witness
`s²`; `AIRGAP A` remains the separate triangular reference
`b*h/2`. They are not aliases, despite sharing A in their names.

The three ordered and independently preserved formal sums are:

1. `SigmaA + SigmaB + SigmaC | AIRGAP A` (ABC);
2. `SigmaC + SigmaD + SigmaB | AIRGAP A` (CDB);
3. `SigmaB + SigmaC + SigmaD | AIRGAP A` (BCD).

`paired_and(ABC,CDB)` denotes the user's `^&^` conjunction,
not consensus, aliasing, equivalence, human consent or release
authority. CDB and BCD have identical arithmetic sums **because addition
is commutative**; the ordered lane metadata is distinct and preserved.

The BCD branch is the source for these nine named C++20 `constexpr`
stages, with a strict precondition on its predecessor:

| Stage | Pure function | Formal operation |
| --- | --- | --- |
| 0 | `start_fold` | Store B,C,D in original order |
| 1 | `fold_b_into_c` | B-first conceptual overlap |
| 2 | `fold_d_into_c` | D-second conceptual overlap |
| 3 | `push_c_into_d` | Record symbolic nesting |
| 4 | `twirl_to_rope` | Rotate complex orientation `i^n` |
| 5 | `loop_into_needle` | Tag intended loop/needle geometry |
| 6 | `insert_into_airgap_a` | Enter independent A validation boundary |
| 7 | `twist_a_90` | Exact imaginary quarter turn `i*(x+i*y)` |
| 8 | `spindle_with_c` | Four additional quarter turns `i^4=1` |

Every stage retains its source B,C,D and the formal sum. Folding,
nesting, imaginary-number rotation, looping and spindling do not
automatically convert disjoint *projected* shapes into a physical
square pyramid. Rope radius, needle sharpness, curvature, stretching,
self-intersection, fabric footprint, and 3-D equivalence remain
**unmeasured/unverified**. The conceptual names record the user's
chosen operator order, not measured dynamics or invented geometry.
Bad operator order and invalid domains return a failed state.

### Independent closed square-pyramid surface

For a **right square pyramid** with square-base side `s`, vertical
height `h`, and face slant `l`:

`4*l*l=4*h*h+s*s` (exact slant consistency, integer-safe)

`base=s*s`, `each_face=s*l/2`,
`lateral=4*each_face`, and **`total=s*s+2*s*l`**.

The four triangular lateral faces and one square base make **five**
faces. A tetrahedron is a different polyhedron with four triangular
faces; this module does not equate them. The optional user nickname
"Tetrid" is only descriptive metadata, never a shape identity claim.

To avoid any square root or non-ISO compiler dependence, the new
`right_square_pyramid()` verifies
`4*l²=4*h²+s²` using the existing 128-bit limb products and returns
the base, one triangular face, all lateral faces, and total surface
as independent Q62 outputs. The normalized exact illustrative sample
`s=3/4, h=1/2, l=5/8` gives:

| Quantity | Q62 area | Decimal17 output |
| --- | ---: | ---: |
| Square base | 2594073385365405696 | 56250000000000000 |
| One triangular face | 1080863910568919040 | 23437500000000000 |
| Four lateral faces | 4323455642275676160 | 93750000000000000 |
| Total square-pyramid area | 6917529027641081856 | 150000000000000000 |

These are normalized square units, not photograph-derived m².
The declared BCD sum remains
`5572453938933093717` Q62 units, whereas the independently
defined square-pyramid area is `6917529027641081856`. They are
**not equal**. Consequently `evaluate_closure()` records both the
actual comparison result and `folding_proves_pyramid=false` even if
numerical equality occurs in another example. The geometric formula
is validated by exact side/height/slant premises, never inferred
from a decorative operator sequence.

No C++ include directives, STL, runtime allocation, remote fetch,
automatic merge, approval, or release is introduced by this protocol.
It is guarded by the existing whole-tree HEAD/TAIL source evidence.
CI must independently compile the new tests before any PASS can be
claimed. **RELEASE=HOLD. Chosen, never assumed.**


## SIGMA-TRIANGLE-PYRAMID-PR34-003: right_triangle_pyramid

Added **alongside** `right_square_pyramid`, without replacing the
square-pyramid formula, earlier symbolic folds, or AIRGAP records.

**Geometric definition is explicit:** the tetrahedron has a
right-triangle base with coordinates `(0,0,0)`, `(x,0,0)`,
`(0,y,0)`, and apex `(0,0,z)`. Thus the three edges incident
to the right-angle vertex are mutually perpendicular. The apex is
directly above that vertex, **not over the base centroid**.
This is a *trirectangular tetrahedron*, here called
`right_triangle_pyramid`. It has four triangular faces.

Let the orthogonal base legs be `x`, `y`, vertical height `z`,
and oblique face **double-area**
`q=sqrt((xy)^2+(xz)^2+(yz)^2)`. The total four-face surface area is:

`S_triangle = (xy + xz + yz + q)/2`.

For no-include C++20 Q62 arithmetic, the caller explicitly supplies
`q` as an unsigned integer Q62 value. The implementation checks the
exact identity `q*q == (xy)^2+(xz)^2+(yz)^2` in 128-bit limbs,
after computing each scaled product as
`floor(leg1*leg2/2^62)`. This **fails closed** when quantized
inputs do not admit an exact oblique double-area witness. It does
not claim to handle all otherwise valid real-valued tetrahedra or
approximate square roots silently.

### Synthetic normalized exact rational example

`x=1/2`, `y=1/2`, `z=1`,
`q=3/4`. Then `xy=1/4`, `xz=1/2`,
`yz=1/2` and `q²=(1/4)²+(1/2)²+(1/2)²`.

| Triangle face | Geometric area | Q62 integer | Decimal17 |
| --- | ---: | ---: | ---: |
| Right-triangle base (xy) | 0.125 | 576460752303423488 | 12500000000000000 |
| Side xz | 0.25 | 1152921504606846976 | 25000000000000000 |
| Side yz | 0.25 | 1152921504606846976 | 25000000000000000 |
| Opposite oblique face | 0.375 | 1729382256910270464 | 37500000000000000 |
| Three lateral faces | 0.875 | 4035225266123964416 | 87500000000000000 |
| **Total four-face surface** | **1.0** | **4611686018427387904** | **100000000000000000** |

The earlier right-square-pyramid normalized sample has area 1.5,
so these two independent models are not conflated.
`evaluate_triangle_closure()` separately compares the B/C/D
formal sum against triangle surface and never claims that folding
establishes a tetrahedron. Domain-invalid inputs and deliberately
wrong oblique witnesses are covered by `static_assert`.

Every sample here uses declared normalized coordinates, not
measurements extracted from photographs. Source validation, approval
and release policy remain independent. **RELEASE=HOLD.**


## SOHCAHTOA-F3-ROTATE-90-PR34-004: exact oblique-face shortcut

The earlier figure is an **already folded** right-triangular pyramid
(tetrahedron), not an exploded or inverse-square model. The user
identified the transformation `R_y(+90):(x,y,z)->(z,y,-x)` as a
shortcut for the gap `||PB|| ||PC||` in the third lateral face.

With apex `P=(0,0,z)` and oblique-face base endpoints
`B=(x,0,0)`, `C=(0,y,0)`:

`PB=(x,0,-z)`, `PC=(0,y,-z)`

`R(PB)=(-z,0,-x)`, `R(PC)=(-z,y,0)`

`R(PB) x R(PC)=(xy,xz,-yz)`

Therefore the face area is **exactly**:

`F3 = ||PB||*||PC||*SIN(theta)/2`
`   = ||R(PB) x R(PC)||/2`
`   = sqrt((xy)^2+(xz)^2+(yz)^2)/2`.

There are **three SIN face factors**: two right-angle side faces
(`SIN(90 degrees)=1`) and the oblique face (whose SIN factor is
implicitly contained in the cross product). No inverse-trig
calculation is needed and no angle is guessed. This is the
shortcut in the user's requested direction; COS and TAN are not
silently introduced as additional operators.

The example `x=y=1/2`, `z=1` gives
`R(PB)=(-1,0,-1/2)`, `R(PC)=(-1,1/2,0)`,
cross `(1/4,1/2,-1/2)`, norm `3/4` and
`F3=3/8`, Q62 `1729382256910270464`.
The other three face areas and total Q62 surface `1.0`
remain unchanged.

`right_triangle_f3_rotated_90()` applies the coordinate
rotation directly in header-only C++20. It multiplies the signed
Q62 cross components, tests their squared norm with existing
128-bit limb functions, and compares the existing authoritative
oblique double-area witness. An inconsistent geometric input
returns `valid=false` rather than approximating a square root.
The F3 shortcut is a **derived** representation of the same
geometry, not independent evidence of the image or a new physical
measurement. The operator/representation is separately named
and auditable. The Euler/Pi/Intensity Q62 source records, HEAD/TAIL
custody, consent and RELEASE=HOLD remain unchanged.
