#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

/* 50 ~ 57 push inst */
static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  //TODO();
  cpu.esp -= 4;
  vaddr_write(cpu.esp, *src1, 4);
  #ifdef DEBUG
    //printf("====> push src=%x, pc=%x \n", *src1, cpu.pc);
  #endif
}

// cpu.esp 表示esp值，&cpu.esp表示寄存器
static inline def_rtl(push_i, word_t imm) {
  cpu.esp -= 4;
  vaddr_write(cpu.esp, imm, 4);
}

static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  // des 一般是 &cpu.pc
  *dest = vaddr_read(cpu.esp, 4);
  #ifdef DEBUG
    //printf("====> pop dest=%x cpu.pc=%x\n", *dest, cpu.pc);
  #endif
  cpu.esp = cpu.esp + 4;
  //TODO();
}

/* just check not to set flags */
static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  // OF 只针对有符号
  if ((*src1 < 0) != (*src2 < 0) && (*res < 0) == (*src2 < 0)) {
    //printf("====> is_overflow ddest=%x, drsc1=%x, pc=%x\n", *ddest, *dsrc1, cpu.pc);
    *dest = 1;
  } else {
    *dest = 0;
  }
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
  if (*src1 < *src2) {
    *dest = 1;
  } else {
    *dest = 0;
  }
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  if ((*src1 < 0) == (*src2 < 0) && (*res < 0) != (*src1 < 0)) {
    *dest = 1;
  } else {
    *dest = 0;
  }
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  if (*res < *src1) {
    *dest = 1;
  } else {
    *dest=0;
  }
  
}

#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    cpu.f = *src; \
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    *dest = cpu.f; \
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  int i;
  word_t temp = 0xff;
  for (i = 0; i < width; i++) {
    temp = temp | (0xff << i * 8);
  }
  if ((temp & *result) == 0) {
    cpu.ZF = 1;
  } else {
    cpu.ZF = 0;
  }
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  cpu.SF = (*result >> (width * 8 - 1)) & 0x1;
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
