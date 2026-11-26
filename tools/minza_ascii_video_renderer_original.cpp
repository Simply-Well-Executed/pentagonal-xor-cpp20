// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Original author-supplied C++20 renderer; logic preserved as submitted.
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <array>
#include <format>
#include <span>
#include <numbers>
#include <concepts>
#include <ranges>
#include <memory>
#include <chrono>
#include <thread>
#include <bit>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

// ============================================================================
// 1. C++20 Concepts & Core Data Types
// ============================================================================

struct RGB {
    uint8_t r, g, b;
    constexpr bool operator==(const RGB&) const noexcept = default;
};

struct YCbCr {
    uint8_t y;   // Luma (brightness)
    uint8_t cb;  // Blue-difference chroma
    uint8_t cr;  // Red-difference chroma
    constexpr bool operator==(const YCbCr&) const noexcept = default;
};

struct ChromaLumaMetrics {
    uint8_t luma;
    int16_t cb_offset;
    int16_t cr_offset;
    float chroma_magnitude;
    float hue_angle_deg;
};

template <typename T>
concept PixelType = std::same_as<T, RGB> || std::same_as<T, YCbCr>;

enum class ColorMode { Mono, Gray, C16, Full16, FullGray };
enum class DitherMode { None, Ordered2, Ordered4, Ordered8, Random, FStein, PentXor };

struct Config {
    std::string input_file;
    int width = 160;
    int height = 45;
    ColorMode color = ColorMode::Full16;
    DitherMode dither = DitherMode::PentXor;

    // Boolean Feature Flags
    bool use_native_yuv = false;     // -native_yuv
    bool use_chroma_metrics = false; // -chroma_metrics
    bool use_short_ramp = false;     // -short_ramp
    bool use_pent_xor_fx = false;    // -pent_xor_fx (XOR chroma/luma interference)
};

// ============================================================================
// 2. Pentagonal Number Sequence & XOR Engine (Inspired by pentagonal-xor-cpp20)
// ============================================================================

struct PentagonalXorEngine {
    uint64_t state = 0x9E3779B97F4A7C15ULL;
    uint64_t weyl  = 0;

    // Generalized pentagonal number: P(k) = k * (3k - 1) / 2 for k = 1, -1, 2, -2, ...
    [[nodiscard]] static consteval int64_t generalized_pentagonal(int64_t n) noexcept {
        if (n == 0) return 0;
        const int64_t k = (n % 2 != 0) ? ((n + 1) / 2) : -(n / 2);
        return (k * (3 * k - 1)) / 2;
    }

    // Compile-time table of the first 64 generalized pentagonal numbers
    static constexpr auto PENT_TABLE = []() consteval {
        std::array<uint64_t, 64> table{};
        for (size_t i = 0; i < table.size(); ++i) {
            table[i] = static_cast<uint64_t>(generalized_pentagonal(static_cast<int64_t>(i + 1)));
        }
        return table;
    }();

    // Spatial-temporal Pentagonal-XOR hash for deterministic low-discrepancy dithering
    [[nodiscard]] constexpr uint64_t next_spatial(uint32_t x, uint32_t y, uint32_t frame) noexcept {
        const uint64_t px = PENT_TABLE[(x + frame) & 63u];
        const uint64_t py = PENT_TABLE[(y ^ (frame >> 1)) & 63u];
        uint64_t z = state ^ (px * 0xBF58476D1CE4E5B9ULL) ^ std::rotl(py, 27);
        z ^= (z >> 30);
        z *= 0x94D049BB133111EBULL;
        z ^= (z >> 31);
        return z;
    }

    // Returns a dither offset in [-24.0f, +24.0f]
    [[nodiscard]] constexpr float dither_offset(uint32_t x, uint32_t y, uint32_t frame) noexcept {
        const uint64_t h = next_spatial(x, y, frame);
        const float norm = static_cast<float>(h & 0xFFFFu) / 65535.0f;
        return (norm - 0.5f) * 48.0f;
    }

