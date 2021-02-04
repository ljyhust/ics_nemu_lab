#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  vaddr_t addr = cpu.idtr_base + *ddest * 8;
  word_t off_low = vaddr_read(addr, 2);
  word_t off_high = vaddr_read(addr + 6, 2);
  vaddr_t jump_addr = off_high << 16 | off_low;
  rtl_push(s, &cpu.eflags);
  rtl_push(s, &cpu.cs);
  rtl_push(s, &s->seq_pc);
  rtl_j(s, jump_addr);
}

void query_intr(DecodeExecState *s) {
  TODO();
}
