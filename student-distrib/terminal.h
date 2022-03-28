#include "keyboard.h"
#include "lib.h"
#include "types.h"

int32_t terminal_open();
int32_t terminal_close();
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes);

