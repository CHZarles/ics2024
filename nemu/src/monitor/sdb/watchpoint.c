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
#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
// new_wp()从free_链表中返回一个空闲的监视点结构
WP *new_wp() {
  if (free_ == NULL) {
    printf("No enough watchpoint!\n");
    assert(0);
  }
  WP *tmp = free_;
  free_ = free_->next;
  tmp->next = head;
  if (head == NULL) {
    tmp->NO = 0;
  } else {
    tmp->NO = head->NO + 1;
  }
  head = tmp;
  return tmp;
}
// free_wp()将一个监视点结构放回free_链表
void free_wp(WP *wp) {
  WP *p = head;
  if (p == wp) {
    head = head->next;
    wp->next = free_;
    free_ = wp;
    return;
  }
  while (p->next != NULL) {
    if (p->next == wp) {
      p->next = wp->next;
      wp->next = free_;
      free_ = wp;
      return;
    }
    p = p->next;
  }
  printf("No such watchpoint!\n");
  assert(0);
}

// helper function
// delte a watchpoint by its NO
void free_wp_bynum(int wp_num) {
  WP *wp = head;
  while (wp != NULL) {
    if (wp->NO == wp_num) {
      free_wp(wp);
      return;
    }
    wp = wp->next;
  }
  printf("No such watchpoint %d\n", wp_num);
}

// display wathpoint
void display_wp() {
  WP *wp = head;
  if (wp == NULL) {
    printf("No watchpoint!\n");
    return;
  }
  printf("%-5s%-20s%-20s\n", "NO", "EXPR", "VALUE");
  while (wp != NULL) {
    printf("%-5d%-20s%-20lu\n", wp->NO, wp->expr, wp->last_value);
    wp = wp->next;
  }
}

// chedk_wp()遍历监视点链表，检查每个监视点的表达式的值是否发生了变化
void check_wp(bool *something_changed) {
  WP *p = head;
  while (p != NULL) {
    bool success = true;
    uint64_t new_val = expr(p->expr, &success);
    Assert(success, "Fail to calculate the value of watchpoint %d\n", p->NO);
    if (new_val != p->last_value) {
      printf("Hit watchpoint %d at %s\n", p->NO, p->expr);
      printf("Old value = %lu\n", p->last_value);
      printf("New value = %lu\n", new_val);
      p->last_value = new_val;
      *something_changed = true;
    }
    p = p->next;
  }
}
