#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "utils.h"

// Key Definitions
#define KEY_NULL 0
#define KEY_ESC 27
#define KEY_BACKSPACE '\b'
#define KEY_TAB '\t'
#define KEY_ENTER 0x1C
#define KEY_RETURN '\r'

#define KEY_INSERT 0x90
#define KEY_DELETE 0x91
#define KEY_HOME 0x92
#define KEY_END 0x93
#define KEY_PAGE_UP 0x94
#define KEY_PAGE_DOWN 0x95
#define KEY_LEFT 0x4B
#define KEY_UP 0x48
#define KEY_RIGHT 0x4D
#define KEY_DOWN 0x50

// Function Keys
#define KEY_F1 0x80
#define KEY_F2 0x81
#define KEY_F3 0x82
#define KEY_F4 0x83
#define KEY_F5 0x84
#define KEY_F6 0x85
#define KEY_F7 0x86
#define KEY_F8 0x87
#define KEY_F9 0x88
#define KEY_F10 0x89
#define KEY_F11 0x8A
#define KEY_F12 0x8B

// Modifier Keys
#define KEY_LCTRL 0x1D
#define KEY_RCTRL 0x1D
#define KEY_LALT 0x38
#define KEY_RALT 0x38
#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36

#define KEY_CAPS_LOCK 0x3A
#define KEY_SCROLL_LOCK 0x46
#define KEY_NUM_LOCK 0x45

// Modifier Masks
#define KEY_MOD_ALT 0x0200
#define KEY_MOD_CTRL 0x0400
#define KEY_MOD_SHIFT 0x0800
#define KEY_MOD_CAPS_LOCK 0x1000
#define KEY_MOD_NUM_LOCK 0x2000
#define KEY_MOD_SCROLL_LOCK 0x4000

// Key State Constants
#define KEY_RELEASE_FLAG 0x80
#define KEY_SCANCODE_MASK 0x7F

// Buffer Size
#define KEYBOARD_BUFFER_SIZE 256

// Key State Macros
#define KEY_IS_PRESS(state) (!((state) & KEY_RELEASE_FLAG))
#define KEY_IS_RELEASE(state) (!!((state) & KEY_RELEASE_FLAG))
#define KEY_SCANCODE(state) ((state) & KEY_SCANCODE_MASK)

// Modifier Check
#define KEY_HAS_MODIFIER(state, mod_mask) (((state) & (mod_mask)))

// Character Lookup Macro
#define KEY_CHAR(_s)                                                           \
  __extension__({                                                              \
    __typeof__(_s) __s = (_s);                                                 \
    KEY_SCANCODE(__s) < 128                                                    \
        ? keyboard_layout_us[KEY_HAS_MODIFIER(__s, KEY_MOD_SHIFT) ? 1 : 0]     \
                            [KEY_SCANCODE(__s)]                                \
        : 0;                                                                   \
  })
// Keyboard State Structure
struct Keyboard {
  u16 mods;        // Modifier states
  bool keys[128];  // Key states (pressed/released)
  bool chars[128]; // Character key states
};

extern u8 keyboard_layout_us[2][128];
extern struct Keyboard keyboard;

// Inline Accessor Macros
#define KEYBOARD_KEY(scancode) (keyboard.keys[(scancode)])
#define KEYBOARD_CHAR(character) (keyboard.chars[(u8)(character)])

void keyboard_init();

#endif
