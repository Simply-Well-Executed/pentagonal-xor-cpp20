<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# Getting Started

## Before you begin

Use a native Windows MSVC C++20 environment and a trusted local source snapshot. The project policy pins CMake 3.30.0 and PowerShell 7.7.0; verify the actual installed versions and the generator before building. The inspected TRIFORCE pipeline requests `Visual Studio 18 2026`. If unavailable, stop and report the toolchain mismatch. Do not silently substitute GCC, Clang, a container or an online dependency.

## Compile LCD

From a configured MSVC developer command prompt, with the inspected source at `C:\src\pentagonal-xor-cpp20` and a separate writable evidence/build directory already created:

```bat
cd /d C:\build\pentagonal-xor
cl /std:c++20 /O2 /W4 /WX /permissive- /utf-8 C:\src\pentagonal-xor-cpp20\tests\LCD_TEST.cpp
LCD_TEST.exe
```

Exit 0 means the LCD test executable returned success. It does not qualify the entire repository or authorize a release. Do not write compiler outputs inside the source tree during a controlled experiment.

## Run the existing pipeline

```powershell
pwsh -NoLogo -NoProfile -NonInteractive -File C:\src\pentagonal-xor-cpp20\ci\Invoke-TriforcePipeline.ps1 -RepositoryRoot C:\src\pentagonal-xor-cpp20 -EvidenceDirectory C:\evidence\run-001
```

Use a fresh evidence path. The pipeline runs the native CORE and PRECISION stages with HEAD/TAIL checks. In the SILVERHAND revision the precision stage expands to twelve tests. A failed prerequisite must stop dependent stages. This example has not been executed as part of this Wiki preparation.

**Source:** [`ci/Invoke-TriforcePipeline.ps1`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/ci/Invoke-TriforcePipeline.ps1), [`tests/LCD_TEST.cpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/tests/LCD_TEST.cpp).

## See also

- [LCD](LCD.md)
- [SILVERHAND](SILVERHAND.md)
- [Troubleshooting](Troubleshooting.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: Documentation home](Home.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: Architecture](Architecture.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [API Reference](API-Reference.md) | [Home](Home.md) | [SILVERHAND](SILVERHAND.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
