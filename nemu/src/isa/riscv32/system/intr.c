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

#include <isa.h>

/* enum { */
/*   SYS_exit, */
/*   SYS_yield, */
/*   SYS_open, */
/*   SYS_read, */
/*   SYS_write, */
/*   SYS_kill, */
/*   SYS_getpid, */
/*   SYS_close, */
/*   SYS_lseek, */
/*   SYS_brk, */
/*   SYS_fstat, */
/*   SYS_time, */
/*   SYS_signal, */
/*   SYS_execve, */
/*   SYS_fork, */
/*   SYS_link, */
/*   SYS_unlink, */
/*   SYS_wait, */
/*   SYS_times, */
/*   SYS_gettimeofday */
/* }; */
/**/
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  //  riscv32触发异常后硬件的响应过程如下
  //  下面模拟硬件行为

  // 1.将当前PC值保存到mepc寄存器
  // NO 本质上是异常号
  /* if (NO == (uint32_t)(-1)) { // yield */
  /*   printf("system call, NO = -1\n"); */
  /*   cpu.csrs.mepc = epc + 4; */
  /* } else if (NO == 1) { // system call */
  /**/
  /*   printf("system call, NO = 1\n"); */
  /*   cpu.csrs.mepc = epc + 4; */
  /* } else { */
  /*   cpu.csrs.mepc = epc; */
  /* } */
  switch (NO) {
  case (uint32_t)(-1):
  case 1:  // yield
  case 0:  // sys_exit
  case 2:  // sys_open
  case 3:  // sys_read
  case 4:  // sys_write
  case 7:  // sys_close
  case 8:  // sys_lseek
  case 9:  // sys_brk
  case 19: // SYS_gettimeofday
    cpu.csrs.mepc = epc + 4;
    break;
  default:
    cpu.csrs.mepc = epc;
    break;
  }
  // 2.在mcause寄存器中设置异常号
  cpu.csrs.mcause = NO;
  /* printf("raise intr: NO = %x, epc = 0x%x\n", NO, epc); */
  // 3.从mtvec寄存器中取出异常入口地址,跳转到异常入口地址
  return cpu.csrs.mtvec;
}

word_t isa_query_intr() { return INTR_EMPTY; }
