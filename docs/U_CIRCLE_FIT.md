# Ⓤ Circle-Fit: Nice / NotNice (PR #34)

**Status: EXPLORATORY. RELEASE=HOLD.** This file records the
user-provided Ⓤ geometric validation example integrated on the
existing source branch as `shared/u_circle_fit.hpp`.

## Provenance and scope

- Input: user-supplied, standalone C++20 source entitled
  `U: geometric validation`, containing `Point`,
  `Triangle2`, `CircleU`, `VALIDATE`, `BUILD_NINE`,
  `COLUMNAR`, `ASTRIX`, Q62 `MULTIPLY`, four-sided
  pyramid witnesses, and seven-term `REDUXUALCHOICE`.
- Source preservation: existing geometric headers
  `sigma_form_area_q62.hpp`,
  `right_triangle_f123_original.hpp`,
  `f123_eighth_fold_experiment.hpp`,
  `sigma_e_redaxualchoice.hpp`,
  `sigma_f_redaxualchoice.hpp`, and
  `four_pyramid_reduxualchoice.hpp` remain unchanged.
- The integrated header preserves all named mathematical sections
  from the supplied example in an isolated namespace.
- This new header **delegates canonical Q62 multiplication** and
  seven-term Reduxualchoice to the established implementations,
  instead of introducing duplicate competing source algorithms.
  The source is thus adapted, **not a byte-for-byte copy** of
  the chat snippet. Differences are explicitly recorded here.

## Geometric contract

`U::Nice` means the complete *projected* triangular region
is inside or on a chosen circle; `U::NotNice` means not contained,
degenerate or out-of-domain. A triangle fits exactly when all
three vertices do, because the convex triangle is the convex
hull of its vertices and the disk is convex.

Circle positions are recorded in doubled integer coordinates;
all tests avoid floating point and approximate pi values.
The new entry-point preflights coordinates within +/-1,000,000
and doubled center within +/-2,000,000. Invalid or degenerate
input fails closed before signed multiplication. No uint64
squared-distance addition can overflow in this domain.

| Witness | Triangle or surface | Circle | U |
| --- | --- | --- | --- |
| Pyramid lateral face | (0,0),(60,0),(30,50) | center (30,25), radius 25 | NotNice |
| Same triangle | (0,0),(60,0),(30,50) | center (30,16), radius 34 | Nice |
| Right "Quaker" | (0,0),(30,0),(0,40) | center (15,20), radius 25 | Nice |
| Nine-triangle square | square [0,60]^2 | center (30,30), radius sqrt(1800) | Nice |
| Astrix | user 5 star vertices | same base circle | Nice |

For the first face the enclosing-circle radius 34 follows from
`(60/2)^2+16^2=34^2`, and the apex-to-center
distance is also `34`. The three face vertices lie on its
boundary. The original height-diameter radius 25 circle fails.

## Nine triangles, Columnar and Astrix

Three strips of size 60x20 each contain one large triangle of
area 600 and two smaller triangles of area 300. Thus:

`3*(600+2*300)=3600=60*60`.

The exact Columnar ratio is `D=A/T=600/3600=1/6`.
Columnar remains an **exploratory label**, not a new established
type of Euclidean triangle.

The Astrix five user-defined vertices and pentagram order
`0,2,4,1,3` are preserved. Every vertex fits in the circle;
because the circle is convex, every straight connecting edge
also fits. This does not prove a physical folded pentagram.

## Four-sided Q62 CORE and Reduxualchoice

The original right square pyramid is unchanged:
`SIDE=3Q/4`, `SLANT=5Q/8`,
`BASE=2594073385365405696`,
each of four lateral faces `F_i=1080863910568919040`,
total `6917529027641081856` (1.5 normalized surface).

For `F=4323455642275676160` (0.9375),
each named `R_k(F)=F/2=2161727821137838080`
and the seven-term `SIGMA=15132094747964866560`
(3.28125). The SIGMA is an algebraic sum,
**not the original pyramid's surface area**.

## Integration and integrity

The `shared/u_circle_fit.hpp` include-free header is added
to the CMake installation, native test source, STD-free portable
probe, and mandatory SHA-256 HEAD/TAIL inventory. The new header
does not require u128 compiler extensions and does not change
any build flags, approval rules, or project licensing.

The result verifies projected geometry only;
`PHYSICAL_FOLD_VERIFIED=false`.
A circle-fit success must not be promoted to evidence of an
actual physical paper fold, missing tip, or pyramid reconstruction.
No CI success or release authorization is presumed.
**RELEASE=HOLD. Chosen, never assumed.**
## REV1 arithmetic correction | U-CIRCLE-FIT-PR34-009-REV1

**Reason:** Native MSVC /O2 compilation of DIAERY 1:1-PARITY-GATE
revealed that the original `SLANT=5ULL*Q/8ULL` computes `5*Q`
first and therefore overflows unsigned 64-bit before division.
With `Q=2^62`, the overflowed result differs from the
canonical `5*(Q/8)=2882303761517117440` witness. The original
static assertions properly failed.

**Bounded correction:** `SLANT=5ULL*(Q/8ULL)`, with a
constant-time exact-value assert and a negative counterexample.
No F1/F2/F3, eight-fold, seven-term, four-sided pyramid, or
nine-triangle geometric source was changed. Original `u_circle_fit.hpp`
source blob `d28c7b7b8604aca910eea47e93b06edbccf9fbe1` remains retrievable through
Git history for audit. This is a new explicit **REV1** experiment
record, not a silent reinterpretation of the original snapshot.

**State:** historical failing arithmetic is CONTRADICTED; overflow-safe
REV1 is under independent native validation. A CI hold must not be
silently cleared or converted into release authority.
