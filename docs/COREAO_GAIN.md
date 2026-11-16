# COREAO RF gain: input → +9 dBi → EIRP output

**Status:** experimental, domain-scoped, original 0BSD C++20 mathematical
operator. This is a **calculation**, not a physical measurement or a
claim of higher transmitter power. Its provenance and scope are distinct
from DecaSigma, COREAO numerical audits, Coreo stereo, and Stability-10D.

## Exact operator and source custody

```text
INPUT     source conducted power P_in, in watts (W)
GAIN      fixed antenna reference gain +9 dBi
OUTPUT    effective isotropic radiated power (EIRP), in watts (W)
REVERSE   reconstructed P_in = EIRP / gain_linear
AUDIT     largest normalized discrepancy between source and reconstruction
```

For a matched, idealized transmit path with no feedline, connector, or
other losses:

```text
power gain relative to isotropic = 10^(9/10) = 7.943282347242816
EIRP[W] = P_in[W] × 7.943282347242816
recovered_P_in[W] = EIRP[W] / 7.943282347242816
```

dBi is a logarithmic *directional* antenna gain reference. EIRP is the
power an ideal isotropic radiator would need to radiate to produce the
same power flux density in the specified reference direction. It is not
the sum of power radiated in all directions and is not an amplifier that
creates additional transmitter power. Under comparable far-field
conditions, the corresponding **field-amplitude ratio** is
`sqrt(10^(9/10)) ≈ 2.818382931264454`; that ratio is not an
audio amplitude multiplier.

**The 0BSD input W remains the authoritative source record.** The
derived EIRP, reconstructed input, and normalized residual are not
permitted to overwrite it. On negative, nonfinite or unrepresentable
numeric input, `admissible=false`; the default zero fields do not
represent a successful measurement or evaluation.

## Implementation

`shared/coreao_gain.hpp` lives in
`namespace swe::coreao::rf_gain` and includes the existing COREAO
header for its normalized error comparison. It does not change
`shared/coreao.hpp` or the Pentagonal XOR parity kernel.

```cpp
#include <swe/coreao_gain.hpp>
auto witness = swe::coreao::rf_gain::apply(1.0); // input conducted watts
if (witness.passed()) {
    // witness.gain_dbi = +9
    // witness.output_eirp_watts ≈ 7.943282347 W
    // witness.recovered_input_watts ≈ 1.0 W
}
```

The gain factor is a fixed, documented binary64 constant to permit an
include-free, allocation-free public API. The calculation uses no
transceiver, DSP, radio control, device access, IO or online service.
It assumes an ideal zero-loss path and does not account for bandwidth,
frequency, antenna geometry, impedance mismatch, polarization,
installation losses, nearby scatterers, duty cycle, local regulations
or real antenna radiation patterns.

If downstream users need a real system link budget, they must provide
measured, frequency-specific antenna gain, feedline loss, and any
other link-budget components. Adding `+9 dBi` to a signal without a
physical antenna and reference direction does not magically create
more radio power or better audio transcription.

## Numerical examples

| Input (conducted) | Gain | Output EIRP |
|---|---|---|
| 0 W | +9 dBi | 0 W |
| 1 mW | +9 dBi | approx. 7.943282 mW |
| 1 W | +9 dBi | approx. 7.943282 W |

## Validation and boundary

`tests/coreao_gain_test.cpp` checks the results independently against
the standard-library `pow(10,0.9)` reference, compares equivalent
amplitude ratio against `sqrt(pow(10,0.9))`, verifies the inverse
input witness and unchanged original COREAO round-trip, and exercises
negative, NaN, infinite, overflow and invalid-tolerance cases.

`tests/consumer/main.cpp` compiles and executes the *installed*
`swe/coreao_gain.hpp` API on the multi-OS CI matrix.

**Release status:** new PR #4 changes are subject to the latest
cross-platform CI run. Original failures remain visible; a subsequent
successful rerun does not rewrite a prior red result. No tag, merge,
or production Android signing is implied by this mathematical addition.
