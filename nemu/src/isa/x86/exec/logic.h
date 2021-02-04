#include "cc.h"

static inline def_EHelper(test) {
  rtl_and(s, s2, ddest, dsrc1);
  rtl_update_ZFSF(s, s2, id_dest->width);
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  //Operand opr_src1 = s->src1;
  //Operand opr_dest = s->dest;
  // r/m32 AND imm8 (sign-extended).
  /* if (s->opcode == 0x83) {
    rtl_sext(s, dsrc1, dsrc1, 1);
  } */
  // if (s->opcode == 0x80) {
  //   printf("ddest=%x, dsrc1=%x, width=%d\n", *ddest, *dsrc1, id_dest->width);
  // }
  rtl_and(s, s0, ddest, dsrc1);
  rtl_update_ZFSF(s, s0, id_dest->width);
  operand_write(s, id_dest, s0);
  print_asm_template2(and);
}

static inline def_EHelper(xor) {
  Operand opr_src = s->src1;
  Operand opr_dest = s->dest;
  rtl_xor(s, s0, opr_dest.preg, opr_src.preg);
  rtl_update_ZFSF(s, s0, id_dest->width);
  operand_write(s, id_dest, s0);
  print_asm_template2(xor);
}

static inline def_EHelper(or) {
  rtl_or(s, s0, ddest, dsrc1);
  rtl_update_ZFSF(s, s0, id_dest->width);
  operand_write(s, id_dest, s0);
  print_asm_template2(or);
}

static inline def_EHelper(not) {
  rtl_not(s, ddest, ddest);
  operand_write(s, id_dest, ddest);
  print_asm_template1(not);
}

static inline def_EHelper(sar) {
  rtl_sar(s, ddest, ddest, dsrc1);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s, id_dest, ddest);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  rtl_shl(s, ddest, ddest, dsrc1);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s, id_dest, ddest);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  rtl_shr(s, ddest, ddest, dsrc1);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s, id_dest, ddest);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shr);
}

static inline def_EHelper(rol) {
  int width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_addi(s, s0, rz, width);
  rtl_andi(s, s1, dsrc1, 0x1f);
  rtl_div_r(s, s2, s1, s0);
  while(*s2 != 0) {
    rtl_msb(s, s0, ddest, width);
    *ddest = *ddest * 2 + *s0;
    *s2 = *s2 - 1;
  }

  if (*s1 != 0) {
    rtl_lsb(s, s0, ddest, width);
    cpu.CF = *s0;
  } else {
    cpu.CF = 0;
  }

  if (*s1 == 1) {
    rtl_msb(s, s0, ddest, width);
    rtl_xori(s, s0, s0, cpu.CF);
    cpu.OF = *s0;
  } else {
    cpu.OF = 0;
  }
  //todo
  //rtl_rol(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(rol);
}

static inline def_EHelper(ror) {
  int width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_addi(s, s0, rz, width);
  rtl_andi(s, s1, dsrc1, 0x1f);
  rtl_div_r(s, s2, s1, s0);
  while(*s2 != 0) {
    rtl_lsb(s, s0, ddest, width);
    *ddest = *ddest / 2 + (*s0 << width);
    *s2 = *s2 - 1;
  }

  if (*s1 != 0) {
    rtl_msb(s, s0, ddest, width);
    cpu.CF = *s0;
  } else {
    cpu.CF = 0;
  }

  if (*s1 == 1) {
    rtl_msb(s, s0, ddest, width);
    rtl_sbit(s, s2, ddest, width * 8 - 1, width);
    rtl_xor(s, s0, s0, s2);
    cpu.OF = *s0;
  } else {
    cpu.OF = 0;
  }
  //todo
  //rtl_rol(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(rol);
}

static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}
