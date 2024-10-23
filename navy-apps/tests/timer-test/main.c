#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

char buf[64];
void double_to_string(double value) {
  int int_part = (int)value;
  double dec_part = value - int_part;
  sprintf(buf, "%d.%d", int_part, (int)(dec_part * 1000000));
  printf("%s\n", buf);
}
int main() {
  // 通过gettimeofday()获取当前时间, 并每过0.5秒输出一句话.
  struct timeval start, current;
  gettimeofday(&start, NULL);
  while (1) {
    gettimeofday(&current, NULL);
    long seconds = current.tv_sec - start.tv_sec;
    long microseconds = current.tv_usec - start.tv_usec;
    /* double elapsed = seconds + microseconds * 1e-6; */
    /* double elapsed = seconds; */

    /**/
    /* double_to_string(elapsed); */
    /* printf("elapsed : %s\n", buf); */
    /* if (elapsed >= 1) { */
    /*   printf("0.5 seconds have passed\n"); */
    /*   gettimeofday(&start, NULL); // reset start time */
    /* } */

    /* usleep(10000); // sleep for 10 milliseconds */
    for (int i = 0; i < 10000000; ++i)
      ;
  }

  return 0;
}
