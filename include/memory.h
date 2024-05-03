#ifndef INCLUDE_INCLUDE_MEMORY_H_
#define INCLUDE_INCLUDE_MEMORY_H_

#include <stdint.h>

#define MEM_SIZE 1024 * 64

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

struct Memory {
  u8 data[MEM_SIZE];
};

void memory_reset(struct Memory *mem);
u8 memory_read(u16 addr, struct Memory *mem);
u8 memory_write(u16 addr, u8 value, struct Memory *mem);

#endif // INCLUDE_INCLUDE_MEMORY_H_
