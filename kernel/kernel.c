#include "include/asteroids.h"
#include "include/idt.h"
#include "include/irq.h"
#include "include/isr.h"
#include "include/keyboard.h"
#include "include/screen.h"
#include "include/timer.h"

#define FPS 30

void kernel_main() {
  idt_init();
  isr_init();
  irq_init();
  timer_init();
  screen_init();
  keyboard_init();

  u32 last_frame = 0;
  while (true) {
    const u32 now = (u32)timer_get();

    if ((now - last_frame) > (TIMER_TPS / FPS)) {
      last_frame = now;
    }
  }
}
