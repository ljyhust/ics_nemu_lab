#include <am.h>
#include <nemu.h>
#include <klib.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  // get key from queue
  //printf("addr = %d \n", KBD_ADDR);
  uint32_t k = inl(KBD_ADDR);
  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  kbd->keycode = k & ~KEYDOWN_MASK;
  //printf("port is %x \n", KBD_ADDR);
  // 以下错误原因：
  // 键有down与up，其code是一样的，具体实现在send_key中
  // 为了区别down与up，加了一个KEYDOWN_MASK
  // 当keydown时，code=keydown | KEYDOWN_MASK
  // 当up时，code=keydown
  // if (k > KEYDOWN_MASK) {
  //   kbd->keycode = k - KEYDOWN_MASK;
  //   kbd->keydown = true;
  // } else {
  //   kbd->keycode = AM_KEY_NONE;
  //   kbd->keydown = false;
  // }
  // if (kbd->keydown) {
  //   printf("keybort=%x, down=%d\n", kbd->keycode, kbd->keydown);
  // }
}
