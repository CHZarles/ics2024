#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
  // 通过gettimeofday()获取当前时间, 并每过0.5秒输出一句话.
  struct timeval start, current;
  gettimeofday(&start, NULL);
  while (1) {
    /* double elapsed = seconds + microseconds * 1e-6; */
    long elapsed = current.tv_sec * 1000000 + current.tv_usec;
    -NDL_GetTicks();

    /* double elapsed = seconds; */

    /**/
    /* double_to_string(elapsed); */
    /* printf("elapsed : %s\n", buf); */
    if (elapsed >= 1000000) {
      printf("1 seconds have passed\n");
      gettimeofday(&start, NULL); // reset start time
    }
    /* usleep(10000); // sleep for 10 milliseconds */
    for (int i = 0; i < 100000; ++i)
      ;
  }

  return 0;
}
