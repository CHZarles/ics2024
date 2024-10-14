#include <am.h>
#include <klib.h> // for debug
#include <nemu.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {

  int i;
/* int w = 0; // TODO: get the correct width */
/* int h = 0; // TODO: get the correct height */
#define N 32
  int w = io_read(AM_GPU_CONFIG).width / N;
  int h = io_read(AM_GPU_CONFIG).height / N;
  printf("w = %d, h = %d\n", w, h);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i++)
    fb[i] = 200;
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
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
