# FOUR-PYRAMID-REDUXUALCHOICE-PR34-008

**EXPLORATORY | RELEASE=HOLD.**
This is a **new, distinct four-sided-lateral pyramid lane**.
It preserves the previous three-side pyramid, F123 baseline,
`SIGMA_F_REDAXUALCHOICE`, and `SIGMA_E_REDAXUALCHOICE`
without source edits or renaming. It is a synthetic mathematical
construction, not the result of physical measurements.

## The four-sided right square pyramid CORE

The independently validated `right_square_pyramid` sample has
square side `s=3/4`, vertical height `h=1/2`,
and slant height `l=5/8`. The squared slant relation is checked
by the prior Q62 implementation: `4l²=4h²+s²`.

| Source witness | Normalized value | Q62 |
| --- | ---: | ---: |
| BASE `s²` | 0.5625 | 2594073385365405696 |
| F1 (one lateral triangle) | 0.234375 | 1080863910568919040 |
| F2 | 0.234375 | 1080863910568919040 |
| F3 | 0.234375 | 1080863910568919040 |
| F4 | 0.234375 | 1080863910568919040 |
| F = F1+F2+F3+F4 | 0.9375 | 4323455642275676160 |
| BASE + F | 1.5000 | 6917529027641081856 |

The four *lateral* faces are triangles. The square base remains a
single geometric square, even though a separate witness partitions
its area into 9 triangles.

## Interpreting "9 base 3-side triangle"

For this one implementation, **nine triangles partition the
square base** without overlaps or gaps. Divide the square into
three horizontal strips of equal height `s/3`, then place
the midpoint on each strip's top edge. Connect that midpoint
to the two bottom corners and two top corners of the strip.
This creates **three nonoverlapping triangles per strip**.

Each strip contains one large triangle of area `s²/6=3/32`
and two smaller triangles each of area `s²/12=3/64`.
Three strips give exactly 9 triangles:

`3 * (s²/6 + 2*s²/12) = s² = 9/16`.

| Area witness | Count | Individual Q62 |
| --- | ---: | ---: |
| Large base triangle | 3 | 432345564227567616 |
| Small base triangle | 6 | 216172782113783808 |
| **Nine-triangle total** | **9** | **2594073385365405696** |

The nine triangles are not all equal area, and this partition is
not a claim about a physically folded pyramid.

## Interpreting "sharing 3 diamond-point-top-over-2-third"

**Assumption for this experiment:** the phrase designates three
distinct marker points on three apex-to-corner edges of the right
square pyramid, sampled on the plane at **two-thirds of the
vertical height from the square base**. It does not assert a
four-corner diamond, an inverse-square relation, a fold operation
or a physical missing tip.

Take base corners `A=(0,0,0)`, `B=(s,0,0)`,
`C=(s,s,0)` and apex `P=(s/2,s/2,h)`.

Each marker has the exact rational coordinate
`P+(corner-P)/3`, so they are:

- On PA: `(s/3,s/3,2h/3)`;
- On PB: `(2s/3,s/3,2h/3)`;
- On PC: `(2s/3,2s/3,2h/3)`.

The implementation stores each coordinate as an integer Q62
numerator **over denominator 3**, rather than rounding the
non-binary-exact height `2h/3`. All three points are distinct
and share that exact rational height. A fourth apex-corner edge
exists, but no fourth marker is assumed. A three-marker witness
is not a proof of a diamond or fold.

## Step-up ratio between sample lateral areas

The frozen triangular pyramid sample has
`F3side=7/8`; the square pyramid has `F4side=15/16`.
Their **sample-only** ratio is

`F4side/F3side = (15/16)/(7/8) = 15/14`.

The code verifies the cross products with the existing unsigned
128-bit *limb* arithmetic. This is not a universal ratio between
three- and four-sided pyramids, nor evidence that the two examples
are transformations of the same shape.

## Reduxualchoice on the four lateral faces

The new four-sided lane retains the user's named operation,
spelled `REDUXUALCHOICE` in this namespace; it **references**
the existing `REDAXUALCHOICE` arithmetic instead of changing
that established experimental algorithm.

For finite `F=15/16` and `-3<=k<=3`:

`R_k(F)=(2^k F)/(2^(k+1))=F/2=15/32=0.46875`.

`Sigma4=sum_{k=-3}^{3}R_k(F)=7F/2=105/32=3.28125`.

| k | Symbolic ratio | Term Q62 |
| ---: | --- | ---: |
| -3 | (F/8)/(2/8) | 2161727821137838080 |
| -2 | (F/4)/(2/4) | 2161727821137838080 |
| -1 | (F/2)/(2/2) | 2161727821137838080 |
| 0 | F/2 | 2161727821137838080 |
| +1 | 2F/4 | 2161727821137838080 |
| +2 | 4F/8 | 2161727821137838080 |
| +3 | 8F/16 | 2161727821137838080 |

**Sigma4 Q62: `15132094747964866560`**.
The finite Sigma is not the pyramid surface area, which
remains `1.5`. The infinite sum diverges for F>0 and there
is no infinite Q62 value.

## Provenance and safety

The exact nine-base partition, three marker rationals, 15/14 sample
ratio, and bounded seven-term Sigma each receive separate
constexpr witnesses and fail-closed domain checks where relevant.
Header code uses no `#include`, STL, dynamic memory, floating
point, remote network, or new compiler dependency. The new header
and this record join the required HEAD/TAIL file inventory.
Historical records and approvals remain distinct and unchanged.
**RELEASE=HOLD. Chosen, never assumed.**
