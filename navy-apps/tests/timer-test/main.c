#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
  // 通过gettimeofday()获取当前时间, 并每过0.5秒输出一句话.
  uint32_t start = NDL_GetTicks();
  while (1) {
    /* double elapsed = seconds + microseconds * 1e-6; */
    uint32_t elapsed = NDL_GetTicks() - start;

    /* double elapsed = seconds; */
    /* printf("elapsed : %d\n", elapsed); */
    /**/
    /* double_to_string(elapsed); */
    /* printf("elapsed : %s\n", buf); */
    if (elapsed >= 1000) {
      printf("1 seconds have passed\n");
      start = NDL_GetTicks();
    }
    /* usleep(10000); // sleep for 10 milliseconds */
    for (int i = 0; i < 10000; ++i)
      ;
  }

  return 0;
}
