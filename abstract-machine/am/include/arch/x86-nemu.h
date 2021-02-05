#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  void *cr3;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip, cs, efalgs;
};

#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
