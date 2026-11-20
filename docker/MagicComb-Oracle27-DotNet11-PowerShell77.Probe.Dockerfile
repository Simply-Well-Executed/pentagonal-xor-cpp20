# syntax=docker/dockerfile:1
# SPDX-License-Identifier: 0BSD
# UNPUBLISHED TOOLCHAIN PROBE. Never push as a release.
# GNU/LGPL OS packages and Oracle NFTC are not 0BSD; license gate will HOLD.
ARG DOTNET11_BASE
FROM ${DOTNET11_BASE} AS toolchain_probe
ARG SOURCE_REVISION
ARG ORACLE_JAVA_SHA256=50d42ab5bacc735aa9b4da9c451f631a4b5c09e00b961df412a26d159c538cfc
ARG POWERSHELL_SHA256=c89d9c682d2c403dfe726f3df948e359675235a5bd6fab535776de69729ca0cb

LABEL org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20" \
      org.opencontainers.image.revision="${SOURCE_REVISION}" \
      org.opencontainers.image.title="HEAD/TAIL experimental .NET11 PowerShell7.7 OracleJava27 (NOT APPROVED)" \
      org.opencontainers.image.licenses="LicenseRef-MIXED-REVIEW-REQUIRED"

ENV DEBIAN_FRONTEND=noninteractive \
    POWERSHELL_TELEMETRY_OPTOUT=1 \
    DOTNET_CLI_TELEMETRY_OPTOUT=1 \
    DOTNET_SKIP_FIRST_TIME_EXPERIENCE=1 \
    JAVA_HOME=/opt/oracle/java \
    PATH=/opt/oracle/java/bin:/opt/microsoft/powershell/7:${PATH}

# Disposable GitHub-hosted build stage. This apt install is explicitly
# NOT 0BSD; resulting GNU-licensed packages require a human exception.
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl tar gzip xz-utils make cmake g++ git grep gawk findutils \
    && rm -rf /var/lib/apt/lists/*

# MagicComb HEAD: fetch Microsoft-signed release asset URL, verify frozen
# official SHA256 BEFORE tar inspection and extraction. No pipe to shell.
RUN set -eux; \
    mkdir -p /opt/microsoft/powershell/7; \
    curl --fail --location --silent --show-error --retry 3 \
      'https://github.com/PowerShell/PowerShell/releases/download/v7.7.0/powershell-7.7.0-linux-x64.tar.gz' \
      --output /tmp/powershell-7.7.0-linux-x64.tar.gz; \
    echo "${POWERSHELL_SHA256}  /tmp/powershell-7.7.0-linux-x64.tar.gz" | sha256sum -c -; \
    if tar -tzf /tmp/powershell-7.7.0-linux-x64.tar.gz | grep -Eq '(^/|(^|/)\.\.(/|$))'; then exit 12; fi; \
    tar --no-same-owner -xzf /tmp/powershell-7.7.0-linux-x64.tar.gz -C /opt/microsoft/powershell/7; \
    chmod 755 /opt/microsoft/powershell/7/pwsh; \
    rm -f /tmp/powershell-7.7.0-linux-x64.tar.gz

# Oracle OFFICIAL Java 27.0.1.1. Archive fetched from Oracle directly.
# This is Oracle NFTC, not 0BSD; the toolchain JDK is NOT published.
RUN set -eux; \
    mkdir -p /opt/oracle; \
    curl --fail --location --silent --show-error --retry 3 \
      'https://download.oracle.com/java/27/latest/jdk-27_linux-x64_bin.tar.gz' \
      --output /tmp/oracle-jdk27.tar.gz; \
    echo "${ORACLE_JAVA_SHA256}  /tmp/oracle-jdk27.tar.gz" | sha256sum -c -; \
    if tar -tzf /tmp/oracle-jdk27.tar.gz | grep -Eq '(^/|(^|/)\.\.(/|$))'; then exit 13; fi; \
    tar --no-same-owner -xzf /tmp/oracle-jdk27.tar.gz -C /opt/oracle; \
    set -- /opt/oracle/jdk-27*; test "$#" -eq 1 && test -x "$1/bin/java"; \
    ln -s "$1" /opt/oracle/java; \
    rm -f /tmp/oracle-jdk27.tar.gz

RUN set -eux; \
    dotnet --version | grep -E '^11\.0\.'; \
    pwsh -NoLogo -NoProfile -Command 'if ($PSVersionTable.PSVersion.ToString() -cne "7.7.0") { throw "PowerShell version mismatch" }; $PSVersionTable.PSVersion.ToString()'; \
    java -version 2>&1 | grep -E 'version "27\.0\.1'; \
    javac -version 2>&1 | grep -E '^javac 27\.0\.1'

WORKDIR /study
COPY --chown=65534:65534 . /study/
RUN /bin/sh /study/ci/Assert-AndroidLinux-NoPython.sh image
# Disposable toolchain still contains copyleft system packages: do not publish.
# Its test runtime is non-root, with all build/test writes confined to tmpfs.
USER 65534:65534
# Tail tests and license inventory run in the GitHub Actions cloud job; this
# Dockerfile NEVER declares a publication tag or a production release.
CMD ["/bin/sh", "-c", "dotnet --version && pwsh -NoLogo -NoProfile -Command '$PSVersionTable.PSVersion.ToString()' && java -version"]
