#include "cpu.h"
#include "memory.h"
#include <stdio.h>

int main(void) {
  struct Memory mem;
  struct CPU cpu;
  cpu.mem = &mem;
  cpu_reset(&cpu);
  cpu.mem->data[0xFFFC] = INS_JMP_ABS;
  cpu.mem->data[0xFFFD] = 0x00;
  cpu.mem->data[0xFFFE] = 0x80;
  cpu.mem->data[0x8000] = INS_LDA_IMM;
  cpu.mem->data[0x8001] = 0x40;
  cpu.mem->data[0x8002] = INS_STA_ABS;
  cpu.mem->data[0x8003] = 0x20;
  cpu.mem->data[0x8004] = 0x01;
  cpu.mem->data[0x8005] = INS_TAX;
  cpu_execute(20, &cpu);
  printf("x = %x\n", cpu.x);

  return 0;
}