    // Pentagonal XOR chroma/luma bit-folding effect
    [[nodiscard]] constexpr uint8_t fold_byte(uint8_t val, uint32_t x, uint32_t y, uint32_t frame) const noexcept {
        const uint64_t p = PENT_TABLE[(x * 3 + y * 5 + frame) & 63u];
        const uint8_t mask = static_cast<uint8_t>((p ^ (p >> 8)) & 0x1Fu);
        return val ^ mask;
    }
};

// ============================================================================
// 3. Compile-Time (consteval) Palettes & Lookup Tables
// ============================================================================

constexpr std::array<RGB, 16> ANSI_16_PALETTE = {{
    {0, 0, 0},       {128, 0, 0},     {0, 128, 0},     {128, 128, 0},
    {0, 0, 128},     {128, 0, 128},   {0, 128, 128},   {192, 192, 192},
    {128, 128, 128}, {255, 0, 0},     {0, 255, 0},     {255, 255, 0},
    {0, 0, 255},     {255, 0, 255},   {0, 255, 255},   {255, 255, 255}
}};

// 32x32x32 (15-bit RGB) compile-time nearest ANSI-16 color lookup table
constexpr auto ANSI16_LUT = []() consteval {
    std::array<uint8_t, 32 * 32 * 32> lut{};
    for (int r = 0; r < 32; ++r) {
        for (int g = 0; g < 32; ++g) {
            for (int b = 0; b < 32; ++b) {
                const int r8 = (r << 3) | (r >> 2);
                const int g8 = (g << 3) | (g >> 2);
                const int b8 = (b << 3) | (b >> 2);
                int best_idx = 0;
                int min_dist = 1'000'000'000;
                for (int i = 0; i < 16; ++i) {
                    const int dr = r8 - ANSI_16_PALETTE[i].r;
                    const int dg = g8 - ANSI_16_PALETTE[i].g;
                    const int db = b8 - ANSI_16_PALETTE[i].b;
                    const int dist = dr * dr + dg * dg + db * db;
                    if (dist < min_dist) {
                        min_dist = dist;
                        best_idx = i;
                    }
                }
                const uint8_t code = static_cast<uint8_t>((best_idx < 8) ? (30 + best_idx) : (90 + (best_idx - 8)));
                lut[(r << 10) | (g << 5) | b] = code;
            }
        }
    }
    return lut;
}();

constexpr int BAYER_2[2][2] = {{0, 2}, {3, 1}};
constexpr int BAYER_4[4][4] = {
    { 0,  8,  2, 10}, {12,  4, 14,  6},
    { 3, 11,  1,  9}, {15,  7, 13,  5}
};
constexpr int BAYER_8[8][8] = {
    { 0, 32,  8, 40,  2, 34, 10, 42}, {48, 16, 56, 24, 50, 18, 58, 26},
    {12, 44,  4, 36, 14, 46,  6, 38}, {60, 28, 52, 20, 62, 30, 54, 22},
    { 3, 35, 11, 43,  1, 33,  9, 41}, {51, 19, 59, 27, 49, 17, 57, 25},
    {15, 47,  7, 39, 13, 45,  5, 37}, {63, 31, 55, 23, 61, 29, 53, 21}
};

constexpr std::string_view LONG_RAMP  = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
constexpr std::string_view SHORT_RAMP = "@%#*+=-:. ";

// ============================================================================
// 4. User-Requested Core Functions (Constexpr / Noexcept Enhanced)
// ============================================================================

[[nodiscard]] inline ChromaLumaMetrics analyze_chroma_luma(YCbCr px) noexcept {
    const int16_t cb_signed = static_cast<int16_t>(px.cb) - 128;
    const int16_t cr_signed = static_cast<int16_t>(px.cr) - 128;

    const float cb_f = static_cast<float>(cb_signed);
    const float cr_f = static_cast<float>(cr_signed);

    const float magnitude = std::hypot(cb_f, cr_f);
    const float angle_rad = std::atan2(cr_f, cb_f);
    const float angle_deg = angle_rad * (180.0f / std::numbers::pi_v<float>);

    return ChromaLumaMetrics{
        .luma = px.y,
        .cb_offset = cb_signed,
        .cr_offset = cr_signed,
        .chroma_magnitude = magnitude,
        .hue_angle_deg = angle_deg
    };
}

