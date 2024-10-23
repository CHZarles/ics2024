#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
  // 通过gettimeofday()获取当前时间, 并每过0.5秒输出一句话.
  struct timeval start, current;
  gettimeofday(&start, NULL);
  while (1) {
    gettimeofday(&current, NULL);
    /* double elapsed = seconds + microseconds * 1e-6; */
    long elapsed = current.tv_sec * 1000000 + current.tv_usec;
    -start.tv_sec * 1000000 - start.tv_usec;

    /* double elapsed = seconds; */

    /**/
    /* double_to_string(elapsed); */
    /* printf("elapsed : %s\n", buf); */
    if (elapsed >= 500000) {
      printf("0.5 seconds have passed\n");
      gettimeofday(&start, NULL); // reset start time
    }
    /* usleep(10000); // sleep for 10 milliseconds */
    for (int i = 0; i < 1000; ++i)
      ;
  }

  return 0;
}
