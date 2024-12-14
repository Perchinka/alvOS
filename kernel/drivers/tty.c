#include "../include/tty.h"
#include "../include/vga.h"
#include <stdint.h>

// Cursor
static size_t tty_row = 0;
static size_t tty_column = 0;
static uint8_t tty_color = VGA_COLOR_GREEN;

void tty_initialize() {
  vga_initialize();
  tty_row = 0;
  tty_column = 0;
  tty_color = VGA_COLOR_GREEN;
}

void tty_write(const char *str) {
  while (*str) {
    tty_putchar(*str++);
  }
}

void tty_putchar(char c) {
  switch (c) {
  case '\n': // Handle newline
    tty_column = 0;
    tty_row++;
    if (tty_row >= VGA_HEIGHT) { // Scroll the screen if needed
      tty_row = VGA_HEIGHT - 1;
      // Scroll up
      for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
          size_t from = y * VGA_WIDTH + x;
          size_t to = (y - 1) * VGA_WIDTH + x;
          VGA_MEMORY[to] = VGA_MEMORY[from];
        }
      }
      // Clear the last line
      for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_put_char(' ', tty_color, x, VGA_HEIGHT - 1);
      }
    }
    break;

  case '\t': // Handle tab (simplified as 4 spaces)
    for (int i = 0; i < 4; i++) {
      tty_putchar(' '); // Insert 4 spaces for a tab
    }
    break;

  case '\b': // Handle backspace
    if (tty_column > 0) {
      tty_column--;
    } else if (tty_row > 0) {
      tty_row--;
      tty_column = VGA_WIDTH - 1;
    }
    vga_put_char(' ', tty_color, tty_column, tty_row);
    break;

  default: // Handle regular characters
    vga_put_char(c, tty_color, tty_column, tty_row);
    tty_column++;
    if (tty_column >= VGA_WIDTH) { // Wrap to the next line
      tty_column = 0;
      tty_row++;
      if (tty_row >= VGA_HEIGHT) { // Scroll if needed
        tty_row = VGA_HEIGHT - 1;
        // Scroll up (same logic as '\n')
        for (size_t y = 1; y < VGA_HEIGHT; y++) {
          for (size_t x = 0; x < VGA_WIDTH; x++) {
            size_t from = y * VGA_WIDTH + x;
            size_t to = (y - 1) * VGA_WIDTH + x;
            VGA_MEMORY[to] = VGA_MEMORY[from];
          }
        }
        // Clear the last line
        for (size_t x = 0; x < VGA_WIDTH; x++) {
          vga_put_char(' ', tty_color, x, VGA_HEIGHT - 1);
        }
      }
    }
    break;
  }
}

#define INPUT_BUFFER_SIZE 256
static char input_buffer[INPUT_BUFFER_SIZE];
static size_t input_index = 0;

void tty_read_line(char *line) {
  while (1) {
    if (input_index > 0 && input_buffer[input_index - 1] == '\n') {
      break;
    }
  }

  for (size_t i = 0; i < input_index - 1; i++) { // Exclude newline
    line[i] = input_buffer[i];
  }
  line[input_index - 1] = '\0'; // Null-terminate the string

  input_index = 0;
}
