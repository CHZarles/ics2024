#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
int main() {
  // 通过gettimeofday()获取当前时间, 并每过0.5秒输出一句话.
  struct timeval start, current;
  gettimeofday(&start, NULL);
  while (1) {
    gettimeofday(&current, NULL);
    long seconds = current.tv_sec - start.tv_sec;
    long microseconds = current.tv_usec - start.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    /**/
    printf("elapsed : %f\n", elapsed);
    /* if (elapsed >= 1) { */
    /*   printf("0.5 seconds have passed\n"); */
    /*   gettimeofday(&start, NULL); // reset start time */
    /* } */

    /* usleep(10000); // sleep for 10 milliseconds */
    for (int i = 0; i < 1000000000; ++i)
      ;
  }

  return 0;
}
