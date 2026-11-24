#include "pentagonal_suite.hpp"

namespace {
constexpr pxor::usize payload_size = 16u * 1024u * 1024u;
static pxor::FixedBuffer<pxor::u8, payload_size> payload{};
}

int main() {
    const bool elevated = pxor::request_moderate_priority();
    const pxor::CorePinResult core = pxor::pin_to_active_core();

    constexpr pxor::ObfuscatedData label("IDKFA - PENTAGONAL SUITE ENGINE", 42);
    constexpr pxor::u8 tag = pxor::EngineAuditor::first_cipher_byte(label);
    label.with_decrypted([&](pxor::StringSlice text) {
        pxor::write_literal("Header : ");
        pxor::write_raw(text.data(), text.size());
        pxor::write_literal(" [Tag: 0x");
        pxor::write_hex_byte(tag);
        pxor::write_literal("]\n");
    });

    constexpr pxor::PentagonalVista vista{1, 4};
    pxor::write_literal("Vista  : ");
    for (pxor::usize i = 0; i < vista.size(); ++i) {
        pxor::write_u64(vista[i]);
        pxor::write_literal(" ");
    }
    pxor::write_literal("\n");

    constexpr pxor::u64 nan_box = pxor::pentagonal_nan_bits(7);
    pxor::write_literal("NaN Tag: ");
    pxor::write_u64(pxor::extract_nan_payload(nan_box));
    pxor::write_literal("\n");

    payload.fill(0x5A);
    const pxor::ByteSlice bytes{payload};

    const pxor::u64 started = pxor::now_us();
    pxor::fast_nop_buffer(bytes);
    pxor::fast_xor_buffer(bytes);
    pxor::fast_or_buffer(bytes);
    pxor::fast_and_buffer(bytes);
    pxor::fast_nand_buffer(bytes);
    const pxor::u64 finished = pxor::now_us();

    const pxor::ConstByteSlice observed{payload};
    pxor::u64 witness = 0;
    for (pxor::usize i = 0; i < observed.size(); ++i) {
        witness = witness * 131u + observed[i];
    }

    pxor::apply_pentagonal_nan({payload.data(), 16}, 1);
    const bool boxed =
        pxor::pack_u64_le(payload.data()) == pxor::pentagonal_nan_bits(1) &&
        pxor::pack_u64_le(payload.data() + 8) == pxor::pentagonal_nan_bits(2);

    pxor::fast_nil_buffer(bytes);
    if (!boxed || payload[0] != 0 || payload[payload_size - 1] != 0) return 2;

    pxor::write_literal("Core   : ");
    if (core.pinned) pxor::write_u64(core.core_id);
    else pxor::write_literal("unpinned");

    pxor::write_literal("\nPriority : ");
    if (elevated) pxor::write_literal("moderately elevated");
    else pxor::write_literal("unchanged");

    pxor::write_literal("\n16MB Suite pipeline processed in: ");
    if (started != 0 && finished >= started) {
        pxor::write_u64(finished - started);
    } else {
        pxor::write_literal("unavailable");
    }
    pxor::write_literal(" us\nWitness: ");
    pxor::write_u64(witness);
    pxor::write_literal("\n");
    return 0;
}
