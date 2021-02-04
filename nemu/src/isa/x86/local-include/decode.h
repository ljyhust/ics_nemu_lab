#include <cpu/exec.h>
#include "rtl.h"

void read_ModR_M(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val);

static inline void operand_reg(DecodeExecState *s, Operand *op, bool load_val, int r, int width) {
  op->type = OP_TYPE_REG;
  op->reg = r;

  if (width == 4) {
    op->preg = &reg_l(r);
  } else {
    assert(width == 1 || width == 2);
    // width=1 or then write the (reg[r] low bit num) to op.val, means load_val into the op.val
    op->preg = &op->val;
    if (load_val) rtl_lr(s, &op->val, r, width);
  }

  print_Dop(op->str, OP_STR_SIZE, "%%%s", reg_name(r, width));
}

static inline void operand_imm(DecodeExecState *s, Operand *op, bool load_val, word_t imm, int width) {
  op->type = OP_TYPE_IMM;
  op->imm = imm;
  if (load_val) {
    rtl_li(s, &op->val, imm);
    op->preg = &op->val;
  }
  print_Dop(op->str, OP_STR_SIZE, "$0x%x", imm);
}

// decode operand helper
#define def_DopHelper(name) void concat(decode_op_, name) (DecodeExecState *s, Operand *op, bool load_val)

/* Refer to Appendix A in i386 manual for the explanations of these abbreviations */

/* Ib, Iv */
static inline def_DopHelper(I) {
  /* pc here is pointing to the immediate */
  word_t imm = instr_fetch(&s->seq_pc, op->width);
  operand_imm(s, op, load_val, imm, op->width);
}

/* I386 manual does not contain this abbreviation, but it is different from
 * the one above from the view of implementation. So we use another helper
 * function to decode it.
 */
/* sign immediate */
static inline def_DopHelper(SI) {
  assert(op->width == 1 || op->width == 4);

  /* TODO: Use instr_fetch() to read `op->width' bytes of memory
   * pointed by 's->seq_pc'. Interpret the result as a signed immediate,
   * and call `operand_imm()` as following.
   *
   operand_imm(s, op, load_val, ???, op->width);
   */
  // 获取接下来的width宽的 操作数
  word_t imm = instr_fetch(&s->seq_pc, op->width);
  // imm write to op.imn
  if(op->width == 1) {
    rtl_sext(s, &imm, &imm, 1);
  }
  operand_imm(s, op, load_val, imm, op->width);
}

/* I386 manual does not contain this abbreviation.
 * It is convenient to merge them into a single helper function.
 */
/* AL/eAX */
static inline def_DopHelper(a) {
  operand_reg(s, op, load_val, R_EAX, op->width);
}

/* This helper function is use to decode register encoded in the opcode. */
/* XX: AL, AH, BL, BH, CL, CH, DL, DH
 * eXX: eAX, eCX, eDX, eBX, eSP, eBP, eSI, eDI
 * 从 opcode中拿到寄存器编号
 */
static inline def_DopHelper(r) {
  operand_reg(s, op, load_val, s->opcode & 0x7, op->width);
}

static inline def_DopHelper(off) {
  sword_t imm = instr_fetch(&s->seq_pc, op->width);
  if (1 == op->width) {
    imm = (int8_t) imm;
  }
  if (2 == op->width) {
    imm = (int16_t) imm;
  }
  op->preg = &op->val;
  rtl_addi(s, op->preg, &s->seq_pc, imm);
  //rtl_addi(s, &s->jmp_pc, &s->seq_pc, imm);
  //printf("====> jmpc_pc %x, seq_pc %x,  dest %x\n", s->jmp_pc, s->seq_pc, *ddest);
  //printf("now set jump seq_pc=%x, jmp_pc=%x, op->width=%x\n",s->seq_pc, s->jmp_pc, op->width);
  // 计算出来放个地方？dest
  // 执行时从那个地方加载出来执行
}

