#ifndef SCREEN_H
#define SCREEN_H

#include "math.h"
#include "utils.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#define COLOR(_r, _g, _b)                                                      \
  ((u8)((((_r) & 0x7) << 5) | (((_g) & 0x7) << 2) | (((_b) & 0x3) << 0)))

#define COLOR_R(_index) (((_index) >> 5) & 0x7)
#define COLOR_G(_index) (((_index) >> 2) & 0x7)
#define COLOR_B(_index) (((_index) >> 0) & 0x3)

#define COLOR_ADD(_index, _d)                                                  \
  __extension__({                                                              \
    __typeof__(_index) _c = (_index);                                          \
    __typeof__(_d) __d = (_d);                                                 \
    COLOR(CLAMP(COLOR_R(_c) + __d, 0, 7), CLAMP(COLOR_G(_c) + __d, 0, 7),      \
          CLAMP(COLOR_B(_c) + __d, 0, 3));                                     \
  })
void screen_init();
void screen_clear(u8 color);
void screen_set_pixel(u8 color, u16 x, u16 y);
void screen_draw_char(char c, u16 x, u16 y, u8 color);
void screen_draw_string(const char *str, u16 x, u16 y, u8 color);
void screen_draw_line(u8 color, u16 x0, u16 y0, u16 x1, u16 y1);
void screen_fill_rect(u8 color, u16 x, u16 y, u16 width, u16 height);

#endif // SCREEN_H
