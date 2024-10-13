#include <am.h>
#include <nemu.h>
#include <stdio.h>
void __am_timer_init() {}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t low32 = inl(RTC_ADDR);
  uint32_t high32 = inl(RTC_ADDR + 4);
  uptime->us = ((uint64_t)high32 << 32) + low32;
  printf("low32: %d \n", low32);
  printf("high32 %d \n", high32);
  printf("uptime: %ld \n", uptime->us);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour = 0;
  rtc->day = 0;
  rtc->month = 0;
  rtc->year = 1900;
}
