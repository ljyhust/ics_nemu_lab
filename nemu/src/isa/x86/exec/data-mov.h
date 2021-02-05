static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
}

static inline def_EHelper(push) {
  //TODO();
  rtl_add(s, s0, ddest, rz);
  rtl_push(s, s0);
  print_asm_template1(push);
}

static inline def_EHelper(pushm) {
  rtl_add(s, s0, ddest, rz);
  rtl_push(s, s0);
  //printf("=====> s result is %x, \n", s->dest.val);
  print_asm_template1(push);
}

static inline def_EHelper(pop) {
  rtl_pop(s, ddest);
  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
  rtlreg_t temp = cpu.esp;
  rtl_push(s, &cpu.eax);
  rtl_push(s, &cpu.ecx);
  rtl_push(s, &cpu.edx);
  rtl_push(s, &cpu.ebx);
  rtl_push(s, &temp);
  rtl_push(s, &cpu.ebp);
  rtl_push(s, &cpu.esi);
  rtl_push(s, &cpu.edi);
  print_asm("pusha");
}

static inline def_EHelper(popa) {
  rtl_pop(s, &cpu.edi);
  rtl_pop(s, &cpu.esi);
  rtl_pop(s, &cpu.ebp);
  rtl_pop(s, s0);
  rtl_pop(s, &cpu.ebx);
  rtl_pop(s, &cpu.edx);
  rtl_pop(s, &cpu.ecx);
  rtl_pop(s, &cpu.eax);
  print_asm("popa");
}

static inline def_EHelper(leave) {
  // Set ESP to EBP, then pop EBP. means %esp:=%ebp, %ebp:=pop()
  //printf("====> leave 1 ebp is %x, esp:%x\n", reg_l(R_EBP), reg_l(R_ESP));
  rtl_mv(s, &reg_l(R_ESP), &reg_l(R_EBP));
  //printf("====> leave 2 ebp is %x, esp:%x\n", reg_l(R_EBP), reg_l(R_ESP));
  rtl_pop(s, &reg_l(R_EBP));
  //printf("====> leave 3 ebp is %x, esp:%x\n", reg_l(R_EBP), reg_l(R_ESP));
  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    //DX:AX := sign-extend of AX.  sign is 15bit of ax
    rtl_msb_ext(s, ddest, dsrc1, 2);
  }
  else {
    // EDX:EAX := sign-extend of EAX. sign is 31bit of eax
    rtl_msb_ext(s, ddest, dsrc1, 4);
  }
  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
    // cbtw
    rtl_sext(s, ddest, dsrc1, 1);
  }
  else {
    // cwtl
    rtl_sext(s, ddest, dsrc1, 2);
  }
  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  // sign extend => sx
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, s0, dsrc1, id_src1->width);
  operand_write(s, id_dest, s0);
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  // zero extend => zx
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_zext(s, s0, dsrc1, id_src1->width); //unnessary
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(movzx);
}

/*  如movsb等, 这些指令需要用到段寄存器DS, ES以及EFLAGS寄存器中的DF标志. 在PA中无需实现这些寄存器, RTFM时认为这些寄存器的值恒为0来理解指令的语义即可. */
static inline def_EHelper(movsb) {
  // zero extend => zx
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  // rtl_zext(s, s0, dsrc1, id_src1->width); unnessary
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(movsb);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}

static inline def_EHelper(xchg) {
  // exchange AL whith reg(ddest)
  // nop
}

static inline def_EHelper(nop) {
  // no
  // The one-byte NOP instruction is an alias mnemonic for the XCHG (E)AX, (E)AX instruction.
}
