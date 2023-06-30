#include "../lib/hw.h"
//#include "../lib/console.h"
#include "../h/my_console.hpp"

void myPrintString(char const* string) {
    while(*string != '\0'){
        my_console::putc(*string);
        string++;
    }
}

void printInteger(uint64 num) {
    if(!num) {
        my_console::putc('0');
        return;
    }
    long x = num;
    int i;
    int negative = 0;
    char buf[32];
    if(num < 0){
        negative = 1;
        x *= -1;
    }
    else
        x = num;
    i = 0;
    while(x) {
        buf[i++] = (x % 10) + '0';
        x /= 10;
    }
    if(negative == 1)
        buf[i++] = '-';
    while(i--)
        my_console::putc(buf[i]);
}