#include <am.h>
#include <nemu.h>

#define RTC_ADDR_SEC  (RTC_ADDR + 4)
//uint32_t uptime();

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  // struct timeval now;
  // gettimeofday(&now, NULL);
  // long seconds = now.tv_sec - inl(RTC_ADDR + 4);
  // long useconds = now.tv_usec - inl(RTC_ADDR);
  // uptime->us = seconds * 1000000 + (useconds + 500);
  uptime->us = inl(RTC_ADDR_SEC) * 1000000 + inl(RTC_ADDR) + 500;
  //uint32_t time = uptime();
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
