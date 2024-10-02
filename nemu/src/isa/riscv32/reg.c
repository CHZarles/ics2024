/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "local-include/reg.h"
#include "common.h"
#include <isa.h>

const char *regs[] = {"$0", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
                      "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                      "a6", "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
                      "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void isa_reg_display() {
  Log("Call isa_reg_display()");
  // 1.count the regs number
  int reg_nums = ARRLEN(cpu.gpr);
  // 2. loop the register
  for (int i = 0; i < reg_nums; ++i) {
    // display info
    // ref:
    // https://stackoverflow.com/questions/31026000/gdb-info-registers-command-second-column-of-output
    // name   value(hex)   value(decimal)
    printf("%-10s   0x%-10x    %-10d\n", reg_name(i), gpr(i), gpr(i));
  }
  // 3. extand , for pc
  printf("pc        0x%-10x    %-10d\n", cpu.pc, cpu.pc);
}

word_t isa_reg_str2val(const char *s, bool *success) {
  Log("Call isa_reg_str2val(s=%s)", s);
  // 1. check whether the input is valid register name
  word_t addr = -1;
  int reg_nums = ARRLEN(cpu.gpr);
  for (int i = 0; i < reg_nums; ++i) {
    /* printf("regs[%d] = %s\n", i, regs[i]); */
    if (strcmp(regs[i], s + 1) == 0) {
      // 2. get the address from register
      *success = true;
      addr = gpr(i);
      /* printf("addr = %d\n", addr); */
      break;
    }
  }
  // extand for pc
  if (strcmp(s, "$pc") == 0) {
    *success = true;
    addr = cpu.pc;
  }
  // 3. return the value
  Assert(addr != -1, "Invalid register name");
  return addr;
}