/* I386 manual does not contain this abbreviation.
 * We decode everything of modR/M byte in one time.
 */
/* Eb, Ew, Ev
 * Gb, Gv
 * Cd,
 * M
 * Rd
 * Sw
 */
static inline void operand_rm(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  read_ModR_M(s, rm, load_rm_val, reg, load_reg_val);
}

/* Ob, Ov */
static inline def_DopHelper(O) {
  op->type = OP_TYPE_MEM;
  s->isa.moff = instr_fetch(&s->seq_pc, 4);
  s->isa.mbase = rz;
  if (load_val) {
    rtl_lm(s, &op->val, s->isa.mbase, s->isa.moff, op->width);
    op->preg = &op->val;
  }

  print_Dop(op->str, OP_STR_SIZE, "0x%x", s->isa.moff);
}

/* Eb <- Gb
 * Ev <- Gv
 * r/m32(dest) <- r32
 */
static inline def_DHelper(G2E) {
  operand_rm(s, id_dest, true, id_src1, true);
}

static inline def_DHelper(mov_G2E) {
  operand_rm(s, id_dest, false, id_src1, true);
}

/* Gb <- Eb
 * Gv <- Ev
 * r32 <- r/m32
 * MM -> Reg
 */
static inline def_DHelper(E2G) {
  operand_rm(s, id_src1, true, id_dest, true);
}

static inline def_DHelper(mov_E2G) {
  operand_rm(s, id_src1, true, id_dest, false);
}

static inline def_DHelper(movb_E2G) {
  id_src1->width = 1;
  operand_rm(s, id_src1, true, id_dest, false);
}

static inline def_DHelper(movw_E2G) {
  id_src1->width = 2;
  operand_rm(s, id_src1, true, id_dest, false);
}

static inline def_DHelper(lea_M2G) {
  operand_rm(s, id_src1, false, id_dest, false);
}

/* AL <- Ib
 * eAX <- Iv
 */
static inline def_DHelper(I2a) {
  decode_op_a(s, id_dest, true);
  decode_op_I(s, id_src1, true);
}

/* Gv <- EvIb
 * Gv <- EvIv
 * use for imul */
static inline def_DHelper(I_E2G) {
  operand_rm(s, id_src2, true, id_dest, false);
  decode_op_I(s, id_src1, true);
}

/* Eb <- Ib
 * Ev <- Iv
 * I -> MM
 */
static inline def_DHelper(I2E) {
  operand_rm(s, id_dest, true, NULL, false);
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(mov_I2E) {
  operand_rm(s, id_dest, false, NULL, false);
  decode_op_I(s, id_src1, true);
}

/* XX <- Ib
 * eXX <- Iv
 */
static inline def_DHelper(I2r) {
  decode_op_r(s, id_dest, true);
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(mov_I2r) {
  decode_op_r(s, id_dest, false);
  decode_op_I(s, id_src1, true);
}

/* used by unary operations */
static inline def_DHelper(I) {
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(r) {
  decode_op_r(s, id_dest, true);
}

static inline def_DHelper(E) {
  operand_rm(s, id_dest, true, NULL, false);
}

static inline def_DHelper(setcc_E) {
  operand_rm(s, id_dest, false, NULL, false);
}

static inline def_DHelper(gp7_E) {
  operand_rm(s, id_dest, false, NULL, false);
}

/* used by test in group3 */
static inline def_DHelper(test_I) {
  decode_op_I(s, id_src1, true);
}

/* ib rimm16 或 rimm32 */
static inline def_DHelper(SI2E) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  // get rm 域
  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;
  decode_op_SI(s, id_src1, true);
  if (id_dest->width == 2) {
    *dsrc1 &= 0xffff;
  }
}

static inline def_DHelper(SI2E_4W) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;
  decode_op_SI(s, id_src1, true);
  if (id_dest->width == 4) {
    // sign for src1
    if (*dsrc1 >> 7) {
      *dsrc1 = (0xffffff >> 8) | *dsrc1;
    }
  }
}

static inline def_DHelper(SI_E2G) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  operand_rm(s, id_src2, true, id_dest, false);
  id_src1->width = 1;
  decode_op_SI(s, id_src1, true);
  if (id_dest->width == 2) {
    *dsrc1 &= 0xffff;
  }
}

