#include "cpu.h"
#include "memory.h"
#include <stdio.h>

int main(void) {
  struct Memory mem;
  struct CPU cpu;
  cpu.mem = &mem;
  cpu_reset(&cpu);
  cpu.ac = 0x40;
  cpu.mem->data[0xFFFC] = INS_JMP_ABS;
  cpu.mem->data[0xFFFD] = 0x00;
  cpu.mem->data[0xFFFE] = 0x00;
  cpu.mem->data[0x0000] = INS_LDA_IMM;
  cpu.mem->data[0x0001] = 0x40;
  cpu.mem->data[0x0002] = INS_STA_ABS;
  cpu.mem->data[0x0003] = 0x20;
  cpu.mem->data[0x0004] = 0x01;
  cpu_execute(20, &cpu);
  printf("0x0120 = %x\n", cpu.mem->data[0x0120]);

  return 0;
}
