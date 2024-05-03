#include "cpu.h"
#include "memory.h"
#include <stdio.h>

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

static void cpu_branch_pc(u8 relative, u32 *cycles, struct CPU *cpu) {
  cpu->pc += relative;
  (*cycles)--;
}

static void cpu_set_ac(u8 value, struct CPU *cpu) {
  cpu->ac = value;
  if (cpu->ac == 0)
    cpu->zero = 1;
  if (cpu->ac & 0x80)
    cpu->negative = 1;
}

static void cpu_set_x(u8 value, struct CPU *cpu) {
  cpu->x = value;
  if (cpu->x == 0)
    cpu->zero = 1;
  if (cpu->x & 0x80)
    cpu->negative = 1;
}

static void cpu_set_y(u8 value, struct CPU *cpu) {
  cpu->y = value;
  if (cpu->y == 0)
    cpu->zero = 1;
  if (cpu->y & 0x80)
    cpu->negative = 1;
}

static void cpu_stack_push(u8 value, struct CPU *cpu) {
  cpu->mem->data[cpu->sp--] = value;
};

static u8 cpu_stack_pop(struct CPU *cpu) { return cpu->mem->data[cpu->sp++]; };

void cpu_execute(u32 cycles, struct CPU *cpu) {
  u8 data[4];
  while (cycles > 0) {
    u8 opcode = cpu_get_mem(&cycles, cpu);
    printf("opcode: $%2x - A:$%2x X:$%2x Y:$%2x PC:$%2x SP:$%2x\n", opcode,
           cpu->ac, cpu->x, cpu->y, cpu->pc - 1, cpu->sp);
    switch (opcode) {
    case INS_ORA_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu_set_ac(cpu->ac & data[0], cpu);
      break;
    case INS_BPL:
      data[0] = cpu_get_mem(&cycles, cpu);
      if (!cpu->negative) {
        cpu_branch_pc(data[0], &cycles, cpu);
      }
      break;
    case INS_CLC:
      cpu->carry = 0;
      cycles--;
      break;
    case INS_JSR:
      cpu_stack_push(cpu->pc & 0xFF, cpu);
      cpu_stack_push(cpu->pc >> 8, cpu);
      cycles--;
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      cpu->sp = (data[1] << 8) | data[0];
      cycles--;
      break;
    case INS_AND_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu_set_ac(cpu->ac & data[1], cpu);
      break;
    case INS_AND_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu_set_ac(cpu->ac & data[0], cpu);
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
      cycles--;
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
    case INS_BCS:
      data[0] = cpu_get_mem(&cycles, cpu);
      if (cpu->carry) {
        cpu->pc += data[0];
        cycles--;
      }
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
    case INS_TXS:
      cpu->sp = cpu->x;
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
      cpu_set_y(data[0], cpu);
      break;
    case INS_LDX_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu_set_x(data[0], cpu);
      break;
    case INS_LDY_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu_set_y(data[1], cpu);
      break;
    case INS_LDA_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu_set_ac(data[1], cpu);
      break;
    case INS_LDX_ZPG:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = memory_read(&cycles, data[0], cpu->mem);
      cpu_set_x(data[1], cpu);
      break;
    case INS_TAY:
      cpu_set_y(cpu->ac, cpu);
      cycles--;
      break;
    case INS_LDA_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      cpu_set_ac(data[0], cpu);
      break;
    case INS_TAX:
      cpu_set_x(cpu->ac, cpu);
      cycles--;
      break;
    case INS_LDA_ABS:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      cpu_set_ac(memory_read(&cycles, (data[1] << 8) | data[0], cpu->mem), cpu);
      break;
    case INS_TSX:
      cpu_set_x(cpu->pc, cpu);
      cycles--;
      break;
    case INS_LDA_ABX:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      cpu_set_ac(
          memory_read(&cycles, ((data[1] << 8) | data[0]) + cpu->x, cpu->mem),
          cpu);
      break;
    case INS_CLV:
      cpu->overflow = 0;
      cycles--;
      break;
    case INS_CMP_IMM:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu->ac - data[0];
      if (cpu->ac >= data[0])
        cpu->carry = 1;
      if (cpu->ac == data[0])
        cpu->zero = 1;
      if (data[1] & 0x80)
        cpu->negative = 1;
      break;
    case INS_CA:
      cpu_set_x(cpu->x - 1, cpu);
      cycles--;
      break;
    case INS_BNE:
      data[0] = cpu_get_mem(&cycles, cpu);
      if (!cpu->carry) {
        cpu->pc += data[0];
        cycles--;
      }
      break;
    case INS_CLD:
      cpu->decimal = 0;
      cycles--;
      break;
    case INS_NOP:
      cycles--;
      break;
    case INS_INC_ABS:
      data[0] = cpu_get_mem(&cycles, cpu);
      data[1] = cpu_get_mem(&cycles, cpu);
      data[2] = memory_read(&cycles, (data[1] << 8) | data[0], cpu->mem);
      memory_write(&cycles, (data[1] << 8) | data[0], data[2] + 1, cpu->mem);
      if (data[2] == 0)
        cpu->zero = 1;
      if (data[2] & 0x80)
        cpu->negative = 1;
      cycles--;
      break;
    case INS_SED:
      cpu->decimal = 1;
      cycles--;
      break;
    default:
      printf("instrucao nao implementada em $%x: $%x\n", cpu->pc, opcode);
      cycles = 0;
      break;
    }
  }
}
