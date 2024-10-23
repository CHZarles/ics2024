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
  if (kbd.keydown == 1) {
    size_t key = kbd.keycode;
    sprintf(buf, "kd %s", keyname[kbd.keycode]);
    // wait for keyup
    while (1) {
      kbd = io_read(AM_INPUT_KEYBRD);
      if (kbd.keydown == 0 && kbd.keycode == AM_KEY_NONE) {
        sprintf(buf, "ku %s", keyname[key]);
        return 0;
      }
    }
  }
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) { return 0; }

size_t fb_write(const void *buf, size_t offset, size_t len) { return 0; }

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
