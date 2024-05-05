#include "cpu.h"
#include "memory.h"
#include "rom.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCALE 2

static void dump_memory(struct CPU *cpu) {
  FILE *mem_dump = fopen("memdump.bin", "wb+");
  fwrite(cpu->mem->data, MEM_SIZE, 1, mem_dump);
  fclose(mem_dump);
}

static void render_tile(int tile_id, struct CPU *cpu, int x, int y) {
  for (int byte = 0; byte < 8; byte++) {
    u8 upper = cpu->mem->data[byte + 16 * tile_id];
    u8 lower = cpu->mem->data[byte + 8 + 16 * tile_id];
    for (int bit = 0; bit < 8; bit++) {
      u8 value = (1 & upper) << 1 | (1 & lower);
      upper = upper >> 1;
      lower = lower >> 1;
      Color col;
      switch (value) {
      case 0:
        col = BLACK;
        break;
      case 1:
        col = DARKGRAY;
        break;
      case 2:
        col = GRAY;
        break;
      case 3:
        col = LIGHTGRAY;
        break;
      }

      DrawRectangle((x + bit) * SCALE, (y + byte) * SCALE, SCALE, SCALE, col);
    }
  }
}

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

  InitWindow(256 * SCALE, 240 * SCALE, "6502");

  while (!WindowShouldClose()) {
    cpu_execute(&cpu);
    dump_memory(&cpu);
    BeginDrawing();
    ClearBackground(WHITE);
    int x = 2, y = 2;
    for (int i = 0; i < 512; i++) {
      render_tile(i, &cpu, x, y);
      x += 10;
      if (x >= 256 - 8) {
        x = 2;
        y += 10;
      }
    }

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
