// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Optional periodic XOR benchmark; never replaces the parity CLI.
// Fixed public storage by default; heap fallback only for larger opt-in sizes.
#include "pentagonal_xor.hpp"

#include <charconv>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <memory>
#include <new>
#include <system_error>

namespace {
constexpr pxor::usize default_payload = 16u * 1024u * 1024u;
constexpr pxor::usize max_payload = 256u * 1024u * 1024u;
static pxor::FixedBuffer<pxor::u8, default_payload> static_payload{};
static char stdout_buffer[65536]{};
} // namespace

int main(int argc, char** argv) {
    pxor::usize bytes = default_payload;
    bool strict_affinity = false;
    if (argc >= 2) {
        if (argc < 3 || argc > 4 || std::strcmp(argv[1], "--bytes") != 0) {
            std::fprintf(stderr,
                "Usage: %s [--bytes 0..268435456 [--strict-affinity]]\n",
                argv[0]);
            return 2;
        }
        unsigned long long requested = 0;
        const char* first = argv[2];
        const char* last = first + std::strlen(first);
        const auto result = std::from_chars(first, last, requested);
        if (result.ec != std::errc{} || result.ptr != last ||
            requested > max_payload ||
            (argc == 4 && std::strcmp(argv[3], "--strict-affinity") != 0)) {
            std::fputs("Invalid byte count or option\n", stderr);
            return 2;
        }
        bytes = static_cast<pxor::usize>(requested);
        strict_affinity = argc == 4;
    }

    std::setvbuf(stdout, stdout_buffer, _IOFBF, sizeof(stdout_buffer));

    // Scheduler hints precede first explicit write to payload storage.
    // They never establish guaranteed NUMA or L1 cache locality.
    const bool priority_raised = pxor::request_moderate_priority();
    const pxor::CorePinResult core = pxor::pin_to_active_core();
    if (strict_affinity && !core.pinned) {
        std::fputs("Affinity pin requested but OS rejected it\n", stderr);
        return 3;
    }

    constexpr pxor::ObfuscatedData label("IDKFA - PENTAGONAL XOR ENGINE", 42);
    constexpr pxor::u8 tag = pxor::EngineAuditor::first_cipher_byte(label);
    label.with_decrypted([&](pxor::StringSlice header) {
        std::printf("Header : %.*s [Tag: 0x%02X]\n",
                    static_cast<int>(header.size()),
                    header.data(), static_cast<unsigned int>(tag));
    });

    // Fixed storage covers the default 16 MiB. Larger requests have a
    // bounded, explicit heap fallback instead of hidden stack overflow.
    std::unique_ptr<pxor::u8[]> heap;
    pxor::u8* storage = static_payload.data();
    if (bytes > static_payload.size()) {
        heap.reset(new (std::nothrow) pxor::u8[bytes]);
        if (!heap) {
            std::fputs("Payload allocation failed\n", stderr);
            return 4;
        }
        storage = heap.get();
    }
    for (pxor::usize i = 0; i < bytes; ++i) storage[i] = 0x5A;

    const auto t0 = std::chrono::steady_clock::now();
    pxor::fast_xor_buffer(pxor::ByteSlice{storage, bytes});
    const auto t1 = std::chrono::steady_clock::now();

    // Shared read-only borrowed view for the checksum; it owns no memory.
    const pxor::ConstByteSlice observed{storage, bytes};
    pxor::u64 witness = 0;
    for (pxor::usize i = 0; i < observed.size(); ++i) {
        witness = witness * 131u + observed[i];
    }
    const auto micros =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    if (core.pinned) {
        std::printf("Core   : %u (pinned)\n", core.core_id);
    } else {
        std::fputs("Core   : unpinned (OS denied/unsupported)\n", stdout);
    }
    std::printf("Priority : %s\n",
                priority_raised ? "moderately elevated" : "unchanged");
    std::fputs("Mode   : periodic 256-byte LUT (not exact seeded stream)\n", stdout);
    std::printf("Bytes  : %zu\n", bytes);
    std::printf("Storage: %s\n",
                heap ? "bounded heap fallback" : "fixed static buffer");
    std::printf("Elapsed: %lld us\n", static_cast<long long>(micros));
    std::printf("Witness: %llu\n", static_cast<unsigned long long>(witness));
    return 0;
}
