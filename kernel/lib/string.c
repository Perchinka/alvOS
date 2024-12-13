#include <stddef.h>

size_t strlen(const char *str) {
  size_t len = 0;
  while (*str++) {
    len++;
  }
  return len;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strcpy(char *dest, const char *src) {
  char *original_dest = dest;

  while (*src != '\0') {
    *dest = *src;
    dest++;
    src++;
  }

  *dest = '\0';

  return original_dest;
}
