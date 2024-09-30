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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  NUMBERS = 255,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},        // spaces
    {"\\+", '+'},             // plus
    {"\\-", '-'},             // subtraction
    {"\\*", '*'},             // times
    {"\\(", '('},             // (
    {"\\)", ')'},             // )
    {"[1-9][0-9]*", NUMBERS}, // numbers
    {"==", TK_EQ},            // equal
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

static Token tokens[32] __attribute__((used)) = {};
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
          break;
        }
        case '-': {
          Log("detect %d th token -> -", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = '-';
          break;
        }
        case '*': {
          Log("detect %d th token -> *", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = '*';
          break;
        }
        case '/': {
          Log("detect %d th token -> //", nr_token);
          Token *tk = &tokens[nr_token++];
          tk->type = '/';
          break;
        }
        case NUMBERS: {
          Log("detect %d th token -> NUMBERS(%.*s)", nr_token, substr_len,
              substr_start);
          if (substr_len < 50) {
            Token *tk = &tokens[nr_token++];
            tk->type = NUMBERS;
            // record str
            memcpy(tk->str, substr_start, substr_len);
          } else {
            // devide it into several tokens
            // target = part1 * 10^N * 10^M * .. + part2 * 10^N1 * ... + ...

            Token *tk = &tokens[nr_token++];
            tk->type = '(';
            char *subsubstr_start = substr_start;
            // while until reach the end
            for (int i = position; i < position + substr_len; i += 50) {
              if (i != position) {

                Token *tk = &tokens[nr_token++];
                tk->type = '+';
              }
              // divde part i
              int subsubstr_len = MIN(i + 50, position + substr_len) - i;
              Token *tk = &tokens[nr_token++];
              tk->type = NUMBERS;
              if (subsubstr_len < 50) { // last one
                memcpy(tk->str, subsubstr_start, subsubstr_len);
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
                  tk->type = NUMBERS;
                  tk->str[0] = '1';
                  for (int i = 1; i < len; ++i)
                    tk->str[i] = '0';
                  tk->str[len] = '\0';

                  dump += 50;
                }
              }
            }
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

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
