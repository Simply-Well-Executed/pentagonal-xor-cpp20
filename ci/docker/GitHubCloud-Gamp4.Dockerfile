# syntax=docker/dockerfile:1
# SPDX-License-Identifier: 0BSD
# Temporary GitHub-hosted GAMP4 validation toolchain, NOT a published release image.
# Base image is resolved by a GitHub cloud step to an actual @sha256 digest.
ARG BASE_IMAGE
FROM ${BASE_IMAGE}
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
         g++ gcc make cmake binutils coreutils grep gawk findutils ca-certificates \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
