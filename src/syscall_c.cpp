#include "../h/syscall_c.h"
#include "../h/abi.hpp"

void* mem_alloc (size_t size) {
    PCB::in_interrupt = true;

    if (size%MEM_BLOCK_SIZE > 0) {
        size = MEM_BLOCK_SIZE*(size/MEM_BLOCK_SIZE + 1);
    }
    PCB::call_code = 3;
    PCB::arg = (void*)size;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());

    PCB::in_interrupt = false;

    return PCB::ret_val;
}

int mem_free(void* mem) {
    PCB::in_interrupt = true;

    if(!mem)
        return -1;
    PCB::call_code = 4;
    PCB::arg = mem;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());

    PCB::in_interrupt = false;

    return 0;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg) {
    PCB::in_interrupt = true;

    PCB::call_code = 0x11;
    PCB::arg = arg;
    PCB::fun_ptr = start_routine;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());
    thread_t new_thread = (thread_t)PCB::ret_val;
    *handle = (thread_t)PCB::ret_val;
    Scheduler::put(new_thread);

    PCB::in_interrupt = false;

    if ((uint64)HEAP_START_ADDR < (uint64)new_thread && (uint64)new_thread <= (uint64)HEAP_END_ADDR)
        return 0;

    return -1;
}

int thread_exit() {
    PCB::in_interrupt = true;

    PCB::call_code = 0x12;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());

    PCB::in_interrupt = false;

    return 0;
}

void thread_dispatch() {
    PCB::in_interrupt = true;

    Scheduler::put(PCB::running);
    PCB* next = Scheduler::get();
    PCB::yield(next);

    PCB::in_interrupt = false;

}

int sem_open(sem_t* handle, unsigned init) {
    PCB::in_interrupt = true;

    PCB::call_code = 0x21;
    size_t help = (size_t)init;
    PCB::arg = (void*)help;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());
    *handle = (sem_t)PCB::ret_val;

    PCB::in_interrupt = false;

    if (*handle)
        return 0;
    return -1;
}

int sem_close(sem_t handle) {
    PCB::in_interrupt = true;

    PCB::call_code = 0x22;
    PCB::arg = handle;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());
    handle = nullptr;

    PCB::in_interrupt = false;

    return 0;
}

int sem_wait(sem_t id) {
    PCB::in_interrupt = true;

    PCB::call_code = 0x23;
    PCB::arg = id;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB* next = Scheduler::get();
    PCB::yield(next);

    PCB::in_interrupt = false;

    if (!id) return -1;

    return 0;
}

int sem_signal(sem_t id) {
    PCB::in_interrupt = true;

    PCB::call_code = 0x24;
    PCB::arg = id;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());
    return 0;
}

char getc() {
    PCB::in_interrupt = true;

    PCB::call_code = 0x41;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());
    char result = (char)(size_t)PCB::ret_val;

    return result;
}

void putc(char c) {
    PCB::in_interrupt = true;

    PCB::call_code = 0x42;
    PCB::arg = (void*)(size_t)c;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());

    PCB::in_interrupt = false;

}