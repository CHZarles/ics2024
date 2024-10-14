#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keydown = true;
  kbd->keycode = AM_KEY_NONE;
  // read key from
  kbd->keycode = inl(KBD_ADDR);
  if (kbd->keycode == AM_KEY_NONE) {
    kbd->keydown = false;
  }
  int k = AM_KEY_NONE;
  k = inl(KBD_ADDR);
  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  kbd->keycode = k & ~KEYDOWN_MASK;
}
