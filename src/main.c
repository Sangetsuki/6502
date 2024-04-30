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

u8 memory_read(u32 *cycles, u16 addr, struct Memory *mem) {
  // memory mirror
  if ((0x0800 <= addr && addr <= 0x0FFF) ||
      (0x1000 <= addr && addr <= 0x17FF) ||
      (0x1800 <= addr && addr <= 0x1FFF)) {
    addr %= 0x0800;
  } else if (0x2008 <= addr && addr <= 0x3FFF) {
    addr = 0x2000 + addr % 8;
  }
  (*cycles)--;
  return mem->data[addr];
}

u8 memory_write(u32 *cycles, u16 addr, u8 value, struct Memory *mem) {
  // memory mirror
  if ((0x0800 <= addr && addr <= 0x0FFF) ||
      (0x1000 <= addr && addr <= 0x17FF) ||
      (0x1800 <= addr && addr <= 0x1FFF)) {
    addr %= 0x0800;
  } else if (0x2008 <= addr && addr <= 0x3FFF) {
    addr = 0x2000 + addr % 8;
  }
  (*cycles)--;
  mem->data[addr] = value;
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
  (*cycles)--;
  return cpu->mem->data[cpu->pc++];
}

enum opcodes {
  INS_CLC = 0x18,
  INS_AND_IDX = 0x21,
  INS_AND_ZPG = 0x25,
  INS_AND_IMM = 0x29,
  INS_AND_ABS = 0x2D,
  INS_AND_IDY = 0x31,
  INS_AND_ZPX = 0x35,
  INS_SEC = 0x38,
  INS_AND_ABY = 0x39,
  INS_AND_ABX = 0x3D,
  INS_JMP_ABS = 0x4C,
  INS_CLI = 0x58,
  INS_ADC_IDX = 0x61,
  INS_ADC_ZPG = 0x65,
  INS_ADC_IMM = 0x69,
  INS_ADC_ABS = 0x6D,
  INS_ADC_IDY = 0x61,
  INS_JMP_IND = 0x6C,
  INS_ADC_ZPX = 0x75,
  INS_SEI = 0x78,
  INS_ADC_ABY = 0x79,
  INS_ADC_ABX = 0x7D,
  INS_STA_ZPG = 0x85,
  INS_STA_ZPX = 0x95,
  INS_LDY_IMM = 0xA0,
  INS_LDA_IDX = 0xA1,
  INS_LDX_IMM = 0xA2,
  INS_LDY_ZPG = 0xA4,
  INS_LDA_ZPG = 0xA5,
  INS_LDX_ZPG = 0xA6,
  INS_LDA_IMM = 0xA9,
  INS_LDY_ABS = 0xAC,
  INS_LDA_ABS = 0xAD,
  INS_LDX_ABS = 0xAE,
  INS_LDA_IDY = 0xB1,
  INS_LDY_ZPX = 0xB4,
  INS_LDA_ZPX = 0xB5,
  INS_LDX_ZPY = 0xB6,
  INS_LDA_ABY = 0xB9,
  INS_LDY_ABX = 0xBC,
  INS_LDA_ABX = 0xBD,
  INS_LDX_ABY = 0xBE,
  INS_CLV = 0xB8,
  INS_CLD = 0xD8,
  INS_NOP = 0xEA,
  INS_SED = 0xF8,
};

void cpu_execute(u32 cycles, struct CPU *cpu) {
  u8 data[10];
  while (cycles > 0) {
    u8 opcode = cpu_get_mem(&cycles, cpu);
    switch (opcode) {
    case INS_CLC:
      cpu->carry = 0;
      cycles--;
      break;
    case INS_AND_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu->ac &= data[1];
      if (cpu->ac == 0)
        cpu->zero = 1;
      if (cpu->ac & 0x80)
        cpu->negative = 1;
      break;
    case INS_AND_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu->ac &= data[0];
      if (cpu->ac == 0)
        cpu->zero = 1;
      if (cpu->ac & 0x80)
        cpu->negative = 1;
      break;
    case INS_SEC:
      cpu->carry = 1;
      cycles--;
      break;
    case INS_JMP_ABS:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      cpu->pc = (data[1] << 8) | data[0];
      break;
    case INS_CLI:
      cpu->irq_disable = 0;
      cycles--;
      break;
    case INS_JMP_IND:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      cpu->pc = (data[1] << 8) | data[0];
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      cpu->pc = (data[1] << 8) | data[0];
      break;
    case INS_SEI:
      cpu->irq_disable = 1;
      break;
    case INS_STA_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      memory_write(&cycles, data[0], cpu->ac, cpu->mem);
      break;
    case INS_STA_ZPX:
      data[0] = cpu_get_mem(&cycles, cpu);
      memory_write(&cycles, data[0] + cpu->x, cpu->ac, cpu->mem);
      cycles--;
      break;
    case INS_LDY_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu->y = data[0];
      if (cpu->y == 0)
        cpu->zero = 1;
      if (cpu->y & 0x80)
        cpu->negative = 1;
      break;
    case INS_LDX_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu->x = data[0];
      if (cpu->x == 0)
        cpu->zero = 1;
      if (cpu->x & 0x80)
        cpu->negative = 1;
      break;
    case INS_LDY_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu->y = data[1];
      if (cpu->y == 0)
        cpu->zero = 1;
      if (cpu->y & 0x80)
        cpu->negative = 1;
      break;
    case INS_LDA_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu->ac = data[1];
      if (cpu->ac == 0)
        cpu->zero = 1;
      if (cpu->ac & 0x80)
        cpu->negative = 1;
      break;
    case INS_LDX_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu->x = data[1];
      if (cpu->x == 0)
        cpu->zero = 1;
      if (cpu->x & 0x80)
        cpu->negative = 1;
      break;
    case INS_LDA_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu->ac = data[0];
      if (cpu->ac == 0)
        cpu->zero = 1;
      if (cpu->ac & 0x80)
        cpu->negative = 1;
      break;
    case INS_CLV:
      cpu->overflow = 0;
      cycles--;
      break;
    case INS_CLD:
      cpu->decimal = 0;
      break;
    case INS_NOP:
      cycles--;
      break;
    case INS_SED:
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
  cpu.mem->data[0x0120] = 0xFC;
  cpu.mem->data[0x0121] = 0xBA;
  cpu.mem->data[0xFFFC] = INS_JMP_IND;
  cpu.mem->data[0xFFFD] = 0x20;
  cpu.mem->data[0xFFFE] = 0x01;
  cpu.mem->data[0xBAFC] = INS_LDA_IMM;
  cpu.mem->data[0xBAFD] = 0x40;
  printf("pc antes: %x\n", cpu.pc);
  cpu_execute(7, &cpu);
  printf("pc depois: %x\n", cpu.pc);
  printf("a = %x\n", cpu.ac);

  return 0;
}
