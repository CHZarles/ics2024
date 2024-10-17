#include <common.h>

void init_mm(void);
void init_device(void);
void init_ramdisk(void);
void init_irq(void);
void init_fs(void);
void init_proc(void);

int main() {
  extern const char logo[];
  printf("%s", logo);
  Log("'Hello World!' from Nanos-lite");
  Log("Build time: %s, %s", __TIME__, __DATE__);

  init_mm(); // 初始化内存管理???

  init_device(); // 初始化设备

  init_ramdisk(); // nemu 将一段内存当作磁盘使用

#ifdef HAS_CTE
  init_irq();
#endif

  init_fs(); // 初始化文件系统

  init_proc(); // 初始化进程

  Log("Finish initialization");

#ifdef HAS_CTE
  yield();
#endif

  panic("Should not reach here");
}
