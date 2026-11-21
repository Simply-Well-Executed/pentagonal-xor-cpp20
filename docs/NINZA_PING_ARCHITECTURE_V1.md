# SUNDER

WRAP -> CHURN (66 degrees UP) -> SUNDER -> RIP -> INTEGRATE -> REVIEW.

SUNDER is the entry point for Ping, the local PiPi architecture, distinct from the PING byte permutation and network ping. WRAP preserves manifest bytes, source hashes and configuration. CHURN is geometric metadata. SUNDER separates payload and provenance; RIP extracts verified manifest bytes without destroying the envelope. INTEGRATE invokes PiPi with 64 warm helpers by default. REVIEW checks the final ledger and accepted receipts. Exit 42 means scoped completion; HOLD preserves evidence. No runtime injection, ASR or physical AIRGAP assurance. Trusted local worker required. Existing release HOLD remains.

    $hash = (Get-FileHash -LiteralPath .\worker.ps1 -Algorithm SHA256).Hash
    pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File .\tools\SUNDER.ps1 -ManifestPath .\ready.json -WorkerPath .\worker.ps1 -WorkerSha256 $hash -RunDirectory .\ping-001

See the [PiPi contract](NINZA_PIPI_STAR_V1.md). Outputs: Ping-envelope.json, Ping-result.json, NINZA/JSONLedger.json. Runtime validation pending.
