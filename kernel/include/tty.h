#ifndef TTY_H
#define TTY_H

#include <stddef.h>
#include <stdint.h>

void tty_initialize();
void tty_write(const char *str);
void tty_putchar(char c);

#endif
