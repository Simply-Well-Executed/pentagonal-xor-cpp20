# DYNAMO | UNIT/SUBUNIT GRID & EXACT TIMELINE

**EXPLORATORY | PR #316 | 0BSD | RELEASE=HOLD**

## Coordinate grid

* X-MAJOR = X UNIT = exactly 1 inch on horizontal axis. Add three dashed equal-stroke-width lighter monochrome stripes at rational positions 1/4, 2/4, 3/4 between neighboring X majors.
* Y-MAJOR = Y UNIT = exactly 1 inch on vertical axis. Add two dashed equal-stroke-width lighter monochrome stripes at rational positions 1/3 and 2/3 between neighboring Y majors.
* Major and subunit use the same GDI pen width (1px) but different brightness and solid/dash patterns. Coordinate source is unchanged; mapping is 30 px/inch in the initial native viewport. X quarter-unit marks require fractional *pixel* placement at 7.5px increments; rounding is for raster position only.
* The earlier reported coordinates, x/y intercept algebra and source S are distinct from constructed trace lines.

## NULL plot reused as timing layer

The second panel carries two semantic layers:
* **Invisible thread** = exact integer femtosecond counter, `1 second = 1,000,000,000,000,000 fs`; stored as `unsigned long long` with overflow HOLD.
* **DARN** = integer seconds and residual femtoseconds computed by quotient/remainder, no floating second rounding. **YARN** = separate user-defined 16-sploon display carry. Neither changes canonical time. Key events advance the timeline. This is a simulated coordinate timeline, **not** actual hardware clock resolution or femtosecond accurate instrumentation.
* The panel depicts progress over a 2-second viewport; the current timeline marker and a source-preserving trace are overlaid.

## Controlled trace format

The accepted patterns are `Y=MX+B`, `Y=X`, `Y=0`; the active one can be cycled in the native app using **T**. They are narrow, whole-string matched format rules; **not a general regular-expression compiler**. Unsupported patterns return `InvalidPattern` instead of executing arbitrary text. This meets the safe always-recomputed trace objective without importing an unsafe scripting or regex runtime. A user-editable regex text input remains not implemented.

Key controls in DYNAMO: `F` advances by 1 femtosecond; `S` advances by 1 second; `T` cycles the bounded trace format; **G** toggles subunit stripes; existing arrows/PgUp/PgDn/M/D remain. Time values stay exact as integer counters. No scheduling claim or asynchronous execution.

## Verification

The header and native view are source-committed. `trace_timeline_test.cpp` includes compile-time tests for conversion, overflow, rational subunit labels and accepted/rejected trace patterns. Native MSVC/CTests and visual checks remain to be independently executed. Do not promote test registration into a PASS claim.

## Native pattern-entry update

**R** enters a 15-character ASCII trace-entry field. Type a complete format and press **Enter** to validate against the bounded grammar. **T** cycles through the three known formats; invalid input displays **HOLD** and draws no trace. Only `Y=MX+B`, `Y=X`, and `Y=0` are currently accepted; there is no arbitrary regular expression engine. Changes recompute the trace on repaint. This is a native interactive implementation rather than a mockup; actual runtime acceptance is still unverified pending CI/native execution.

The graph subunits are drawn with 1px pens in light monochrome dash strokes; X divisions are exactly quarters and Y exactly thirds of their respective 1-inch majors. The time axis is a separate 2-second viewing pane using exact integer femtosecond ledger values. Raster positions and trace display samples never replace the integer authority.

## THREAD / DARN / SPLOON / YARN correction

**Status: EXPLORATORY, not a physical unit standard.** This correction supersedes the previous identification of yarn with unrounded seconds.

- THREAD: 1 femtosecond, counted in the exact integer ledger.
- DARN: exact quotient/remainder of the fs count by `10^15`. The expression `fs^15` means femtoseconds to the 15th power in conventional units, **not** 10^15 femtoseconds. We therefore preserve `10^15 fs = 1 s` as the time conversion.
- SPLOON: one of 16 equally spaced fractional sectors of a one-second DARN. For this test, one sploon = `62,500,000,000,000 fs` exactly.
- YARN: derived display seconds. The 16-sector index `k = floor(16*remainder/10^15)` falls in 0..15. Evaluate the user's additive `.1` and `.6` threshold with integers, not rounded binary fractions: `(10*k+16)/160`. If this expression is strictly greater than 0.6, add one derived display second. If it reaches or crosses 1.0, add one **additional** derived second. For `k=8`, adjusted = 0.6, no carry; `k=9`, 0.6625, one carry; `k=15`, 1.0375, two carries. This double-carry interpretation is exploratory and intentionally separate from conventional rounding and true elapsed time.
- The exact DARN numerator and femtosecond remainder are unchanged even when YARN jumps by two seconds. Exact data **always** wins over the display. The test fixtures verify 8/9/14/15 boundary sectors and do not silently treat YARN as an actual physical clock.
- Native DYNAMO now prints the exact DARN seconds and residual fs, the derived YARN seconds and the current sector, with `HOLD` on overflow.

Original explanation is retained in version history; no ordinary timer uses this exploratory carry as an actual change in elapsed seconds. `RELEASE=HOLD`.

## IMPEREAL STANDARD MEASURES / n-1663r

**EXPLORATORY naming and transform, not an established measurement standard.** The 16 sploon sectors are now labeled **IMPEREAL STANDARD MEASURES** (user spelling retained). For the active sector integer `n` from 0 through 15, the display computes `n - 1663*r` with `r = pi^2`. Because conventional pi is irrational and the C++20 code uses a finite `double` approximation, the displayed number is *approximate*. It is a **dimensionless exploratory diagnostic**, not a measurement conversion and not a correction to true elapsed time. The derived value is negative for all allowed `n=0..15`; this alone does not invalidate the original nonnegative sector. The graphical panel labels the output accordingly.

Exact standard imperial/metric length conversion is separately supported through the ratio `1 inch = 25400 micrometres` (equivalently 25.4 mm). `inches_to_micrometres` returns an integer for whole-inch inputs or HOLD on integer overflow; `micrometres_to_inches` returns the exact fraction `um/25400`, without truncation or requiring pi. Arbitrary fractional conversions require rational input/output representations and must not be silently reduced to rounded floating output. Unit-bearing conversion values must not be mixed with time-sector variables. Exact dimensional conversion does not imply that any physical measurement is known without uncertainty.

Test witnesses check integer conversion, overflow, the proposed n-domain and nonmutation of exact DARN time. **No completed native MSVC/CTest run has been confirmed; RELEASE=HOLD.**
