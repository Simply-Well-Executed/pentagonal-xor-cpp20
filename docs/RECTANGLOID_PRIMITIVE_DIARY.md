# Diary: 3 folds + 2 caps = rectangloid primitive

**EXPLORATORY | RECTANGLOID-PR34-014 | RELEASE=HOLD**

A user-requested primitive geometric model for C++20, provided alongside
an offline standalone XHTML 1.0 Strict viewer. The native source models
the geometry, **not** XHTML or browser scripting. The viewer is supplied
separately as a downloadable file, rather than adding project-authored
JavaScript to the C++ repository or changing its no-JS source policy.

## Source: three real folds

A flat 60 x 60 sheet divided into four 60 x 15 panels folds at three
distinct 90-degree hinges (flat y=15,30,45). The inherited
`three_geometric_folds.hpp` witness accounts for all 8 wall
triangles and exact face rigidity. This experiment neither edits
nor reinterprets the old nine-triangle square-base partition.

The folded, initially open-ended square tube has a length of 60
and cross section 15 x 15. Two *copies* of a 60 x 60 source sheet
provide two 15 x 15 square caps, each triangulated into 2 triangles.
Material custody per source copy is:
`3600 = 225 cap area + 3375 offcut area`.
This constructs **12 outward-wound triangle facets**, 8 walls and
4 cap facets. The original copies are not themselves each used as a cap.

## PING: exact mesh closure, orientation, area and volume

- Every directed triangle edge has exactly one opposite directed edge.
  Missing a cap triangle fails `ALL_EDGES_PAIRED`.
- Signed volume is derived from the oriented triangle mesh as
  `V6 = Σ dot(a,cross(b,c)) = 81000`, so `V=13500`.
- Surface area `S=2(60*15 + 60*15 + 15*15)=4050`.
- Eight wall triangles + four cap triangles each have nonzero area.
- No arbitrary physical length unit was supplied: 13500 is
  **cubic model units**, not grams or liters. Mass and water
  density cannot be inferred.
- This proves a closed **mathematical manifold boundary**,
  not physical welding, seal integrity, hydrostatic capacity
  or a real solid block of material.

## Optional water and YANG rendering layer

The user-selected diary display defaults to `Fill::Water`, but
the geometry itself is a closed boundary independent of fill.
The thin/crinkled YANG coating is a **visual option** without
invented thickness or a finite-element shell stress model.
The C++20 primitive provides a sample camera at (120,45,60),
calculates outward-facing normals and demonstrates that
6 of 12 surface triangles face that camera. This is a
convex-body **back-face filter**, **not** a rasterized Z-buffer,
Z-clip pipeline, general occlusion implementation or GPU renderer.
The XHTML visualization can display a cutaway and water styling;
neither source proves physical fluid filling.

## Source code and test custody

`shared/rectangloid_primitive_cxx20.hpp` is ISO C++20,
include-free, STL-free, no floating point or compiler-only u128.
It reuses the previous `fold::PROJECT` geometric source rather
than duplicating its formulas. It contains compile-time
`static_assert` geometry, boundary, camera and corruption tests.
The native and portable C++20 probes, root CMake install
inventory and HEAD/TAIL source-custody inventory reference it.

The exploratory source was separately compiled locally with
GCC and Clang using `-std=c++20 -O2 -pedantic-errors` and
an interface-compatible local stand-in for the earlier geometry
header; **that does not establish a native MSVC build pass**.
Existing INCEPTION-GATE maintainers must approve their own
holds. No release authorization is inferred.

**Chosen, never assumed. RELEASE=HOLD.**
