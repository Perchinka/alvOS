#ifndef UTIL_H
#define UTIL_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef u32 size_t;
typedef u32 uintptr_t;
typedef float f32;
typedef double f64;

typedef u8 bool;
#define true (1)
#define false (0)

#define NULL (0)
#define PACKED __attribute__((packed))

#ifndef asm
#define asm __asm__ volatile
#endif

#define CLI() asm("cli")
#define STI() asm("sti")

#define HIBIT(_x) (31 - __builtin_clz((_x)))

// returns the lowest set bit of x
#define LOBIT(_x)                                                              \
  __extension__({                                                              \
    __typeof__(_x) __x = (_x);                                                 \
    HIBIT(__x & -__x);                                                         \
  })

// returns _v with _n-th bit = _x
#define BIT_SET(_v, _n, _x)                                                    \
  __extension__({                                                              \
    __typeof__(_v) __v = (_v);                                                 \
    (__v ^ ((-(_x) ^ __v) & (1 << (_n))));                                     \
  })
static inline u16 inports(u16 port) {
  u16 r;
  asm("inw %1, %0" : "=a"(r) : "dN"(port));
  return r;
}

static inline void outports(u16 port, u16 data) {
  asm("outw %1, %0" : : "dN"(port), "a"(data));
}

static inline u8 inb(u16 port) {
  u8 r;
  asm("inb %1, %0" : "=a"(r) : "dN"(port));
  return r;
}

static inline void outb(u16 port, u8 data) {
  asm("outb %1, %0" : : "dN"(port), "a"(data));
}

static inline size_t strlen(const char *str) {
  size_t len = 0;
  while (*str++) {
    len++;
  }
  return len;
}

static inline int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

static inline int strncmp(const char *s1, const char *s2, size_t n) {
  while (n > 0) {
    if (*s1 != *s2) {
      return *(unsigned char *)s1 - *(unsigned char *)s2;
    }
    if (*s1 == '\0') {
      break;
    }
    s1++;
    s2++;
    n--;
  }
  return 0;
}

static inline char *strcpy(char *dest, const char *src) {
  char *original_dest = dest;

  while (*src != '\0') {
    *dest = *src;
    dest++;
    src++;
  }

  *dest = '\0';

  return original_dest;
}

static inline void memset(void *dst, u8 value, size_t n) {
  u8 *d = dst;

  while (n-- > 0) {
    *d++ = value;
  }
}

static inline void *memcpy(void *dst, const void *src, size_t n) {
  u8 *d = dst;
  const u8 *s = src;

  while (n-- > 0) {
    *d++ = *s++;
  }

  return d;
}

static inline void *memmove(void *dst, const void *src, size_t n) {
  if (dst < src) {
    return memcpy(dst, src, n);
  }

  u8 *d = dst;
  const u8 *s = src;

  for (size_t i = n; i > 0; i--) {
    d[i - 1] = s[i - 1];
  }

  return dst;
}

#endif
