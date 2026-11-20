// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Standalone high-throughput periodic XOR benchmark; not the parity CLI.
#include "pentagonal_xor.hpp"

#include <charconv>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <span>
#include <vector>

namespace {
constexpr std::size_t default_payload = 16u * 1024u * 1024u;
constexpr std::size_t max_payload = 256u * 1024u * 1024u;
char stdout_buffer[65536]{};
} // namespace

int main(int argc, char** argv) {
    std::size_t bytes = default_payload;
    bool strict_affinity = false;

    // Parse only CLI options; do not allocate/touch a payload before pinning.
    if (argc >= 2) {
        if (argc < 3 || argc > 4 || std::strcmp(argv[1], "--bytes") != 0) {
            std::fprintf(stderr, "Usage: %s [--bytes 0..268435456 [--strict-affinity]]\n",
                         argv[0]);
            return 2;
        }
        unsigned long long requested = 0;
        const char* const begin = argv[2];
        const char* const end = begin + std::strlen(begin);
        const auto parse = std::from_chars(begin, end, requested);
        if (parse.ec != std::errc{} || parse.ptr != end ||
            requested > max_payload ||
            (argc == 4 && std::strcmp(argv[3], "--strict-affinity") != 0)) {
            std::fputs("Invalid byte count or option\n", stderr);
            return 2;
        }
        bytes = static_cast<std::size_t>(requested);
        strict_affinity = argc == 4;
    }

    std::setvbuf(stdout, stdout_buffer, _IOFBF, sizeof(stdout_buffer));

    // Explicitly best effort; operating systems can deny CPU/priority changes.
    // These happen before first-touch of the payload's storage.
    const bool priority_raised = pxor::request_moderate_priority();
    const auto pinned_core = pxor::pin_to_active_core();
    if (strict_affinity && !pinned_core) {
        std::fputs("Affinity pin requested but OS rejected it\n", stderr);
        return 3;
    }

    constexpr pxor::ObfuscatedData label("IDKFA - PENTAGONAL XOR ENGINE", 42);
    const auto plaintext = label.decrypt(42);

    std::vector<std::byte> payload(bytes, std::byte{0x5A}); // first touch after pin

    const auto start = std::chrono::steady_clock::now();
    pxor::fast_xor_buffer(std::span<std::byte>{payload.data(), payload.size()});
    const auto finish = std::chrono::steady_clock::now();

    // Observable, deterministic witness to prevent dead-code elimination.
    std::uint64_t witness = 0;
    for (const std::byte value : payload) {
        witness = witness * 131u + std::to_integer<unsigned int>(value);
    }
    const auto micros =
        std::chrono::duration_cast<std::chrono::microseconds>(finish - start)
            .count();

    std::printf("Header : %s\n", plaintext.data());
    if (pinned_core) {
        std::printf("Core   : %u (pinned)\n", *pinned_core);
    } else {
        std::fputs("Core   : unpinned (OS denied/unsupported)\n", stdout);
    }
    std::printf("Priority : %s\n", priority_raised ? "moderately elevated" : "unchanged");
    std::printf("Mode   : periodic 256-byte LUT (not exact seeded stream)\n");
    std::printf("Bytes  : %zu\n", payload.size());
    std::printf("Elapsed: %lld us\n", static_cast<long long>(micros));
    std::printf("Witness: %llu\n", static_cast<unsigned long long>(witness));
    return 0;
}
