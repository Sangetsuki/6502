#include "cpu.h"
#include "memory.h"
#include <stdio.h>

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
