#include <am.h>
#include <klib.h>
#include <riscv/riscv.h>

/* enum { */
/*   SYS_exit, */
/*   SYS_yield, */
/*   SYS_open, */
/*   SYS_read, */
/*   SYS_write, */
/*   SYS_kill, */
/*   SYS_getpid, */
/*   SYS_close, */
/*   SYS_lseek, */
/*   SYS_brk, */
/*   SYS_fstat, */
/*   SYS_time, */
/*   SYS_signal, */
/*   SYS_execve, */
/*   SYS_fork, */
/*   SYS_link, */
/*   SYS_unlink, */
/*   SYS_wait, */
/*   SYS_times, */
/*   SYS_gettimeofday */
/* }; */
/**/
static Context *(*user_handler)(Event, Context *) = NULL;

Context *__am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
    case (uint32_t)(-1):
      ev.event = EVENT_YIELD;
      break;
    case 0: // sys_exit
    case 1: // yield
    case 2: // sys_open
    case 3: // sys_read
    case 4: // sys_write
    case 7: // sys_close
    case 8: // sys_lseek
    case 9: // sys_brk
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
