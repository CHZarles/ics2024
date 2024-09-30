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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format = "#include <stdio.h>\n"
                           "int main() { "
                           "  unsigned result = 1u*(%s); "
                           "  printf(\"%%u\", result); "
                           "  return 0; "
                           "}";

// a variable record the length of buf
unsigned int buf_len = 0;
// a variable record unmatch parentheses
unsigned int wait_to_fill = 0;
unsigned int stack_count = 0;
#define BUF_MAX_SIZE 65536u
int choose(int n) { return rand() % n; }
// TODO: generate number greater than 9 (but must be represented by 32bit)
// NOTE: this code don't generate 0, fix it later
static void gen_num() {
  int n = choose(10);
  if (n == 0)
    n += 1;
  buf[buf_len++] = '0' + n;
}

static void gen_char(char c) { buf[buf_len++] = c; }

static void gen_rand_op() {
  int idx = choose(4);
  char ops[4] = {'+', '-', '*', '/'};
  buf[buf_len++] = ops[idx];
}
//
/* 如何保证表达式进行无符号运算? */
/* 如何随机插入空格? */
/* 如何生成长表达式, 同时不会使buf溢出? */
/* 如何过滤求值过程中有除0行为的表达式? */
//
static void gen_rand_expr() {

  stack_count += 1;
  if (stack_count > 1000 || buf_len + wait_to_fill == BUF_MAX_SIZE) {
    // defend 如何生成长表达式, 同时不会使buf溢出?
    // 在预见会出问题的时候直接生成一位数返回
    buf[buf_len++] = choose(8) + '1';
    return;
  }
  if (buf_len == BUF_MAX_SIZE) {
    return;
  }
  switch (choose(3)) {
  case 0:
    gen_num();
    break;
  case 1:
    gen_char('(');
    wait_to_fill += 1;
    gen_rand_expr();
    gen_char(')');
    break;
  default:
    wait_to_fill += 2;
    gen_rand_expr();
    gen_rand_op();
    gen_rand_expr();
    break;
  }

  stack_count -= 1;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++) {
    // reset buf
    buf_len = 0;
    wait_to_fill = 0;
    gen_rand_expr();
    // don't forget this
    buf[buf_len] = '\0';
    // display buf 's length
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);
    // NOTE: add a -Werror flag to make sure the generated code is correct
    int ret = system("gcc -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0)
      continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
