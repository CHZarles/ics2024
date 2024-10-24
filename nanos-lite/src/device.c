#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) [AM_KEY_##key] = #key,

static const char *keyname[256]
    __attribute__((used)) = {[AM_KEY_NONE] = "NONE", AM_KEYS(NAME)};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  /* return 0; */
  for (size_t i = 0; i < len; i++) {
    putch(((char *)buf)[i]);
  }
  return len;
}

size_t serial_read(void *buf, size_t offset, size_t len) { return 0; }

size_t events_read(void *buf, size_t offset, size_t len) {

  // 0. read keyboerd event from io_read keyboerd
  // 1. write event to buf
  /* 按下按键事件, 如kd RETURN表示按下回车键 */
  /* 松开按键事件, 如ku A表示松开A键 */
  AM_INPUT_KEYBRD_T kbd = io_read(AM_INPUT_KEYBRD);
  if (kbd.keydown) {
    // limit the length of buf
    int ret = snprintf(buf, len, "kd %s", keyname[kbd.keycode]);
    /* printf("Press key: %s\n", keyname[kbd.keycode]); */
    return ret;
  } else {
    if (kbd.keycode == AM_KEY_NONE) {
      return 0;
    }

    int ret = snprintf(buf, len, "ku %s", keyname[kbd.keycode]);
    /* printf("Release key: %s\n", keyname[kbd.keycode]); */

    return ret;
  }
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  // 0. read width and height from ioe
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  // 1. write width and height to buf with specified format
  int ret = sprintf(buf, "WIDTH: %d\nHEIGHT: %d\n", cfg.width, cfg.height);
  // NOTE: 返回值不确定, 讲义没用规定
  return ret;
}

// 这里是将frame buffer看成了流式文件
// 这里的 offset 是指的是要写入的位置（frame buffer的位置）
size_t fb_write(const void *buf, size_t offset, size_t len) {

  /* 0.从offset计算出frame buffer的数值对应的屏幕上的坐标 */
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int x = offset % cfg.width;
  int y = offset / cfg.width;
  /* 1.len是要写入的像素的个数 */
  // x + len < cfg.width ????
  int len_ = x + len < cfg.width ? len : cfg.width - x;
  /* 调用IOE来进行绘图.  */
  /* 将frame buffer中的内容同步到屏幕上. */
  printf("x = %d, y = %d, len = %d\n", x, y, len_);
  printf("Call io_write to draw\n");
  io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, len_, 1, true);
  // return what ????
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
