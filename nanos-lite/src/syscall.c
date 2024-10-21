#include "syscall.h"
#include <common.h>
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
  case 1: // SYS_yield
    yield();
    c->GPRx = 0;
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
