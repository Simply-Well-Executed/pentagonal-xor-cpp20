// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Standalone native bytewise XOR throughput CLI; NOT the parity CLI.
// Zero C/C++ standard-library headers or calls. System APIs are explicit.
#include "pentagonal_xor.hpp"

namespace {
constexpr pxor::usize default_payload = 16u * 1024u * 1024u;
constexpr pxor::usize max_payload = 256u * 1024u * 1024u;
static pxor::FixedBuffer<pxor::u8, default_payload> fixed_payload{};
}

int main(int argc, char** argv) {
    pxor::usize bytes = default_payload;
    bool strict_affinity = false;
    if (argc >= 2) {
        if (argc < 3 || argc > 4 ||
            !pxor::equals_cstr(argv[1], "--bytes")) {
            pxor::write_literal(
                "Usage: pentagonal_xor_cli [--bytes 0..268435456 [--strict-affinity]]\n",
                true);
            return 2;
        }
        if (!pxor::parse_decimal_limit(argv[2], max_payload, bytes) ||
            (argc == 4 &&
             !pxor::equals_cstr(argv[3], "--strict-affinity"))) {
            pxor::write_literal("Invalid byte count or option\n", true);
            return 2;
        }
        strict_affinity = argc == 4;
    }

    // Request before first explicit payload write. Neither is a guarantee.
    const bool priority_raised = pxor::request_moderate_priority();
    const pxor::CorePinResult core = pxor::pin_to_active_core();
    if (strict_affinity && !core.pinned) {
        pxor::write_literal("Affinity pin rejected or unsupported\n", true);
        return 3;
    }

    constexpr pxor::ObfuscatedData label("IDKFA - PENTAGONAL XOR ENGINE", 42);
    constexpr pxor::u8 tag = pxor::EngineAuditor::first_cipher_byte(label);
    label.with_decrypted([&](pxor::StringSlice text) {
        pxor::write_literal("Header : ");
        pxor::write_raw(text.data(), text.size());
        pxor::write_literal(" [Tag: 0x");
        pxor::write_hex_byte(tag);
        pxor::write_literal("]\n");
    });

    // Fixed static storage at 16 MiB or less. Larger requests use native
    // OS pages rather than C++ new, malloc or the standard-library allocator.
    pxor::u8* mapped = nullptr;
    pxor::u8* storage = fixed_payload.data();
    if (bytes > fixed_payload.size()) {
        mapped = pxor::allocate_pages(bytes);
        if (!mapped) {
            pxor::write_literal("Native page allocation failed\n", true);
            return 4;
        }
        storage = mapped;
    }
    for (pxor::usize i = 0; i < bytes; ++i) storage[i] = 0x5A;

    const pxor::u64 first = pxor::now_us();
    pxor::fast_xor_buffer(pxor::ByteSlice{storage, bytes});
    const pxor::u64 last = pxor::now_us();

    // Read-only borrowed view, not OS-shared memory or thread synchronization.
    const pxor::ConstByteSlice observed{storage, bytes};
    pxor::u64 witness = 0;
    for (pxor::usize i = 0; i < observed.size(); ++i) {
        witness = witness * 131u + observed[i];
    }

    pxor::write_literal("Core   : ");
    if (core.pinned) pxor::write_u64(core.core_id);
    else pxor::write_literal("unpinned");
    pxor::write_literal("\nPriority : ");
    pxor::write_literal(priority_raised ? "moderately elevated" : "unchanged");
    pxor::write_literal("\nMode   : periodic 256-byte LUT (not exact seeded stream)\n");
    pxor::write_literal("Bytes  : ");
    pxor::write_u64(static_cast<pxor::u64>(bytes));
    pxor::write_literal("\nStorage: ");
    if (mapped) pxor::write_literal("native OS pages");
    else pxor::write_literal("fixed static buffer");
    pxor::write_literal("\nElapsed: ");
    if (first != 0 && last >= first) pxor::write_u64(last - first);
    else pxor::write_literal("unavailable");
    pxor::write_literal(" us\nWitness: ");
    pxor::write_u64(witness);
    pxor::write_literal("\n");

    pxor::release_pages(mapped, bytes);
    return 0;
}
