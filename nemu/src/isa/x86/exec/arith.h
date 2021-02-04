#include "cc.h"

/* static inline 必须定义在h文件中其它文件才可以看到，在调用处内敛展开 */
static inline def_EHelper(add) {
  Operand opr_src1 = s->src1;
  //Operand opr_src2 = s->src2;
  Operand opr_dest = s->dest;
  // copy reg num to src2
  //rtl_lr(s, opr_src2.preg, opr_dest.reg, opr_dest.width);
  rtlreg_t src2 = *(opr_dest.preg);
  rtl_add(s, opr_dest.preg, opr_dest.preg, opr_src1.preg);
  rtl_is_add_carry(s, s0, opr_dest.preg, opr_src1.preg);
  rtl_set_CF(s, s0);
  rtl_is_add_overflow(s, s1, opr_dest.preg, opr_src1.preg, &src2, opr_dest.width);
  rtl_set_OF(s, s1);
  rtl_update_ZFSF(s, opr_dest.preg, opr_dest.width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(add);
}

static inline def_EHelper(sub) {
  Operand opr_src = s->src1;
  //Operand opr_src2 = s->src2;
  Operand opr_dest = s->dest;
  // 用val或imm ?
  //rtl_lr(s, opr_src2.preg, opr_dest.reg, opr_dest.width);
  rtlreg_t src2 = *opr_dest.preg;
  rtl_sub(s, opr_dest.preg, opr_dest.preg, opr_src.preg);
  rtl_is_sub_carry(s, s0, &src2, opr_src.preg);
  rtl_set_CF(s, s0);
  rtl_is_sub_overflow(s, s1, opr_dest.preg, &src2, opr_src.preg, opr_dest.width);
  rtl_set_OF(s, s1);
  rtl_update_ZFSF(s, opr_dest.preg, opr_dest.width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(sub);
}

static inline def_EHelper(cmp) {
  //TODO();
  //printf("====> ddest=%x, drsc1=%x, pc=%x\n", *ddest, *dsrc1, cpu.pc);
  //rtl_sub(s, s2, dsrc1, ddest);
  rtl_sub(s, s2, ddest, dsrc1);
  rtl_is_sub_carry(s, s0, ddest, dsrc1);
  rtl_set_CF(s, s0);
  rtl_is_sub_overflow(s, s1, s2, ddest, dsrc1, id_dest->width);
  rtl_set_OF(s, s1);
  rtl_update_ZFSF(s, s2, id_dest->width);
  //printf("  cpu.eflags,cpu.pc=%x, OF=%d, CF=%d, ZF=%d, SF=%d\n", cpu.pc, cpu.OF, cpu.CF, cpu.ZF, cpu.SF);
  print_asm_template2(cmp);
}

static inline def_EHelper(inc) {
  rtlreg_t src2 = 0x1;
  rtlreg_t src1 = *ddest;
  rtl_add(s, ddest, &src1, &src2);
  rtl_is_add_overflow(s, s1, ddest, &src1, &src2, id_dest->width);
  rtl_set_OF(s, s1);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s, id_dest, ddest);
  print_asm_template1(inc);
}

static inline def_EHelper(dec) {
  rtlreg_t src2 = 0x1;
  rtlreg_t src1 = *ddest;
  rtl_sub(s, ddest, &src1, &src2);
  rtl_is_sub_overflow(s, s1, ddest, &src1, &src2, id_dest->width);
  rtl_set_OF(s, s1);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s, id_dest, ddest);
  print_asm_template1(inc);
  print_asm_template1(dec);
}

static inline def_EHelper(neg) {
  if (*ddest == 0) {
    cpu.CF = 0;
  } else {
    cpu.CF = 1;
  }
  *s0 = 0;
  *s1 = *ddest;
  rtl_neg(s, ddest, s1);
  rtl_is_sub_overflow(s, s2, ddest, s0, s1, id_dest->width);
  rtl_set_OF(s, s2);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  print_asm_template1(neg);
}

static inline def_EHelper(adc) {
  rtl_get_CF(s, s0);
  rtl_add(s, s0, dsrc1, s0);
  rtl_add(s, s1, ddest, s0);
  rtl_update_ZFSF(s, s1, id_dest->width);
  rtl_is_add_overflow(s, s2, s1, ddest, dsrc1, id_dest->width);
  rtl_set_OF(s, s2);
  if (id_dest->width != 4) {
    rtl_andi(s, s1, s1, 0xffffffffu >> ((4 - id_dest->width) * 8));
  }
  rtl_is_add_carry(s, s2, s1, s0);
  rtl_is_add_carry(s, s0, s0, dsrc1);
  rtl_or(s, s0, s0, s2);
  rtl_set_CF(s, s0);
  operand_write(s, id_dest, s1);
  print_asm_template2(adc);
}

