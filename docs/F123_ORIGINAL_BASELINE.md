# F1 / F2 / F3 Original Folded-Tetrahedron Baseline

**Status:** ESTABLISHED CODE SNAPSHOT, not an established claim about
photographs or the geometry of a proposed fold. This baseline is an
additive read-only reference for future experiments. No earlier source
is modified or retired.

## Provenance and custody

- Repository: `Simply-Well-Executed/pentagonal-xor-cpp20`
- PR: `#34`
- **Original captured commit:** `65a86d5ecd474efb41a7694d52b656e43636ba3c`
- **Original source file:** `shared/sigma_form_area_q62.hpp`
- **Original source Git blob SHA:** `414f894bcdb5726e4b7a569c43239f01676ee5c6`
- Portable, include-free **frozen witness header**:
  `shared/right_triangle_f123_original.hpp`
- Snapshot ID: `TRI-F123-FROZEN-PR34-001`; source and original formula
  values are immutable *by convention and audit*, not by cryptographic
  write-protection. The original Git commit is historical evidence.
- **A future geometry experiment may import/compare against this
  snapshot, never overwrite or reinterpret it as the new result.**

## Original named areas: faces and the separate base

For the right-triangle base tetrahedron with vertices
`O=(0,0,0)`, `B=(x,0,0)`, `C=(0,y,0)`,
`P=(0,0,z)`, set `x=y=1/2`, `z=1`. All coordinates
below are synthetic normalized square units, not photograph-derived.

| Frozen role | Formula | Q62 area | Area |
| --- | --- | ---: | ---: |
| F1, triangle OPB (xz) | `x*z/2` | 1152921504606846976 | 0.25000000000000000 |
| F2, triangle OPC (yz) | `y*z/2` | 1152921504606846976 | 0.25000000000000000 |
| F3, triangle PBC (oblique) | `sqrt((xy)^2+(xz)^2+(yz)^2)/2` | 1729382256910270464 | 0.37500000000000000 |
| Independent base OBC | `x*y/2` | 576460752303423488 | 0.12500000000000000 |
| F1+F2+F3 | lateral area | 4035225266123964416 | 0.87500000000000000 |
| Base+F1+F2+F3 | total surface area | 4611686018427387904 | 1.00000000000000000 |

The exact original Q62 parameter `q=3/4` is the **double area of
the oblique triangular face**. The original function checks
`q^2=(xy)^2+(xz)^2+(yz)^2` on the quantized values, and fails
closed if they disagree. The normalized example fits exactly.

The original 90-degree change `R_y(+90):(x,y,z)->(z,y,-x)`
acts on `PB=(x,0,-z)`, `PC=(0,y,-z)`. Their rotated cross product
has components `(xy,xz,-yz)` and the same norm, reproducing F3.
**This is a rigid orientation transformation; it does not construct
or unfold a physical missing tip.**

## Exact original source copy: four-face function

The following is copied verbatim from the original Git blob given
above; it is historical evidence, not an edited replacement.

```cpp
// RIGHT TRIANGULAR PYRAMID (trirectangular tetrahedron):
// Three edges from the right-angle vertex are mutually perpendicular.
// Base vertices: (0,0,0), (x,0,0), (0,y,0); apex: (0,0,z).
// The apex stands directly over the RIGHT-ANGLE VERTEX, not the centroid.
// Four triangular faces, unlike the five faces of a square pyramid.
//
// Let xy=x*y, xz=x*z, yz=y*z. The oblique face has
// double-area q = sqrt(xy^2 + xz^2 + yz^2).
// All inputs and xy/xz/yz/q are Q62 quantities. We REQUIRE the exact
// squared identity in the quantized domain, avoiding sqrt, floats,
// library includes, and false claims of exactness after rounding.
struct TrianglePyramid {
    Q62Area base, face_xz, face_yz, face_oblique;
    Q62Area lateral, total;
    bool right_triangle_geometry_verified;
};
[[nodiscard]] constexpr TrianglePyramid right_triangle_pyramid(
    u64 x_leg, u64 y_leg, u64 vertical_height,
    u64 oblique_double_area) noexcept {
    const TrianglePyramid invalid{
        {0ULL,false},{0ULL,false},{0ULL,false},{0ULL,false},
        {0ULL,false},{0ULL,false},false};
    if (x_leg==0ULL || y_leg==0ULL || vertical_height==0ULL ||
        oblique_double_area==0ULL ||
        x_leg>unit || y_leg>unit || vertical_height>unit ||
        oblique_double_area>2ULL*unit)
        return invalid;
    const u64 xy=shift62(multiply(x_leg,y_leg));
    const u64 xz=shift62(multiply(x_leg,vertical_height));
    const u64 yz=shift62(multiply(y_leg,vertical_height));
    const Wide expected=wide_add(
        wide_add(multiply(xy,xy),multiply(xz,xz)),multiply(yz,yz));
    if (!equal_wide(multiply(oblique_double_area,oblique_double_area),
                    expected))
        return invalid;
    // Quantize each face individually, then sum. Input Q62 scalars
    // are dimensionless, so outputs are normalized square units.
    const Q62Area base{xy/2ULL,true};
    const Q62Area face_xz{xz/2ULL,true};
    const Q62Area face_yz{yz/2ULL,true};
    const Q62Area face_oblique{oblique_double_area/2ULL,true};
    const Q62Area lateral{face_xz.value+face_yz.value+
                          face_oblique.value,true};
    const Q62Area total{base.value+lateral.value,true};
    return {base,face_xz,face_yz,face_oblique,lateral,total,true};
}
```

