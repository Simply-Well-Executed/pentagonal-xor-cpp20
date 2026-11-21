# SPDX-License-Identifier: 0BSD
# Stage 06 is OPENBSD3 / historical release 3.9; the alias 'openbsd4' is
# a publication-order nickname, not a claim to contain the OpenBSD operating system.
# Builds a Linux/amd64 freestanding program off-container using C++20 -O2.
FROM scratch
LABEL org.opencontainers.image.title="ALONZ ENVELOPE | stage 06 source witness"
LABEL org.opencontainers.image.description="C++20 Linux/amd64 offline diagnostic for canonical OPENBSD3 3.9 stage 06; not an OpenBSD OS image"
LABEL org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20"
LABEL org.opencontainers.image.vendor="Simply Well Executed"
LABEL org.opencontainers.image.licenses="0BSD"
LABEL org.opencontainers.image.version="alonz-envelope-stage06-thailonga-v2"
LABEL org.opencontainers.image.ref.name="STAGE=06;IDENTITY=OPENBSD3;OPENBSD_RELEASE=3.9;CORE=OPENBSD2;CORE_MODE=THAILONGA_POSTFLIGHT;CANONICAL_CORE=NONE"\nLABEL com.simplywellexecuted.core-provenance="THAILONGA_POSTFLIGHT_V1,AFTER_STAGE10,NOT_STAGE06_INPUT"
LABEL com.simplywellexecuted.image-alias="openbsd4"
LABEL com.simplywellexecuted.native-openbsd-builds="0"
LABEL com.simplywellexecuted.release-status="HOLD"
COPY --chmod=0555 stage06 /alonz
COPY --chmod=0444 serial-witnesses.jsonl /evidence/serial-witnesses.jsonl
COPY --chmod=0444 native-evidence.tsv /evidence/native-evidence.tsv
COPY --chmod=0444 thailonga-stage07-postflight.jsonl /evidence/thailonga-stage07-postflight.jsonl
COPY --chmod=0444 LICENSE /LICENSE
USER 65534:65534
ENTRYPOINT ["/alonz"]
