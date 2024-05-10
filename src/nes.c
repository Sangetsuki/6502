#include "nes.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "rom.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct NES *create_NES() {
  struct NES *nes = malloc(sizeof(struct NES));
  nes->cpu = malloc(sizeof(struct CPU));
  nes->cpu->mem = malloc(sizeof(struct Memory));
  nes->ppu = malloc(sizeof(struct PPU));
  cpu_reset(nes->cpu);
  return nes;
}

void free_NES(struct NES *nes) {
  free(nes->cpu->mem);
  free(nes->cpu);
  free(nes->ppu);
}

int load_ROM(struct NES *nes, const char *path) {
  struct CPU *cpu = nes->cpu;
  struct RomHeader *header = &nes->romHeader;
  memset(header, 0, sizeof(struct RomHeader));
  read_rom_header(path, header);
  u8 mapper = (header->flags_6 & 0xF0) | ((header->flags_7 & 0xF0) << 8);
  printf("mapper: %x\n", mapper);
  const char *title = (char *)header->nes;
  if (strncmp(title, "NES", 3) != 0) {
    memset(header, 0, sizeof(struct RomHeader));
    return false;
  }
  printf("Essa ROM tem %d kb de programa e %d kb de graficos\n",
         header->prg_rom_size * 16, header->chr_rom_size * 8);
  u8 *rom_data = alloc_rom(path);
  memcpy(&cpu->mem->data[0x8000], rom_data, 0x8000);
  memcpy(&cpu->mem->data[0x0000], rom_data + 0x8000, 0x2000);

  free(rom_data);
  cpu->pc = btohw(cpu->mem->data[0xFFFC], cpu->mem->data[0xFFFD]);

  printf("VECTOR RESET: $%x\n", nes->cpu->pc);
  return true;
}
