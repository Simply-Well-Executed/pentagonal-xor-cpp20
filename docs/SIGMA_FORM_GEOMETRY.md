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
