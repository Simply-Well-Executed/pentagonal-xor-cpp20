# SIGMA-F-REDAXUALCHOICE-PR34-007

**EXPLORATORY | RELEASE=HOLD.** This is a new, separate
**carbon-copy expansion** of the user's full right-triangle pyramid
and seven Redaxualchoice terms. It is **not a rename or replacement**
of `SIGMA-E-REDAXUALCHOICE-PR34-006`.

## Original source and exact carbon-copy values

Historical source: `shared/right_triangle_f123_original.hpp`,
frozen from commit `65a86d5ecd474efb41a7694d52b656e43636ba3c`.
The copied calculations are in a distinct, include-free namespace
`swe::sigma_f_redaxualchoice` and assert every original source
value against the frozen baseline. The original remains unchanged.

| Name | Exact normalized value | Q62 integer |
| --- | --- | ---: |
| X,Y,Z | 1/2, 1/2, 1 | 2305843009213693952, 2305843009213693952, 4611686018427387904 |
| OBLIQUE double area | 3/4 | 3458764513820540928 |
| BASE | 1/8 | 576460752303423488 |
| F1 = FACE_XZ | 1/4 | 1152921504606846976 |
| F2 = FACE_YZ | 1/4 | 1152921504606846976 |
| F3 = FACE_OBLIQUE | 3/8 | 1729382256910270464 |
| TOTAL = BASE + F1+F2+F3 | 1 | 4611686018427387904 |
| **F = SIGMA_F123** | **7/8** | **4035225266123964416** |

The right-triangular pyramid's four-face total surface remains **1**;
the three-face lateral area F is **7/8**. These are normalized
mathematical examples, not measured physical pyramid dimensions.

## The exact seven-term expansion

`R_k(F)=(2^k*F)/(2^(k+1))=F/2` for integer `-3<=k<=3`.

| k | Numerator / denominator | Q62 result |
| ---: | --- | ---: |
| -3 | (F/8)/(2/8) | 2017612633061982208 |
| -2 | (F/4)/(2/4) | 2017612633061982208 |
| -1 | (F/2)/(2/2) | 2017612633061982208 |
| 0 | F/2 | 2017612633061982208 |
| +1 | 2F/4 | 2017612633061982208 |
| +2 | 4F/8 | 2017612633061982208 |
| +3 | 8F/16 | 2017612633061982208 |

`SIGMA_REDAXUALCHOICE = sum_{-3}^{3} R_k(F) = 7F/2 = 49/16`.

**Exact Q62 output: `14123288431433875456`**.
Decimal representation: **3.06250000000000000**.

The finite sum is an *algebraic aggregation of equivalent ratios*;
it is not the pyramid's 3-D surface area and it does not recover a
folded missing tip. An infinite extension for F != 0 diverges.
No literal infinity is stored as an unsigned integer.

## Compiler and custody

The requested `__uint128_t` expressions appear exactly behind
a compiler capability guard; they are not ISO C++20 and do not exist
on native MSVC. MSVC uses the existing include-free four-limb
Q62 arithmetic from `shared/sigma_form_area_q62.hpp`.
No new external include, STL, compiler flag, network step, or
package dependency is introduced.

Include `sigma_form_area_q62.hpp`,
`right_triangle_f123_original.hpp`, and then the new
`sigma_f_redaxualchoice.hpp`. Native and STD-free tests assert
the original geometry, each independent term, the seven-term sum
and the distinction from the previously implemented SigmaE.

The HEAD/TAIL source SHA256 ledger lists both new files.
All historical source records, `SIGMA-E` tests, approval paths,
and independent release checks remain unmodified.
**RELEASE=HOLD. Chosen, never assumed.**
