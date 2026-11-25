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
* **Invisible yarn** = integer seconds and residual femtoseconds computed by quotient/remainder, no floating second rounding or claims of physical measurement. Key events advance the timeline. This is a simulated coordinate timeline, **not** actual hardware clock resolution or femtosecond accurate instrumentation.
* The panel depicts progress over a 2-second viewport; the current timeline marker and a source-preserving trace are overlaid.

## Controlled trace format

The accepted patterns are `Y=MX+B`, `Y=X`, `Y=0`; the active one can be cycled in the native app using **T**. They are narrow, whole-string matched format rules; **not a general regular-expression compiler**. Unsupported patterns return `InvalidPattern` instead of executing arbitrary text. This meets the safe always-recomputed trace objective without importing an unsafe scripting or regex runtime. A user-editable regex text input remains not implemented.

Key controls in DYNAMO: `F` advances by 1 femtosecond; `S` advances by 1 second; `T` cycles the bounded trace format; **G** toggles subunit stripes; existing arrows/PgUp/PgDn/M/D remain. Time values stay exact as integer counters. No scheduling claim or asynchronous execution.

## Verification

The header and native view are source-committed. `trace_timeline_test.cpp` includes compile-time tests for conversion, overflow, rational subunit labels and accepted/rejected trace patterns. Native MSVC/CTests and visual checks remain to be independently executed. Do not promote test registration into a PASS claim.

## Native pattern-entry update

**R** enters a 15-character ASCII trace-entry field. Type a complete format and press **Enter** to validate against the bounded grammar. **T** cycles through the three known formats; invalid input displays **HOLD** and draws no trace. Only `Y=MX+B`, `Y=X`, and `Y=0` are currently accepted; there is no arbitrary regular expression engine. Changes recompute the trace on repaint. This is a native interactive implementation rather than a mockup; actual runtime acceptance is still unverified pending CI/native execution.

The graph subunits are drawn with 1px pens in light monochrome dash strokes; X divisions are exactly quarters and Y exactly thirds of their respective 1-inch majors. The time axis is a separate 2-second viewing pane using exact integer femtosecond ledger values. Raster positions and trace display samples never replace the integer authority.
