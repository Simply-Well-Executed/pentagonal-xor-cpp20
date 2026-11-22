# SIGMA-E-REDAXUALCHOICE-PR34-006

**EXPLORATORY**. This experiment extends F123-EIGHTH-FOLD-PR34-005
with the user's named *Redaxualchoice* finite ratio-expansion rule.

## Source and scope

The `E` here is the previously recorded experimental **egress**
from the F1/F2/F3 cross-slot, **not** the previous exploratory
`F=F1+F2+F3` sum. The original immutable-by-convention
`right_triangle_f123_original.hpp` (source commit
`65a86d5ecd474efb41a7694d52b656e43636ba3c`)
remains unchanged, as does
`f123_eighth_fold_experiment.hpp`.

`F1=1/4, F2=1/4, F3=3/8`, hence
`E=F1*F2*F3=3/128`, exactly
`108086391056891904` in normalized Q62 units.

For any finite scalar E and an integer k, define

`R_k(E)=(2^k*E)/(2^(k+1))=E/2`.

The **bounded** seven-term sum for the explicit experimental
window `-3<=k<=3` is

`SigmaE=sum_{k=-3}^{3} R_k(E)=7E/2=21/256=0.08203125`.

The Q62 value is **378302368699121664**.
Each term is **54043195528445952** Q62 units,
equivalent to 0.01171875.

| k | Numerator representation | Denominator representation | Q62 result |
| ---: | --- | --- | ---: |
| -3 | E/8 | 2/8 | 54043195528445952 |
| -2 | E/4 | 2/4 | 54043195528445952 |
| -1 | E/2 | 2/2 | 54043195528445952 |
| 0 | E | 2 | 54043195528445952 |
| +1 | 2E | 4 | 54043195528445952 |
| +2 | 4E | 8 | 54043195528445952 |
| +3 | 8E | 16 | 54043195528445952 |

Negative exponents use exact rational numerator/denominator
representation, not prematurely rounded integer division.
All arithmetic is unsigned Q62 integers in the committed
example. Out-of-domain k, a non-divisible E, missing provenance
or a changed E fails closed. No ISO C++20 `__uint128_t`
dependency is introduced: the new header uses include-free
native operations; the test adds an optional wide-integer oracle
only on compilers that support `__uint128_t`.

An infinite extension of this sum would diverge for E>0;
it cannot be stored as a finite Q62 value. No geometric
pyramid tip, surface area, or calibration has been proved.
The product of physical area-valued F terms would be
dimensionally area cubed, not area. The symbolic result
is therefore not a real-world area measurement.

**Source custody:** the new header and document join the
HEAD/TAIL SHA256 inventory; prior sources, tests, approvals
and Git history are retained. **RELEASE=HOLD.**
