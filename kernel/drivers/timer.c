#include "../include/timer.h"
#include "../include/irq.h"
#include "../include/isr.h"

#define PIT_A 0x40
#define PIT_B 0x41
#define PIT_C 0x42
#define PIT_CONTROL 0x43

#define PIT_MASK 0xFF
#define PIT_SET 0x36 // Square Wave Generation

#define PIT_HZ                                                                 \
  1193181 // Not sure if it's right as it's frequency that works for IBM
          // PC-compatible systems, but as long as alvOS works thats good enough
#define DIV_OF_FREQ(_frequency) (PIT_HZ / (_frequency))
#define FREQ_OF_DIV(_divisor) (PIT_HZ / (_divisor))
#define REAL_FREQ_OF_FREQ(_f) (FREQ_OF_DIV(DIV_OF_FREQ((_f))))

static struct {
  u64 frequency;
  u64 divisor;
  u64 ticks;
} state;

static void timer_set(int hz) {
  outb(PIT_CONTROL, PIT_SET);

  u16 d = (u16)(1193131.666 / hz);
  outb(PIT_A, d & PIT_MASK);        // low byte
  outb(PIT_A, (d >> 8) & PIT_MASK); // high byte
}

u64 timer_get() { return state.ticks; }

static void timer_handler(struct Registers *regs) { state.ticks++; }

void timer_init() {
  const u64 freq = REAL_FREQ_OF_FREQ(TIMER_TPS);
  state.frequency = freq;
  state.divisor = DIV_OF_FREQ(freq);
  state.ticks = 0;

  timer_set(TIMER_TPS);
  irq_install(0, timer_handler);
}
