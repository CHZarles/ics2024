#include <am.h>
#include <klib.h>
#include <riscv/riscv.h>

static Context *(*user_handler)(Event, Context *) = NULL;

Context *__am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
    case (uint32_t)(-1):
      ev.event = EVENT_YIELD;
      break;
    case 0: // sys_exit
    case 1: // sys_yield
    case 4: // sys_write
      ev.event = EVENT_SYSCALL;
      break;
    default:
      ev.event = EVENT_ERROR;
      break;
    }
    // display Context *c
    // 1. display general registers
    /* for (int i = 0; i < 32; i++) { */
    /*   if (i == 1) */
    /*     continue; */
    /*   printf("x[%d] = %x\n", i + 1, c->gpr[i]); */
    /* } */
    // 2. display csr registers
    /* printf("mcause = %x\n", c->mcause); */
    /* printf("mepc = %x\n", c->mepc); */
    /* printf("mstatus = %x\n", c->mstatus); */

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context *(*handler)(Event, Context *)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() { return false; }

void iset(bool enable) {}
