#include "memory.h"
#include "ppu.h"

static inline u16 addr_fix(u16 addr) {
  if ((0x0800 <= addr && addr <= 0x1FFF)) {
    addr &= 0x07FF;
  } else if (0x2008 <= addr && addr <= 0x3FFF) {
    addr = 0x2000 + addr % 8;
  }
  return addr;
}

void memory_reset(struct Memory *mem) {
  for (int i = 0; i < MEM_SIZE; i++) {
    mem->data[i] = 0;
  }
}

u8 memory_read(u16 addr, struct Memory *mem) {
  addr = addr_fix(addr);

  return mem->data[addr];
}

u8 memory_write(u16 addr, u8 value, struct Memory *mem) {
  addr = addr_fix(addr);

  mem->data[addr] = value;
  return mem->data[addr];
}
