#include <am.h>
#include <klib.h> // for debug
#include <nemu.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  // read info rom VGACTL_ADDR
  uint32_t height = inw(VGACTL_ADDR);
  uint32_t width = inw(VGACTL_ADDR + 2);
  /* printf("gpu config width = %d, height = %d\n", width, height); */
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = width,
                           .height = height,
                           .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  /* AM帧缓冲控制器, 可写入绘图信息,
   * 向屏幕(x, y)坐标处绘制w*h的矩形图像.
   * 图像像素按行优先方式存储在pixels中,
   * 每个像素用32位整数以00RRGGBB的方式描述颜色. */

  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if (!ctl->sync && (w == 0 || h == 0))
    return;
  uint32_t *pixels = ctl->pixels;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t screen_w = inw(VGACTL_ADDR + 2);
  for (int i = y; i < y + h; i++) {
    for (int j = x; j < x + w; j++) {
      fb[screen_w * i + j] =
          pixels[w * (i - y) + (j - x)]; // 缓冲区是一个像素块
    }
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
