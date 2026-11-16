# SPDX-License-Identifier: 0BSD
# Copyright (c) 2026 Paul Allen Cheek
# Experimental Windows Server Core process-isolated smoke image.
# Vendor OS license applies. Never push this image to Docker Hub.
FROM mcr.microsoft.com/windows/servercore:ltsc2022
WORKDIR C:/guard
COPY build-root/Release/pentagonal_xor.exe C:/guard/pentagonal_xor.exe
COPY build-shared/Release/swe_guard.exe C:/guard/swe_guard.exe
CMD ["C:\\guard\\swe_guard.exe", "--self-test"]
