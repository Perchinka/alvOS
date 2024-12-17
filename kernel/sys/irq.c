#include "../include/irq.h"
#include "../include/isr.h"
#include "../include/system.h"

// PIC constants
#define PIC1 0x20
#define PIC1_OFFSET 0x20
#define PIC1_DATA (PIC1 + 1)

#define PIC2 0xA0
#define PIC2_OFFSET 0x28
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20
#define PIC_MODE_8086 0x01
#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10

#define IRQ_START 32
#define IRQ_END 47
#define IRQ2 2
#define EOI_THRESHOLD 0x40 // Threshold for PIC2 EOI handling

// Inline wait macro
#define PIC_WAIT()                                                             \
  do {                                                                         \
    asm("jmp 1f\n\t"                                                           \
        "1:\n\t"                                                               \
        "jmp 2f\n\t"                                                           \
        "2:");                                                                 \
  } while (0)

// IRQ handlers
static void (*handlers[16])(struct Registers *regs) = {0};

static void stub(struct Registers *regs) {
  if (regs->int_no >= IRQ_START && regs->int_no <= IRQ_END) {
    size_t irq_line = regs->int_no - IRQ_START;
    if (handlers[irq_line]) {
      handlers[irq_line](regs);
    }
  }

  // Send End-of-Interrupt (EOI)
  if (regs->int_no >= EOI_THRESHOLD) {
    outb(PIC2, PIC_EOI);
  }
  outb(PIC1, PIC_EOI);
}

// Remap IRQs from PIC1 and PIC2
static void irq_remap() {
  const u8 mask1 = inb(PIC1_DATA);
  const u8 mask2 = inb(PIC2_DATA);

  outb(PIC1, ICW1_INIT | ICW1_ICW4);
  PIC_WAIT();
  outb(PIC2, ICW1_INIT | ICW1_ICW4);
  PIC_WAIT();

  outb(PIC1_DATA, PIC1_OFFSET);
  PIC_WAIT(); // Set PIC1 base offset
  outb(PIC2_DATA, PIC2_OFFSET);
  PIC_WAIT(); // Set PIC2 base offset

  outb(PIC1_DATA, (1 << IRQ2));
  PIC_WAIT(); // Tell PIC1 about PIC2 (IRQ2)
  outb(PIC2_DATA, IRQ2);
  PIC_WAIT(); // Tell PIC2 its cascade identity

  outb(PIC1_DATA, PIC_MODE_8086);
  PIC_WAIT();
  outb(PIC2_DATA, PIC_MODE_8086);
  PIC_WAIT();

  outb(PIC1_DATA, mask1); // Restore saved masks
  outb(PIC2_DATA, mask2);
}

// Set mask to disable an IRQ
static void irq_set_mask(size_t irq_line) {
  const u16 port = irq_line < 8 ? PIC1_DATA : PIC2_DATA;
  const u8 value = inb(port) | (1 << (irq_line % 8));
  outb(port, value);
}

// Clear mask to enable an IRQ
static void irq_clear_mask(size_t irq_line) {
  const u16 port = irq_line < 8 ? PIC1_DATA : PIC2_DATA;
  const u8 value = inb(port) & ~(1 << (irq_line % 8));
  outb(port, value);
}

// Install an IRQ handler
void irq_install(size_t irq_line, void (*handler)(struct Registers *)) {
  if (irq_line >= 16) {
    // Optionally handle invalid IRQ line
    panic("IRQ line out of range");
    return;
  }
  CLI(); // Disable interrupts
  handlers[irq_line] = handler;
  irq_clear_mask(irq_line); // Enable IRQ line
  STI();                    // Re-enable interrupts
}

// Initialize the IRQ system
void irq_init() {
  irq_remap();

  for (size_t i = 0; i < 16; i++) {
    isr_install(IRQ_START + i, stub); // Map IRQ handlers to ISR stubs
  }
}
