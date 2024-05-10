#include "memory.h"
#include "ppu.h"

void memory_reset(struct Memory *mem) {
  for (int i = 0; i < MEM_SIZE; i++) {
    mem->data[i] = 0;
  }
}

u8 memory_read(u16 addr, struct Memory *mem) {
  // memory mirror
  if ((0x0800 <= addr && addr <= 0x1FFF)) {
    addr &= 0x07FF;
  } else if (0x2008 <= addr && addr <= 0x3FFF) {
    addr = 0x2000 + addr % 8;
  }
  return mem->data[addr];
}

u8 memory_write(u16 addr, u8 value, struct Memory *mem) {
  // memory mirror
  if ((0x0800 <= addr && addr <= 0x1FFF)) {
    addr &= 0x07FF;
  } else if (0x2008 <= addr && addr <= 0x3FFF) {
    addr = 0x2000 + addr % 8;
  }
  mem->data[addr] = value;
  return mem->data[addr];
}
