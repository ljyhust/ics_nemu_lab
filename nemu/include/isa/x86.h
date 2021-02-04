#ifndef __ISA_X86_H__
#define __ISA_X86_H__

#include <common.h>


// memory
#define x86_IMAGE_START 0x100000
#define x86_PMEM_BASE 0x0

typedef union x86_reg {
  uint32_t _32;
  uint16_t _16;
  uint8_t _8[2];
} GPR;
// reg


/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  /* Do NOT change the order of the GPRs' definitions. */

  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
   * in PA2 able to directly access these registers.
   * enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
    enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
    enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };
   */
  union {
    GPR gpr[8];
    struct {
      union {
        rtlreg_t eax;
        uint16_t ax;
        struct {
          uint8_t ah;
          uint8_t al;
        };
      };
      union {
        rtlreg_t ecx;
        uint16_t cx;
        struct
        {
          uint8_t  ch;
          uint8_t  cl;
        };
        
      };
      union {
        rtlreg_t edx;
        uint16_t  dx;
        struct
        {
          uint8_t   dh;
          uint8_t   dl;
        };      
      };
      union {
        rtlreg_t ebx;
        uint16_t  bx;
        struct
        {
          uint8_t   bh;
          uint8_t   bl;
        };  
      };
      union {
        rtlreg_t esp;
        uint16_t  sp;
        struct
        {
          uint8_t   sph;
          uint8_t   spl;
        };
        
      };
      union {
        rtlreg_t ebp;
        uint16_t  bp;
        struct
        {
          uint8_t  bph;
          uint8_t  bpl;
        };      
      };
      union {
        rtlreg_t esi;
        uint16_t  si;
        struct
        {
          uint8_t  sih;
          uint8_t  sil;
        };
      };
      union {
        rtlreg_t edi;
        uint16_t  di;
        struct
        {
          uint8_t  dih;
          uint8_t  dil;
        };   
      };
    };
  };
  
  /* EFLAGS CF ZF SF OF only indicate the result of arithmetic instructions, b_n means not use*/
  union {
    rtlreg_t eflags;
    struct {
      uint8_t CF   :1;  /* This flag indicates an overflow condition for unsigned-integer arithmetic */
      uint8_t b_1  :1;
      uint8_t PF   :1;
      uint8_t b_3  :1;
      uint8_t AF   :1;
      uint8_t b_5  :1;
      uint8_t ZF   :1;  /* result is zero */
      uint8_t SF   :1;
      uint8_t TF   :1;
      uint8_t IF   :1;
      uint8_t DF   :1;
      uint8_t OF   :1; /* This flag indicates an overflow condition for signed-integer */
      uint8_t IOPL :2;
      uint8_t NT   :1;
      uint8_t b_15 :1;
      uint8_t RF   :1;
      uint8_t VM   :1;
      uint16_t not_impl :14;
    };
  };
  struct {
    uint32_t idtr_base;
    uint16_t idtr_limit;
  };
  uint32_t cs;
  vaddr_t pc;
} x86_CPU_state;

// decode
typedef struct {
  bool is_operand_size_16;
  uint8_t ext_opcode;
  const rtlreg_t *mbase;
  rtlreg_t mbr;
  word_t moff;
} x86_ISADecodeInfo;

#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))
#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define x86_has_mem_exception() (false)

#endif
