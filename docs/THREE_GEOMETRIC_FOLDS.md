# Three distinct 90-degree folds, not three TriDi turns

**EXPLORATORY | PR #34 | RELEASE=HOLD**

This is a NEW geometric experiment, not a rewrite of the nine-triangle
partition or the prior MINZA / NINZA / BiDi / TriDi operators.

## Construction

Take a 60x60 flat square sheet. Partition it into FOUR connected
60x15 rectangular strips, each triangulated once along a diagonal.
This yields eight triangles of area 450 each. The source nine-triangle
partition remains available separately, with the same total area
`3600`. The different partition is explicit, not a hidden mutation.

Perform **three actual 90-degree folds of the downstream panels**:

1. Hinge at original flat `y=15`: rotate panels 1,2,3 together.
2. Hinge at `y=30`: rotate panels 2,3 together about the now-3D
   hinge, keeping panels 0,1 fixed.
3. Hinge at `y=45`: rotate panel 3 about its now-3D hinge,
   keeping panels 0,1,2 fixed.

At each stage the algorithm constructs new 3D vertex positions from
the accumulated right-angle orientations. It independently verifies
the three real hinge events: prior panels stay fixed, a trailing
non-hinge vertex moves, the active hinge stays fixed, and adjacent
panel directions are perpendicular.

## Four corner coordinates of the final cross section

In the plane perpendicular to the panel width (`x`):

| Flat Y | Folded (Y,Z) |
|---:|---|
| 0 | (0,0) |
| 15 | (15,0) |
| 30 | (15,15) |
| 45 | (0,15) |
| 60 | (0,0) |

The free edges at flat y=0 and y=60 coincide for the width of the
sheet after the third fold. This forms the geometric four-wall
square tube with a 15x15 cross-section and a length of 60.
The two ends remain OPEN; no face caps or mechanical seam attachment
are implied. It is **not** a closed cube.

## Exact integrity checks

- **Eight nondegenerate triangles:** 900 doubled grid-area units each;
  all triangles have unchanged three side lengths, and their 3D
  cross-product magnitudes remain exact at all four stages.
- **Panel connections:** each of three hinge lines at
  `y=15,30,45` remains continuous, checked at x=0,30,60.
- **Three actual motions:** each fold moves a point not on its
  hinge, after its own neighboring upstream panels remain fixed.
- **90-degree test:** per-hinge tangent directions have dot=0,
  signed cross=+1 (no floating-point tolerances).
- **Seam closure:** the free strip edges coincide after fold 3 but
  NOT after fold 2.
- **Area:** 8 x 450 = 3600, matching the preserved nine-triangle
  source's 3600 and confirming no face area is lost.

This construction models ideal rigid panels and mathematical hinge
rotations. It does not establish that a real sheet of paper, a
particular material, the original nine-triangle crease pattern,
or a physical cube performs these folds.

## Provenance and implementation

New header: `shared/three_geometric_folds.hpp`.
New record: `docs/THREE_GEOMETRIC_FOLDS.md`.

Native and STD-free C++20 tests reference the new header, which
is include-free and does not use floating point or compiler-specific
u128. CMake and HEAD/TAIL SHA256 inventory receive new entries.
Every earlier file, including `shared/minza_direct_ping_tridi.hpp`
and `shared/u_circle_fit.hpp`, stays byte-for-byte unchanged.

The original TriDi permutation remains a valid separate algebraic
experiment; it must not be mistaken for this three-hinge construction.

Prior PR #34 CI was held for maintainer acknowledgment at
INCEPTION-GATE. This experiment does not bypass that gate,
invent approvals, or claim native compilation success.

**RELEASE=HOLD. Chosen, never assumed.**
