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
#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/ifetch.h>
#include <stdint.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  TYPE_I,
  TYPE_U,
  TYPE_S,
  TYPE_N, // none
  TYPE_J, // for jal
  TYPE_R, // for add
  TYPE_B, // for beq
};

#define src1R()                                                                \
  do {                                                                         \
    *src1 = R(rs1);                                                            \
  } while (0)
#define src2R()                                                                \
  do {                                                                         \
    *src2 = R(rs2);                                                            \
  } while (0)
#define immI()                                                                 \
  do {                                                                         \
    *imm = SEXT(BITS(i, 31, 20), 12);                                          \
  } while (0)
#define immU()                                                                 \
  do {                                                                         \
    *imm = SEXT(BITS(i, 31, 12), 20) << 12;                                    \
  } while (0)
#define immS()                                                                 \
  do {                                                                         \
    *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7);                   \
  } while (0)
#define immJ()                                                                 \
  do {                                                                         \
    *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | (BITS(i, 19, 12) << 12) |        \
           (BITS(i, 20, 20) << 11) | (BITS(i, 30, 21) << 1);                   \
  } while (0)
#define immB()                                                                 \
  do {                                                                         \
    *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | (BITS(i, 7, 7) << 11) |          \
           (BITS(i, 30, 25) << 5) | (BITS(i, 11, 8) << 1);                     \
  } while (0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2,
                           word_t *imm, int type) {
  uint32_t i = s->isa.inst;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd = BITS(i, 11, 7);
  switch (type) {
  case TYPE_I:
    src1R();
    immI();
    break;
  case TYPE_U:
    immU();
    break;
  case TYPE_S:
    src1R();
    src2R();
    immS();
    break;
  case TYPE_J:
    immJ();
    break;
  case TYPE_R:
    src1R();
    src2R();
    break;
  case TYPE_B:
    src1R();
    src2R();
    immB();
    break;
  case TYPE_N:
    break;
  default:
    panic("unsupported type = %d", type);
  }
}

static int decode_exec(Decode *s) {
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */)                   \
  {                                                                            \
    int rd = 0;                                                                \
    word_t src1 = 0, src2 = 0, imm = 0;                                        \
    decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type));           \
    __VA_ARGS__;                                                               \
    printf("imm = %d\n", imm);                                                 \
  }

  INSTPAT_START();
  // type u
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc, U,
          R(rd) = s->pc + imm);
  // 2.4.1. Integer Register-Immediate Instructions
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui, U, R(rd) = imm);

  // type I
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu, I,
          R(rd) = Mr(src1 + imm, 1));
  // 2.4.1. Integer Register-Immediate Instructions
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi, I,
          R(rd) = src1 + imm);
  // 2.5.1. Unconditional Jumps
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr, I, R(rd) = s->pc + 4;
          s->dnpc = (src1 + imm) & 0xfffffffe);
  // 2.6. Load and Store Instructions
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw, I,
          R(rd) = Mr(src1 + imm, 4));

  // 2.6. Load and Store Instructions
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh, I,
          R(rd) = SEXT(Mr(src1 + imm, 2), 16));

  // 2.6. Load and Store Instructions
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu, I,
          R(rd) = (uint32_t)Mr(src1 + imm, 2)); // zero extend to 32 bit

  // 2.4.1. Integer Register-Immediate Instructions
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu, I,
          R(rd) = (src1 < imm) ? 1 : 0); // sext 确实是返回无符号
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai, I,
          R(rd) = (int32_t)src1 >> BITS(imm, 4, 0)); // arithmetic right shift
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli, I,
          R(rd) = src1 >> BITS(imm, 5, 0)); // logical right shift
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli, I,
          R(rd) = src1 << BITS(imm, 5, 0)); // logical left shift
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi, I,
          R(rd) = src1 & imm); // 20 ~ 25

  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori, I,
          R(rd) = src1 ^ imm);
  // type r
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add, R,
          R(rd) = src1 + src2);

  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub, R,
          R(rd) = src1 - src2);

  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu, R,
          R(rd) = (src1 < src2) ? 1 : 0); // compare unsigned

  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt, R,
          R(rd) = ((int32_t)src1 < (int32_t)src2) ? 1 : 0); // compare signed

  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor, R,
          R(rd) = src1 ^ src2);

  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and, R,
          R(rd) = src1 & src2);

  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or, R, R(rd) = src1 | src2);

  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll, R,
          R(rd) = src1 << BITS(src2, 4, 0));

  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul, R,
          R(rd) = (int32_t)src1 * (int32_t)src2);

  // 13.2. Division Operations
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div, R,
          R(rd) = (int32_t)src1 / (int32_t)src2);

  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu, R,
          R(rd) = src1 / src2);
  // 13.2. Division Operations
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem, R,
          R(rd) = (int32_t)src1 % (int32_t)src2);

  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu, R,
          R(rd) = src1 % src2);
  // 13.1. Multiplication Operations
  // xlen 32
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh, R,
          R(rd) = ((int64_t)src1 * (int64_t)src2) >> 32); // RV32
  // type j
  // 2.5.1. Unconditional Jumps
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal, J, R(rd) = s->pc + 4;
          s->dnpc = s->pc + imm);

  // type s
  // 2.6. Load and Store Instructions
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb, S,
          Mw(src1 + imm, 1, src2));
  // 2.6. Load and Store Instructions
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw, S,
          Mw(src1 + imm, 4, src2));

  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh, S,
          Mw(src1 + imm, 2, src2));

  // type N
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak, N,
          NEMUTRAP(s->pc, R(10))); // R(10) is $a0

  // type b
  // 2.5.2 Conditional Branches
  INSTPAT(
      "??????? ????? ????? 000 ????? 11000 11", beq, B,
      if (src1 == src2) { s->dnpc = s->pc + imm; });

  INSTPAT(
      "??????? ????? ????? 001 ????? 11000 11", bne, B,
      if (src1 != src2) { s->dnpc = s->pc + imm; });

  INSTPAT(
      "??????? ????? ????? 101 ????? 11000 11", bge, B,
      if ((int32_t)src1 >= (int32_t)src2) { s->dnpc = s->pc + imm; });

  INSTPAT(
      "??????? ????? ????? 111 ????? 11000 11", bgeu, B,
      if (src1 >= src2) { s->dnpc = s->pc + imm; });
  // 2.5.2 Conditional Branches
  INSTPAT(
      "??????? ????? ????? 100 ????? 11000 11", blt, B,
      if ((int32_t)src1 < (int32_t)src2) { s->dnpc = s->pc + imm; });

  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv, N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  // debug
  /* word_t origin = Mr(0x80000024, 4); */
  /* printf("Mr(0x80000024, 4) = %x\n", origin); */
  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
