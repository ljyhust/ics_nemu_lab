#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, (void *)0);
  return (uint32_t)(tv.tv_sec * 1000);
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/enents", 0, 0);
  return read(fd, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
    return;
  }
  int screen_fd = open("/proc/dispinfo", 0, 0);
  char buf[64];
  char str_w[8];
  char str_h[8];
  char* p_w = str_w;
  char* p_h = str_h;
  read(screen_fd, buf, sizeof(buf));
  char* p = buf;
  while(*p) {
    if (*p == 'W') {
      p += 2;
      while(*p <= '9' && *p >= '0') {
        *p_w = *p;
        p_w++;
        p++;
      }
    }
    if (*p == 'H') {
      p += 2;
      while(*p <= '9' && *p >= '0') {
        *p_h = *p;
        p_h++;
        p++;
      }
    }
    p++;
  }

  int sum_w = 0;
  int sum_h = 0;
  int index_w = 1;
  int index_h = 1;
  for (int i = 7; i >= 0; i--) {
    if(str_w[i] <= '9' && str_w[i] >= '0') {
      sum_w += (str_w[i] - '0') * index_w;
      index_w *= 10; 
    }
    if(str_h[i] <= '9' && str_h[i] >= '0') {
      sum_h += (str_h[i] - '0') * index_h;
      index_h *= 10; 
    }
  }
  screen_h = sum_h;
  screen_w = sum_w;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  // pixels len = w * h * sizeof (uint32_t) * sizeof(uint32_t)
  int len = w * h * sizeof (uint32_t);
  void* buf = malloc(len + 64);
  memcpy(buf, (void *)pixels, len);
  char x_y[64];
  sprintf(x_y, "x:%d,y:%d,w:%d,h:%d\n", x, y, w, h);
  memcpy((void *)(buf + len), x_y, sizeof(x_y));
  int fd = open("/dev/fb", 0, 0);
  write(fd, buf, len);
  free(buf);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
