#ifndef INCLUDE_INCLUDE_CPU_H_
#define INCLUDE_INCLUDE_CPU_H_

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

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

enum opcodes {
  INS_ORA_IMM = 0x09,
  INS_BPL = 0x10,
  INS_CLC = 0x18,
  INS_JSR = 0x20,
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
  INS_STA_ABS = 0x8D,
  INS_STA_ZPX = 0x95,
  INS_STA_ABY = 0x99,
  INS_TXS = 0x9A,
  INS_STA_ABX = 0x9D,
  INS_LDY_IMM = 0xA0,
  INS_LDA_IDX = 0xA1,
  INS_LDX_IMM = 0xA2,
  INS_LDY_ZPG = 0xA4,
  INS_LDA_ZPG = 0xA5,
  INS_LDX_ZPG = 0xA6,
  INS_TAY = 0xA8,
  INS_LDA_IMM = 0xA9,
  INS_TAX = 0xAA,
  INS_LDY_ABS = 0xAC,
  INS_LDA_ABS = 0xAD,
  INS_LDX_ABS = 0xAE,
  INS_BCS = 0xB0,
  INS_LDA_IDY = 0xB1,
  INS_LDY_ZPX = 0xB4,
  INS_LDA_ZPX = 0xB5,
  INS_LDX_ZPY = 0xB6,
  INS_LDA_ABY = 0xB9,
  INS_TSX = 0xBA,
  INS_LDY_ABX = 0xBC,
  INS_LDA_ABX = 0xBD,
  INS_LDX_ABY = 0xBE,
  INS_CLV = 0xB8,
  INS_CMP_IMM = 0xC9,
  INS_CA = 0xCA,
  INS_BNE = 0xD0,
  INS_CLD = 0xD8,
  INS_NOP = 0xEA,
  INS_INC_ABS = 0xEE,
  INS_SED = 0xF8,
};

void cpu_reset(struct CPU *cpu);
u8 cpu_get_mem(u32 *cycles, struct CPU *cpu);
void cpu_execute(u32 cycles, struct CPU *cpu);
void cpu_load_rom(const char *path, struct CPU *cpu);

#endif // INCLUDE_INCLUDE_CPU_H_
