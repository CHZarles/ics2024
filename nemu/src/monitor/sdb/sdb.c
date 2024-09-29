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

#include "sdb.h"
#include "debug.h"
#include <cpu/cpu.h>
#include <isa.h>
#include <memory/paddr.h> // NOTE: for pa1 assignmen
#include <readline/history.h>
#include <readline/readline.h>
#include <time.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin.
 */
static char *rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) { return -1; }

// step n
static int string_to_int(const char *args, int *steps) {
  // 检查输入指针是否为空
  Assert(steps != NULL, "result should not be NULL");
  Log("Call string_to_int(args = %p, steps = %p)", (void *)args,
      (void *)&steps);
  if (args == NULL) {
    Log("*steps <- 1");
    *steps = 1;
    return 0;
  }
  int items_parsed = sscanf(args, "%d", steps);
  Assert(items_parsed == 1, "Parse integer fail");
  return 0;
}
static int step_n(char *args) {
  // 1. parse args to int
  Log("Call step_n(%p)", (void *)args);
  int steps;
  Assert(string_to_int(args, &steps) == 0, "Call string_to_int fail");
  Log("Run si %d", steps);
  // 2. call
  cpu_exec(steps);

  return 0;
}

// info w, info r
static int info_some(char *args) {

  Log("Call info_some(%p)", (void *)args);
  // 1. parse args
  size_t args_len = strlen(args);
  Assert(args_len == 1, "Only support info w or info r");
  char info_type = args[0];
  Assert(info_type == 'r' || info_type == 'w', "Only support info w or info r");
  Log("Run info %c", info_type);
  if (info_type == 'r') {
    isa_reg_display();
  } else if (info_type == 'w') {
    // TODO: do it after finish watch point
    return 0;
  }
  return 0;
}

// x N EXPT
static int display_mem(char *args) {

  Log("Call display_mem(%p)", (void *)args);
  // 1. receive 2 parameters
  unsigned int lines;
  paddr_t start_addr;
  int ret = sscanf(args, "%u   %x", &lines, &start_addr);
  Assert(ret == 2, "Received unvaild parameters");
  Log("Run x %d 0x%x", lines, start_addr);
  if (!likely(in_pmem(start_addr))) {
    Log("address 0x%x is out of bound", start_addr);
    return 0;
  }
  // 2. display memory
  for (int i = 0; i < lines; ++i) {
    // display addr
    printf("0x%08x: ", start_addr);
    for (int k = 0; k < 1; ++k) {
      if (!likely(in_pmem(start_addr + k))) {
        break;
      }
      int val = paddr_read(start_addr + k, 1);
      printf("0x%02x ", val);
    }
    printf("\n");
    start_addr += 4;
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},

    /* TODO: Add more commands */
    {"si", "si [N] , Run N (default 1) steps", step_n},
    {"info", "info [r | w] , display register / display watch point",
     info_some},
    {"x", "x N EXPR , display memory range from [EXPR, EXPR + N]", display_mem},

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  } else {
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() { is_batch_mode = true; }

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD) {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
