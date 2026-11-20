FROM scratch
ARG SOURCE_COMMIT
LABEL org.opencontainers.image.title="Pentagonal XOR multi-OS GAMP4-gated artifact payload"
LABEL org.opencontainers.image.source="https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20"
LABEL org.opencontainers.image.description="Non-executable archive transport. Not a Windows, macOS, or Android runtime container."
LABEL org.opencontainers.image.revision="${SOURCE_COMMIT}"
COPY . /release/
