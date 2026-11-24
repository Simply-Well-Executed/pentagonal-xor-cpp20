# SPDX-License-Identifier: 0BSD
# THAILONGA postflight V1 stage-07 immutable derivative image (separate from frozen v1); REELNATIVE is a display name.
# The runtime is Linux/amd64. There is NO native OpenBSD binary here.
# No libc, shell, Python, Node, Go runtime, package manager or base image.
FROM scratch
LABEL org.opencontainers.image.title="REELNATIVE-RETURN-THAILONGA | stage 07 postflight core projection"
LABEL org.opencontainers.image.description="Frozen OpenBSD4_RETURN with separate YIN/YANG THAILONGA postflight projection, Linux diagnostic only"
LABEL org.opencontainers.image.licenses="0BSD"
LABEL org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20"
LABEL org.opencontainers.image.vendor="Simply Well Executed"
LABEL org.opencontainers.image.version="reelnative-return-thailonga-evidence-v2"
LABEL org.opencontainers.image.ref.name="STAGE=07;CORE=NONE;PASS=RETURN;OPENBSD=4.9"
LABEL com.simplywellexecuted.canonical-stage07-core="NONE"
LABEL com.simplywellexecuted.thailonga-projection-core="OPENBSD2"
LABEL com.simplywellexecuted.thailonga-mode="POSTFLIGHT_ONLY"
LABEL com.simplywellexecuted.native-openbsd-builds="0"
LABEL com.simplywellexecuted.release-status="HOLD"
LABEL com.simplywellexecuted.short-registry-alias="openbsd3"
COPY --chmod=0555 stage07 /reelnative
COPY --chmod=0444 serial-witnesses.jsonl /evidence/serial-witnesses.jsonl
COPY --chmod=0444 native-evidence.tsv /evidence/native-evidence.tsv
COPY --chmod=0444 thailonga-postflight-v1.jsonl /evidence/thailonga-postflight-v1.jsonl
COPY --chmod=0444 LICENSE /LICENSE
USER 65534:65534
ENTRYPOINT ["/reelnative"]
