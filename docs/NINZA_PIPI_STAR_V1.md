# NINZA PiPi star supervisor v1

EXPLORATORY. Prior FIFONFU v1 and historical FRESH protocols remain unchanged.
The PiPi star is an architectural name, not a new mathematical identity.
Source-of-truth: [Invoke-NinzaPiPi.ps1](../tools/Invoke-NinzaPiPi.ps1).
Diagram: [PiPi star](NINZA_PIPI_STAR.svg).

- FIFONFU: First In First Out, Never Fuck Up; discipline through notes.
- LIFODFU: Last In First Out, Don't Fuck Up; agility with emergency help.
- One bounded pool pre-spawns 64 independent PowerShell helper processes by default.
  Use fewer on constrained systems. This is an explicit startup cost, not a claim
  that 64 is optimal or that the machine has 64 physical cores.
- Initial manifest order is the only arrival-order authority. Explicit FIFONFU
  jobs enqueue at the tail; unmarked jobs are LIFODFU and push onto the stack.
  Unknown lane names are rejected. When both lanes have work, alternate claims,
  starting FIFO. A retry is a new admission on the same lane. Finite batch and
  bounded attempts prevent unbounded LIFO overtaking in this version.
- Helpers are pre-started, wait for a single lease and exit after their job.
  This is a one-shot warm pool, not a persistent multi-job worker. Replenish
  only for pending work, up to the shared cap. Unleased helpers exit 0 with
  IDLE_TTL receipt after 15 seconds; idle retirement is not job failure.
  An idle/lease race may consume an attempt; no concurrent duplicate lease.
- Parent alone reads/writes the protected JSONLedger snapshot. Helpers access
  only per-attempt mailbox files, with no ledger path in their arguments.
  Normal progress via heartbeat/receipt. Emergency JSON is a dedicated route
  to the parent, which records HELP_REQUIRED and confirms stop without retry.
  No automatic emergency resolution or promotion.
- Exit 42 and matching SHA-256 receipt = job completion, scoped to FIFONFU/PiPi.
  Other exits, malformed receipts and running TTL failures have bounded retries
  only for explicitly retry-safe work, after confirmed process exit.
  Startup deadline 90s, heartbeat TTL 10s, hard attempt limit 300s, poll 200ms,
  attempts 3. Deadlines are sampled, not real-time hard guarantees.
- The parent copies and checks the worker/helper source, records the lease before
  dispatch, and writes one ledger snapshot for progress batches. The filesystem
  convention is NOT an access-control sandbox, strong authentication or proof
  of hardware AIRGAP. Trusted workers must remain self-contained, terminate,
  not spawn descendants or access shared state, not network, and not perform
  irreversible effects. Startup failure or unconfirmed stop holds the run.
  Restart after a parent crash, dynamic arrivals, ASR and actual transcription
  integration remain unimplemented. Prior run directories are not reused.

## Run

Manifest:
    {"protocol":"NINZA-PIPI-READY-v1","retry_safe":true,"jobs":[{"job_id":"older","lane":"FIFONFU"},{"job_id":"newer"}]}

Worker contract: accepts JobId, Attempt, HeartbeatPath, ReceiptPath, OutputPath,
EmergencyPath. The original heartbeat and receipt formats are retained.
Emergency format:
    {"protocol":"NINZA-LIFODFU-EMERGENCY-v1","job_id":"newer","attempt":1,"reason":"UNRESOLVED_DEPENDENCY"}

PowerShell:
    $hash = (Get-FileHash -LiteralPath .\worker.ps1 -Algorithm SHA256).Hash
    pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File .\tools\Invoke-NinzaPiPi.ps1 -ManifestPath .\ready.json -WorkerPath .\worker.ps1 -WorkerSha256 $hash -RunDirectory .\pipi-001

Test without private media:
    pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File .\tests\Test-NinzaPiPi.ps1

Fixtures use small worker counts and cover FIFO/LIFO arbitration, warm start,
idle cleanup, non-42 and TTL retries, emergency HOLD and independent completion.
PowerShell runtime verification is pending; the default 64-worker load is not
benchmarked. No changes to STD-free C++20 core or release HOLD. No media committed.
The diagram depicts four conceptual graph domains and logical star routing,
not a physical RF antenna or actual omnidirectional performance measurement.
