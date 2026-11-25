# PentaDi | Four LadderDi stages with shared NULL

**EXPLORATORY / RELEASE=HOLD**

Five conceptual positions: RUNG, EXCITE, EXCITED, ATTEST, and one **common NULL**. The NULL checkpoint is not the fifth passing task. It is a shared fail-closed witness that preserves absent/unknown qualification information. Each stage independently reports PASS, FAIL, or NULL. An actual FAIL blocks; an unperformed or missing stage is NULL/HOLD; only four explicit PASS results qualify the four-stage sequence. The common NULL is never silently rewritten to PASS or used to fabricate another participant. DYNAMO native tests and the EXCITED verification stage are sub-evidence requirements for ATTEST, not a substitute for the common NULL. No deletion of records and no automatic merge or release.

The C++20 `pentadi.hpp` implements the pure compile-time gate, with `pentadi_test.cpp` static assertions for all-pass, absent, failure and no auto-promotion. LadderDi's PowerShell ATTEST reports explicit `PENTADI|COMMON=NULL` and rejects any non-success prerequisite. Its identity is project-local terminology, not an established mathematical theorem. All recorded source outcomes remain authoritative.
