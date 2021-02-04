#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

static const char* KEY_KD = "kd ";
static const char* KEY_KU = "ku ";

size_t serial_write(const void *buf, size_t offset, size_t len) {
  assert(NULL != buf);
  char* temp = (char *)buf;
  while(len) {
    putch(*temp++);
    len--;
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  assert(len > 3);
  AM_INPUT_KEYBRD_T key;
  ioe_read(AM_INPUT_KEYBRD, &key);
  if (key.keycode == AM_KEY_NONE) {
    return 0;
  }
  for (int k = 0; k < 3; k++) {
    if (key.keydown) {
      *((char *)buf + k) = KEY_KD[k];
    } else
    {
      *((char *)buf + k) = KEY_KU[k];
    }
  }
  
  const char* temp = keyname[key.keycode];
  int i = 3;
  for (; i < len; i++) {
    if(temp) {
      *((char *) buf + i) = *temp;
    }
    temp++;
  }
  if (len > i) {
    *((char *) buf + i) = '\n';
  }
  return 1;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T gpu_t;
  ioe_read(AM_GPU_CONFIG, &gpu_t);
  int w = gpu_t.width;
  int h = gpu_t.height;
  sprintf((char *)buf, "W:%d\nH:%d", w, h);
  return sizeof((char *)buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  //AM_GPU_FBDRAW_T draw_t;
  AM_GPU_FBDRAW_T draw_t;
  void* temp = malloc(len - 64);
  memcpy(temp, buf, len - 64);
  draw_t.pixels = temp;
  draw_t.sync = true;
  // char* params = (char *)(buf + len);
  // while(*params && *params != '\n') {
  //   switch (*params)
  //   {
  //     case 'x':
  //       params += 2;
  //       if (*params == ',') break;

  //       break;
  //     case 'y':
  //       params += 2;
  //       break;
  //     case 'w':
  //       params += 2;
  //       break;
  //     case 'h':
  //       params += 2;
  //       break;
  //     default:
  //       break;
  //   }
  //   params++;
  // }
  draw_t.x = 0;
  draw_t.y = 0;
  draw_t.w = 128;
  draw_t.h = 128;
  //todo
  ioe_write(AM_GPU_FBDRAW, &draw_t);
  //free(temp);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
