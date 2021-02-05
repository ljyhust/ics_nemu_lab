#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState *s, int width) {
  if (width == -1) return;
  if (width == 0) {
    width = s->isa.is_operand_size_16 ? 2 : 4;
  }
  s->src1.width = s->dest.width = s->src2.width = width;
}

/* 0x80, 0x81, 0x83 需要通过 Mod R/M 确定具体操作 后面跟 Mod R/M imm立即数*/
static inline def_EHelper(gp1) {
  switch (s->isa.ext_opcode) {
    EXW(0, add, id_dest->width) EXW(1, or, id_dest->width) EXW(2, adc, id_dest->width)
    EXW(3, sbb, id_dest->width) EXW(4, and, id_dest->width) EXW(5, sub, id_dest->width) 
    EXW(6, xor, id_dest->width) EXW(7, cmp, id_dest->width)
  }
}

/* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 */
static inline def_EHelper(gp2) {
  switch (s->isa.ext_opcode) {
    EXW(0, rol, id_dest->width) EXW(1, ror, id_dest->width) EMPTY(2) EMPTY(3)
    EXW(4, shl, id_dest->width) EXW(5, shr, id_dest->width) EMPTY(6) EXW(7, sar, id_dest->width)
  }
}

/* 0xf6, 0xf7 */
static inline def_EHelper(gp3) {
  switch (s->isa.ext_opcode) {
    IDEXW(0, test_I, test, id_dest->width) EMPTY(1) EXW(2, not, id_dest->width) EXW(3, neg, id_dest->width)
    EXW(4, mul, id_dest->width) EXW(5, imul1, id_dest->width) 
    EXW(6, div, id_dest->width) EXW(7, idiv, id_dest->width)
  }
}

