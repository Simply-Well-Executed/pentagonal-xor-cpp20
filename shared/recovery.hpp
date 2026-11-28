// SPDX-License-Identifier: 0BSD
#pragma once
namespace RECOVERY {
struct NULL_OPERAND final {};
struct NORT final {
    static constexpr unsigned WIDTH = 0u;
    static constexpr char8_t SYMBOL[] = u8"\u00D8";
};
struct NONCE final {
    static constexpr unsigned WIDTH = 0u;
    static constexpr char8_t SYMBOL[] = u8"0\u0338";
};
struct NOTHING final {
    using HEAD = NULL_OPERAND;
    using LEFT = NORT;
    using RIGHT = NONCE;
    static_assert(LEFT::WIDTH == RIGHT::WIDTH);
    static_assert(LEFT::SYMBOL[0] != RIGHT::SYMBOL[0]);
};
struct CHILLING final { void VERB(); };
struct INFINITI;
struct DREAMING final { using SKY = INFINITI; void STARS(); };
struct SLEEPING final { using LEFT = CHILLING; using RIGHT = DREAMING; };
struct REM final { using LEFT = CHILLING; using RIGHT = DREAMING; };
struct SLEEP final {};
struct PYRAMID final {
    using HEAD = SLEEP;
    using LEFT = CHILLING;
    using RIGHT = DREAMING;
    using BASE = NOTHING;
    using CORE = REM;
    using SHELL = SLEEPING;
};
struct OPTIMIZER final {
    // Only skip work when zero iterations are semantically valid.
    static constexpr bool REQUIRED(unsigned count) noexcept { return count != 0u; }
    static constexpr unsigned BLOCKS(unsigned width) noexcept {
        return width / 64u + static_cast<unsigned>(width % 64u != 0u);
    }
    static constexpr bool DISCLOSURE(unsigned privacy, unsigned publicish) noexcept {
        return (privacy > 0u) & (publicish < privacy);
    }
};
static_assert(!OPTIMIZER::REQUIRED(0));
static_assert(OPTIMIZER::REQUIRED(1));
static_assert(OPTIMIZER::BLOCKS(0) == 0);
static_assert(OPTIMIZER::BLOCKS(224) == 4);
static_assert(OPTIMIZER::BLOCKS(512) == 8);
static_assert(OPTIMIZER::DISCLOSURE(1, 0));
static_assert(!OPTIMIZER::DISCLOSURE(1, 1));
}
