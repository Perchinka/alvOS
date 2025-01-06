#include "../include/screen.h"
#include "../include/font.h"
#include "../include/system.h"
#include "../include/utils.h"

static u8 *BUFFER = (u8 *)0xA0000;

// VGA control port addresses
#define PALETTE_MASK 0x3C6
#define PALETTE_READ 0x3C7
#define PALETTE_WRITE 0x3C8
#define PALETTE_DATA 0x3C9

void screen_init() {
  outb(PALETTE_MASK, 0xFF);
  outb(PALETTE_WRITE, 0);
  for (u8 i = 0; i < 255; i++) {
    outb(PALETTE_DATA, (((i >> 5) & 0x7) * (256 / 8)) / 4);
    outb(PALETTE_DATA, (((i >> 2) & 0x7) * (256 / 8)) / 4);
    outb(PALETTE_DATA, (((i >> 0) & 0x3) * (256 / 4)) / 4);
  }

  // set color 255 = white
  outb(PALETTE_DATA, 0x3F);
  outb(PALETTE_DATA, 0x3F);
  outb(PALETTE_DATA, 0x3F);
}

void screen_clear(u8 color) { memset(BUFFER, color, SCREEN_SIZE); }

void screen_set_pixel(u8 color, u16 x, u16 y) {
  if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
    BUFFER[y * SCREEN_WIDTH + x] = color;
  }
}

void screen_draw_char(char c, u16 x, u16 y, u8 color) {
  assert(c >= 0, "INVALID CHARACTER");

  const u8 *glyph = FONT[(size_t)c];

  for (size_t yy = 0; yy < 8; yy++) {
    for (size_t xx = 0; xx < 8; xx++) {
      if (glyph[yy] & (1 << xx)) {
        screen_set_pixel(color, x + xx, y + yy);
      }
    }
  }
}
void screen_draw_string(const char *str, u16 x, u16 y, u8 color) {
  char c;

  while ((c = *str++) != 0) {
    screen_draw_char(c, x, y, color);
    x += 8;
  }
}

void screen_draw_line(u8 color, u16 x0, u16 y0, u16 x1, u16 y1) {
  // Ensure both points are within bounds
  if ((x0 < 0 && x1 < 0) || (x0 >= SCREEN_WIDTH && x1 >= SCREEN_WIDTH) ||
      (y0 < 0 && y1 < 0) || (y0 >= SCREEN_HEIGHT && y1 >= SCREEN_HEIGHT)) {
    return; // Entire line is outside the screen
  }

  // Clip the line to the screen boundaries using a simplified Cohen-Sutherland
  // approach
  const int LEFT = 0, RIGHT = SCREEN_WIDTH - 1, TOP = 0,
            BOTTOM = SCREEN_HEIGHT - 1;

  // Clip x0, y0
  if (x0 < LEFT) {
    y0 += (y1 - y0) * (LEFT - x0) / (x1 - x0);
    x0 = LEFT;
  } else if (x0 > RIGHT) {
    y0 += (y1 - y0) * (RIGHT - x0) / (x1 - x0);
    x0 = RIGHT;
  }
  if (y0 < TOP) {
    x0 += (x1 - x0) * (TOP - y0) / (y1 - y0);
    y0 = TOP;
  } else if (y0 > BOTTOM) {
    x0 += (x1 - x0) * (BOTTOM - y0) / (y1 - y0);
    y0 = BOTTOM;
  }

  // Clip x1, y1
  if (x1 < LEFT) {
    y1 += (y0 - y1) * (LEFT - x1) / (x0 - x1);
    x1 = LEFT;
  } else if (x1 > RIGHT) {
    y1 += (y0 - y1) * (RIGHT - x1) / (x0 - x1);
    x1 = RIGHT;
  }
  if (y1 < TOP) {
    x1 += (x0 - x1) * (TOP - y1) / (y0 - y1);
    y1 = TOP;
  } else if (y1 > BOTTOM) {
    x1 += (x0 - x1) * (BOTTOM - y1) / (y0 - y1);
    y1 = BOTTOM;
  }

  // Bresenham's algorithm after clipping
  int dx = abs(x1 - x0);
  int dy = -abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (1) {
    // Draw only if within bounds
    if (x0 >= 0 && x0 < SCREEN_WIDTH && y0 >= 0 && y0 < SCREEN_HEIGHT) {
      screen_set_pixel(color, x0, y0);
    }

    if (x0 == x1 && y0 == y1)
      break;

    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void screen_fill_rect(u8 color, u16 x, u16 y, u16 width, u16 height) {
  for (u16 yy = 0; yy < height; yy++) {
    for (u16 xx = 0; xx < width; xx++) {
      screen_set_pixel(color, x + xx, y + yy);
    }
  }
}
