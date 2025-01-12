#ifndef UTILS_H
#define UTILS_H

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

// ----- Memory -----

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

#define HEAP_SIZE 1024 * 1024 // 1 MB

typedef struct MemoryBlock {
  size_t size;              // Size of the block
  struct MemoryBlock *next; // Pointer to the next block
  u8 free;                  // Is the block free?
} MemoryBlock;

static u8 heap[HEAP_SIZE];            // Static memory for the heap
static MemoryBlock *free_list = NULL; // Pointer to the first free block

static inline void init_heap(void) {
  free_list = (MemoryBlock *)heap;
  free_list->size = HEAP_SIZE - sizeof(MemoryBlock);
  free_list->next = NULL;
  free_list->free = 1;
}

static inline void *malloc(size_t size) {
  MemoryBlock *current = free_list;

  while (current != NULL) {
    if (current->free && current->size >= size) {

      // If the block is larger than required, split it
      if (current->size > size + sizeof(MemoryBlock)) {
        MemoryBlock *new_block =
            (MemoryBlock *)((u8 *)current + sizeof(MemoryBlock) + size);
        new_block->size = current->size - size - sizeof(MemoryBlock);
        new_block->next = current->next;
        new_block->free = 1;

        current->size = size;
        current->next = new_block;
      }
      current->free = 0;
      return (void *)((u8 *)current + sizeof(MemoryBlock));
    }
    current = current->next;
  }

  // If no suitable block found, return NULL
  return NULL;
}

static inline void free(void *ptr) {
  if (ptr == NULL) {
    return;
  }

  MemoryBlock *block = (MemoryBlock *)((u8 *)ptr - sizeof(MemoryBlock));
  block->free = 1;

  MemoryBlock *current = free_list;
  while (current != NULL) {
    if (current->free && current->next && current->next->free) {
      current->size += current->next->size + sizeof(MemoryBlock);
      current->next = current->next->next;
    }
    current = current->next;
  }
}

// ----- Strings -----

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

static inline char *strcat(char *dest, const char *src) {
  char *ptr = dest;

  while (*ptr != '\0') {
    ptr++;
  }

  while (*src != '\0') {
    *ptr++ = *src++;
  }

  *ptr = '\0';

  return dest;
}
// ----- Converters -----
static inline void ftoa(f32 value, char *buffer, int precision) {
  if (value < 0) {
    *buffer++ = '-';
    value = -value;
  }

  i32 int_part = (i32)value;
  f32 fractional_part = value - (f32)int_part;

  char temp[12];
  char *temp_ptr = temp;
  if (int_part == 0) {
    *temp_ptr++ = '0';
  } else {
    while (int_part > 0) {
      *temp_ptr++ = '0' + (int_part % 10);
      int_part /= 10;
    }
  }
  *temp_ptr = '\0';

  for (char *start = temp, *end = temp_ptr - 1; start < end; start++, end--) {
    char t = *start;
    *start = *end;
    *end = t;
  }

  strcpy(buffer, temp);
  buffer += strlen(temp);

  *buffer++ = '.';

  for (int i = 0; i < precision; i++) {
    fractional_part *= 10;
  }
  i32 frac_as_int = (i32)(fractional_part + 0.5f);
  temp_ptr = temp;
  if (frac_as_int == 0) {
    *temp_ptr++ = '0';
  } else {
    while (frac_as_int > 0) {
      *temp_ptr++ = '0' + (frac_as_int % 10);
      frac_as_int /= 10;
    }
  }
  *temp_ptr = '\0';

  for (char *start = temp, *end = temp_ptr - 1; start < end; start++, end--) {
    char t = *start;
    *start = *end;
    *end = t;
  }

  strcpy(buffer, temp);
  buffer += strlen(temp);

  *buffer = '\0';
}

static inline char *itoa(int value, char *buffer) {
  char temp[12];
  int i = 0;
  int is_negative = (value < 0);

  if (is_negative) {
    value = -value;
  }

  // Generate digits in reverse order
  do {
    temp[i++] = (value % 10) + '0';
    value /= 10;
  } while (value > 0);

  if (is_negative) {
    temp[i++] = '-';
  }

  // Reverse the string into the buffer
  int j = 0;
  while (i > 0) {
    buffer[j++] = temp[--i];
  }
  buffer[j] = '\0';

  return buffer;
}
#endif
