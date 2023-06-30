#ifndef MY_CONSOLE_HPP
#define MY_CONSOLE_HPP

#include "../lib/hw.h"

class my_console {
public:
    static void putc(char c);
    static char getc();

    static char curr;
    static bool should_write;
    static char* data_adr;
};

#endif
