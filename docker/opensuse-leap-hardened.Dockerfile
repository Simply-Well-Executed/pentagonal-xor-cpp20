# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# openSUSE Leap 16.0 base index digest verified at Docker Hub.
# Image and installed distribution packages retain their own licenses.
# 0BSD covers only this project's authored content, not OS packages.
# syntax=docker/dockerfile:1.7
FROM opensuse/leap:16.0@sha256:a4115e2f5aea9712a7c73974fd94000a034fc3051ef29b5f7722fbf04070dd9d AS source

LABEL org.opencontainers.image.title="Pinned openSUSE Leap source-study C++ builder" \
      org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20" \
      org.opencontainers.image.description="openSUSE Leap 16.0 source build and validation environment; mixed third-party OS licenses"

USER root
RUN zypper --non-interactive --gpg-auto-import-keys refresh && \
    zypper --non-interactive install --no-recommends \
       cmake gcc gcc-c++ make ninja git ca-certificates shadow && \
    zypper clean --all
WORKDIR /study
COPY --chown=65534:65534 . /study/
RUN chown -R 65534:65534 /study
USER 65534:65534
CMD ["cmake", "--version"]

# Test and compiler outputs stay in a temporary stage, not in the published image.
FROM source AS validation
RUN --network=none cmake -S . -B /study/build-leap -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
RUN --network=none cmake --build /study/build-leap --parallel 2
RUN --network=none ctest --test-dir /study/build-leap --output-on-failure
RUN --network=none cmake -S shared -B /study/build-shared-leap -DCMAKE_BUILD_TYPE=Release
RUN --network=none cmake --build /study/build-shared-leap --parallel 2
RUN --network=none ctest --test-dir /study/build-shared-leap --output-on-failure
RUN --network=none cmake --install /study/build-leap --prefix /study/stage
RUN --network=none cmake -S tests/consumer -B /study/build-consumer-leap -DSWE_AUDIT_PREFIX=/study/stage
RUN --network=none cmake --build /study/build-consumer-leap --parallel 2
RUN --network=none ctest --test-dir /study/build-consumer-leap --output-on-failure