[[nodiscard]] inline YCbCr extract_ycbcr_pixel(const AVFrame* yuv_frame, int x, int y) noexcept {
    std::span<const uint8_t> y_row(
        yuv_frame->data[0] + (y * yuv_frame->linesize[0]),
        static_cast<size_t>(yuv_frame->width)
    );
    const uint8_t luma = y_row[static_cast<size_t>(x)];

    const int chroma_x = x >> 1;
    const int chroma_y = y >> 1;
    const size_t chroma_width = static_cast<size_t>(yuv_frame->width >> 1);

    std::span<const uint8_t> cb_row(
        yuv_frame->data[1] + (chroma_y * yuv_frame->linesize[1]),
        chroma_width
    );
    std::span<const uint8_t> cr_row(
        yuv_frame->data[2] + (chroma_y * yuv_frame->linesize[2]),
        chroma_width
    );

    return YCbCr{
        .y  = luma,
        .cb = cb_row[static_cast<size_t>(chroma_x)],
        .cr = cr_row[static_cast<size_t>(chroma_x)]
    };
}

[[nodiscard]] constexpr char pixel_to_ascii_art(RGB pixel) noexcept {
    constexpr std::string_view ramp = SHORT_RAMP;
    const uint32_t brightness = (299u * pixel.r + 587u * pixel.g + 114u * pixel.b) / 1000u;
    const size_t index = (brightness * (ramp.size() - 1)) / 255u;
    return ramp[std::min(index, ramp.size() - 1)];
}

// ============================================================================
// 5. Color Conversions & ANSI State-Tracked Encoder
// ============================================================================

[[nodiscard]] constexpr YCbCr rgb_to_ycbcr(RGB c) noexcept {
    const int y  = static_cast<int>( 0.299f * c.r + 0.587f * c.g + 0.114f * c.b);
    const int cb = static_cast<int>(-0.168736f * c.r - 0.331264f * c.g + 0.5f * c.b + 128.0f);
    const int cr = static_cast<int>( 0.5f * c.r - 0.418688f * c.g - 0.081312f * c.b + 128.0f);
    return YCbCr{
        static_cast<uint8_t>(std::clamp(y, 0, 255)),
        static_cast<uint8_t>(std::clamp(cb, 0, 255)),
        static_cast<uint8_t>(std::clamp(cr, 0, 255))
    };
}

[[nodiscard]] constexpr RGB ycbcr_to_rgb(YCbCr yuv) noexcept {
    const float y  = static_cast<float>(yuv.y);
    const float cb = static_cast<float>(yuv.cb) - 128.0f;
    const float cr = static_cast<float>(yuv.cr) - 128.0f;
    const int r = static_cast<int>(y + 1.402f * cr);
    const int g = static_cast<int>(y - 0.344136f * cb - 0.714136f * cr);
    const int b = static_cast<int>(y + 1.772f * cb);
    return RGB{
        static_cast<uint8_t>(std::clamp(r, 0, 255)),
        static_cast<uint8_t>(std::clamp(g, 0, 255)),
        static_cast<uint8_t>(std::clamp(b, 0, 255))
    };
}

struct AnsiStyle {
    int fg_code = -1;
    bool is_256 = false;
    bool bold = false;
    constexpr bool operator==(const AnsiStyle&) const noexcept = default;
};

[[nodiscard]] constexpr AnsiStyle compute_rgb_style(RGB c, uint8_t luma, ColorMode mode) noexcept {
    const bool bold = (mode == ColorMode::Full16 && luma > 170);
    switch (mode) {
        case ColorMode::Mono:
            return {37, false, false};
        case ColorMode::Gray:
            return {(luma < 85) ? 90 : (luma < 170 ? 37 : 97), false, false};
        case ColorMode::FullGray:
            return {232 + static_cast<int>((luma / 255.0f) * 23.0f), true, false};
        case ColorMode::C16:
        case ColorMode::Full16: {
            const size_t lut_idx = ((c.r >> 3) << 10) | ((c.g >> 3) << 5) | (c.b >> 3);
            return {ANSI16_LUT[lut_idx], false, bold};
        }
    }
    return {37, false, false};
}

