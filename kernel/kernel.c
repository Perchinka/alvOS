#include "include/idt.h"
#include "include/irq.h"
#include "include/isr.h"
#include "include/keyboard.h"
#include "include/timer.h"
#include "include/tty.h"

void kernel_main() {
  idt_init();
  isr_init();
  irq_init();
  timer_init();

  tty_initialize();
  tty_write("Welcome to alvOS 0.0.1\n");

  keyboard_init();
  while (true) {
    asm("hlt");
  }
}
