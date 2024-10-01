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

#include "debug.h"
#include "memory/vaddr.h" // NOTE: for dereference
#include <isa.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <readline/chardefs.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  /* TODO: Add more token types */
  NUMBER,
  HEX_NUMBER,
  REGISTER,
  DEREFERENCE,
  TK_NEQ,
  AND,
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},             // spaces
    {"\\+", '+'},                  // plus
    {"-", '-'},                    // subtraction
    {"\\*", '*'},                  // times
    {"/", '/'},                    // times
    {"\\(", '('},                  // (
    {"\\)", ')'},                  // )
    {"0x[0-9][0-9]*", HEX_NUMBER}, // hex numbers , 这个的位置要在NUMBER之前
    {"[0-9][0-9]*", NUMBER},       // numbers
    {"\\$[a-zA-Z][a-zA-Z0-9_]*", REGISTER}, // register
    /* {"\\*[0-9][0-9]*", DEREFERENCE},        // reference */
    {"==", TK_EQ},  // equal
    {"!=", TK_NEQ}, // not equal
    {"&&", AND},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;
// NOTE: here only support 32 tokens
/* static Token tokens[32] __attribute__((used)) = {}; */
static Token tokens[65535] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
          pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
            rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
        case TK_NOTYPE: // space
          break;
        case '+': {
          Log("detect %d th token -> +", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = '+';
          tk->str[0] = '+';
          tk->str[1] = '\0';
          break;
        }
        case '-': {
          Log("detect %d th token -> -", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = '-';
          tk->str[0] = '-';
          tk->str[1] = '\0';
          break;
        }
        case '*': {
          Log("detect %d th token -> *", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = '*';
          tk->str[0] = '*';
          tk->str[1] = '\0';
          break;
        }
        case '/': {
          Log("detect %d th token -> /", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = '/';
          tk->str[0] = '/';
          tk->str[1] = '\0';
          break;
        }
        case '(': {

          Log("detect %d th token -> (", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = '(';
          tk->str[0] = '(';
          tk->str[1] = '\0';
          break;
        }
        case ')': {

          Log("detect %d th token -> )", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = ')';
          tk->str[0] = ')';
          tk->str[1] = '\0';
          break;
        }
        case TK_EQ: {

          Token *tk = &tokens[nr_token++];
          tk->type = TK_EQ;
          memcpy(tk->str, substr_start, substr_len);
          tk->str[substr_len] = '\0';
          Log("detect %d th token -> %s", nr_token, tk->str);
          break;
        }
        case TK_NEQ: {

          Token *tk = &tokens[nr_token++];
          tk->type = TK_NEQ;
          memcpy(tk->str, substr_start, substr_len);
          tk->str[substr_len] = '\0';
          Log("detect %d th token -> %s", nr_token, tk->str);
          break;
        }
        case AND: {

          Token *tk = &tokens[nr_token++];
          tk->type = AND;
          memcpy(tk->str, substr_start, substr_len);
          tk->str[substr_len] = '\0';
          Log("detect %d th token -> %s", nr_token, tk->str);
          break;
        }
        // not able
        /* case DEREFERENCE: { */
        /**/
        /*   Token *tk = &tokens[nr_token++]; */
        /*   tk->type = DEREFERENCE; */
        /*   memcpy(tk->str, substr_start, substr_len); */
        /*   tk->str[substr_len] = '\0'; */
        /*   Log("detect %d th token -> %s", nr_token, tk->str); */
        /*   break; */
        /* } */
        case REGISTER: {

          Token *tk = &tokens[nr_token++];
          tk->type = REGISTER;
          memcpy(tk->str, substr_start, substr_len);
          tk->str[substr_len] = '\0';
          Log("detect %d th token -> %s", nr_token, tk->str);
          break;
        }
        case NUMBER: {
          Log("detect NUMBER");
          if (substr_len < 50) {
            Token *tk = &tokens[nr_token++];
            tk->type = NUMBER;
            // record str
            memcpy(tk->str, substr_start, substr_len);
            tk->str[substr_len] = '\0';
            Log("detect %d th token -> %s", nr_token, tk->str);
          } else {
            // devide it into several tokens
            // target = part1 * 10^N * 10^M * .. + part2 * 10^N1 * ... + ...
            // add token (
            int group_st = nr_token;
            Token *tk = &tokens[nr_token++];
            tk->type = '(';
            char *subsubstr_start = substr_start;
            // while until reach the end
            for (int i = position; i < position + substr_len; i += 50) {
              if (i != position) {
                // add token +
                Token *tk = &tokens[nr_token++];
                tk->type = '+';
              }
              // divde part i
              int subsubstr_len = MIN(i + 50, position + substr_len) - i;
              Token *tk = &tokens[nr_token++];
              tk->type = NUMBER;
              if (subsubstr_len < 50) { // last one
                memcpy(tk->str, subsubstr_start, subsubstr_len);
                tk->str[subsubstr_len] = '\0';

              } else { // not last one
                memcpy(tk->str, subsubstr_start, 50);
                // divde 10^N MIN.
                int dump = i + 50;
                while (dump < position + substr_len) {
                  // add token  *
                  Token *tk = &tokens[nr_token++];
                  tk->type = '*';

                  // calc len
                  int len = MIN(dump + 50, position + substr_len) - dump;

                  // add token 100000
                  tk = &tokens[nr_token++];
                  tk->type = NUMBER;
                  tk->str[0] = '1';
                  for (int i = 1; i < len; ++i)
                    tk->str[i] = '0';
                  tk->str[len] = '\0';

                  dump += 50;
                }
              }
            }
            // add token (
            tk = &tokens[nr_token++];
            tk->type = ')';
            int group_ed = nr_token;
            // display
            for (int i = group_st; i < group_ed; ++i) {
              printf("%s", tokens[i].str);
            }
          }
          break;
        }
        case HEX_NUMBER: {
          Log("detect HEX_NUMBER");
          if (substr_len < 50) {
            Token *tk = &tokens[nr_token++];
            tk->type = HEX_NUMBER;
            // record str
            memcpy(tk->str, substr_start, substr_len);
            tk->str[substr_len] = '\0';
            Log("detect %d th token -> %s", nr_token, tk->str);
          } else {
            Assert(0, "Hex number longer than 50 not support yet");
          }
          break;
        }
        default:
          TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

// 左右闭区间
// 0 符合 ( < expr > )
// - 不符合 < expr > 而且不是一个正确的表达式 ，如：(4 + 3)) * ((2 - 1)
// 1 不符合 < expr > 但是一个正确的表达式，如：(4 + 3) * (2 - 1)
#define EX
int check_parentheses(int p, int q) {
  int count = 0;
  bool correctd_format = (tokens[p].type == '(' && tokens[q].type == ')');
  for (int i = p; i <= q; ++i) {
    if (tokens[i].type == '(') {
      count++;
    } else if (tokens[i].type == ')') {
      count--;
    }
    if (count == 0 && i != q) {
      correctd_format = false;
    }
  }
  if (count != 0) // 不正确的表达式
    return -1;
  if (correctd_format) // 符合 ( < expr > ), 是一个正确的表达式
    return 0;
  else // 不符合 < expr > 但是一个正确的表达式
    return 1;
}

#define MAX_OP 300
// Helper function to check if a token is an operator
bool is_operator(int type) {
  return type == '+' || type == '-' || type == '*' || type == '/' ||
         type == TK_EQ || type == TK_NEQ || type == AND;
}
// Helper function to get the priority of an operator
int get_operator_priority(int type) {
  switch (type) {
  case DEREFERENCE:
    return 0;
  case '+':
  case '-':
    return 1;
  case '*':
  case '/':
    return 2;
  case TK_EQ:
  case TK_NEQ:
    return 3;
  case AND:
    return 4;
  default:
    return MAX_OP;
  }
}
// find out main operator
// return the position of 主运算符 in the token expression;
int find_main_op(int p, int q) {
  // 1. 非运算符的token不是主运算符.
  // 2. 出现在一对括号中的token不是主运算符.
  // 3. 注意到这里不会出现有括号包围整个表达式的情况
  // 4. 主运算符的优先级在表达式中是最低的.
  // 这是因为主运算符是最后一步才进行的运算符.
  // 5. 当有多个运算符的优先级都是最低时, 根据结合性,
  // 最后被结合的运算符才是主运算符.
  int main_op = -1;
  int min_priority = MAX_OP; // ascall range < 300
  int bracket_count = 0;

  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      bracket_count++;
    } else if (tokens[i].type == ')') {
      bracket_count--;
    } else if (bracket_count == 0 && is_operator(tokens[i].type)) {
      int priority = get_operator_priority(tokens[i].type);
      if (priority <= min_priority) {
        min_priority = priority;
        main_op = i;
      }
    }
  }

  return main_op;
}
// malloc a global str
char message[64435];
char *tokens_str(int p, int q) {
  int len = 0;
  for (int i = p; i <= q; ++i) {
    len += sprintf(message + len, "%s", tokens[i].str);
  }
  message[len] = '\0';
  return message;
}
// 左右闭区间
uint32_t eval(int p, int q) {
  Log("Given p = %d, q = %d , eval %s", p, q, tokens_str(p, q));
  if (p > q) {
    Assert(p > q, "Error eval: p > q");
    return 0;
  } else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    // 从数值类型的角度来说，似乎不存在一个数位数会大于50
    // 实验规定数值是整数类型
    if (tokens[p].type == NUMBER) {
      return (uint32_t)atoi(tokens[p].str);
    } else if (tokens[p].type == HEX_NUMBER) {
      char *endptr;
      uint32_t val = (uint32_t)strtol(tokens[p].str, &endptr, 16);
      Assert(*endptr == '\0',
             "Something wrong may happen while call strtol() to convert hex "
             "number %s",
             tokens[p].str);
      return val;
    } else if (tokens[p].type == REGISTER) {
      bool success = true;
      uint32_t val = isa_reg_str2val(tokens[p].str, &success);
      Assert(success, "Error eval: invalid register name %s, p = %d, q = %d",
             tokens[p].str, p, q);
      return val;
    } else {
      Assert(0, "Error eval: p == q");
    }
  }
  int state = check_parentheses(p, q);
  Assert(state != -1, "Found an unvaild expr");
  if (state == 0) { // 符合 ( <expr> )
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  } else {
    // DEBUG:
    /* printf("p = %d, q = %d , %s\n", p, q, tokens_str(p, q)); */
    /* We should do more things here. */
    int op = find_main_op(p, q);
    // DEREFERENCE 这个操作符的优先级是最低的
    // 如果这个操作符是主运算符，那么就需要特殊处理
    if (tokens[op].type == DEREFERENCE) {
      return vaddr_read(eval(op + 1, q), 4);
    }
    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);
    switch (tokens[op].type) {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2;
    case '*':
      return val1 * val2;
    case '/':
      return val1 / val2;
    case TK_EQ:
      return val1 == val2;
    case TK_NEQ:
      return val1 != val2;
    case AND:
      return val1 && val2;

    default:
      Log("Unknow op : %c", tokens[op].type);
      assert(0);
    }
  }
}
word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  /* TODO: Insert codes to evaluate the expression. */

  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '(' ||
                                  is_operator(tokens[i - 1].type)))
      tokens[i].type = DEREFERENCE;
  }

  int p = 0, q = nr_token - 1;
  return eval(p, q);
}
