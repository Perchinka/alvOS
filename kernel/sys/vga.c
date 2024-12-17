#include "../include/vga.h"

static uint16_t *vga_buffer = VGA_MEMORY;

void vga_initialize() { vga_clear(); }

void vga_put_char(char c, uint8_t color, size_t x, size_t y) {
  if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
    return;
  }
  size_t index = y * VGA_WIDTH + x;
  vga_buffer[index] = (uint16_t)c | ((uint16_t)color << 8);
}

void vga_write_string(const char *str, uint8_t color, size_t x, size_t y) {
  while (*str) {
    vga_put_char(*str++, color, x, y);
    if (x >= VGA_WIDTH) { // Wrap to the next line
      x = 0;
      y++;
    }
    if (y >= VGA_HEIGHT) { // Stop if we exceed screen height for now, latter
                           // will make it scroll somehow :D
      break;
    }
  }
}

void vga_clear() {
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      vga_put_char(' ', VGA_COLOR_BLACK, x, y);
    }
  }
}
