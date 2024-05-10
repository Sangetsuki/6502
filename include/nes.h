#ifndef INCLUDE_INCLUDE_NES_H_
#define INCLUDE_INCLUDE_NES_H_

#include "rom.h"
struct NES {
  struct CPU *cpu;
  struct PPU *ppu;
  struct RomHeader romHeader;
};

struct NES *create_NES();
int load_ROM(struct NES *nes, const char *path);
void free_NES(struct NES *nes);

#endif // INCLUDE_INCLUDE_NES_H_
