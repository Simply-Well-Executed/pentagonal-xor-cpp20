# FIFONFU parent supervision, v1

EXPLORATORY. Adds subprocess supervision to the independent C++20 PING
executor introduced in bc84f84e50a4785087ac47e654b1c59b33c9b127.
The operator clarified the military-inspired project terminology on
2026-11-20:

- FIFODFU = First In, First Out, Don't Fuck Up (the earlier discipline).
- FIFONFU = First In, First Out, Never Fuck Up (the matured discipline).
- The method of improvement is keeping notes.

FIFO governs admission and dispatch claims, not parallel start/finish order.
NFU is an aspiration to prevent repeated and undocumented mistakes, not
an assertion that software cannot fail; it is not a frequency-based
eviction algorithm. The parent-readable JSONLedger is the operational
notebook: failures remain recorded, progress is monitored, and retries
are informed by explicit evidence. Initial ready order comes from the
caller manifest; eligible retries are new admissions at the queue tail.
Tail placement is this version's explicit recovery choice. The code does
not independently verify arrival timestamps or implement dynamic admissions.
Never silently discard a failed attempt, assume success, or treat a
successful retry as erasure of the original failure.

## Parent/child contract

The parent owns JSONLedger.json, keeps it in process memory and writes a
flushed temporary JSON snapshot followed by same-directory replacement.
Readers see the last published snapshot. Every attempt has a unique
directory, PID, monotonic heartbeat deadline, hard deadline, outcome
and exit code. A child can publish an increasing integer seq heartbeat
with the matching protocol, job_id and attempt. Replayed, malformed or
stale records do not refresh the timer. Short-lived children need not
heartbeat if they finish and produce a valid receipt before the deadline.

Success is scoped: **42 + COMPLETE receipt + matching job/attempt and
SHA-256 of output.bin**. Non-42, missing/corrupt receipt, TTL and hard
deadline failures are recorded, with a bounded retry appended to the
ready queue. A killed child must be confirmed exited before retry; an
unconfirmed stop places the parent on HOLD without retry. The parent
returns 42 only if all jobs complete; otherwise non-42. This does NOT
redefine the historical FRESH exit 42 (HOLD) or CTest exit 0.

The worker must be locally trusted, hash-pinned, self-contained,
retry-safe/idempotent, and use only its assigned attempt files. It must
not spawn descendants, write shared state, make network calls, or perform
irreversible external effects. Hashes and file conventions are integrity
checks, not a sandbox or authenticated evidence. Each attempt is isolated
by directory, not permissions. OS scheduling and timer granularity limit
wall-clock precision. Parent crash leaves the last snapshot for manual
review; automatic restart reconciliation and process ownership across
parent crashes are NOT implemented. No automatic duplicate respawn on
parent restart. There is no automatic resume; use a new run directory
only after checking previous processes and outputs. Physical AIRGAP is
not established by this supervisor. No actual transcription backend is
provided; PING's C++20 thread scheduler remains independent.

## Use

Explicit ready manifest:
```json
{"protocol":"NINZA-FIFONFU-READY-v1","retry_safe":true,"job_ids":["block-000","block-001"]}
```

Worker PowerShell script parameters: JobId, Attempt, HeartbeatPath,
ReceiptPath, OutputPath. Atomically replace heartbeat.json with
{"protocol":"NINZA-FIFONFU-HEARTBEAT-v1","job_id":"block-000","attempt":1,"seq":0}
and increasing seq values. Write output.bin, close it, then publish
receipt.json with protocol NINZA-FIFONFU-RECEIPT-v1, job_id, attempt,
status COMPLETE, and lowercase output_sha256; exit 42 last. The worker
fixture is an example, not the production transcription adapter.

```powershell
$hash = (Get-FileHash -LiteralPath .\worker.ps1 -Algorithm SHA256).Hash
pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File .\tools\Invoke-NinzaFifonfu.ps1 -ManifestPath .\ready.json -WorkerPath .\worker.ps1 -WorkerSha256 $hash -RunDirectory .\ninza-run-001 -Workers 4
```

Run synthetic recovery fixtures:
```powershell
pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File .\tests\Test-NinzaFifonfu.ps1
```

Tests cover success, non-42 retry, stale heartbeat TTL, hard attempt
deadline even with heartbeats, permanent failure and false success.
CTest registration is opt-in with SWE_NINZA_PING_EXECUTOR and pwsh
available. Tests return 0 on asserted success and retain the ledger.
Actual runtime evidence must be checked separately. Release HOLD and
all historical protocols stay unchanged.
