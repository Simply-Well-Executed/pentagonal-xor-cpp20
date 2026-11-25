# THAILONGA FREQ | COUNT at VALUE, unlimited by arbitrary constants

**EXPLORATORY / RELEASE=HOLD / 0BSD.** FREQ does not replace NULL scientific evidence. No observations -> NULL NO DATA, not a fabricated empirical zero. The native UI uses user-entered manual values, NOT imported medical research datasets.

## Canonical measurement semantics

FREQ(X at VALUE v) = number of accepted X records exactly equal to v. Independently: FREQ(Y at VALUE v), FREQ(Z at VALUE v). Each count is a 64-bit unsigned integer; each accepted input remains in the dynamically grown source array unchanged. `count_at_value` is exact floating-value equality of stored canonical doubles and is distinct from tolerance or binned frequency. In scientifically sourced data, measurement precision, method and units must be defined before aggregating near-equal readings.

The displayed histogram has **16 configurable-in-source visual bins**, but NO 16-record or 64-record ceiling. Bin interval derives from the actual dataset minimum and maximum, rather than an arbitrary 0..12-inch range. For equal observations, the first bin receives every count and the fitted bell remains NULL ZERO VARIANCE. Counts are never synthesized by bell fitting.

The native viewer dynamically grows X, Y and Z arrays by doubling as needed. All three capacity allocations must succeed before any one dimension's count changes, preserving transactional triplets of manual selections. If native memory allocation/overflow prevents growth, the event is rejected with explicit MEMORY HOLD and prior accepted records remain untouched. Allocations are released on window destruction. This is limited by actual addressable memory and integer representation, not an arbitrary 64-observation policy; infinite storage cannot be promised.

## Visual inspection

H toggles between original THAILONGA X/Y/Z + PROJECTIONS and a separate FREQ view. A appends the three current eligible values to manual observational ledgers. Each axis has its own 16-bin distribution and Y=COUNT. The Gaussian overlay is only a fitted model for N>=3 with nonzero sample variance; its parametric estimate is not an observed count. Lack of data or evidence remains a distinct NULL state. Only manual values are shown, never paper samples as though retrieved.

An arbitrary 365-degree inspection metaphor is not an independent source record or a change to angular unit definitions. The frequency implementation has no hidden rotation-derived count limit.

## Tests and qualification

`freq_test.cpp` tests observed counts at exact values, data-driven bounds, 130 appended records (regression for prior 64 limit), memory capacity request without losing data, and invalid/zero-variance states. Registered in DIAERY CTest and nine LadderDi lanes, compiler native MSVC C++20 /O2. RELEASE=HOLD until actual CI/native execution is verified. Source/provenance remains authoritative.