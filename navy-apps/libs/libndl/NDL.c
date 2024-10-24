#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
uint32_t NDL_GetTicks() {
  struct timeval current;
  gettimeofday(&current, NULL);
  return current.tv_sec * 1000 + current.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  size_t fd = open("/dev/events", "r+");
  if (read(fd, buf, len) > 0) {
    close(fd);
    return 1;
  }
  close(fd);
  return 0;
}

static int canvas_w = 0, canvas_h = 0;
void NDL_OpenCanvas(int *w, int *h) {
  printf("enter NDL_OpenCanvas\n");
  // get screen width and height
  int fd = open("/proc/dispinfo", "r");
  printf("Open disinfo\n");
  char buf[128];
  read(fd, buf, sizeof(buf));
  printf("read disinfo\n");
  // 打开一张(*w) X (*h)的画布
  // 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
  int sw, sh;
  printf("buf = %s", buf);
  sscanf(buf, "WIDTH: %d\nHEIGHT: %d\n", &screen_w, &screen_h);
  printf("screen_w = %d, screen_h = %d\n", screen_w, screen_h);
  if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
    canvas_h = screen_h;
    canvas_w = screen_w;
  } else {
    canvas_w = *w;
    canvas_h = *h;
  }

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w;
    screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0)
        continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0)
        break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fd = open("/dev/fb", "w");
  // 一行一行画
  for (int x = 0; x < canvas_h; ++x) {
    read(fd, pixels + x * canvas_w, canvas_w);
  }
  fclose(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {}

void NDL_CloseAudio() {}

int NDL_PlayAudio(void *buf, int len) { return 0; }

int NDL_QueryAudio() { return 0; }

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {}
