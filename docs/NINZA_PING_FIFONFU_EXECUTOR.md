# NINZA PING executor / FIFONFU admission seam

EXPLORATORY. Release HOLD unchanged. Parent PR head ca8c75f65d0e1b9d4a8beea58296279b72cd34f6.

The local transcription PING v1 uses independent 54-byte blocks (9 lanes x 6 columns). The new host executor supports independent, bounded batches, including PING twirl and untwirl using the existing canonical lane mapping. It does not prove arbitrary workloads or dependent speech tokens can run in parallel. The original sigma_penta_ping.hpp, inverse, HANDI observer and STD-free CORE are unchanged.

The operator clarified the military-inspired project terminology on 2026-11-20: FIFODFU = First In, First Out, Don't Fuck Up; FIFONFU = First In, First Out, Never Fuck Up. The distinction is operational maturity through keeping notes, not a claim of infallibility. NFU here does not mean a frequency-based eviction algorithm. The parent owns the ready queue and its arrival order; this executor consumes an explicit permutation of ready job IDs, rejecting missing or invalid orders before dispatch. It does not independently establish arrival order, dependencies, or dynamic admissions. The parent supervisor in [FIFONFU supervision v1](NINZA_FIFONFU_SUPERVISION_V1.md) records attempts, outcomes, heartbeats and recovery. FIFO claim order does not guarantee OS start or completion order.

1-64 native workers; maximum 65536 jobs per batch; 1 worker is explicit serial mode, no automatic fallback. Claim order follows the supplied order, not OS start/finish timing. Each callback must terminate and read immutable input, write only its unique provisional output slot, and avoid external side effects and nested execution. Exceptions/false produce HOLD, all workers join, and failed provisional outputs must not be published. Pool creation failure holds without dispatch. Not transactional rollback. COMPLETE is callback success, not transcription accuracy, security approval or independent attestation. No network PING, physical AIRGAP assertion or ASR.

Opt-in C++20 host adapter uses standard threads outside the STD-free core; platform runtime and license review are pending. No default build change, no release authorization. Earlier OpenBSD serial protocols are independent.

Build with CMake:
    cmake -S . -B build-ping-executor -DBUILD_TESTING=ON -DSWE_NINZA_PING_EXECUTOR=ON
    cmake --build build-ping-executor --config Release --target ninza_ping_scheduler_test
    ctest --test-dir build-ping-executor -C Release -R ninza_ping_scheduler_explicit_order --output-on-failure

The test calls the canonical lane mapping in both directions, verifies serial/parallel equivalence, restored bytes, exact-once dispatch, invalid input HOLD, callback failure and concurrent overlap. The transcription CLI itself remains unchanged until the policy and adapter are integrated. No audio or transcript data is committed.
