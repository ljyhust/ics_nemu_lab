#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char *expr_str;
  /* TODO: Add more members if necessary */
  word_t val;
} WP;

WP* new_wp();
void free_wp(WP *last_wp, WP *wp);
void free_wp_no(int no);
bool eval_wp();
void print_watchpoints();
#endif
