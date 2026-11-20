// SPDX-License-Identifier: 0BSD
// Call-free MACHINE-CODE witness for the canonical five-input parity core.
// The source invokes one static-inline canonical function; /O2 or -O2 must
// inline it. Verify the object disassembly: never assume optimization did so.
#include "../shared/pentagonal_xor_std_free.h"

extern "C" double swe_pxor_zero_call_core(
    double h0, double h1, double h2, double h3, double h4) noexcept
{
    const Features5 features{h0, h1, h2, h3, h4};
    return swe_pxor_xor_parity5(features);
}
