#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void itoa_base(int d, char *dest, int base) {
  assert(base == 10 || base == 16 || base == 2);
  int width = 1;
  int m = 0;
  while (d / base) {
    m = d % base;
    dest[width - 1] = m > 9 ? m - 10 + 'a' : m + '0';
    width++;
    d = d/base;
  }
  dest[width - 1] = d > 9 ? d - 10 + 'a' : d + '0';
  for (int i = 0, j = width - 1; i < j; ) {
    char temp = dest[i];
    dest[i] = dest[j];
    dest[j] = temp;
    i++;
    j--;
  }
  dest[width] = '\0';
}

void itoa(int d, char *dest) {
  itoa_base(d, dest, 10);
}

void itoa_hex(int d, char *dest) {
  itoa_base(d, dest, 16);
}

int printf(const char *fmt, ...) {
  va_list valist;
  int d;
  va_start(valist, fmt);
  for(const char *p = fmt; *p; p++) {
    if (*p == '%') {
      //putch(*p);
      switch (*(p + 1)) {
        case 'd':
          d = va_arg(valist, int);
          char s[20]; 
          itoa(d, s);
          putstr(s);
          p++;
          break;
        case 'x':
          d = va_arg(valist, int);
          char s2[20];
          itoa_hex(d, s2);
          putstr(s2);
          p++;
          break;
        case 's':
          putstr(va_arg(valist, char *));
          p++;
          break;
        case 'c':
          putch((char)va_arg(valist, int));
          p++;
          break;
        case 'p':
          d = (uintptr_t) va_arg(valist, void *);
          char s3[20];
          itoa_hex(d, s3);
          putstr(s3);
          p++;
          break;
        default:
          putch(*p);
          //putstr("error");
          break;
      }
    }else {
      putch(*p);
    }
  }
  va_end(valist);
  //putstr("varlist end\n");
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list valist;
  int d;
  int i = 0;
  va_start(valist, fmt);
  for(const char *p = fmt; *p; p++) {
    if (*p == '%') {
      switch (*(p + 1)) {
        case 'd':
          d = va_arg(valist, int);
          char s[20] = {0}; 
          itoa(d, s);
          for(const char *k = s; *k && *k != '\0'; k++) {
            out[i++] = *k;
          }
          p++;
          break;
        case 'x':
          d = va_arg(valist, int);
          char s2[20] = {0};
          itoa_hex(d, s2);
          for(const char *k2 = s2; *k2&&*k2 != '\0'; k2++) {
            out[i++] = *k2;
          }
          p++;
          break;
        case 's':
          for(const char *k3=va_arg(valist, char *); *k3&&*k3 != '\0'; k3++) {
            out[i++] = *k3;
          }
          p++;
          break;
        default:
          break;
      }
    }else {
      out[i++] = *p;
    }
  }
  va_end(valist);
  out[i] = '\0';
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  return 0;
}

#endif