static inline def_DHelper(gp2_1_E) {
  operand_rm(s, id_dest, true, NULL, false);
  operand_imm(s, id_src1, true, 1, 1);
}

static inline def_DHelper(gp2_cl2E) {
  operand_rm(s, id_dest, true, NULL, false);
  // shift instructions will eventually use the lower
  // 5 bits of %cl, therefore it is OK to load %ecx
  operand_reg(s, id_src1, true, R_ECX, 4);
}

static inline def_DHelper(gp2_Ib2E) {
  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}

/* Ev <- GvIb
 * use for shld/shrd */
static inline def_DHelper(Ib_G2E) {
  operand_rm(s, id_dest, true, id_src2, true);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}

/* Ev <- GvCL
 * use for shld/shrd */
static inline def_DHelper(cl_G2E) {
  operand_rm(s, id_dest, true, id_src2, true);
  // shift instructions will eventually use the lower
  // 5 bits of %cl, therefore it is OK to load %ecx
  operand_reg(s, id_src1, true, R_ECX, 4);
}

static inline def_DHelper(cwtl_sa) {
  id_src1->width = 2;
  operand_reg(s, id_src1, true, R_EAX, 2);
  operand_reg(s, id_dest, false, R_EAX, 4);
}

static inline def_DHelper(clcc_a2d) {
  operand_reg(s, id_src1, false, R_EAX, 4);
  operand_reg(s, id_dest, false, R_EDX, 4);
}

static inline def_DHelper(O2a) {
  decode_op_O(s, id_src1, true);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(a2O) {
  decode_op_a(s, id_src1, true);
  decode_op_O(s, id_dest, false);
}

static inline def_DHelper(J) {
  decode_op_SI(s, id_dest, false);
  // the target address can be computed in the decode stage
  s->jmp_pc = id_dest->simm + s->seq_pc;
}

static inline def_DHelper(push_r) {
   decode_op_r(s, id_src1, true);
}

static inline def_DHelper(push_SI) {
  decode_op_SI(s, id_src1, true);
}

static inline def_DHelper(push_ESI) {
  operand_rm(s, id_src1, true, NULL, false);
}

static inline def_DHelper(in_I2a) {
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(in_dx2a) {
  operand_reg(s, id_src1, true, R_DX, 2);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(out_a2I) {
  decode_op_a(s, id_src1, true);
  id_dest->width = 1;
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(out_a2dx) {
  //printf("=====> out_a2dx rax=%x \n", reg_l(R_EAX));
  decode_op_a(s, id_src1, true);
  operand_reg(s, id_dest, true, R_DX, 2);
  //printf("=====> out_a2dx rax=%x, id_src1=%x, id_dest=%x\n", reg_l(R_EAX),  *dsrc1, *ddest);
}

/* cb cd cq for relative address from this address*/
static inline def_DHelper(c_off) {
  // 解析偏移量并放入jump_pc中
  // s->jmp_pc = instr_fetch(s->seq_pc, s->dest.width) + s->seq_pc;
  decode_op_off(s, id_dest, true);
  //printf("now jump is %x\n", s->jmp_pc);
}

static inline void operand_write(DecodeExecState *s, Operand *op, rtlreg_t* src) {
  if (op->type == OP_TYPE_REG) { 
    rtl_sr(s, op->reg, src, op->width);
  }
  else if (op->type == OP_TYPE_MEM) { rtl_sm(s, s->isa.mbase, s->isa.moff, src, op->width); }
  else { assert(0); }
}