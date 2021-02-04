#include <monitor/difftest.h>

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);
void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr);

static inline def_EHelper(lidt) {
  // 设置idtr寄存器
  rtl_lm(s, s0, dsrc1, 0, 2);
  rtl_lm(s, s1, dsrc1, 2, 4);
  cpu.idtr_limit = *s0;
  cpu.idtr_base = *s1;
  print_asm_template1(lidt);
}

static inline def_EHelper(mov_r2cr) {
  TODO();
  print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {
  TODO();
  print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(int) {
  Assert(*ddest < cpu.idtr_limit, "ivalid no");
  raise_intr(s, *ddest, s->seq_pc);
  print_asm("int %s", id_dest->str);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_dut(1, 2);
#endif
}

static inline def_EHelper(iret) {
  rtl_pop(s, &s->seq_pc);
  rtl_pop(s, &cpu.cs);
  rtl_pop(s, &cpu.eflags);
  //printf("iret cte: eip=%x, cs=%x, eflags=%x\n", s->seq_pc, cpu.cs, cpu.eflags);
  print_asm("iret");

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(in) {
  switch (id_dest->width) {
  case 1:
    *s0 = pio_read_b(*dsrc1);
    break;
  case 2: 
    *s0 = pio_read_w(*dsrc1);
    break;
  case 4: 
    *s0 = pio_read_l(*dsrc1);
    break;
  default:
    break;
  }
  operand_write(s, id_dest, s0);
  *s0 = 0;
  // if (id_dest->width == 1) {
  //   reg_b(R_AL) = (uint8_t) src;
  // }
  // if (id_dest->width == 4) {
  //   *ddest = src;
  // }
  print_asm_template2(in);
}

static inline def_EHelper(out) {
  if (id_src1->width == 1) {
    pio_write_b(*ddest, *dsrc1);
  }
  if (id_src1->width == 2) {
    pio_write_w(*ddest, *dsrc1);
  }
  if (id_src1->width == 4) {
    pio_write_l(*ddest, *dsrc1);
  }
  print_asm_template2(out);
}