[[nodiscard]] constexpr AnsiStyle compute_chroma_style(const ChromaLumaMetrics& m, ColorMode mode) noexcept {
    if (mode == ColorMode::Mono) return {37, false, false};
    if (mode == ColorMode::Gray || mode == ColorMode::FullGray || m.chroma_magnitude < 14.0f) {
        if (mode == ColorMode::FullGray) {
            return {232 + static_cast<int>((m.luma / 255.0f) * 23.0f), true, false};
        }
        return {(m.luma < 90) ? 90 : (m.luma < 180 ? 37 : 97), false, false};
    }

    int ansi_base = 37;
    const float deg = m.hue_angle_deg;
    if (deg >= 75.0f && deg < 140.0f)         ansi_base = 31; // Red
    else if (deg >= 140.0f || deg < -160.0f)  ansi_base = 33; // Yellow
    else if (deg >= -160.0f && deg < -100.0f) ansi_base = 32; // Green
    else if (deg >= -100.0f && deg < -40.0f)  ansi_base = 36; // Cyan
    else if (deg >= -40.0f && deg < 20.0f)    ansi_base = 34; // Blue
    else                                      ansi_base = 35; // Magenta

    const bool boost = (mode == ColorMode::Full16 && (m.chroma_magnitude > 45.0f || m.luma > 170));
    if (boost) ansi_base += 60;
    return {ansi_base, false, (mode == ColorMode::Full16 && m.chroma_magnitude > 50.0f)};
}

// ============================================================================
// 6. RAII FFmpeg Resource Management
// ============================================================================

struct AVFormatDeleter { void operator()(AVFormatContext* p) const noexcept { avformat_close_input(&p); } };
struct AVCodecDeleter  { void operator()(AVCodecContext* p)  const noexcept { avcodec_free_context(&p); } };
struct SwsDeleter      { void operator()(SwsContext* p)      const noexcept { sws_freeContext(p); } };
struct AVFrameDeleter  { void operator()(AVFrame* p)         const noexcept { av_frame_free(&p); } };
struct AVPacketDeleter { void operator()(AVPacket* p)        const noexcept { av_packet_free(&p); } };

using FormatPtr = std::unique_ptr<AVFormatContext, AVFormatDeleter>;
using CodecPtr  = std::unique_ptr<AVCodecContext, AVCodecDeleter>;
using SwsPtr    = std::unique_ptr<SwsContext, SwsDeleter>;
using FramePtr  = std::unique_ptr<AVFrame, AVFrameDeleter>;
using PacketPtr = std::unique_ptr<AVPacket, AVPacketDeleter>;

// ============================================================================
// 7. CLI Parsing & Main Render Loop
// ============================================================================

