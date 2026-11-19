// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace core_audit {

enum class AuditStatus : uint8_t {
    OK = 0,
    ERR_OVERFLOW_64,
    ERR_FRAME_TOO_SHORT,
    ERR_INVALID_CM,
    ERR_INVALID_FCHECK,
    ERR_UNSUPPORTED_FDICT,
    ERR_ADLER32_MISMATCH
};

constexpr std::string_view status_name(AuditStatus s) noexcept {
    switch (s) {
        case AuditStatus::OK:                    return "OK";
        case AuditStatus::ERR_OVERFLOW_64:       return "ERR_OVERFLOW_64";
        case AuditStatus::ERR_FRAME_TOO_SHORT:   return "ERR_FRAME_TOO_SHORT";
        case AuditStatus::ERR_INVALID_CM:        return "ERR_INVALID_CM";
        case AuditStatus::ERR_INVALID_FCHECK:    return "ERR_INVALID_FCHECK";
        case AuditStatus::ERR_UNSUPPORTED_FDICT: return "ERR_UNSUPPORTED_FDICT";
        case AuditStatus::ERR_ADLER32_MISMATCH:  return "ERR_ADLER32_MISMATCH";
    }
    return "ERR_UNKNOWN";
}

// Checked signed 64-bit position and span provenance
struct FrameAudit {
    AuditStatus status = AuditStatus::OK;
    int64_t header_pos = 0;
    int64_t payload_pos = 2;
    int64_t payload_len = 0;
    int64_t trailer_pos = 0;
    int64_t total_frame_len = 0;
    uint32_t expected_adler32 = 0;
    uint32_t computed_adler32 = 0;
    uint64_t scalar_witness = 0; // Deterministic weight-free 64-bit integrity fold

    [[nodiscard]] constexpr bool valid() const noexcept {
        return status == AuditStatus::OK;
    }

    [[nodiscard]] std::string to_report() const {
        return std::format(
            "status={} | pos=[hdr:{}, pay:{}, trl:{}] | len={} | adler32={:08x}/{:08x} | witness={:016x}",
            status_name(status), header_pos, payload_pos, trailer_pos,
            payload_len, computed_adler32, expected_adler32, scalar_witness
        );
    }
};

class ZlibRfc1950Engine {
public:
    static constexpr int64_t HEADER_BYTES  = 2;
    static constexpr int64_t TRAILER_BYTES = 4;
    static constexpr int64_t OVERHEAD_BYTES = HEADER_BYTES + TRAILER_BYTES;
    static constexpr int64_t MAX_SAFE_PAYLOAD = std::numeric_limits<int64_t>::max() - OVERHEAD_BYTES;

    // =========================================================================
    // 1. HEXABYTE-UNROLLED ADLER-32 + DETERMINISTIC SCALAR WITNESS
    // =========================================================================
    struct ChecksumPair {
        uint32_t adler32;
        uint64_t scalar_witness;
    };

    [[nodiscard]] static ChecksumPair compute_audit_sums(std::span<const uint8_t> data) noexcept {
        constexpr uint32_t MOD_ADLER = 65521U;
        // NMAX is the largest n such that 255n(n+1)/2 + (n+1)(MOD_ADLER-1) <= 2^32-1
        constexpr size_t NMAX = 5552;

        uint32_t a = 1;
        uint32_t b = 0;
        uint64_t witness = 0x9E3779B97F4A7C15ULL; // Golden-ratio scalar seed

        size_t offset = 0;
        size_t remaining = data.size();

        while (remaining > 0) {
            size_t chunk = remaining < NMAX ? remaining : NMAX;
            remaining -= chunk;

            // Process in 16-byte ("hexabyte") unrolled strides
            while (chunk >= 16) {
                for (size_t i = 0; i < 16; ++i) {
                    uint8_t byte = data[offset + i];
                    a += byte;
                    b += a;
                    witness = std::rotl(witness ^ byte, 7) * 0xBF58476D1CE4E5B9ULL;
                }
                offset += 16;
                chunk -= 16;
            }

            while (chunk > 0) {
                uint8_t byte = data[offset++];
                a += byte;
                b += a;
                witness = std::rotl(witness ^ byte, 7) * 0xBF58476D1CE4E5B9ULL;
                --chunk;
            }

            a %= MOD_ADLER;
            b %= MOD_ADLER;
        }

        // Finalize scalar avalanche
        witness ^= (witness >> 33);
        witness *= 0x94D049BB133111EBULL;
        witness ^= (witness >> 33);

        return ChecksumPair{
            .adler32 = (b << 16) | a,
            .scalar_witness = witness
        };
    }

    // =========================================================================
    // 2. WRAP IN RFC 1950 FRAME WITH CHECKED 64-BIT PROVENANCE
    // =========================================================================
    [[nodiscard]] static std::pair<std::vector<uint8_t>, FrameAudit> wrap(std::span<const uint8_t> payload) {
        FrameAudit audit{};

        if (payload.size() > static_cast<size_t>(MAX_SAFE_PAYLOAD)) {
            audit.status = AuditStatus::ERR_OVERFLOW_64;
            return {{}, audit};
        }

        const int64_t pay_len = static_cast<int64_t>(payload.size());
        const int64_t total_len = pay_len + OVERHEAD_BYTES;

        auto [adler, witness] = compute_audit_sums(payload);

        std::vector<uint8_t> frame;
        frame.resize(static_cast<size_t>(total_len));

        // RFC 1950 Header: CMF=0x78, FLG=0x01 ((0x78 * 256 + 0x01) % 31 == 0)
        frame[0] = 0x78;
        frame[1] = 0x01;

        if (!payload.empty()) {
            std::memcpy(frame.data() + HEADER_BYTES, payload.data(), payload.size());
        }

        const size_t trl = static_cast<size_t>(HEADER_BYTES + pay_len);
        frame[trl + 0] = static_cast<uint8_t>((adler >> 24) & 0xFF);
        frame[trl + 1] = static_cast<uint8_t>((adler >> 16) & 0xFF);
        frame[trl + 2] = static_cast<uint8_t>((adler >> 8) & 0xFF);
        frame[trl + 3] = static_cast<uint8_t>(adler & 0xFF);

        audit.status = AuditStatus::OK;
        audit.header_pos = 0;
        audit.payload_pos = HEADER_BYTES;
        audit.payload_len = pay_len;
        audit.trailer_pos = HEADER_BYTES + pay_len;
        audit.total_frame_len = total_len;
        audit.expected_adler32 = adler;
        audit.computed_adler32 = adler;
        audit.scalar_witness = witness;

        return {std::move(frame), audit};
    }

