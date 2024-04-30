#include "cpu.h"
#include "memory.h"

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
    case INS_STA_ABS:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      memory_write(&cycles, (data[1] << 8) | data[0], cpu->ac, cpu->mem);
      break;
    case INS_STA_ZPX:
      data[0] = cpu_get_mem(&cycles, cpu);
      memory_write(&cycles, data[0] + cpu->x, cpu->ac, cpu->mem);
      cycles--;
      break;
    case INS_STA_ABY:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      memory_write(&cycles, ((data[1] << 8) | data[0]) + cpu->y, cpu->ac,
                   cpu->mem);
      cycles--;
      break;
    case INS_STA_ABX:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      memory_write(&cycles, ((data[1] << 8) | data[0]) + cpu->x, cpu->ac,
                   cpu->mem);
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
    case INS_TAY:
      cpu->y = cpu->ac;
      if (cpu->y == 0)
        cpu->zero = 1;
      if (cpu->y & 0x80)
        cpu->negative = 1;
      cycles--;
      break;
    case INS_LDA_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu->ac = data[0];
      if (cpu->ac == 0)
        cpu->zero = 1;
      if (cpu->ac & 0x80)
        cpu->negative = 1;
      break;
    case INS_TAX:
      cpu->x = cpu->ac;
      if (cpu->x == 0)
        cpu->zero = 1;
      if (cpu->x & 0x80)
        cpu->negative = 1;
      cycles--;
      break;
    case INS_TSX:
      cpu->x = cpu->sp;
      if (cpu->x == 0)
        cpu->zero = 1;
      if (cpu->x & 0x80)
        cpu->negative = 1;
      cycles--;
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
      cycles = 0;
      break;
    }
  }
}
