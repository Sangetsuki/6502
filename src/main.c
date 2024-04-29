#include <stdint.h>
#include <stdio.h>

#define MEM_SIZE 1024 * 64

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

struct Memory {
  u8 data[MEM_SIZE];
};

void memory_reset(struct Memory *mem) {
  for (int i = 0; i < MEM_SIZE; i++) {
    mem->data[i] = 0;
  }
}

u8 memory_read(u32 *cycles, u8 addr, struct Memory *mem) {
  (*cycles)--;
  return mem->data[addr];
}

struct CPU {
  u8 ac;
  u8 y;
  u8 x;
  u16 pc;
  u8 sp;

  u8 carry : 1;
  u8 zero : 1;
  u8 irq_disable : 1;
  u8 decimal : 1;
  u8 brk : 1;
  u8 overflow : 1;
  u8 negative : 1;

  struct Memory *mem;
};

void cpu_reset(struct CPU *cpu) {
  cpu->pc = 0xFFFC;
  cpu->decimal = 0;
  cpu->sp = 0x00; // 0x0100;
  cpu->ac = cpu->x = cpu->y = 0;
  memory_reset(cpu->mem);
}

u8 cpu_get_mem(u32 *cycles, struct CPU *cpu) {
  *cycles = *cycles - 1;
  return cpu->mem->data[cpu->pc++];
}

void cpu_execute(u32 cycles, struct CPU *cpu) {
  u8 data[10];
  while (cycles > 0) {
    u8 opcode = cpu_get_mem(&cycles, cpu);
    switch (opcode) {
    case 0x18:
      cpu->carry = 0;
      cycles--;
      break;
    case 0x38:
      cpu->carry = 1;
      cycles--;
      break;
    case 0x58:
      cpu->irq_disable = 0;
      cycles--;
      break;
    case 0x78:
      cpu->irq_disable = 1;
      break;
    case 0xA0:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu->y = data[0];
      if (cpu->y == 0)
        cpu->zero = 1;
      if (cpu->y & 0x80)
        cpu->negative = 1;
      break;
    case 0xA2:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu->x = data[0];
      if (cpu->x == 0)
        cpu->zero = 1;
      if (cpu->x & 0x80)
        cpu->negative = 1;
      break;
    case 0xA4:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu->y = data[1];
      if (cpu->y == 0)
        cpu->zero = 1;
      if (cpu->y & 0x80)
        cpu->negative = 1;
      break;
    case 0xA5:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu->ac = data[1];
      if (cpu->ac == 0)
        cpu->zero = 1;
      if (cpu->ac & 0x80)
        cpu->negative = 1;
      break;
    case 0xA6:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu->x = data[1];
      if (cpu->x == 0)
        cpu->zero = 1;
      if (cpu->x & 0x80)
        cpu->negative = 1;
      break;
    case 0xA9:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu->ac = data[0];
      if (cpu->ac == 0)
        cpu->zero = 1;
      if (cpu->ac & 0x80)
        cpu->negative = 1;
      break;
    case 0xB8:
      cpu->overflow = 0;
      cycles--;
      break;
    case 0xD8:
      cpu->decimal = 0;
      break;
    case 0xEA:
      cycles--;
      break;
    case 0xF8:
      cpu->decimal = 1;
      cycles--;
      break;
    default:
      printf("Nao implementado!\n");
      cycles = 0;
      break;
    }
  }
}

int main(void) {
  struct Memory mem;
  struct CPU cpu;
  cpu.mem = &mem;
  cpu_reset(&cpu);
  cpu.mem->data[0x0069] = 10;
  cpu.mem->data[0xFFFC] = 0xEA;
  cpu.mem->data[0xFFFD] = 0xEA;
  printf("pc antes: %x\n", cpu.pc);
  cpu_execute(2, &cpu);
  printf("pc depois: %x", cpu.pc);
  return 0;
}
