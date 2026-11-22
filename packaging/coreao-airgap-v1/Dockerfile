# SPDX-License-Identifier: 0BSD
# Candidate and postflight stages are Linux/amd64 scratch-only custody diagnostics.
# Neither a physical airgap nor an OpenBSD native build is claimed.
FROM scratch AS trial
ARG SOURCE_SHA
LABEL org.opencontainers.image.title="COREAO MAGICCOMB airgap V1, SPIN9-YANG / STAR27-YIN"
LABEL org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20"
LABEL org.opencontainers.image.revision="${SOURCE_SHA}"
LABEL org.opencontainers.image.licenses="0BSD"
LABEL com.simplywellexecuted.network="NONE_DURING_BUILD_AND_RUNTIME"
LABEL com.simplywellexecuted.coreao-mode="DERIVED_POSTFLIGHT_ONLY"
LABEL com.simplywellexecuted.native-openbsd-builds="0"
LABEL com.simplywellexecuted.release-status="HOLD"
COPY --chmod=0555 coreao-tick /coreao-tick
COPY --chmod=0444 source.zip /custody/exact-github-source.zip
COPY --chmod=0444 serial-witnesses.jsonl /custody/serial-witnesses.jsonl
COPY --chmod=0444 native-evidence.tsv /custody/native-evidence.tsv
COPY --chmod=0444 LICENSE /LICENSE
USER 65534:65534
ENTRYPOINT ["/coreao-tick"]

FROM trial AS postflight
COPY --chmod=0444 coreao-airgap-receipt-v1.json /custody/coreao-airgap-receipt-v1.json
