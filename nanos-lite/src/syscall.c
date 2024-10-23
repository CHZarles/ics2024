#include "syscall.h"
#include <common.h>

void strace(Context *c) {

  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
  case 0: // SYS_exit
    Log("Call exit\n");
    break;
  case 1: // SYS_yield
    Log("Call yield\n");
    break;
  case 4: // SYS_write
    Log("Call SYS_write\n");
    break;
  case 9: // SYS_brk
    Log("Call brk\n");
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
  case 0: // SYS_exit
    halt(c->GPR2);
    break;
  case 1: // SYS_yield
    yield();
    c->GPRx = 0;
    break;
  case 4: // SYS_write
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
  case 9: // SYS_brk
    // TODO ...
    c->GPRx = 0;
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  strace(c);
}
