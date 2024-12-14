#include <stddef.h>

#ifndef STRING_H
#define STRING_H
#define VGA_MEMORY ((uint16_t *)0xB8000)

size_t strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);

#endif
