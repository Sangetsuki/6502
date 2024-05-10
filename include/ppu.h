#ifndef INCLUDE_INCLUDE_PPU_H_
#define INCLUDE_INCLUDE_PPU_H_

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

struct PPU {
  u8 pattern_tables[0x2000];
  u8 vram[0x3F00 - 0x2000];
  u8 palettes[0x4000 - 0x3F00];
};

#endif // INCLUDE_INCLUDE_PPU_H_
