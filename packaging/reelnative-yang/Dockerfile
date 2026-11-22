# SPDX-License-Identifier: 0BSD
# Stage-09 OPENBSD2_YANG immutable witness container; REELNATIVE is a display name.
# The runtime is Linux/amd64. There is NO native OpenBSD binary here.
# No libc, shell, Python, Node, Go runtime, package manager or base image.
FROM scratch
LABEL org.opencontainers.image.title="REELNATIVE-YANG | OpenBSD2 stage 09 evidence"
LABEL org.opencontainers.image.description="Pinned YANG witness and independent YIN predecessor; Linux diagnostic, not OpenBSD OS"
LABEL org.opencontainers.image.licenses="0BSD"
LABEL org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20"
LABEL org.opencontainers.image.vendor="Simply Well Executed"
LABEL org.opencontainers.image.version="reelnative-yang-evidence-v1"
LABEL org.opencontainers.image.ref.name="STAGE=09;CORE=OPENBSD2"
LABEL com.simplywellexecuted.native-openbsd-builds="0"
LABEL com.simplywellexecuted.release-status="HOLD"
LABEL com.simplywellexecuted.short-registry-alias="openbsd1"
COPY --chmod=0555 stage09 /reelnative
COPY --chmod=0444 serial-witnesses.jsonl /evidence/serial-witnesses.jsonl
COPY --chmod=0444 native-evidence.tsv /evidence/native-evidence.tsv
COPY --chmod=0444 LICENSE /LICENSE
USER 65534:65534
ENTRYPOINT ["/reelnative"]
