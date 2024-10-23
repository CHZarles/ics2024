#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset; // ADD: 当前文件打开的位置
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
extern size_t serial_write(const void *buf, size_t offset, size_t len);
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
#include "files.h"
};

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < LENGTH(file_table); i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      return i;
    }
  }
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  if (fd <= 2) {
    Log("ignore read %s", file_table[fd].name);
    return 0;
  }
  size_t read_len = len;
  size_t open_offset = file_table[fd].open_offset;
  size_t size = file_table[fd].size;
  size_t disk_offset = file_table[fd].disk_offset;
  if (open_offset > size)
    return 0;
  if (open_offset + len > size)
    read_len = size - open_offset;
  ramdisk_read(buf, disk_offset + open_offset, read_len);
  file_table[fd].open_offset += read_len;
  return read_len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (fd == 0) {
    Log("ignore write %s", file_table[fd].name);
    return 0;
  }
  // special file
  if (file_table[fd].write != NULL) {
    return file_table[fd].write(buf, file_table[fd].open_offset, len);
  }
  /* if (fd == 1 || fd == 2) { */
  /*   for (size_t i = 0; i < len; i++) { */
  /*     putch(((char *)buf)[i]); */
  /*   } */
  /*   return len; */
  /* } */

  size_t write_len = len;
  size_t open_offset = file_table[fd].open_offset;
  size_t size = file_table[fd].size;
  size_t disk_offset = file_table[fd].disk_offset;
  if (open_offset > size)
    return 0;
  if (open_offset + len > size)
    write_len = size - open_offset;
  ramdisk_write(buf, disk_offset + open_offset, write_len);
  file_table[fd].open_offset += write_len;
  return write_len;
}
size_t fs_lseek(int fd, size_t offset, int whence) {
  if (fd <= 2) {
    Log("ignore lseek %s", file_table[fd].name);
    return 0;
  }
  Finfo *file = &file_table[fd];
  size_t new_offset = 0;
  switch (whence) {

  case SEEK_SET:
    new_offset = offset;
    break;
  case SEEK_CUR:
    new_offset = file->open_offset + offset;
    break;
  case SEEK_END:
    new_offset = file->size + offset;
    break;
  default:
    panic("Unsupport whence");
    return -1;
  }
  if (new_offset < 0 || new_offset > file->size) {
    Log("Seek position out of bounds");
    return -1;
  }
  file->open_offset = new_offset;
  return new_offset;
}

// 由于sfs没有维护文件打开的状态, fs_close()可以直接返回0, 表示总是关闭成功.
int fs_close(int fd) {
  // reset open_offset
  file_table[fd].open_offset = 0;
  return 0;
}

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
