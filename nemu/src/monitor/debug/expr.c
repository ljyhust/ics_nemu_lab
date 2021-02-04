#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>
#include <stdlib.h>
enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_NUM, TK_REG,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"==", TK_EQ},        // equal
  {"[0-9]+|0x[0-9a-f]+", TK_NUM},
  {"\\$e[a-d]x|\\$e(sp|bp|si|di)", TK_REG},
  
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  // init pattern buffer to re array by regcomp, thus regexec can be applied in string
  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char *str;
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
          case TK_NUM: 
          case '+': 
          case '-': 
          case '*': 
          case '/': 
          case '(': 
          case ')':
          case TK_REG: 
          case TK_EQ: tokens[nr_token].str = malloc((substr_len + 1) * sizeof(char));
                      strncpy(tokens[nr_token].str, substr_start, substr_len);
                      tokens[nr_token].str[substr_len] = '\0';
                      tokens[nr_token].type = rules[i].token_type;
                      nr_token++;
                      break;
          case TK_NOTYPE:break;
          //FIX
          default: printf("no match for the symbol %c\n", substr_start[0]); return false;
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

  // 表达式计算
  // 判断主运算符 ((2 + 9) * 3) + 4 * 9 
  // 1. 遇到 ( 需要找到对应的 ) 并跳过，如果遇到非括号内的 '+' 或 '-'  则返回
  // 2. 没有遇到，则返回第一个 * 或 /
  word_t evaluate(int p, int q, bool *flag);
  bool res_flag = true;
  word_t res_val = evaluate(0, nr_token - 1, &res_flag);
  if (!res_flag) {
    printf("error expr, please check!\n");
  }
  *success = true;
  return res_val;
  //int i = 0;
  //for(; i < nr_token; i++) {
  //  Token token = tokens[i];
  //  printf("token type %d, str %s\n", token.type, token.str);
  //}
  //return 0;
}

/**
 * 判断是否括号封闭表达式
 */
bool check_parentheses(int p, int q) {
  if (p >= q) {
    return false;
  }
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    // 遍历找括号
    int i = p + 1, j = q - 1;
    while(i < q && tokens[i].type != '(') {i++;}
    while(j > p && tokens[j].type != ')') {j--;}
    if (tokens[i].type == '(' && tokens[j].type == ')') {
      return check_parentheses(i, j);
    }
    return true;
    
  }
  return false;
}

/**
 * 查找主运算符的位置 
 */
int find_main_op(int p, int q) {
  // 从左到右找运算符，如果找到'+' 或 '-' 低的运算符，缓存起来，否则以最后一个运算符为准
  // 如果找到'('，则找最右边第一个 ')'，并再从 ')' 后面开始找运算符
  int op = p;
  while(p < q) {
    switch (tokens[p].type)
    {
      case '*':
      case '/':
        if (tokens[op].type == '+' || tokens[op].type == '-') {
          // no to do
        } else {
          op = p;
        }
        break;
      case '+':
      case '-':
        op = p;
      case '(': ;
        int i = p + 1;
        while(i <= q) {
          if (check_parentheses(p, i)) {
            p = i + 1;
            break;
          }
          i++;
        }
        break;
      default:
        break;
    }
    p++;
  }
  int type =tokens[op].type;
  if (type != '+' && type != '-' && type != '*' && type != '/') {
    return -1;
  }
  return op;
}

/**
 * 计算表达式的值
 */
word_t evaluate(int p, int q, bool *flag) {
  if (!*flag) {
    return -1;
  }
  if (p > q) {
    *flag = false;
    return -1;
  } else if(p == q) {
    if (tokens[p].type == TK_NUM) {
      // hex num
      if (strlen(tokens[p].str) > 2 && strncmp(tokens[p].str, "0x", 2) == 0) {
        return (word_t)strtoul(tokens[p].str, NULL, 16);
      }
      return (word_t)atoi(tokens[p].str);
    }
    if (tokens[p].type == TK_REG) {
      return isa_reg_str2val(&tokens[p].str[1], flag);
    }
    *flag = false;
    return -1;
  } else if(check_parentheses(p, q)) {
    return evaluate(p + 1, q - 1, flag);
  } else {
    // 判断主运算符
    int op = find_main_op(p, q);
    if (-1 == op) {
      *flag = false;
      return -1;
    }
    word_t left = evaluate(p, op - 1, flag);
    word_t right = evaluate(op + 1, q, flag);

    switch (tokens[op].type)
    {
    case '+':
      return left + right;
    case '-':
      return left - right;
    case '*':
      return left * right;
    case '/':
      return left / right;
    default:
      *flag = false;
      break;
    }
  }
  return -1;
}
