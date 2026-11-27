# Policy and Source Custody

## Current project direction

- Native MSVC C++20; 0BSD for new project-authored code; no new disallowed interpreter, POSIX/GNU or network runtime dependency.
- Prefer exact arithmetic; explicitly declare any rounding or approximate numerical model. Historical quotient/remainder witnesses remain historical.
- Keep source, evidence and release decisions separate. Source revision is authoritative; derived documentation does not redefine an API.

Existing repository workflows and historical artifacts do not all satisfy the stated direction. In particular, GitHub-hosted bootstrap steps and legacy language/toolchain records still exist. Do not present the tree as fully offline or policy-clean until the applicable gates demonstrate it.

## Document maintenance

Use the pinned source links to check facts. Mark draft, prepared, validated and released states distinctly. Preserve prior versions and their evidence. Do not remove source bounds, rename interfaces or alter validation merely because documentation is reorganized.

**Source:** [`LICENSE`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/LICENSE), [`AGENTS.md`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/AGENTS.md), [`docs/GNU_POSIX_ZERO_APPROVAL_POLICY.md`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/GNU_POSIX_ZERO_APPROVAL_POLICY.md), [`docs/LICENSE_AND_MARKS.md`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557/docs/LICENSE_AND_MARKS.md).

## See also

- [Architecture](Architecture.md)
- [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)
- [Release Status](Release-Status.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) · [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) · PR #337 draft; not merged or released.

---

## Navigation

Previous: [Troubleshooting](Troubleshooting.md) · [Index](Index.md) · [Home](Home.md) · Next: [Release Status](Release-Status.md)

**Referenced by:** [Source Map](Source-Map.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
