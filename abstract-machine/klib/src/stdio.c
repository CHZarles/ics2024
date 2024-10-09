#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) { panic("Not implemented"); }

int vsprintf(char *out, const char *fmt, va_list ap) {
  // using vsnprintf to implement vsprintf
  int ret = vsnprintf(out, 0x7fffffff, fmt, ap);
  return ret;
}

int sprintf(char *out, const char *fmt, ...) {
  // using vsprintf to implement sprintf
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  // using vsnprintf to implement snprintf
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int i = 0;
  for (; i < n && fmt[i] != '\0'; i++) {
    // %s
    if (fmt[i] == '%' && fmt[i + 1] == 's') {
      char *str = va_arg(ap, char *);
      int j = 0;
      for (; j < strlen(str) && i < n; j++, i++) {
        out[i] = str[j];
      }
      i--;
    } else if (fmt[i] == '%' && fmt[i + 1] == 'd') {
      int num = va_arg(ap, int);
      char buf[20];
      int j = 0;
      if (num == 0) {
        buf[j++] = '0';
      } else {
        if (num < 0) {
          out[i++] = '-';
          num = -num;
        }
        while (num > 0) {
          buf[j++] = num % 10 + '0';
          num /= 10;
        }
      }
      for (int k = j - 1; k >= 0 && i < n; k--, i++) {
        out[i] = buf[k];
      }
      i--;

    } else {
      out[i] = fmt[i];
    }
  }
  if (i < n)
    out[i] = '\0';
  else
    out[n - 1] = '\0';

  return i;
  /* panic("Not implemented"); */
}

#endif
