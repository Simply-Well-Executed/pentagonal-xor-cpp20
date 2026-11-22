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
