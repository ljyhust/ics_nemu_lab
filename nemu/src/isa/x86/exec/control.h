#include "cc.h"

static inline def_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(s, *ddest);

  print_asm("jmp %x", s->jmp_pc);
}

static inline def_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, s0, cc);
  rtl_jrelop(s, RELOP_NE, s0, rz, *ddest);
  //printf("  cpu.eflags,cpu.pc=%x, OF=%d, CF=%d, ZF=%d, SF=%d\n", cpu.pc, cpu.OF, cpu.CF, cpu.ZF, cpu.SF);
  print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
}

static inline def_EHelper(jmp_rm) {
  rtl_jr(s, ddest);

  print_asm("jmp *%s", id_dest->str);
}

static inline def_EHelper(call) {
  // the target address is calculated at the decode stage
  //rtl_j(s, s->jmp_pc);
  rtl_j(s, *ddest);
  // 写寄存器，%esp = s->seq_pc 
  rtl_push(s, &s->seq_pc);
  // TODO
  print_asm("call %x", s->jmp_pc);
}

static inline def_EHelper(ret) {
  // 取出后放哪里？
  // rtl_pop(s, &s->seq_pc);
  //TODO();
  rtl_pop(s, &s->seq_pc);
  print_asm("ret");
}

static inline def_EHelper(ret_imm) {
  TODO();
  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm) {
  TODO();
  print_asm("call *%s", id_dest->str);
}