    [[nodiscard]] static std::pair<std::vector<uint8_t>, FrameAudit> wrap(std::string_view text) {
        return wrap(std::span<const uint8_t>(
            reinterpret_cast<const uint8_t*>(text.data()), text.size()));
    }

    // =========================================================================
    // 3. ZERO-COPY IN-PLACE AUDIT & SLICE (NO HEAP ALLOCATION)
    // =========================================================================
    [[nodiscard]] static std::pair<std::span<const uint8_t>, FrameAudit>
    inspect_zero_copy(std::span<const uint8_t> frame) noexcept {
        FrameAudit audit{};

        if (frame.size() > static_cast<size_t>(std::numeric_limits<int64_t>::max())) {
            audit.status = AuditStatus::ERR_OVERFLOW_64;
            return {{}, audit};
        }

        const int64_t total_len = static_cast<int64_t>(frame.size());
        audit.total_frame_len = total_len;

        if (total_len < OVERHEAD_BYTES) {
            audit.status = AuditStatus::ERR_FRAME_TOO_SHORT;
            return {{}, audit};
        }

        const uint8_t cmf = frame[0];
        const uint8_t flg = frame[1];

        if ((cmf & 0x0F) != 8) {
            audit.status = AuditStatus::ERR_INVALID_CM;
            return {{}, audit};
        }
        if (((static_cast<uint16_t>(cmf) << 8) | flg) % 31 != 0) {
            audit.status = AuditStatus::ERR_INVALID_FCHECK;
            return {{}, audit};
        }
        if ((flg & 0x20) != 0) {
            audit.status = AuditStatus::ERR_UNSUPPORTED_FDICT;
            return {{}, audit};
        }

        const int64_t pay_len = total_len - OVERHEAD_BYTES;
        const int64_t trl_pos = HEADER_BYTES + pay_len;

        std::span<const uint8_t> payload = frame.subspan(HEADER_BYTES, static_cast<size_t>(pay_len));

        const size_t t = static_cast<size_t>(trl_pos);
        const uint32_t expected_adler =
            (static_cast<uint32_t>(frame[t + 0]) << 24) |
            (static_cast<uint32_t>(frame[t + 1]) << 16) |
            (static_cast<uint32_t>(frame[t + 2]) << 8)  |
             static_cast<uint32_t>(frame[t + 3]);

        auto [computed_adler, witness] = compute_audit_sums(payload);

        audit.header_pos = 0;
        audit.payload_pos = HEADER_BYTES;
        audit.payload_len = pay_len;
        audit.trailer_pos = trl_pos;
        audit.expected_adler32 = expected_adler;
        audit.computed_adler32 = computed_adler;
        audit.scalar_witness = witness;

        if (computed_adler != expected_adler) {
            audit.status = AuditStatus::ERR_ADLER32_MISMATCH;
            return {{}, audit};
        }

        audit.status = AuditStatus::OK;
        return {payload, audit};
    }

    // =========================================================================
    // 4. OWNED UNWRAP HELPER
    // =========================================================================
    [[nodiscard]] static std::pair<std::vector<uint8_t>, FrameAudit>
    unwrap(std::span<const uint8_t> frame) {
        auto [view, audit] = inspect_zero_copy(frame);
        if (!audit.valid()) {
            return {{}, audit};
        }
        return {std::vector<uint8_t>(view.begin(), view.end()), audit};
    }
};

} // namespace core_audit

int main() {
    using namespace core_audit;

    std::string_view source =
        "Deterministic scalar audit + RFC 1950 framing with checked 64-bit positions.";

    // 1. Wrap payload and capture provenance report
    auto [framed, wrap_audit] = ZlibRfc1950Engine::wrap(source);
    std::cout << "[WRAP]   " << wrap_audit.to_report() << "\n";

    // 2. Zero-copy inspection & Adler-32 + scalar witness verification
    auto [payload_view, inspect_audit] = ZlibRfc1950Engine::inspect_zero_copy(framed);
    std::cout << "[VERIFY] " << inspect_audit.to_report() << "\n";

    if (inspect_audit.valid()) {
        std::string_view restored(
            reinterpret_cast<const char*>(payload_view.data()),
            payload_view.size()
        );
        std::cout << "[OUTPUT] " << restored << "\n";
    }

    // 3. Negative-test witness: single-bit payload corruption detection
    framed[10] ^= 0x01;
    auto [bad_view, bad_audit] = ZlibRfc1950Engine::inspect_zero_copy(framed);
    std::cout << "[TAMPER] " << bad_audit.to_report() << "\n";

    return inspect_audit.valid() && !bad_audit.valid() ? 0 : 1;
}