static inline def_EHelper(sbb) {
  rtl_get_CF(s, s0);
  rtl_add(s, s0, dsrc1, s0);
  rtl_sub(s, s1, ddest, s0);
  rtl_update_ZFSF(s, s1, id_dest->width);
  rtl_is_sub_overflow(s, s2, s1, ddest, dsrc1, id_dest->width);
  rtl_set_OF(s, s2);
  rtl_is_add_carry(s, s2, s0, dsrc1);
  rtl_is_sub_carry(s, s0, ddest, s0);
  rtl_or(s, s0, s0, s2);
  rtl_set_CF(s, s0);
  operand_write(s, id_dest, s1);
  print_asm_template2(sbb);
}

static inline def_EHelper(mul) {
  rtl_lr(s, s0, R_EAX, id_dest->width);
  rtl_mul_lo(s, s1, ddest, s0);

  switch (id_dest->width) {
    case 1:
      rtl_sr(s, R_AX, s1, 2);
      break;
    case 2:
      rtl_sr(s, R_AX, s1, 2);
      rtl_shri(s, s1, s1, 16);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      rtl_mul_hi(s, s0, ddest, s0);
      rtl_sr(s, R_EDX, s0, 4);
      rtl_sr(s, R_EAX, s1, 4);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
static inline def_EHelper(imul1) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_EAX, 1);
      rtl_imul_lo(s, s1, ddest, s0);
      rtl_sr(s, R_AX, s1, 2);
      break;
    case 2:
      rtl_lr(s, s0, R_EAX, 2);
      rtl_imul_lo(s, s1, ddest, s0);
      rtl_sr(s, R_AX, s1, 2);
      rtl_shri(s, s1, s1, 16);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ; rtlreg_t *pdest = ddest;
      if (ddest == &cpu.edx) {
        rtl_mv(s, s0, ddest);
        pdest = s0;
      }
      rtl_imul_hi(s, &cpu.edx, pdest, &cpu.eax);
      rtl_imul_lo(s, &cpu.eax, pdest, &cpu.eax);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
static inline def_EHelper(imul2) {
  rtl_sext(s, dsrc1, dsrc1, id_src1->width);
  rtl_sext(s, ddest, ddest, id_dest->width);

  rtl_imul_lo(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);

  print_asm_template2(imul);
}

// imul with three operands
static inline def_EHelper(imul3) {
  rtl_sext(s, dsrc1, dsrc1, id_src1->width);
  rtl_sext(s, dsrc2, dsrc2, id_src1->width);

  rtl_imul_lo(s, ddest, dsrc2, dsrc1);
  operand_write(s, id_dest, ddest);

  print_asm_template3(imul);
}

// no sign extend
static inline def_EHelper(imul2_ns) {
  rtl_imul_lo(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);
}

static inline def_EHelper(imul3_ns) {
  rtl_imul_lo(s, ddest, dsrc2, dsrc1);
  operand_write(s, id_dest, ddest);
}

static inline def_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_AX, 2);
      rtl_div_q(s, s1, s0, ddest);
      rtl_sr(s, R_AL, s1, 1);
      rtl_div_r(s, s1, s0, ddest);
      rtl_sr(s, R_AH, s1, 1);
      break;
    case 2:
      rtl_lr(s, s0, R_AX, 2);
      rtl_lr(s, s1, R_DX, 2);
      rtl_shli(s, s1, s1, 16);
      rtl_or(s, s0, s0, s1);
      rtl_div_q(s, s1, s0, ddest);
      rtl_sr(s, R_AX, s1, 2);
      rtl_div_r(s, s1, s0, ddest);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ; rtlreg_t *pdest = ddest;
      if (ddest == &cpu.eax) pdest = s0;
      rtl_mv(s, s0, &cpu.eax);
      rtl_div64_q(s, &cpu.eax, &cpu.edx, s0, pdest);
      rtl_div64_r(s, &cpu.edx, &cpu.edx, s0, pdest);
      break;
    default: assert(0);
  }

  print_asm_template1(div);
}

static inline def_EHelper(idiv) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_AX, 2);
      rtl_idiv_q(s, s1, s0, ddest);
      rtl_sr(s, R_AL, s1, 1);
      rtl_idiv_r(s, s1, s0, ddest);
      rtl_sr(s, R_AH, s1, 1);
      break;
    case 2:
      rtl_lr(s, s0, R_AX, 2);
      rtl_lr(s, s1, R_DX, 2);
      rtl_shli(s, s1, s1, 16);
      rtl_or(s, s0, s0, s1);
      rtl_idiv_q(s, s1, s0, ddest);
      rtl_sr(s, R_AX, s1, 2);
      rtl_idiv_r(s, s1, s0, ddest);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ; rtlreg_t *pdest = ddest;
      if (ddest == &cpu.eax) pdest = s0;
      rtl_mv(s, s0, &cpu.eax);
      rtl_idiv64_q(s, &cpu.eax, &cpu.edx, s0, pdest);
      rtl_idiv64_r(s, &cpu.edx, &cpu.edx, s0, pdest);
      break;
    default: assert(0);
  }

  print_asm_template1(idiv);
}
