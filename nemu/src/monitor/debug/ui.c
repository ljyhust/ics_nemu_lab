#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "memory/vaddr.h"
#include "memory/paddr.h"

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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
  // param is uint64_t  -1 means max - 1 there max address
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  if (args == NULL) {
    cpu_exec(1);
    return 0;
  }
  if (strlen(args) > 1) {
    printf("Params only one number, exit NEMU and run again.\n");
    return -1;
  }
  uint64_t si_step = (uint64_t) atoi(args);
  cpu_exec(si_step);
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) {
    return 0;
  }

  if (strlen(args) > 1) {
    printf("Please select info [r|w]. \n");
    return 0;
  }
  if (strcmp(args, "r") == 0) {
    isa_reg_display();
  }
  if (strcmp(args, "w") == 0) {
    print_watchpoints();
  }
  return 0;
}

/**
 * x [N EXPR]
 */ 
static int cmd_x(char *args) {
  if (NULL == args) {
    printf("Invalid arguments!\n");
    return 0;
  }
  // strtok get N argument
  char *step_n = strtok(args, " ");
  int n = atoi(step_n);
  
  // strtok get EXPR
  char *expr_s = strtok(NULL, "");
  // computer the value
  bool res_flag = false;
  word_t expr_val = expr(expr_s, &res_flag);
  if (!res_flag) {
    printf("error expr.\n");
    return 0;
  }
  paddr_t paddr_val = (paddr_t)expr_val;
  int i;
  // print paddr
  for (i = 0; i < n; i++) {
    word_t val = vaddr_read(paddr_val + i * 4, 4);
    printf("0x%08x\t", val);
  }
  printf("\n");
  return 0;
}
static int cmd_expr(char *args) {
  bool res = false;
  word_t val = expr(args, &res);
  printf("expr success is %d, and result = %d\n", res, val);
  return 0;
}

/* w [expr]  args = [expr] */
static int cmd_w(char *args) {
  if (NULL == args) {
    return 0;
  }
  bool res = false;
  word_t val = expr(args, &res);
  if (!res) {
    printf("w expr error %s \n", args);
    return 0 ;
  }
  WP *wp = new_wp();
  // args 指针位置在运行时内容变了，wp->expr_str不再是当初的内容
  //wp->expr_str = args; //error
  wp->expr_str = (char *)malloc(strlen(args));
  strcpy(wp->expr_str, args);
  wp->val = val;
  printf("watchpoint %d: %s\n", wp->NO, wp->expr_str);
  return 0;
}

static int cmd_d(char *args) {
  if (NULL == args) {
    printf("please enter the watchpoint num you want to delete.\n");
    return 0;
  }
  int n = atoi(args);
  free_wp_no(n);
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Continue the execution by [N] step", cmd_si},
  { "info", "info [r|w]", cmd_info },
  { "x", "scan memory value", cmd_x },
  { "p", "expr", cmd_expr},
  { "w", "watchpoint exp", cmd_w},
  { "d", "delete watchpoint", cmd_d },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
