// SPDX-License-Identifier: 0BSD
#include "../shared/recovery.hpp"
using namespace RECOVERY;
static_assert(OPTIMIZER::BLOCKS(1)==1);
static_assert(OPTIMIZER::BLOCKS(2)==1);
static_assert(OPTIMIZER::BLOCKS(4)==1);
static_assert(OPTIMIZER::BLOCKS(8)==1);
static_assert(OPTIMIZER::BLOCKS(16)==1);
static_assert(OPTIMIZER::BLOCKS(32)==1);
static_assert(OPTIMIZER::BLOCKS(64)==1);
static_assert(OPTIMIZER::BLOCKS(128)==2);
static_assert(OPTIMIZER::BLOCKS(192)==3);
static_assert(OPTIMIZER::BLOCKS(256)==4);
static_assert(OPTIMIZER::BLOCKS(384)==6);
static_assert(OPTIMIZER::BLOCKS(448)==7);
static_assert(OPTIMIZER::BLOCKS(480)==8);
static_assert(OPTIMIZER::BLOCKS(4096)==64);
static_assert(OPTIMIZER::DISCLOSURE(512,0));
static_assert(OPTIMIZER::DISCLOSURE(512,1));
static_assert(!OPTIMIZER::DISCLOSURE(512,512));
static_assert(!OPTIMIZER::DISCLOSURE(0,0));
static_assert(NORT::SYMBOL[0]==char8_t(0xC3));
static_assert(NORT::SYMBOL[1]==char8_t(0x98));
static_assert(NONCE::SYMBOL[0]==u8'0');
static_assert(NONCE::SYMBOL[1]==char8_t(0xCC));
static_assert(NONCE::SYMBOL[2]==char8_t(0xB8));
int main() { return 0; }
