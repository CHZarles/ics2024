#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
  // 通过gettimeofday()获取当前时间, 并每过0.5秒输出一句话.
  uint32_t start = NDL_GetTicks();
  while (1) {
    /* double elapsed = seconds + microseconds * 1e-6; */
    long elapsed = NDL_GetTicks - start;

    /* double elapsed = seconds; */

    /**/
    /* double_to_string(elapsed); */
    /* printf("elapsed : %s\n", buf); */
    if (elapsed >= 1000000) {
      printf("1 seconds have passed\n");
      start = NDL_GetTicks();
    }
    /* usleep(10000); // sleep for 10 milliseconds */
    for (int i = 0; i < 1000; ++i)
      ;
  }

  return 0;
}
