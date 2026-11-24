# OctaDi G-table: Python2 first, Python3 second, Python23 expansion

Experiment ID: OCTADI-G-001. Source license: 0BSD.

The single **octadi/G.py** file contains class G. Its two-entry table begins at index zero with `Python2` and index one with `Python3`. `Python23` is an additional compatibility **algorithm**, not a third interpreter or a new version of Python.

The finite exhaustive order set is **two binary permutations (2!)** and **six ternary permutations (3!)**, tested independently by both Python 2 and 3 against the SAME nine canonical PentaDi records. This yields sixteen interpreter-layout trials if both runtimes exist; a 255-lap check evaluates each layout, alongside an OR-envelope negative canary. The wrapper recomputes SHA-256 and checks identical stdout across interpreter processes, intact input source, and intentional corruption rejection.

No imports, `pip`, external packages, network requests, dynamic includes, installers or GitHub actions implemented in Python are present. **Runtime provisioning is separate from laboratory execution**. Only a separately installed Python **2.7.18** can satisfy the Python2 requirement. Python3 runs separately and may not impersonate Python2; the exact observed Python3 version and executable digest are recorded. Python2 is end-of-life and functional test PASS never implies a current security maintenance guarantee.

If either locally provisioned executable is absent or fails its version/behavioral check, the overall OCTADI workflow remains **HOLD**, even if all eight layouts pass under the other interpreter. Never downgrade a full two-interpreter test to a single-runtime PASS.

The shared PR-PARITY exclusion applies a **one-path exception only** for `octadi/G.py`; all other unsanctioned Python sources remain forbidden, and the offline interpreter-custody scan continues to report unresolved provenance separately. A successful OCTADI experiment is not full PR-PARITY approval, an air-gap physical proof, or permission to merge or release.

For local execution: set `OCTADI_PY2_EXE` and `OCTADI_PY3_EXE` to absolute paths of separately installed interpreters. Then use PowerShell Core with `-NoLogo -NoProfile -NonInteractive -File octadi/Invoke-OctaDiGTable.ps1 -RepositoryRoot . -EvidenceDirectory <fresh-external-folder>`.

No historical data or third-party interpreter binaries are included in this repository.
