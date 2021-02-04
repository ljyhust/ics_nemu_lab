#include <common.h>
#include "syscall.h"
#include "fs.h"

void sys_exit(Context *c);
void sys_write(Context *c);
void sys_brk(Context *c);
void sys_gettimeofday(Context *c);
void sys_read(Context *c);
void sys_open(Context *c);
void sys_sleek(Context *c);
extern void* mm_sbrk(uintptr_t increment);
extern size_t fs_lseek(int fd, size_t offset, int whence);
extern int fs_close(int fd);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  //printf("====> sys_n=%d, params=%x, %x, %x\n", a[0], c->GPR2, c->GPR3, c->GPR4);
  switch (a[0]) {
    case SYS_yield: yield(); c->GPRx = 0; break;
    case SYS_exit: sys_exit(c); break;
    case SYS_write: sys_write(c); break;
    case SYS_brk: sys_brk(c); break;
    case SYS_gettimeofday: sys_gettimeofday(c); break;
    case SYS_read: sys_read(c); break;
    case SYS_open: sys_open(c); break;
    case SYS_lseek: sys_sleek(c); break;
    case SYS_close: c->GPRx = fs_close(c->GPR2); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

void sys_exit(Context *c) {
  //printf("params %x, %x, %x\n", c->GPR2, c->GPR3, c->GPR4);
  halt(c->GPR2);
}

void sys_write(Context *c) {
  //assert(c->GPR2 == 1 || c->GPR2 == 2);
  char* buff = (char *)(void *)c->GPR3;
  int len = c->GPR4;
  printf("====> sys_n=%d, params=%x, %x, %x\n", c->GPR1, c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = fs_write(c->GPR2, buff, len);
}

void sys_brk(Context *c) {
  size_t incr = c->GPR2;
  c->GPRx = (uintptr_t)mm_sbrk(incr);
}

void sys_gettimeofday(Context *c) {
  //printf("get timer \n");
  uint64_t uptime = 0;
  ioe_read(AM_TIMER_UPTIME, &uptime);
  __time_t seconds = (__time_t)(uptime / 1000000);
  __suseconds_t suseconds = (__suseconds_t)(uptime - seconds * 1000000);
  void* tv_start = (void *)c->GPR2;
  *((__time_t *)tv_start) = seconds;
  *((__suseconds_t *)(tv_start + sizeof(__time_t))) = suseconds;
  c->GPRx = 0;
}

void sys_read(Context *c) {
  c->GPRx = fs_read(c->GPR2, (void *)c->GPR3, (size_t) c->GPR4);
}

void sys_open(Context *c) {
  char* path = (char *)c->GPR2;
  c->GPRx = fs_open(path, (int)c->GPR3, (int)c->GPR4);
}

void sys_sleek(Context *c) {
  c->GPRx = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
}