#include "rom.h"
#include <stdio.h>
#include <stdlib.h>

void read_rom_header(const char *path, struct RomHeader *dest) {
  FILE *rom = fopen(path, "rb");
  fread(dest, sizeof(struct RomHeader), 1, rom);
  fclose(rom);
}

u8 *alloc_rom(const char *path) {
  struct RomHeader header;
  FILE *rom = fopen(path, "rb");
  fread(&header, sizeof(struct RomHeader), 1, rom);
  u32 size = header.prg_rom_size * 16 * 1024 + header.chr_rom_size * 8 * 1024;
  u8 *dest = malloc(size);
  fread(dest, sizeof(u8), size, rom);
  fclose(rom);
  return dest;
}
