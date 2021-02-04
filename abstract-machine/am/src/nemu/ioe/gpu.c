#include <am.h>
#include <nemu.h>
#include <klib.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
static inline int min(int x, int y) { return (x < y) ? x : y; }

void __am_gpu_init() {
  
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t screen_w_h = inl(VGACTL_ADDR);
  int w = screen_w_h >> 16;
  int h = screen_w_h & 0x0000ffff;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}

// screen sync
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (NULL != ctl->pixels) {
    //out to vmem
    int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
    uint32_t screen_w_h = inl(VGACTL_ADDR);
    int W = screen_w_h >> 16;
    int H = screen_w_h & 0x0000ffff;
    uint32_t *pixels = ctl->pixels;
    int cp_bytes = sizeof(uint32_t) * min(w, W - x);
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    for (int j = 0; j < h && y + j < H; j ++) {
      memcpy(&fb[(y + j) * W + x], pixels, cp_bytes);
      pixels += w;
    }
  }
  
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
  
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
