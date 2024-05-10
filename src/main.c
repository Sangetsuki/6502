#include "cpu.h"
#include "memory.h"
#include "nes.h"
#include "rom.h"
#include <raylib.h>
#include <stdio.h>

#define SCALE 3

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
  struct NES *nes;
  nes = create_NES();

  if (!load_ROM(nes, "./mario.nes")) {
    free_NES(nes);
    return 1;
  }

  InitWindow(256 * SCALE, 240 * SCALE, "6502");

  while (!WindowShouldClose()) {
    cpu_execute(nes->cpu);
    dump_memory(nes->cpu);
    BeginDrawing();
    ClearBackground(WHITE);
    int x = 2, y = 2;
    for (int i = 0; i < 512; i++) {
      render_tile(i, nes->cpu, x, y);
      x += 10;
      if (x >= 248) {
        x = 2;
        y += 10;
      }
    }

    EndDrawing();
  }
  CloseWindow();

  free_NES(nes);
  return 0;
}
