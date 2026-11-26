# MagicComb FILTERPASS for telemetry ingress and egress

Status: EXPLORATORY contract in PR 97, not deployed and not a RELEASE approval
Authority: CURRENT source and tested implementation, with Git history preserving prior states
Scope: defensive NGINX/application/collector boundary design and regression acceptance criteria
Existing MagicComb implementation: ci/Invoke-MagicComb.ps1 checks repository text at rest
Existing COREAO entry point: ci/Invoke-CoreaoAirgapMagicCombV1.ps1 is a separate source and build pipeline
Neither existing script is evidence that this runtime telemetry contract is implemented

## Security invariant: no collector bypass

Every network ingress, application trust transition, telemetry handoff, and network egress must have a distinct validation policy
A sender is never allowed to skip the collector's own receiving filter
A logging agent and a SIEM are untrusted by default, including their configuration, credentials, queries and response webhooks
No filter is allowed to treat a successful prior filter as blanket authorization for a later boundary
Direct sending from an ingress probe to an external collector is disallowed

Proposed flow:

Client -> filter ingress -> NGINX -> filter upstream egress -> service ingress -> application -> service egress -> response filter -> client
Local validated event -> bounded local spool -> telemetry ingress filter -> canonical event validation -> regression/anomaly policy -> telemetry egress filter -> mutually authenticated collector ingress -> local audit or explicitly enabled SIEM adapter

The arrows from separate traffic filters to the local spool are bounded in-process or host-local transfers, not hidden external network paths
Collector data is inert data, never a shell command, script, URL fetching directive, or automatic execution permission
The event collector has its own filter and authentication boundary, independent from the filter that generated an event

## Byte and provenance requirements

HEAD receives raw bytes with exact source boundary, byte length, direction, trust zone, identity and monotonic sequence
Validate UTF-8 strictly where UTF-8 is required, and represent non-text binary payloads separately
Record explicit interpretation context for HTML entities, Markdown, Unicode bidi isolates and normalization decisions
Raw bytes, decoded code points, canonical comparison representation and rendered presentation are different artifacts
Never silently apply NFKC to live user input, protocol identifiers, credentials, exact legal texts, historical source, hashes or signed payloads
NFKC output can be examined as a non-authoritative comparison view only, with original bytes kept intact
Malformed controls, mismatched bidi isolate depth, prohibited encoding, or inconsistent context are explicit HOLD findings rather than automatic destructive cleaning
Validate both input and output boundaries independently, including outbound log fields

## Event schema, minimal and privacy-preserving

schema_version, event_id, correlation_id, source_station, destination_station, ingress_or_egress, exact_policy_version, sequence, observed_time, source_type, decision, reason_code, length, optional digest, local_witness_id
Avoid raw HTTP bodies, cookies, passwords, session tokens, credentials, identifiers not needed for correlation, and private content
Hashing sensitive low-entropy values does not automatically anonymize them
For audit storage, apply authenticated origin and integrity verification with rotation, replay defense, access controls and retention limits
Do not put hardcoded secrets, tokens, remote endpoints, or event samples with private data in Git history

## Telemetry defaults and fail behavior

Default exporter mode LOCAL_ONLY
Datadog or any remote SIEM requires an explicit reviewed configuration change and a separate approved egress policy
Use bounded append-only local spool with quotas, overflow counters and independent spool health alarms
Do not allow a collector outage to create unbounded memory growth or raw-log network fallback
Production traffic fail behavior must be explicitly defined per boundary with service availability and safety considered, never silently changed by an exporter outage
The telemetry subsystem cannot bypass source-data validation even when retrying, reconnecting or replaying
Pre-egress scrub must happen before external transmission
All remote collector responses are untrusted data

## Automated security biters

OBSERVE -> independent corroboration -> authorized scoped containment -> expiry -> rollback -> recorded review
No SIEM event alone grants the authority to run a command, change credentials, delete files, trigger destructive incident response or block a user
Use bounded temporary rate limits or isolation only for declared rules and protected scope
Apply evidence thresholds, allowlists, false-positive review, emergency disable and audit of all actuator calls
Unauthorized, ambiguous or unverified response request means HOLD, not inferred consent

## Continuous regression acceptance

Implement a reproducible fixture suite and an independent watch process before activation
The user phrase 24/7/365/99999 is a requested continuous endurance target, not a defined unit or a claim of current coverage

Required negative and positive trials:
1 Valid UTF-8 byte-exact roundtrip and length preservation
2 Invalid UTF-8 rejection without replacement characters or silent truncation
3 HTML entity interpretation separated from raw source, including haod&#x61;
4 Bidi isolate depth correct, extra PDI and unclosed isolate detected, explicit newline boundaries observed
5 Unicode normalization comparison does not alter preserved source bytes
6 Forged event origin, signature mismatch and missing station ID rejected
7 Duplicate event IDs, missing sequence numbers, delayed replay and reordered events detected
8 Attempts to send events directly to external SIEM without telemetry egress filter rejected
9 Collector outage, backpressure, capped queues, restart and disk-full recovery tested
10 Event-to-command payloads remain inert and cannot become an executable response
11 Spool path traversal, symlink and confused-deputy attempts rejected
12 Sanitization of secrets and private fields checked before any permitted remote send
13 Regression of boundary topology and policy versions detects unauthorized path changes
14 Containment always expires and provides a reversible recorded rollback
15 Event-bus and watcher health monitored independently to detect silent failure

Define repeatable expected results, source commit, fixture digest, environment, error reason and PASS/HOLD for each test
Do not label the suite PASS unless it was actually executed and all required checks passed

## Eulger's Jig / HEXABIT

Eulger's Jig is user-proposed exploratory terminology for a byte-oriented adaptation of Euler's Jig
It is not an established IEEE, NIST or mathematical primitive in this repository
Definition is pending: input alphabet and framing, nine-chain operator order, G topology, exact HEXABIT representation, mapping from nine repetitions to output, acceptance predicate, and reference test vectors
Do not implement or assert physical momentum from topology by analogy
Until definitions are signed off, Jig checks report UNDEFINED or HOLD, never fabricated PASS
NICE may provide workforce/task accountability categories but does not define byte-level network correctness

## Integration boundaries

Map NGINX access/logging and transport configuration separately from an actual payload-inspection engine
For production implementation prefer native C++20 with explicit compile and license review under the repository's existing policy
PowerShell may serve as offline audit and regression orchestration, not a hidden internet telemetry side channel
Do not introduce Python, Node/npm, an unreviewed cloud exporter, or automatically executed dependencies
No mutation to frozen experimental protocol or historical evidence without a new ID and review

## Release policy

This PR records a design and acceptance contract only
No running proxy integration, active SIEM forwarding, telemetry filter, synthetic probes, 24/7 watcher, build success or CI success is claimed
RELEASE HOLD until implementation and all independent regression evidence exist
Chosen never assumed