## Exact original source copy: rotated F3

```cpp
// SOHCAHTOA-F3-ROTATE-90-PR34-004.
// F3 = |PB|*|PC|*SIN(theta)/2 = |PB x PC|/2.
// Rotate both rays exactly using R_y(+90): (x,y,z)->(z,y,-x).
// The cross product transforms with the rays, preserving its length;
// therefore NO square root, inverse-trig function or numerical SIN
// approximation is needed to check the oblique triangular face.
// This is an area-preserving rigid rotation of an ALREADY folded solid,
// not proof of fabric folding or a physical instant 90-degree motion.
struct SignedQ62Vec3 { s64 x,y,z; };
[[nodiscard]] constexpr SignedQ62Vec3 rotate_y_90(
    SignedQ62Vec3 vector) noexcept {
    return {vector.z,vector.y,-vector.x};
}
[[nodiscard]] constexpr s64 product_signed_q62(
    s64 x,s64 y) noexcept {
    const u64 product=shift62(multiply(magnitude(x),magnitude(y)));
    const s64 signed_product=static_cast<s64>(product);
    return (x<0)!=(y<0) ? -signed_product : signed_product;
}
// Components are formed from normalized source rays with |each|<=Q.
// For this contract, each result component has magnitude <=Q.
[[nodiscard]] constexpr SignedQ62Vec3 cross_q62(
    SignedQ62Vec3 a, SignedQ62Vec3 b) noexcept {
    return {
        product_signed_q62(a.y,b.z)-product_signed_q62(a.z,b.y),
        product_signed_q62(a.z,b.x)-product_signed_q62(a.x,b.z),
        product_signed_q62(a.x,b.y)-product_signed_q62(a.y,b.x)
    };
}
struct RotatedF3Witness {
    SignedQ62Vec3 rotated_PB,rotated_PC,rotated_cross;
    Q62Area oblique_face;
    bool orientation_verified;
    bool area_preserved;
    bool valid;
};
[[nodiscard]] constexpr RotatedF3Witness right_triangle_f3_rotated_90(
    u64 x_leg,u64 y_leg,u64 vertical_height,
    u64 oblique_double_area) noexcept {
    const TrianglePyramid original=right_triangle_pyramid(
        x_leg,y_leg,vertical_height,oblique_double_area);
    if (!original.right_triangle_geometry_verified)
        return {{0,0,0},{0,0,0},{0,0,0},{0ULL,false},
                false,false,false};
    // The oblique face uses vertices P=(0,0,z),
    // B=(x,0,0), C=(0,y,0).
    // Hence PB=(x,0,-z), PC=(0,y,-z).
    const SignedQ62Vec3 PB{
        static_cast<s64>(x_leg),0,-static_cast<s64>(vertical_height)};
    const SignedQ62Vec3 PC{
        0,static_cast<s64>(y_leg),-static_cast<s64>(vertical_height)};
    const SignedQ62Vec3 PB90=rotate_y_90(PB);
    const SignedQ62Vec3 PC90=rotate_y_90(PC);
    const SignedQ62Vec3 cross90=cross_q62(PB90,PC90);
    const u64 xy=shift62(multiply(x_leg,y_leg));
    const u64 xz=shift62(multiply(x_leg,vertical_height));
    const u64 yz=shift62(multiply(y_leg,vertical_height));
    // Shortcut: R(PB)xR(PC)=(xy,xz,-yz). No angle is evaluated.
    const bool orientation=
        cross90.x==static_cast<s64>(xy) &&
        cross90.y==static_cast<s64>(xz) &&
        cross90.z==-static_cast<s64>(yz);
    const Wide sum_of_squares=wide_add(
        wide_add(multiply(magnitude(cross90.x),
                          magnitude(cross90.x)),
                 multiply(magnitude(cross90.y),
                          magnitude(cross90.y))),
        multiply(magnitude(cross90.z),magnitude(cross90.z)));
    const bool area=orientation &&
        equal_wide(sum_of_squares,
                   multiply(oblique_double_area,oblique_double_area)) &&
        original.face_oblique.valid &&
        original.face_oblique.value==oblique_double_area/2ULL;
    return {PB90,PC90,cross90,original.face_oblique,
            orientation,area,area};
}
```

## EXPLORATORY: "folded upper two-thirds" and missing tip

The new hypothesis is that an earlier folded component may represent
the "two-thirds and up" region, and that constructing a complete pyramid
requires reusing that component for the tip. This is **not yet
demonstrated** by the original numerical witness, photographs,
or a defined geometric construction.

"Two-thirds" could refer to height, volume, face area, ordering, or
a fold partition. Its meaning, cutting plane and orientation have not
been frozen as experimental inputs. No tip coordinates, folded topology,
material deformation, or assembly inverse are inferred or silently
declared proven.

For future experiments, start from an immutable `TRI-F123-FROZEN-PR34-001`
copy, create a **new experiment identifier**, separately define what
"two-thirds" measures, and compare the result against the original F1,
F2, F3, base and full-surface Q62 values. Do not make a failed fold
into the established baseline or call a matching area a verified shape.

## Integrity status

The existing `ci/Assert-TriforceHeadTail.ps1` now requires this
baseline document and header; the whole-source SHA-256 HEAD/TAIL ledger
will include them at runtime. Explicit approvals and external identity
checks remain unchanged. No release or merge is authorized.
**RELEASE=HOLD. Chosen, never assumed.**
