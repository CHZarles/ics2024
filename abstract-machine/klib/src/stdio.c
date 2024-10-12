#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <limits.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {

  // use putch to implement this function
  va_list ap;
  va_start(ap, fmt);
  int out_idx = 0;
  int fmt_idx = 0;
  while (fmt[fmt_idx] != '\0') {
    if (fmt[fmt_idx] == '%' && fmt[fmt_idx + 1] == 's') {
      fmt_idx += 2;
      char *str = va_arg(ap, char *);
      for (int j = 0; str[j] != '\0'; j++) {
        putch(str[j]);
      }
    } else if (fmt[fmt_idx] == '%' && fmt[fmt_idx + 1] == 'd') {
      fmt_idx += 2;
      int num = va_arg(ap, int);
      char buf[20];
      int j = 0;
      if (num == 0) {
        buf[j++] = '0';
      } else {
        if (num < 0) {
          putch('-');
          num = -num;
        }
        while (num > 0) {
          buf[j++] = num % 10 + '0';
          num /= 10;
        }
      }
      for (int k = j - 1; k >= 0; k--) {
        putch(buf[k]);
      }
    } else {
      putch(fmt[fmt_idx++]);
    }
  }

  return out_idx;
}

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
  int out_idx = 0;
  int fmt_idx = 0;
  while (out_idx < n - 1 && fmt[fmt_idx] != '\0') {
    if (fmt[fmt_idx] == '%' && fmt[fmt_idx + 1] == 's') {
      fmt_idx += 2;
      char *str = va_arg(ap, char *);
      for (int j = 0; str[j] != '\0' && out_idx < n - 1; j++, out_idx++) {
        out[out_idx] = str[j];
      }
    } else if (fmt[fmt_idx] == '%' && fmt[fmt_idx + 1] == 'd') {
      fmt_idx += 2;
      int num = va_arg(ap, int);
      char buf[20];
      int j = 0;
      if (num == 0) {
        buf[j++] = '0';
      } else if (num == INT_MIN) {
        printf("INT_MIN\n");
        out[out_idx++] = '-';
        /* strcpy(buf, "2147483648"); */
        strcpy(buf, "8463847412)");
        j += 10;
      } else {
        if (num < 0) {
          if (out_idx < n - 1) {
            out[out_idx++] = '-';
          }
          num = -num;
        }
        while (num > 0) {
          buf[j++] = num % 10 + '0';
          num /= 10;
        }
      }
      for (int k = j - 1; k >= 0 && out_idx < n - 1; k--, out_idx++) {
        out[out_idx] = buf[k];
      }
    } else {
      out[out_idx++] = fmt[fmt_idx++];
    }
  }
  out[out_idx] = '\0';
  return out_idx;
}

#endif
