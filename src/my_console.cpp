#include "../h/my_console.hpp"

char my_console::curr = 0;
bool my_console::should_write = false;
char* my_console::data_adr = (char*)CONSOLE_TX_DATA;

void my_console::putc(char c) {
    char* data_adr = (char*)CONSOLE_TX_DATA;
    *data_adr = c;
}

char my_console::getc() {
    char* status = (char*)CONSOLE_STATUS;
    char mask = 0x01;
    while(((*status)&mask) == 0);
    char* data_adr = (char*)CONSOLE_RX_DATA;
    return *data_adr;
}
