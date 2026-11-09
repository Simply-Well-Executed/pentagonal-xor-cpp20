# SWE Trust Boundary Guard (C++20)
This is a **proof of concept**, not a security certification.

## Grounding
Derived from the *design principles* in the upstream [nvm threat model](https://github.com/nvm-sh/nvm/blob/master/.github/THREAT_MODEL.md), especially the distinction between trusted executable payloads and **untrusted metadata**. This is SWE-authored C++20 code; it does not copy nvm's shell implementation.

## The five channels
1. HTTPS-only scheme.
2. Exact allow-list for HTTPS authorities, rejecting userinfo and noncanonical URL forms.
3. Relative resource path segments without traversal, backslashes or encoded delimiters.
4. Strict **syntax-only** 64-digit SHA-256 hex digest.
5. Conservative artifact filename characters.

**Decision:** AND all five booleans. OR and XOR are *display-only diagnostics*. An odd number of passing controls does **not** imply safety. The engine's recursive fuzzy XOR formula is used for a parity check, not an integrity digest.

## Limitations
- SHA-256 *syntax* validation does not calculate any hash or establish authenticity. The website GitHub pipeline separately runs `sha256sum -c` over tracked assets. Verify signing/provenance independently.
- URL host parsing is deliberately conservative and intended for these four approved SWE endpoints only. It is not a general-purpose URL parser or SSRF defense. HTTP redirect destinations are not followed or validated.
- Text checks cannot guarantee real filesystem canonicalization or symlink safety. Callers must also enforce sandbox containment and avoid shell-evaluating untrusted metadata.
- TLS verification, permissions, secrets, database access control, dependency provenance and incident response require separate layered controls.

## Build and test
```powershell
cmake -S shared -B shared/build
cmake --build shared/build --config Release
ctest --test-dir shared/build -C Release --output-on-failure
```
CI runs the test suite on Linux (x64/ARM64), Windows (x64/x86), and macOS (Intel/Apple Silicon).

License: [0BSD](../LICENSE). Copyright (c) 2026 Paul Allen Cheek.