Config parse_args(std::span<char*> args) {
    Config cfg;
    for (size_t i = 1; i < args.size(); ++i) {
        std::string_view arg = args[i];
        if (arg == "-i" && i + 1 < args.size()) {
            cfg.input_file = args[++i];
        } else if (arg == "-window_size" && i + 1 < args.size()) {
            std::string_view val = args[++i];
            if (auto x_pos = val.find('x'); x_pos != std::string_view::npos) {
                cfg.width  = std::stoi(std::string(val.substr(0, x_pos)));
                cfg.height = std::stoi(std::string(val.substr(x_pos + 1)));
            }
        } else if (arg == "-color" && i + 1 < args.size()) {
            std::string_view val = args[++i];
            if (val == "mono")          cfg.color = ColorMode::Mono;
            else if (val == "gray")     cfg.color = ColorMode::Gray;
            else if (val == "16")       cfg.color = ColorMode::C16;
            else if (val == "full16")   cfg.color = ColorMode::Full16;
            else if (val == "fullgray") cfg.color = ColorMode::FullGray;
        } else if (arg == "-dither" && i + 1 < args.size()) {
            std::string_view val = args[++i];
            if (val == "none")          cfg.dither = DitherMode::None;
            else if (val == "ordered2") cfg.dither = DitherMode::Ordered2;
            else if (val == "ordered4") cfg.dither = DitherMode::Ordered4;
            else if (val == "ordered8") cfg.dither = DitherMode::Ordered8;
            else if (val == "random")   cfg.dither = DitherMode::Random;
            else if (val == "fstein")   cfg.dither = DitherMode::FStein;
            else if (val == "pentxor")  cfg.dither = DitherMode::PentXor;
        } else if (arg == "-native_yuv") {
            cfg.use_native_yuv = true;
        } else if (arg == "-chroma_metrics") {
            cfg.use_chroma_metrics = true;
        } else if (arg == "-short_ramp") {
            cfg.use_short_ramp = true;
        } else if (arg == "-pent_xor_fx") {
            cfg.use_pent_xor_fx = true;
        }
    }
    return cfg;
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Config cfg = parse_args(std::span<char*>(argv, static_cast<size_t>(argc)));
    if (cfg.input_file.empty()) {
        std::cerr << "Usage: " << argv[0]
                  << " -i <video> [-window_size XxY] [-color mono|gray|16|full16|fullgray]"
                  << " [-dither none|ordered2|ordered4|ordered8|random|fstein|pentxor]"
                  << " [-native_yuv] [-chroma_metrics] [-short_ramp] [-pent_xor_fx]\n";
        return 1;
    }

    if (cfg.use_native_yuv) {
        cfg.width  &= ~1;
        cfg.height &= ~1;
    }

    AVFormatContext* raw_fmt = nullptr;
    if (avformat_open_input(&raw_fmt, cfg.input_file.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Error: Could not open input file.\n";
        return 1;
    }
    FormatPtr fmt_ctx(raw_fmt);
    avformat_find_stream_info(fmt_ctx.get(), nullptr);

    const int stream_idx = av_find_best_stream(fmt_ctx.get(), AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (stream_idx < 0) {
        std::cerr << "Error: No video stream found.\n";
        return 1;
    }

    AVStream* video_stream = fmt_ctx->streams[stream_idx];
    const AVCodec* decoder = avcodec_find_decoder(video_stream->codecpar->codec_id);
    CodecPtr dec_ctx(avcodec_alloc_context3(decoder));
    avcodec_parameters_to_context(dec_ctx.get(), video_stream->codecpar);
    avcodec_open2(dec_ctx.get(), decoder, nullptr);

    // Frame pacing calculation
    double fps = av_q2d(video_stream->avg_frame_rate);
    if (fps <= 0.0 || fps > 240.0) fps = 30.0;
    const auto frame_duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<double>(1.0 / fps)
    );

    const AVPixelFormat target_fmt = cfg.use_native_yuv ? AV_PIX_FMT_YUV420P : AV_PIX_FMT_RGB24;
    SwsPtr sws_ctx(sws_getContext(
        dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
        cfg.width, cfg.height, target_fmt,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    ));

    FramePtr raw_frame(av_frame_alloc());
    FramePtr scaled_frame(av_frame_alloc());
    scaled_frame->format = target_fmt;
    scaled_frame->width  = cfg.width;
    scaled_frame->height = cfg.height;
    av_frame_get_buffer(scaled_frame.get(), 32);

    PacketPtr pkt(av_packet_alloc());

    const size_t cell_count = static_cast<size_t>(cfg.width * cfg.height);
    std::vector<float> luma_grid(cell_count);
    std::vector<RGB>   rgb_grid(cell_count);
    std::vector<YCbCr> yuv_grid(cell_count);

    PentagonalXorEngine pent_engine;
    uint32_t frame_num = 0;

    std::string frame_out;
    frame_out.reserve(cell_count * 10);

    std::cout << "\033[?25l\033[2J";

    while (av_read_frame(fmt_ctx.get(), pkt.get()) >= 0) {
        if (pkt->stream_index == stream_idx) {
            avcodec_send_packet(dec_ctx.get(), pkt.get());
            while (avcodec_receive_frame(dec_ctx.get(), raw_frame.get()) == 0) {
                const auto frame_start = std::chrono::steady_clock::now();

                sws_scale(sws_ctx.get(), raw_frame->data, raw_frame->linesize, 0,
                          dec_ctx->height, scaled_frame->data, scaled_frame->linesize);

                // 1. Sample Pixels (Native YUV420P vs RGB24)
                for (int y : std::views::iota(0, cfg.height)) {
                    for (int x : std::views::iota(0, cfg.width)) {
                        const size_t idx = static_cast<size_t>(y * cfg.width + x);

                        if (cfg.use_native_yuv) {
                            YCbCr yuv = extract_ycbcr_pixel(scaled_frame.get(), x, y);
                            if (cfg.use_pent_xor_fx) {
                                yuv.cb = pent_engine.fold_byte(yuv.cb, x, y, frame_num);
                                yuv.cr = pent_engine.fold_byte(yuv.cr, y, x, frame_num);
                            }
                            yuv_grid[idx]  = yuv;
                            luma_grid[idx] = static_cast<float>(yuv.y);
                            if (!cfg.use_chroma_metrics || cfg.use_short_ramp) {
                                rgb_grid[idx] = ycbcr_to_rgb(yuv);
                            }
                        } else {
                            const uint8_t* row = scaled_frame->data[0] + y * scaled_frame->linesize[0];
                            RGB px{row[x * 3], row[x * 3 + 1], row[x * 3 + 2]};
                            if (cfg.use_pent_xor_fx) {
                                px.r = pent_engine.fold_byte(px.r, x, y, frame_num);
                                px.b = pent_engine.fold_byte(px.b, y, x, frame_num);
                            }
                            rgb_grid[idx] = px;
                            if (cfg.use_chroma_metrics) {
                                yuv_grid[idx]  = rgb_to_ycbcr(px);
                                luma_grid[idx] = static_cast<float>(yuv_grid[idx].y);
                            } else {
                                luma_grid[idx] = (299u * px.r + 587u * px.g + 114u * px.b) / 1000.0f;
                            }
                        }
                    }
                }

                // 2. Apply Spatial/Error-Diffusion Dithering
                const size_t ramp_len = cfg.use_short_ramp ? SHORT_RAMP.size() : LONG_RAMP.size();
                if (cfg.dither == DitherMode::FStein) {
                    const float step = 255.0f / (ramp_len - 1);
                    for (int y : std::views::iota(0, cfg.height)) {
                        for (int x : std::views::iota(0, cfg.width)) {
                            const int idx = y * cfg.width + x;
                            const float old_val = std::clamp(luma_grid[idx], 0.0f, 255.0f);
                            const float new_val = std::round(old_val / step) * step;
                            luma_grid[idx] = new_val;
                            const float err = old_val - new_val;

                            if (x + 1 < cfg.width) luma_grid[idx + 1] += err * (7.0f / 16.0f);
                            if (y + 1 < cfg.height) {
                                if (x > 0) luma_grid[idx + cfg.width - 1] += err * (3.0f / 16.0f);
                                luma_grid[idx + cfg.width] += err * (5.0f / 16.0f);
                                if (x + 1 < cfg.width) luma_grid[idx + cfg.width + 1] += err * (1.0f / 16.0f);
                            }
                        }
                    }
                } else if (cfg.dither != DitherMode::None) {
                    for (int y : std::views::iota(0, cfg.height)) {
                        for (int x : std::views::iota(0, cfg.width)) {
                            const int idx = y * cfg.width + x;
                            float offset = 0.0f;
                            switch (cfg.dither) {
                                case DitherMode::Ordered2:
                                    offset = (BAYER_2[y & 1][x & 1] / 4.0f - 0.5f) * 48.0f;
                                    break;
                                case DitherMode::Ordered4:
                                    offset = (BAYER_4[y & 3][x & 3] / 16.0f - 0.5f) * 48.0f;
                                    break;
                                case DitherMode::Ordered8:
                                    offset = (BAYER_8[y & 7][x & 7] / 64.0f - 0.5f) * 48.0f;
                                    break;
                                case DitherMode::Random:
                                    offset = pent_engine.dither_offset(x * 13u, y * 17u, frame_num);
                                    break;
                                case DitherMode::PentXor:
                                    offset = pent_engine.dither_offset(x, y, frame_num);
                                    break;
                                default: break;
                            }
                            luma_grid[idx] = std::clamp(luma_grid[idx] + offset, 0.0f, 255.0f);
                        }
                    }
                }

                // 3. Delta-Compressed ANSI Frame Assembly
                frame_out.assign("\033[H");
                AnsiStyle current_style{};

                for (int y : std::views::iota(0, cfg.height)) {
                    for (int x : std::views::iota(0, cfg.width)) {
                        const size_t idx = static_cast<size_t>(y * cfg.width + x);
                        const uint8_t dithered_luma = static_cast<uint8_t>(std::clamp(luma_grid[idx], 0.0f, 255.0f));

                        // Character Selection
                        char ascii_char = ' ';
                        if (cfg.use_short_ramp) {
                            RGB adj = rgb_grid[idx];
                            if (cfg.dither != DitherMode::None) {
                                const uint32_t orig_b = (299u * adj.r + 587u * adj.g + 114u * adj.b) / 1000u;
                                const float scale = (orig_b > 0) ? (float(dithered_luma) / float(orig_b)) : 0.0f;
                                adj.r = static_cast<uint8_t>(std::clamp(adj.r * scale, 0.0f, 255.0f));
                                adj.g = static_cast<uint8_t>(std::clamp(adj.g * scale, 0.0f, 255.0f));
                                adj.b = static_cast<uint8_t>(std::clamp(adj.b * scale, 0.0f, 255.0f));
                            }
                            ascii_char = pixel_to_ascii_art(adj);
                        } else {
                            const size_t c_idx = (static_cast<size_t>(dithered_luma) * (LONG_RAMP.size() - 1)) / 255u;
                            ascii_char = LONG_RAMP[std::min(c_idx, LONG_RAMP.size() - 1)];
                        }

                        // Color & Style Calculation
                        AnsiStyle next_style{};
                        if (cfg.use_chroma_metrics) {
                            YCbCr yuv = yuv_grid[idx];
                            yuv.y = dithered_luma;
                            next_style = compute_chroma_style(analyze_chroma_luma(yuv), cfg.color);
                        } else {
                            next_style = compute_rgb_style(rgb_grid[idx], dithered_luma, cfg.color);
                        }

                        // Emit escape codes ONLY when style transitions
                        if (next_style != current_style) {
                            if (next_style.bold != current_style.bold) {
                                frame_out += next_style.bold ? "\033[1m" : "\033[22m";
                            }
                            if (next_style.fg_code != current_style.fg_code || next_style.is_256 != current_style.is_256) {
                                if (next_style.is_256) {
                                    std::format_to(std::back_inserter(frame_out), "\033[38;5;{}m", next_style.fg_code);
                                } else {
                                    std::format_to(std::back_inserter(frame_out), "\033[{}m", next_style.fg_code);
                                }
                            }
                            current_style = next_style;
                        }

                        frame_out.push_back(ascii_char);
                    }
                    frame_out += "\033[0m\n";
                    current_style = AnsiStyle{};
                }

                std::cout << frame_out << std::flush;
                ++frame_num;

                // Maintain real-time video cadence
                const auto elapsed = std::chrono::steady_clock::now() - frame_start;
                if (elapsed < frame_duration) {
                    std::this_thread::sleep_for(frame_duration - elapsed);
                }
            }
        }
        av_packet_unref(pkt.get());
    }

    std::cout << "\033[?25h\033[0m";
    return 0;
}