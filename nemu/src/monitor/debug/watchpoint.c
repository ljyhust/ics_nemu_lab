#include "watchpoint.h"
#include "expr.h"
#include <stdlib.h>

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/* create new wp in head */
WP* new_wp() {
  if (NULL == free_) {
    assert(0);
  }
  WP *res = free_;
  free_ = free_->next;
  res->next = head;
  head = res;
  return res;
}

/* sigle linked list free */
void free_wp(WP *last_wp, WP *wp) {
  if (NULL == last_wp) {
    head = wp->next;
  } else {
    last_wp->next = wp->next;
  }
  wp->next = free_;
  free_ = wp;
  wp->val = 0;
  if (NULL != wp->expr_str) {
    free(wp->expr_str);
  }
}

void free_wp_no(int no) {
  if (no < 0 || no >= NR_WP) {
    printf("invalid watchpoint num.");
    return;
  }
  WP *last = NULL;
  WP *point = head;
  while (NULL != point) {
    if (point->NO == no) {
      free_wp(last, point);
      return;
    }
    last = point;
    point = point->next;
  }
  
}

/* eval all watchpoint and check stop */
bool eval_wp() {
  WP *point = head;
  while (NULL != point) {
    bool eval_flag = true;
    Log("eval point %d, %s", point->NO, point->expr_str);
    word_t new_val = expr(point->expr_str, &eval_flag);
    if (eval_flag && new_val != point->val) {
      printf("watchpoint %d: %s\n", point->NO, point->expr_str);
      printf("Old value %d\nNew Value %d\n", point->val, new_val);
      point->val = new_val;
      return true;
    }
    point = point->next;
  }
  return false;
}

/* print all watchpoints */
void print_watchpoints() {
  if (NULL == head) {
    printf("NO watchpoints\n");
    return;
  }
  WP *point = head;
  printf("%-16s\t%-16s\t%-32s\n", "Num", "Type", "What");
  while (NULL != point) {
    printf("%-16d\t%-16s\t%-32s\n", point->NO, "watchpoint", point->expr_str);
    point = point->next;
  }
}