# COREAO STANDARDIZER/1 | XOR-OR-XOR

Status: project-local exploratory reproducibility contract, not an industry
standard, cryptographic authentication, or release authority.

For unsigned 8-bit LEFT, RIGHT, CORE, ENVELOPE:
1. RIGHT_CORE = RIGHT XOR CORE (nested right-side XOR).
2. XOR_SHELL = LEFT XOR RIGHT_CORE (surrounding XOR).
3. STANDARD = ENVELOPE OR XOR_SHELL (outer OR).

Explicit grouping: ENVELOPE OR (LEFT XOR (RIGHT XOR CORE)).
A masked-corruption canary flips bit 0x04 in CORE and requires the XOR stage
to detect it even when OR-wrapped output is unchanged. OR alone is insufficient
evidence of integrity.

Test inputs: LEFT=0x96, RIGHT=0x69, ENVELOPE=0x24; CORE=1..255. Each round
checks the independent XOR oracle, XOR reversibility, OR containment, and
the negative canary. No approximate or truncated arithmetic.

The same workflow invokes a fresh native PowerShell process 255 times per PR,
then verifies each SHA-256 TSV receipt and the source files. This means
255 executions inside one GitHub Actions run per PR, not 255 separate GitHub
workflow run IDs or 255 complete native builds. Native MSVC C++20 /O2 and CTest
qualification still belongs to PR-PARITY. Failed checks remain HOLD.
