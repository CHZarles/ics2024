#include "syscall.h"
#include <common.h>

void strace(Context *c) {

  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
  case 0: // SYS_exit
    printf("exit(%d)\n", c->GPR2);
    break;
  case 1: // SYS_yield
    printf("yield()\n");
    break;
  case 4: // SYS_write
    Log("write(%d, %s, %d)\n", c->GPR2, (char *)c->GPR3, c->GPR4);
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
    int ret = -1;
    size_t len = c->GPR4;
    // 检查fd的值, 如果fd是1或2(分别代表stdout和stderr)
    int fd = c->GPR2;
    char *buf = (char *)c->GPR3;
    if (fd == 1 || fd == 2) // stdout , stderr
    {
      // 则将buf为首地址的len字节输出到串口(使用putch()即可).
      for (size_t i = 0; i < len; i++) {
        putch(buf[i]); // no return value
      }
      ret = len;
    }
    // 最后还要设置正确的返回值
    c->GPRx = ret;
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  strace(c);
}
