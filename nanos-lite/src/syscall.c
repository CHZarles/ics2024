#include "syscall.h"
#include <common.h>

void strace(Context *c) {

  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
  case SYS_exit: // SYS_exit
    Log("Call exit");
    break;
  case SYS_yield: // SYS_yield
    Log("Call yield");
    break;
  case SYS_write: // SYS_write
    Log("Call SYS_write");
    break;
  case SYS_brk: // SYS_brk
    Log("Call brk");
    break;
  case SYS_open:
    Log("Call open");
    break;
  case SYS_read:
    Log("Call read");
    break;
  case SYS_close:
    Log("Call close");
    break;
  case SYS_lseek:
    Log("Call lseek");
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
  case SYS_exit: // SYS_exit
    halt(c->GPR2);
    break;
  case SYS_yield: // SYS_yield
    yield();
    c->GPRx = 0;
    break;
  case SYS_read: // SYS_read
  {
    size_t len = c->GPR4;
    int fd = c->GPR2;
    char *buf = (char *)c->GPR3;
    extern size_t fs_read(int fd, void *buf, size_t len);
    c->GPRx = fs_read(fd, buf, len);
    break;
  }
  case SYS_write: // SYS_write
  {
    size_t len = c->GPR4;
    // 检查fd的值, 如果fd是1或2(分别代表stdout和stderr)
    int fd = c->GPR2;
    char *buf = (char *)c->GPR3;
    extern size_t fs_write(int fd, const void *buf, size_t len);
    c->GPRx = fs_write(fd, buf, len);
    /* if (fd == 1 || fd == 2) // stdout , stderr */
    /* { */
    /*   // 则将buf为首地址的len字节输出到串口(使用putch()即可). */
    /*   for (size_t i = 0; i < len; i++) { */
    /*     putch(buf[i]); // no return value */
    /*   } */
    /*   ret = len; */
    /* } */
    /* // 最后还要设置正确的返回值 */
    /* c->GPRx = ret; */
    break;
  }
  case SYS_open: // SYS_open
    // 0. get filename
    char *path = (char *)c->GPR2;
    // 1. get flags
    int flags = c->GPR3;
    // 2. get mode
    int mode = c->GPR4;
    // 3. call fs_open
    extern int fs_open(const char *pathname, int flags, int mode);
    c->GPRx = fs_open(path, flags, mode);
    break;
  case SYS_close: {
    // 0. get fd
    int fd = c->GPR2;
    // 1. call fs_close
    extern int fs_close(int fd);
    c->GPRx = fs_close(fd);
    break;
  }

  case SYS_lseek: { // 0. get fd
    int fd = c->GPR2;
    // 1. get offset
    size_t offset = c->GPR3;
    // 2. get whence
    int whence = c->GPR4;
    // 3. call fs_lseek
    extern size_t fs_lseek(int fd, size_t offset, int whence);
    c->GPRx = fs_lseek(fd, offset, whence);
    break;
  }
  case SYS_brk: { // SYS_brk
    // TODO ...
    c->GPRx = 0;
    break;
  }
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  strace(c);
}
