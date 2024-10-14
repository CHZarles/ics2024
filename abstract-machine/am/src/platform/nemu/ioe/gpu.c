#include <am.h>
#include <klib.h> // for debug
#include <nemu.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {

  int i;
/* int w = 0; // TODO: get the correct width */
/* int h = 0; // TODO: get the correct height */
#define N 32
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  printf("w = %d, h = %d\n", w, h);
  /* w = 100, h = 100; */
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i++)
    fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  // read info rom VGACTL_ADDR
  uint16_t width = inw(VGACTL_ADDR);
  uint16_t height = inw(VGACTL_ADDR + 2);
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = width,
                           .height = height,
                           .vmemsz = 0};

  /* *cfg = (AM_GPU_CONFIG_T) { */
  /*   .present = true, .has_accel = false, */
  /*   .width = 0, .height = 0, */
  /*   .vmemsz = 0 */
  /* }; */
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  /* AM帧缓冲控制器, 可写入绘图信息,
   * 向屏幕(x, y)坐标处绘制w*h的矩形图像.
   * 图像像素按行优先方式存储在pixels中,
   * 每个像素用32位整数以00RRGGBB的方式描述颜色. */
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *pixels = ctl->pixels;
  uint32_t offset = ctl->x * ctl->w + ctl->y;
  for (int i = 0; i < ctl->w; ++i) {
    for (int j = 0; j < ctl->h; ++j) {
      uint32_t pixel = pixels[i * ctl->w + j];
      fb[offset + i * ctl->w + j] = pixel;
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
