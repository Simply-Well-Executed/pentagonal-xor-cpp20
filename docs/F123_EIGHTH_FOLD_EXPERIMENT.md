# F123-EIGHTH-FOLD-PR34-005

**EXPLORATORY | RELEASE=HOLD.** Adds a separate user-proposed symbolic
experiment to PR #34. No existing source, baseline record, formula,
provenance record, Git history, or approval is replaced or retired.

## Source and interpretation

Frozen authority: `shared/right_triangle_f123_original.hpp`,
originating at commit
`65a86d5ecd474efb41a7694d52b656e43636ba3c`. Its values
are `F1=1/4`, `F2=1/4`, `F3=3/8` in normalized area
units. The existing `shared/sigma_form_area_q62.hpp` computes
the checked rotated 90-degree cross product.

The user's proposed ordering is interpreted as:

```text
F1 * || R(PB) [old F3 at the cross-product gap] R(PC) || * (F2/2)
```

We use a **named F3 cross-product slot** that *delegates to*
`right_triangle_f3_rotated_90`. The old F3 is an area scalar,
not a vector binary operator. Substituting it literally for
the mathematical cross-product would be undefined. Accordingly,
`f3_in_cross_slot` retains the valid rotated cross vector and checks:

`F3_original = ||R(PB) x R(PC)|| / 2`.

This is explicitly a **chosen interpretation** of the user's
experimental notation, not a claim that the user originally defined
a novel cross-product algebra.

## Denominator expansion and numeric witness

The fraction replacements are **exactly equal**:

`F2/2 = (2*F2)/4 = (4*F2)/8`

Every denominator doubling also doubles its numerator, hence no
value changes when the final denominator reaches `8`. With the
frozen sample, `||cross||=2F3=3/4` and `F2/2=1/8`:

`E = F1 * ||cross|| * (4F2/8)`
`  = F1 * (2F3) * (F2/2)`
`  = F1*F2*F3 = 3/128 = 0.0234375`.

| Derived quantity | Q62 value | Decimal value |
| --- | ---: | ---: |
| F1 original | 1152921504606846976 | 0.25 |
| F2 original | 1152921504606846976 | 0.25 |
| F3 original | 1729382256910270464 | 0.375 |
| Rotated norm = 2F3 | 3458764513820540928 | 0.75 |
| F2/2 = 2F2/4 = 4F2/8 | 576460752303423488 | 0.125 |
| F1 * norm | 864691128455135232 | 0.1875 |
| **Final egress** | **108086391056891904** | **0.0234375** |

Because the multiplication uses area-valued witnesses, the result is
a **formal normalized product**, not the surface area of a triangle
or pyramid. If F1/F2/F3 were assigned physical units of area,
their product would have dimensions of area cubed, not area.
It must not be described as the "missing tip" area without a defined
geometric construction and dimensional correction.

The new include-free header
`shared/f123_eighth_fold_experiment.hpp` depends on inclusion of
`shared/sigma_form_area_q62.hpp` and
`shared/right_triangle_f123_original.hpp` first, preserving the
single canonical source of arithmetic and frozen numeric witnesses.
Both native and std-free test sources include it, with constexpr
positive, negative, 2/4/8 equivalence, and optional
`__uint128_t` oracle assertions.

The original F1/F2/F3 baseline is left **exactly unchanged**;
source and docs are now in HEAD/TAIL SHA256 inventory. This audit is
not a proof of a folded pyramid, not a geometry reconstruction from
photographs, and not authorization for release. Chosen, never assumed.
