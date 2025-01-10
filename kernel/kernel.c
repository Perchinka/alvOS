#include "include/asteroids.h"
#include "include/idt.h"
#include "include/irq.h"
#include "include/isr.h"
#include "include/keyboard.h"
#include "include/screen.h"
#include "include/system.h"
#include "include/timer.h"

void kernel_main() {
  idt_init();
  isr_init();
  irq_init();
  timer_init();
  screen_init();
  keyboard_init();

  game_loop();
}
