#ifndef PCB_HPP
#define PCB_HPP

#include "../lib/hw.h"
#include "../h/print.hpp"
#include "../h/scheduler.hpp"

class Thread;

class PCB {
public:
    using Body = void (*)(void*);

    PCB(bool alloc_stack, Body body, void* arg);

    ~PCB();

    void initKernel();

    void initPCB(bool alloc_stack, Body body, void* arg);

    bool isPcbDone() const;

    uint64 *getStack() const;

    void setBody(Body body);

    static __attribute__ ((noinline)) void yield(PCB* other);

    static PCB* running;
    static PCB* kernel;

    static void* ret_val;
    static uint64 call_code;
    static void* arg;
    static Body fun_ptr;
    void* body_arg = nullptr;

    static bool in_interrupt;

    friend class Thread;

private:
    static __attribute__ ((noinline)) void saveRegisters();
    static __attribute__ ((noinline)) void loadRegisters();

    static __attribute__ ((noinline)) void kernel_body(void* arg);

    static void start();

protected:
    Body body;
    bool use_body = true;
    bool pcb_done = false;
    bool pcb_started = false;
    uint64* stack;
    size_t stack_size = DEFAULT_STACK_SIZE;
    uint64 my_sp;
    uint64 my_ra;

    Thread* my_thread;
};

#endif
