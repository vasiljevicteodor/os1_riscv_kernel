#ifndef ABI_HPP
#define ABI_HPP

#include "../lib/console.h"
#include "../h/riscv.hpp"

void toUserMode();

extern "C" void sysCall();

extern "C" void handleSysCall();

void toSystemMode();

void* kAllocate(size_t size);

void switchContext();

#endif
