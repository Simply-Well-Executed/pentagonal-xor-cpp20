# SPDX-License-Identifier: 0BSD
# Freestanding Linux/amd64 evidence runner, explicitly NOT OpenBSD or release-qualified.
# No shell, Python, Node, Go runtime, GNU userland, package manager, or linked libc.
FROM scratch
LABEL org.opencontainers.image.title="OpenBSD 2 COREAO Stage 10 evidence (Linux diagnostic)"
LABEL org.opencontainers.image.description="Independent YIN/YANG witness display, NOT an OpenBSD native build"
LABEL org.opencontainers.image.licenses="0BSD"
LABEL org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20"
LABEL org.opencontainers.image.version="coreao-evidence-v1"
LABEL org.opencontainers.image.vendor="Simply Well Executed"
LABEL org.opencontainers.image.ref.name="CORE=OPENBSD2"
LABEL com.simplywellexecuted.native-openbsd-builds="0"
LABEL com.simplywellexecuted.release-status="HOLD"
COPY --chmod=0555 stage10 /coreao
COPY --chmod=0444 serial-witnesses.jsonl /evidence/serial-witnesses.jsonl
COPY --chmod=0444 native-evidence.tsv /evidence/native-evidence.tsv
COPY --chmod=0444 LICENSE /LICENSE
USER 65534:65534
ENTRYPOINT ["/coreao"]
