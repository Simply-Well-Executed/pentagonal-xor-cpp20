# SPDX-License-Identifier: 0BSD
# Full-source REELNATIVE autostart candidate. No default toolchain: fail closed.
# DO NOT use an unreviewed Linux/GNU/POSIX base image. Toolchain and runtime
# must be independently approved and explicitly pinned to OCI SHA256 digests.
# Both builder and runtime bases are supplied through the gated GitHub workflow.
ARG TOOLCHAIN_IMAGE
ARG RUNTIME_IMAGE
FROM ${TOOLCHAIN_IMAGE} AS full-source-validation
LABEL org.opencontainers.image.title="SWE full repository C++20 / CTest validation"
LABEL org.opencontainers.image.licenses="0BSD"
LABEL com.simplywellexecuted.stage="source-build-tests-only"
WORKDIR /opt/swe/source
# Include LICENSE, LICENSES/, Archive/Retired/, .github/, docs/, android/, java/
# and all other non-secret tracked files. .dockerignore only excludes VCS,
# generated outputs and sensitive key/credential files.
COPY . /opt/swe/source/
# JSON exec form: no implicit /bin/sh or Bash; build and test are inside Docker.
RUN ["pwsh","-NoLogo","-NoProfile","-File","/opt/swe/source/ci/Invoke-FullSourceAutostartNativeSuite.ps1","-RepositoryRoot","/opt/swe/source","-OutputDirectory","/opt/swe/build","-Platform","Linux"]

FROM ${RUNTIME_IMAGE} AS autostart
ARG SOURCE_SHA
ARG PROFILE
ARG EVIDENCE_REF
LABEL org.opencontainers.image.title="SWE complete-source normal-CLI autostart"
LABEL org.opencontainers.image.description="Full C++20 root/shared/consumer tested, normal parity executable autostarts; not OpenBSD OS"
LABEL org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20"
LABEL org.opencontainers.image.revision="${SOURCE_SHA}"
LABEL org.opencontainers.image.licenses="0BSD"
LABEL com.simplywellexecuted.runtime-profile="${PROFILE}"
LABEL com.simplywellexecuted.evidence-reference="${EVIDENCE_REF}"
LABEL com.simplywellexecuted.native-openbsd-builds="0"
LABEL com.simplywellexecuted.release-status="NATIVE_OPENBSD_HOLD"
# The full original source and license reference library remain inspectable.
COPY --from=full-source-validation --chown=65534:65534 /opt/swe/source/ /opt/swe/source/
COPY --from=full-source-validation --chown=65534:65534 /opt/swe/build/install/ /opt/swe/app/
USER 65534:65534
WORKDIR /opt/swe/source
# Direct normal production executable; no shell, process supervisor, restart loop or new interpreter.
ENTRYPOINT ["/opt/swe/app/bin/pentagonal_xor"]
CMD ["0.80","0.70","0.90","0.60","0.95"]
