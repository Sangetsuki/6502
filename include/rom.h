#ifndef INCLUDE_INCLUDE_ROM_H_
#define INCLUDE_INCLUDE_ROM_H_

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

struct RomHeader {
  u8 nes[4];
  u8 prg_rom_size;
  u8 chr_rom_size;
  u8 flags_6;
  u8 flags_7;
  u8 flags_8;
  u8 flags_9;
  u8 flags_10;
  u8 unused[5];
};

void read_rom_header(const char *path, struct RomHeader *dest);
u8 *alloc_rom(const char *path);

#endif // INCLUDE_INCLUDE_ROM_H_
