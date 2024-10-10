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

#include <capstone/capstone.h>
#include <common.h>
#include <dlfcn.h>

static size_t (*cs_disasm_dl)(csh handle, const uint8_t *code, size_t code_size,
                              uint64_t address, size_t count, cs_insn **insn);
static void (*cs_free_dl)(cs_insn *insn, size_t count);

static csh handle;

void init_disasm() {
  void *dl_handle;
  dl_handle = dlopen("tools/capstone/repo/libcapstone.so.5", RTLD_LAZY);
  assert(dl_handle);

  cs_err (*cs_open_dl)(cs_arch arch, cs_mode mode, csh *handle) = NULL;
  cs_open_dl = dlsym(dl_handle, "cs_open");
  assert(cs_open_dl);

  cs_disasm_dl = dlsym(dl_handle, "cs_disasm");
  assert(cs_disasm_dl);

  cs_free_dl = dlsym(dl_handle, "cs_free");
  assert(cs_free_dl);

  cs_arch arch = MUXDEF(
      CONFIG_ISA_x86, CS_ARCH_X86,
      MUXDEF(CONFIG_ISA_mips32, CS_ARCH_MIPS,
             MUXDEF(CONFIG_ISA_riscv, CS_ARCH_RISCV,
                    MUXDEF(CONFIG_ISA_loongarch32r, CS_ARCH_LOONGARCH, -1))));
  cs_mode mode = MUXDEF(
      CONFIG_ISA_x86, CS_MODE_32,
      MUXDEF(CONFIG_ISA_mips32, CS_MODE_MIPS32,
             MUXDEF(CONFIG_ISA_riscv,
                    MUXDEF(CONFIG_ISA64, CS_MODE_RISCV64, CS_MODE_RISCV32) |
                        CS_MODE_RISCVC,
                    MUXDEF(CONFIG_ISA_loongarch32r, CS_MODE_LOONGARCH32, -1))));
  int ret = cs_open_dl(arch, mode, &handle);
  assert(ret == CS_ERR_OK);

#ifdef CONFIG_ISA_x86
  cs_err (*cs_option_dl)(csh handle, cs_opt_type type, size_t value) = NULL;
  cs_option_dl = dlsym(dl_handle, "cs_option");
  assert(cs_option_dl);

  ret = cs_option_dl(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
  assert(ret == CS_ERR_OK);
#endif
}

// a iringbuf code
#define IRING_BUF_SIZE 10
static char iringbuf[IRING_BUF_SIZE][100];
size_t iringbuf_current_idx = 0;
extern NEMUState nemu_state;

void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte) {
  cs_insn *insn;
  size_t count = cs_disasm_dl(handle, code, nbyte, pc, 0, &insn);
  // display pc
  /* printf("0x%" PRIx64 ":\t", pc); */
  // display binary code
  /* for (int i = nbyte - 1; i >= 0; i--) { */
  /*   printf("%02x ", code[i]); */
  /* } */
  /* printf("\n"); */
  // debug log

  // store current command info
  sprintf(iringbuf[iringbuf_current_idx], "0x%" PRIx64 ":\t%s\t\t%s\n",
          insn[0].address, insn[0].mnemonic, insn[0].op_str);
  iringbuf_current_idx = iringbuf_current_idx % IRING_BUF_SIZE;

  /* for (size_t i = 0; i < count; i++) { */
  /*   printf("0x%" PRIx64 ":\t%s\t\t%s\n", insn[i].address, insn[i].mnemonic,
   */
  /*          insn[i].op_str); */
  /* } */
  if (nemu_state.state == NEMU_ABORT) {
    // display iringbuf
    printf(" ------------------------ \n");
    for (int i = 0; i < IRING_BUF_SIZE; i++) {
      if (i == iringbuf_current_idx) {
        printf("===> %s", iringbuf[i]);
        break;
      }
      printf("     %s", iringbuf[i]);
    }
    printf(" ------------------------ \n");
  }
  assert(count == 1);
  int ret = snprintf(str, size, "%s", insn->mnemonic);
  if (insn->op_str[0] != '\0') {
    snprintf(str + ret, size - ret, "\t%s", insn->op_str);
  }
  cs_free_dl(insn, count);
}
