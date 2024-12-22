#include "screen.h"
#include "font.h"
#include "system.h"
#include "utils.h"

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

void screen_set(u8 color, u16 x, u16 y) {
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
        screen_set(color, x + xx, y + yy);
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
