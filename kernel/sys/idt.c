#include "../include/idt.h"

typedef struct PACKED {
  u16 offset_low;  // Lower 16 bits of ISR handler address
  u16 selector;    // Code segment selector
  u8 zero;         // Always zero
  u8 type;         // Flags and gate type
  u16 offset_high; // Upper 16 bits of ISR handler address
} IDTEntry;

typedef struct PACKED {
  u16 limit;      // Size of IDT table
  uintptr_t base; // Base address of IDT table
} IDTPointer;

static struct {
  IDTEntry entries[256];
  IDTPointer pointer;
} idt;

// In entry_point.asm
extern void idt_load(uintptr_t idt_ptr);

#define IDT_USER_MODE 0x60

void idt_set(u8 index, void (*base)(struct Registers *), u16 selector,
             u8 flags) {
  idt.entries[index] = (IDTEntry){
      .offset_low = (uintptr_t)base & 0xFFFF,
      .offset_high = ((uintptr_t)base >> 16) & 0xFFFF,
      .selector = selector,
      .type = flags | IDT_USER_MODE,
      .zero = 0,
  };
}

void idt_init() {
  idt.pointer.limit = sizeof(idt.entries) - 1;
  idt.pointer.base = (uintptr_t)&idt.entries[0];

  memset(&idt.entries[0], 0, sizeof(idt.entries));

  idt_load((uintptr_t)&idt.pointer);
}
