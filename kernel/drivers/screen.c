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
  int dx = abs(x1 - x0);
  int dy = -abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (true) {
    screen_set_pixel(color, x0, y0);
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

void screen_draw_circle(u8 color, u16 xc, u16 yc, u16 radius) {
  int x = 0, y = radius;
  int d = 3 - 2 * radius;

  while (y >= x) {
    screen_set_pixel(color, xc + x, yc + y);
    screen_set_pixel(color, xc - x, yc + y);
    screen_set_pixel(color, xc + x, yc - y);
    screen_set_pixel(color, xc - x, yc - y);
    screen_set_pixel(color, xc + y, yc + x);
    screen_set_pixel(color, xc - y, yc + x);
    screen_set_pixel(color, xc + y, yc - x);
    screen_set_pixel(color, xc - y, yc - x);

    x++;

    if (d > 0) {
      y--;
      d = d + 4 * (x - y) + 10;
    } else {
      d = d + 4 * x + 6;
    }
  }
}

void screen_draw_ellipse(u8 color, float xc, float yc, float a, float b,
                         float rotation) {
  const int segments = 100;
  float angle_step = 2.0f * PI / segments;

  float prev_x = a * cos(0);
  float prev_y = b * sin(0);

  float cos_r = cos(rotation);
  float sin_r = sin(rotation);
  float rotated_prev_x = prev_x * cos_r - prev_y * sin_r + xc;
  float rotated_prev_y = prev_x * sin_r + prev_y * cos_r + yc;

  for (int i = 1; i <= segments; i++) {
    float angle = i * angle_step;
    float x = a * cos(angle);
    float y = b * sin(angle);

    float rotated_x = x * cos_r - y * sin_r + xc;
    float rotated_y = x * sin_r + y * cos_r + yc;

    screen_draw_line(color, rotated_prev_x, rotated_prev_y, rotated_x,
                     rotated_y);

    rotated_prev_x = rotated_x;
    rotated_prev_y = rotated_y;
  }
}
