# NINZA PING executor / FIFONFU admission seam

EXPLORATORY. Release HOLD unchanged. Parent PR head ca8c75f65d0e1b9d4a8beea58296279b72cd34f6.

The local transcription PING v1 uses independent 54-byte blocks (9 lanes x 6 columns). The new host executor supports independent, bounded batches, including PING twirl and untwirl using the existing canonical lane mapping. It does not prove arbitrary workloads or dependent speech tokens can run in parallel. The original sigma_penta_ping.hpp, inverse, HANDI observer and STD-free CORE are unchanged.

FIFONFU is the requested policy name. Its exact prior definition was not available for this revision. No FIFO/NFU approximation is selected. Caller must provide a complete explicit permutation of ready job IDs; missing or invalid order returns HOLD without executing jobs. This is a policy admission seam, not a completed FIFONFU implementation. Dynamic arrivals, dependencies, counters, fairness, eviction and priority semantics are not assumed.

1-64 native workers; maximum 65536 jobs per batch; 1 worker is explicit serial mode, no automatic fallback. Claim order follows the supplied order, not OS start/finish timing. Each callback must terminate and read immutable input, write only its unique provisional output slot, and avoid external side effects and nested execution. Exceptions/false produce HOLD, all workers join, and failed provisional outputs must not be published. Pool creation failure holds without dispatch. Not transactional rollback. COMPLETE is callback success, not transcription accuracy, security approval or independent attestation. No network PING, physical AIRGAP assertion or ASR.

Opt-in C++20 host adapter uses standard threads outside the STD-free core; platform runtime and license review are pending. No default build change, no release authorization. Earlier OpenBSD serial protocols are independent.

Build with CMake:
    cmake -S . -B build-ping-executor -DBUILD_TESTING=ON -DSWE_NINZA_PING_EXECUTOR=ON
    cmake --build build-ping-executor --config Release --target ninza_ping_scheduler_test
    ctest --test-dir build-ping-executor -C Release -R ninza_ping_scheduler_explicit_order --output-on-failure

The test calls the canonical lane mapping in both directions, verifies serial/parallel equivalence, restored bytes, exact-once dispatch, invalid input HOLD, callback failure and concurrent overlap. The transcription CLI itself remains unchanged until the policy and adapter are integrated. No audio or transcript data is committed.
