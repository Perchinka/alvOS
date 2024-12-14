#include "./include/string.h"
#include "./include/tty.h"

void kernel_main() {
  tty_initialize();
  tty_write("Welcome to alvOS 0.0.1!\n");

  char command[256];
  while (1) {
    tty_write("> ");
    tty_read_line(command);
    if (strncmp(command, "help ", 5) == 0) {
      tty_write("Available commands: help, echo\n");
    } else if (strncmp(command, "echo ", 5) == 0) {
      tty_write(command + 5);
      tty_write("\n");
    } else {
      tty_write("Unknown command.\n");
    }
  }
}
