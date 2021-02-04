#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  if (dest != src1) rtl_add(s, dest, src1, rz);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  *dest = ~(*src1);
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  *dest = 0 - *src1;
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  // 用sign位拓展，补全高位
  rtlreg_t temp = *src1; 
  switch (width){
    case 1: temp = (uint8_t) *src1; break;
    case 2: temp = (uint16_t) *src1; break;
    case 4: temp = (uint32_t) *src1; break;
    default: assert(0);
      break;
  }
  if ((temp) >> (width * 8 - 1) == 0) {
    *dest = temp;
    return;
  }
  switch(id_dest->width - width) {
    case 1: *dest = (0xff << (8 * width)) | temp; break;
    case 2: *dest = (0xffff << (8 * width)) | temp; break;
    case 3: *dest = (0xffffff << (8 * width)) | temp; break;
    default: assert(0);break;
  }
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  rtlreg_t temp = *src1; 
  switch (width){
    case 1: temp = (uint8_t) *src1; break;
    case 2: temp = (uint16_t) *src1; break;
    case 4: temp = (uint32_t) *src1; break;
    default: assert(0);
      break;
  }
  *dest = temp;
}

static inline def_rtl(msb_ext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  if ((*src1) >> (width * 8 - 1) == 0) {
    *dest = 0;
    return;
  }
  switch (width) {
    case 1:
      *dest = 0xff;
      break;
    case 2:
      *dest = 0xffff;
      break;
    case 4:
      *dest = 0xffffffff;
      break;
    default:
      break;
  }
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  rtlreg_t temp = *src1; 
  //Log("msb width=%d \n", width);
  switch (width){
    case 1: temp = (uint8_t) *src1; break;
    case 2: temp = (uint16_t) *src1; break;
    case 4: temp = (uint32_t) *src1; break;
    default: assert(0);
      break;
  }
  *dest = temp >> (width * 8 - 1);
}

static inline def_rtl(sbit, rtlreg_t* dest, const rtlreg_t* src1, int index, int width) {
  // 求某个位置index的bit位
  rtlreg_t temp = *src1; 
  switch (width){
    case 1: temp = (uint8_t) *src1; break;
    case 2: temp = (uint16_t) *src1; break;
    case 4: temp = (uint32_t) *src1; break;
    default: assert(0);
      break;
  }
  *dest = ((temp) << (width * 8 - index)) >> (width * 8 - 1);
}

static inline def_rtl(lsb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  *dest = *src1 % 2;
}

#endif
