#include "./include/tty.h"

void kernel_main() {
  tty_initialize();
  tty_write("Hello, OS World!\n");
  tty_write("This is a basic TTY driver using VGA.\n");
}