/* 0xfe */
static inline def_EHelper(gp4) {
  switch (s->isa.ext_opcode) {
    EXW(0, inc, id_dest->width) EMPTY(1) EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

/* 0xff */
static inline def_EHelper(gp5) {
  switch (s->isa.ext_opcode) {
    EXW(0, inc, id_dest->width) EXW(1, dec, id_dest->width) EXW(2, call, id_dest->width) EMPTY(3)
    EXW(4, jmp, id_dest->width) EMPTY(5) EX(6, push) EMPTY(7)
  }
}

/* 0x0f 0x01*/
static inline def_EHelper(gp7) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1) EMPTY(2) EXW(3, lidt, id_dest->width)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

/* setcc as sete sets setne... */
static inline def_EHelper(2byte_esc) {
  uint8_t opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;
  switch (opcode) {
  /* TODO: Add more instructions!!! */
    IDEX (0x01, gp7_E, gp7)
    IDEX (0xa5, E2G, shl)
    IDEX (0xb6, movb_E2G, movzx)
    IDEX (0xb7, movw_E2G, movzx)
    IDEX (0xbe, movb_E2G, movsx)
    IDEX (0xbf, movw_E2G, movsx)
    IDEX (0x82, c_off, jcc)
    IDEX (0x83, c_off, jcc)
    IDEX (0x84, c_off, jcc)
    IDEX (0x85, c_off, jcc)
    IDEX (0x86, c_off, jcc)
    IDEX (0x87, c_off, jcc)
    IDEX (0x88, c_off, jcc)
    IDEX (0x89, c_off, jcc)
    IDEX (0x8c, c_off, jcc)
    IDEX (0x8d, c_off, jcc)
    IDEX (0x8e, c_off, jcc)
    IDEX (0x8f, c_off, jcc)
    IDEXW(0x92, setcc_E, setcc, 1)
    IDEXW(0x94, setcc_E, setcc, 1)
    IDEXW(0x95, setcc_E, setcc, 1)
    IDEX (0xaf, E2G, imul2_ns)
    default: exec_inv(s);
  }
}

static inline void fetch_decode_exec(DecodeExecState *s) {
  uint8_t opcode;
again:
  // 取1个字节？
  opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;
  switch (opcode) {
    EX   (0x0f, 2byte_esc)
    IDEXW(0x00, G2E, add, 1)
    IDEX (0x01, G2E, add)
    IDEXW(0x02, E2G, add, 1)
    IDEX (0x03, E2G, add)
    IDEX (0x05, I2a, add)
    IDEXW(0x08, G2E, or, 1)
    IDEX (0x09, G2E, or)
    IDEXW(0x0a, E2G, or, 1)
    IDEX (0x0b, E2G, or)
    IDEX (0x0d, I2a, or)
    IDEX (0x11, G2E, adc)
    IDEX (0x13, E2G, adc)
    IDEX (0x19, G2E, sbb)
    IDEX (0x1b, E2G, sbb)
    IDEX (0x21, G2E, and)
    IDEXW(0x22, E2G, and, 1)
    IDEX (0x23, E2G, and)
    IDEXW(0x24, I2a, and, 1)
    IDEX (0x25, I2a, and)
    IDEX (0x29, G2E, sub)
    IDEX (0x2b, E2G, sub)
    IDEX (0x2d, I2a, sub)
    IDEXW(0x80, I2E, gp1, 1)
    IDEX (0x81, I2E, gp1)
    IDEX (0x83, SI2E, gp1)
    IDEXW(0x84, G2E, test, 1)
    IDEX (0x85, G2E, test)
    IDEXW(0x88, mov_G2E, mov, 1)
    IDEX (0x89, mov_G2E, mov)
    IDEXW(0x8a, mov_E2G, mov, 1)
    IDEX (0x8b, mov_E2G, mov)
    IDEX (0x8d, lea_M2G, lea)
    IDEXW(0xa0, O2a, mov, 1)
    IDEX (0xa1, O2a, mov)
    IDEXW(0xa2, a2O, mov, 1)
    IDEX (0xa3, a2O, mov)
    EX   (0xa4, movsb)
    IDEXW(0xa8, I2a, test, 1)
    IDEXW(0xb0, mov_I2r, mov, 1)
    IDEXW(0xb1, mov_I2r, mov, 1)
    IDEXW(0xb2, mov_I2r, mov, 1)
    IDEXW(0xb3, mov_I2r, mov, 1)
    IDEXW(0xb4, mov_I2r, mov, 1)
    IDEXW(0xb5, mov_I2r, mov, 1)
    IDEXW(0xb6, mov_I2r, mov, 1)
    IDEXW(0xb7, mov_I2r, mov, 1)
    IDEX (0xb8, mov_I2r, mov)
    IDEX (0xb9, mov_I2r, mov)
    IDEX (0xba, mov_I2r, mov)
    IDEX (0xbb, mov_I2r, mov)
    IDEX (0xbc, mov_I2r, mov)
    IDEX (0xbd, mov_I2r, mov)
    IDEX (0xbe, mov_I2r, mov)
    IDEX (0xbf, mov_I2r, mov)
    IDEXW(0xc0, gp2_Ib2E, gp2, 1)
    IDEX (0xc1, gp2_Ib2E, gp2)
    IDEXW(0xc6, mov_I2E, mov, 1)
    IDEX (0xc7, mov_I2E, mov)
    EX   (0xc9, leave)
    EX   (0xcf, iret)
    IDEXW(0xcd, I, int,1)
    IDEXW(0xd0, gp2_1_E, gp2, 1)
    IDEX (0xd1, gp2_1_E, gp2)
    IDEXW(0xd2, gp2_cl2E, gp2, 1)
    IDEX (0xd3, gp2_cl2E, gp2)
    EX   (0xd6, nemu_trap)
    IDEXW(0xf6, E, gp3, 1)
    IDEX (0xf7, E, gp3)
    IDEXW(0xfe, E, gp4, 1)
    IDEX (0xff, E, gp5)
    IDEXW(0xe4, in_I2a, in, 1)
    IDEX (0xe5, in_I2a, in)
    IDEXW(0xe6, out_a2I, out, 1)
    IDEX (0xe7, out_a2I, out)
    IDEX (0xe9, c_off, jmp)
    IDEXW(0xeb, c_off, jmp, 1)
    IDEXW(0xec, in_dx2a, in, 1)
    IDEX (0xed, in_dx2a, in)
    IDEXW(0xee, out_a2dx, out, 1)
    IDEX (0xef, out_a2dx, out)
    IDEX (0xe8, c_off, call)
    EX   (0xc3, ret)
    IDEX (0x40, r, inc)
    IDEX (0x41, r, inc)
    IDEX (0x43, r, inc)
    IDEX (0x42, r, inc)
    IDEX (0x45, r, inc)
    IDEX (0x46, r, inc)
    IDEX (0x47, r, inc)
    IDEX (0x48, r, dec)
    IDEX (0x49, r, dec)
    IDEX (0x4a, r, dec)
    IDEX (0x4b, r, dec)
    IDEX (0x4e, r, dec)
    IDEX (0x50, r, push)
    IDEX (0x51, r, push)
    IDEX (0x52, r, push)
    IDEX (0x53, r, push)
    IDEX (0x54, r, push)
    IDEX (0x55, r, push)
    IDEX (0x56, r, push)
    IDEX (0x57, r, push)
    IDEX (0x58, r, pop)
    IDEX (0x59, r, pop)
    IDEX (0x5a, r, pop)
    IDEX (0x5b, r, pop)
    IDEX (0x5d, r, pop)
    IDEX (0x5e, r, pop)
    IDEX (0x5f, r, pop)
    IDEX (0x68, I, push)
    EX   (0x60, pusha)
    EX   (0x61, popa)
    IDEXW(0x6a, I, push, 1)
    IDEX (0x6b, SI_E2G, imul3_ns)
    IDEX (0x31, G2E, xor)
    IDEXW(0x32, E2G, xor, 1)
    IDEX (0x33, E2G, xor)
    IDEX (0x35, I2a, xor)
    IDEXW(0x38, G2E, cmp, 1)
    IDEX (0x39, G2E, cmp)
    IDEX (0x3b, E2G, cmp)
    IDEXW(0x3c, I2a, cmp, 1)
    IDEX (0x3d, I2a, cmp)
    IDEXW(0x72, c_off, jcc, 1)
    IDEXW(0x73, c_off, jcc, 1)
    IDEXW(0x74, c_off, jcc, 1)
    IDEXW(0x75, c_off, jcc, 1)
    IDEXW(0x76, c_off, jcc, 1)
    IDEXW(0x77, c_off, jcc, 1)
    IDEXW(0x78, c_off, jcc, 1)
    IDEXW(0x79, c_off, jcc, 1)
    IDEXW(0x7c, c_off, jcc, 1)
    IDEXW(0x7d, c_off, jcc, 1)
    IDEXW(0x7e, c_off, jcc, 1)
    IDEXW(0x7f, c_off, jcc, 1)
    EX   (0x90, nop)
    IDEX (0x98, cwtl_sa, cwtl)
    IDEX (0x99, clcc_a2d, cltd)
    //IDEX (0xff, push_ESI, push)
  case 0x66: s->isa.is_operand_size_16 = true; goto again;
  default: exec_inv(s);
  }
}

vaddr_t isa_exec_once() {
  DecodeExecState s;
  s.is_jmp = 0;
  s.isa = (ISADecodeInfo) { 0 };
  s.seq_pc = cpu.pc;

  fetch_decode_exec(&s);
  
  //printf("====>opcode=%x, ebp=%x, esp=%x, eax=%x, ecx=%x, edx=%x, ebx=%x, edi=%x, esi=%x, cpu.pc=%x\n",
  //   s.opcode, reg_l(R_EBP), reg_l(R_ESP), reg_l(R_EAX), reg_l(R_ECX), cpu.edx, cpu.ebx, cpu.edi, cpu.esi, cpu.pc);
  
  update_pc(&s);

  return s.seq_pc;
}
