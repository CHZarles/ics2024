#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum { FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB };

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < LENGTH(file_table); i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      return i;
    }
  }
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  return file_table[fd].read(buf, 0, len);
}

size_t fs_write(int fd, const void *buf, size_t len) {
  return file_table[fd].write(buf, 0, len);
}
size_t fs_lseek(int fd, size_t offset, int whence) {
  switch (whence) {
  case SEEK_SET:
    break;
  case SEEK_CUR:
    offset += file_table[fd].disk_offset;
    break;
  case SEEK_END:
    offset += file_table[fd].size;
    break;
  default:
    panic("whence is invalid");
  }
  if (offset < 0 || offset > file_table[fd].size) {
    return -1;
  }
  return file_table[fd].disk_offset = offset;
}

// 由于sfs没有维护文件打开的状态, fs_close()可以直接返回0, 表示总是关闭成功.
int fs_close(int fd) { return 0; }

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
