#include "cpu.h"
#include "memory.h"
#include "rom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  struct Memory mem;
  struct CPU cpu;
  cpu.mem = &mem;
  cpu_reset(&cpu);
  struct RomHeader rom_header;
  read_rom_header("./mario.nes", &rom_header);

  u8 mapper = (rom_header.flags_6 & 0xF0) | ((rom_header.flags_7 & 0xF0) << 8);
  printf("mapper: %x\n", mapper);
  const char *title = (char *)&rom_header.nes;
  puts(title);
  printf("Essa ROM tem %d kb de programa e %d kb de graficos\n",
         rom_header.prg_rom_size * 16, rom_header.chr_rom_size * 8);

  u8 *rom_data = alloc_rom("./mario.nes");

  // mapper 0
  memcpy(&cpu.mem->data[0x8000], rom_data, 0x8000);
  memcpy(&cpu.mem->data[0x0000], rom_data + 0x8000, 0x2000);

  free(rom_data);

  cpu.pc = (cpu.mem->data[0xFFFD] << 8) | cpu.mem->data[0xFFFC];
  printf("VECTOR RESET: $%x\n", cpu.pc);
  cpu_execute(&cpu);

  return 0;
}
