# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Public 'source' stage: SDK-free, Gradle-free Android source-study.
# Project source is 0BSD; third-party image layers retain their own licenses.
# Private 'validation' stage: SDK is present only in disposable CI.
ARG DOTNET_SDK_BASE=mcr.microsoft.com/dotnet/sdk:10.0-noble
FROM ${DOTNET_SDK_BASE} AS source
ARG SOURCE_REVISION=unverified-source

LABEL org.opencontainers.image.title="Android source study v1 (SDK-free)" \
      org.opencontainers.image.revision="${SOURCE_REVISION}" \
      org.opencontainers.image.description="0BSD offline acoustic source and Java17/PowerShell build harness" \
      org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20"

ENV POWERSHELL_TELEMETRY_OPTOUT=1 \
    DOTNET_CLI_TELEMETRY_OPTOUT=1 \
    DOTNET_SKIP_FIRST_TIME_EXPERIENCE=1 \
    JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64

USER root
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      openjdk-17-jdk-headless ca-certificates curl unzip cmake g++ make ninja-build git && \
    rm -rf /var/lib/apt/lists/* && \
    useradd --uid 10001 --create-home --shell /usr/sbin/nologin swebuild

# Python is forbidden in the Android/Linux source-study runtime, including
# transitive base-image packages. Purge only package-manager-owned Python
# packages; never silently delete arbitrary OS files or bypass a failed test.
RUN set -eu; \
    installed="$(dpkg-query -W -f='${binary:Package} ${db:Status-Abbrev}\n' | \
      awk '$2 == "ii" && $1 ~ /^(python([0-9.]|-)|libpython[0-9.]|pypy|pipx)(:|$)/ {print $1}')"; \
    if test -n "$installed"; then \
      apt-get purge --yes --auto-remove $installed; \
    fi; \
    rm -rf /var/lib/apt/lists/*

WORKDIR /study
COPY --chown=10001:10001 . /study/
# Historical no-Python shell gate is retired; this build deliberately stops at HOLD.
# This is not a replacement image audit or permission to publish GNU-dependent layers.
RUN ["pwsh", "-NoLogo", "-NoProfile", "-NonInteractive", "-File", "/study/ci/FRESH-GammaGamma.ps1"]
RUN chown -R 10001:10001 /study
USER 10001:10001
ENTRYPOINT ["pwsh", "-NoLogo", "-NoProfile"]
CMD ["-File", "./ci/Test-ReleaseSafety.ps1"]

# Never publish this stage: Android SDK components require their own license.
FROM source AS android-sdk
USER root
ARG ACCEPT_ANDROID_SDK_TERMS=false
RUN test "$ACCEPT_ANDROID_SDK_TERMS" = "true"
RUN pwsh -NoLogo -NoProfile -File ./ci/Initialize-AndroidSdk.ps1 -SdkHome /opt/android-sdk -AcceptLicenses
ENV ANDROID_HOME=/opt/android-sdk \
    ANDROID_SDK_ROOT=/opt/android-sdk
RUN chown -R 10001:10001 /study
USER 10001:10001

# Manual SDK target is intentionally not distributable. The trusted guard
# runs its tests with --network none and minimal Linux capabilities.
FROM android-sdk AS validation
RUN pwsh -NoLogo -NoProfile -File ./ci/Invoke-AndroidDockerValidation.ps1

# Linux native C11/C++20 guards also execute inside a disposable container.
# No Android SDK is installed in this stage.
FROM source AS linux-validation
RUN pwsh -NoLogo -NoProfile -File ./ci/Invoke-LinuxDockerValidation.ps1
