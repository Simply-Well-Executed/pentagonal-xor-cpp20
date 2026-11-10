# Pentagonal XOR and SWE Trust Boundary Guard

This repository contains two related but distinct components:

- **Pentagonal XOR std-free C API**: an include-free, allocation-free header for five scalar inputs and derived scores.
- **SWE Trust Boundary Guard**: a separate C++20 proof of concept for validating release metadata.

Neither component is a security certification.

## Pentagonal XOR: std-free C API

Header: `shared/pentagonal_xor_std_free.h`

Properties:

- No `#include` directives, standard headers, C++ standard library, heap allocation, or platform API calls.
- C-compatible declarations and `static inline` functions.
- Compiles as strict C11 and can also be included by C++.
- Five-input AND, OR, odd-parity XOR probability, partial score, and historical fifth-root score.
- Array and struct forms of the parity expression.
- Fifth root uses bounded Newton iteration plus power-of-two range reduction; no `pow()` or `math.h`.

### Input contract

The five feature values supplied to `swe_pxor_scores5` are expected to be finite doubles in `[0,1]`. Under that interpretation:

- `and_score` is the product of all five values.
- `or_score` is one minus the product of their complements.
- `xor_score` is the probability of an odd number of successes, assuming independent Bernoulli inputs.
- `partial` is `or_score - and_score`.
- `legacy` is the real fifth root of `and_score`.

The fifth-root helper supports finite signed inputs, including negative values. NaN and infinities are outside its documented contract. Floating-point results are approximate, not exact rational arithmetic.

### Windows legacy-target selection

The header defines `SWE_PXOR_WINDOWS_LEGACY_TARGET` at preprocessing time:

- `1`: `_WIN32` is defined and `_WIN32_WINNT` is absent or below `0x0A00`.
- `0`: non-Windows target, or Windows 10+ API target.

**Important:** this is compile-target classification, not detection of the Windows version installed at runtime. `_WIN32_WINNT` is a build target/API compatibility setting. A binary cannot reliably infer the actual host OS version from this macro. The std-free header is portable and may be used on every target; legacy Windows builds can select it using this macro without importing the C++20 implementation.

Example:

```c
#include "pentagonal_xor_std_free.h"

#if SWE_PXOR_WINDOWS_LEGACY_TARGET
/* Keep the legacy-compatible, std-free path. */
#endif
```

### Linux, Android, and Unix preprocessor targets

The same include-free header also exposes normalized integer flags:

| Macro | Meaning |
|---|---|
| `SWE_PXOR_TARGET_LINUX_KERNEL` | Compiler advertises a Linux kernel target, including Android toolchains that define `__linux__` |
| `SWE_PXOR_TARGET_LINUX` | Linux kernel target excluding Android |
| `SWE_PXOR_TARGET_ANDROID` | Android target using `__ANDROID__` |
| `SWE_PXOR_TARGET_UNIX` | Compiler advertises `__unix__` or `__unix` |
| `SWE_PXOR_TARGET_APPLE` | Apple Mach target |
| `SWE_PXOR_TARGET_WINDOWS` | Windows target using `_WIN32` |

All flags evaluate to `0` or `1`. They describe the **compile target** and can overlap where appropriate (e.g., Linux and Unix). No runtime OS probing, external includes, or Valve SDK code is needed.

For example:

```c
#include "pentagonal_xor_std_free.h"

#if SWE_PXOR_TARGET_LINUX
/* Linux-specific integration belongs here; the math stays portable. */
#elif SWE_PXOR_TARGET_ANDROID
/* Android-specific integration. */
#elif SWE_PXOR_TARGET_WINDOWS
/* Windows-specific integration. */
#endif
```

**Reference and licensing:** Valve's [Half-Life / GoldSrc SDK](https://github.com/ValveSoftware/halflife) demonstrates distinct Linux Makefiles and platform-specific preprocessor branches. It is a design reference only. Its SDK license has restrictions that differ from this project's 0BSD license; no Valve code was copied.

## SWE Trust Boundary Guard (C++20)

The guard is a separate five-channel metadata-validation proof of concept. It checks HTTPS scheme, exact trusted authority, relative path syntax, SHA-256 hex syntax, and artifact-name syntax. Its release decision is AND. OR and XOR are diagnostics only.

## Build and test

PowerShell 7:

```powershell
cmake -S shared -B shared/build
cmake --build shared/build --config Release
ctest --test-dir shared/build -C Release --output-on-failure
```

CI builds the C++20 guard and compiles the std-free header as strict C11 across Linux x64/ARM64, Windows x64/x86, and macOS Intel/Apple Silicon.

License: [0BSD](../LICENSE). Copyright (c) Paul Allen Cheek.
