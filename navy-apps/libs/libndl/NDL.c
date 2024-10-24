#include <assert.h>
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
  // get screen width and height
  int fd = open("/proc/dispinfo", "r");
  char buf[128];
  read(fd, buf, sizeof(buf));
  // 打开一张(*w) X (*h)的画布
  // 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
  int sw, sh;
  sscanf(buf, "WIDTH: %d\nHEIGHT: %d\n", &screen_w, &screen_h);
  /* printf("screen_w = %d, screen_h = %d\n", screen_w, screen_h); */
  if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  }
  if (*w > screen_w)
    *w = screen_w;
  if (*h > screen_h)
    *h = screen_h;
  close(fd);

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
  int fd = open("/dev/fb", 0);
  // 得到在屏幕上,让画布居中的左上角点
  assert(screen_h >= h);
  assert(screen_w >= w);
  int my = (screen_h - h) / 2;
  int mx = (screen_w - w) / 2;
  // printf("NDL_DrawRect fd:%d\n", fd);
  for (y = 0; y < h; y++) {
    // printf("NDL_DrawRect mx: %d, y+my: %d\n", mx, y+my);
    lseek(fd, (y + my) * screen_w + mx, SEEK_SET);
    write(fd, (void *)&pixels[y * w], w);
  }
  close(fd);
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
