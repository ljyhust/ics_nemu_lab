#include <stdio.h>
#include <stdint.h>
#include <NDL.h>
// extern int NDL_Init(uint32_t flags);
// extern void NDL_Quit();
// extern uint32_t NDL_GetTicks();

int main() {
    NDL_Init(0);
  uint32_t start = NDL_GetTicks() / 1000;
  uint32_t now = NDL_GetTicks() / 1000;

  while(now - start < 1) {
      now = NDL_GetTicks() / 1000;
  }
  int i = now - start;
  while (1)
  {
      now = NDL_GetTicks() / 1000;
      if (now > i) {
          printf("time is %d\n", now);
          i++;
      }
  }
  NDL_Quit();
  return 0;
}