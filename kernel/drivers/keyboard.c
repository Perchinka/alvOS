#include "../include/tty.h"
#include <stdint.h>

// I/O port functions
static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static inline void outb(uint16_t port, uint8_t value) {
  __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Keymap (simplified US QWERTY)
static const char keymap[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', // Backspace
    '\t', // Tab
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', // Enter
    0,                                                                // Ctrl
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,   // Left
                                                                      // Shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, // Right Shift
    '*', 0, ' ',                                               // Space
    // Remaining keys can be added as needed
};

// Keyboard input buffer
#define INPUT_BUFFER_SIZE 256
static char input_buffer[INPUT_BUFFER_SIZE];
static size_t input_index = 0;

void keyboard_poll() {
  // Check if data is available in the output buffer
  if (inb(0x64) & 0x01) {
    uint8_t scan_code = inb(0x60); // Read scan code
    if (scan_code < 128) {         // Ignore key releases
      char key = keymap[scan_code];
      if (key) {
        if (key == '\n') {
          // Process newline
          input_buffer[input_index++] = key;
          input_buffer[input_index] = '\0';
          tty_write(input_buffer);
          input_index = 0; // Reset buffer
        } else if (key == '\b') {
          // Handle backspace
          if (input_index > 0) {
            input_index--;
          }
        } else if (input_index < INPUT_BUFFER_SIZE - 1) {
          input_buffer[input_index++] = key;
        }
      }
    }
  }
}
