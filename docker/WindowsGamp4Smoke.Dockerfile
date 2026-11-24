ARG WINDOWS_BASE_IMAGE
FROM ${WINDOWS_BASE_IMAGE}
LABEL org.opencontainers.image.title="GAMP4-tested native Windows x64 smoke image"
LABEL org.opencontainers.image.description="Built locally on a compatible Windows x64 Docker host. Never auto-published."
WORKDIR C:/release
COPY bin/pentagonal_xor_std_free.exe C:/release/pentagonal_xor_std_free.exe
COPY bin/pentagonal_gamp4_validation_test.exe C:/release/pentagonal_gamp4_validation_test.exe
USER ContainerUser
RUN ["C:\\release\\pentagonal_gamp4_validation_test.exe"]
RUN ["C:\\release\\pentagonal_xor_std_free.exe", "0.80", "0.70", "0.90", "0.60", "0.95"]
CMD ["C:\\release\\pentagonal_xor_std_free.exe", "0.80", "0.70", "0.90", "0.60", "0.95"]
