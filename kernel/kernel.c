#include "include/idt.h"
#include "include/irq.h"
#include "include/isr.h"
#include "include/keyboard.h"
#include "include/screen.h"
#include "include/timer.h"

void kernel_main() {
  idt_init();
  isr_init();
  irq_init();
  timer_init();

  screen_clear(0x00);
  screen_draw_string("Hello graphics world", 10, 10, 0x3F);

  keyboard_init();
  while (1) {
  }
}
