#include <fs.h>

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_DISPLAY, FD_EVENT};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. used means always used, not gc
this is GNU c format*/
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
  [FD_DISPLAY] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_EVENT] = {"/dev/enents", 0, 0, events_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode) {
  int len = sizeof(file_table) / sizeof(Finfo);
  for(int i = 0; i < len; i++) {
    if (strcmp(file_table[i].name, pathname) == 0) {
      file_table[i].open_offset = file_table[i].disk_offset;
      return i;
    }
  }
  assert(0);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  int size = sizeof(file_table) / sizeof(Finfo);
  assert(fd >= 0 && fd < size);
  if (file_table[fd].read) {
    return file_table[fd].read(buf, 0, len);
  }
  ramdisk_read(buf, file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  int size = sizeof(file_table) / sizeof(Finfo);
  assert(fd >= 0 && fd < size);
  if (file_table[fd].write) {
    return file_table[fd].write(buf, 0, len);
  }
  ramdisk_write(buf, file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  int size = sizeof(file_table) / sizeof(Finfo);
  assert(fd >= 0 && fd < size);
  switch (whence) {
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      break;
    case SEEK_SET:
      file_table[fd].open_offset = file_table[fd].disk_offset + offset;
      break;
    case SEEK_END:
      file_table[fd].open_offset = file_table[fd].disk_offset + file_table[fd].size;
      return file_table[fd].disk_offset + file_table[fd].size + offset;
    default:
      break;
  }
  return file_table[fd].open_offset;
}
int fs_close(int fd) {
  return 0;
}
