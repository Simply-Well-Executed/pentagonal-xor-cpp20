# Troubleshooting Native Qualification

Start with the exact workflow run and tested event commit. GitHub PR runs can use a merge commit rather than the branch head. Read the first substantive error rather than treating downstream skips as new failures.

| Symptom | What to do |
|---|---|
| `unverifiable_build_descendant_executable_path` | Preserve JSTAR JSON and tool paths; investigate sampling and executable provenance. Do not whitelist by name alone or promote HOLD. |
| `C2607` in portable core | Inspect the original assertion and independent arithmetic witness; repair the implementation or explicitly revise a new protocol, not expected values merely to turn CI green. |
| `IRON` Python path HOLD | Review active versus historical custody and dependent coverage contracts before migration. |
| POSIX/GNU policy HOLD | Inventory legacy scripts and the probe; plan a native replacement with provenance. |
| SILVERHAND inventory mismatch | Compare exact CTest names with the prepared manifest; do not drop missing tests. |
| SILVERHAND receipt already exists | Preserve the receipt; use a new external evidence directory. |
| Zero LCD denominator | Treat quotient as undefined; do not silently divide or substitute zero. |

Observed earlier failures include `tests/std_free_portable_probe.cpp` assertions at lines 51 and 87, `octadi/G.py`, 18 POSIX scripts plus an unapproved GNU probe, and missing executable-path provenance. These are documented blockers, not permission to remove controls. See the original [Checks run](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/46076267109) and [TALON-head TRIFORCE job](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/46077689750/job/140769216323).

When changing a controlled test, preserve the old record, issue a new protocol/revision, and do not compare different protocols as equivalent.

## See also

- [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)
- [Release Status](Release-Status.md)
- [Source Map](Source-Map.md)

---
**Version:** [source revision `e6dbd19d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/e6dbd19d1cd3c23fdb202d5bfb9458d4db830557) | [PR #337](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/337) | PR #337 draft; not merged or released.

---

## Navigation

- [Previous: TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: Policy and Custody](Policy-and-Custody.md)
- [Accessibility guide](Accessibility.md)

**Referenced by:** [Getting Started](Getting-Started.md) | [Home](Home.md) | [Release Status](Release-Status.md) | [SILVERHAND](SILVERHAND.md) | [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)

- Return to [Documentation entry](README.md)

- Return to [Sidebar](_Sidebar.md)
