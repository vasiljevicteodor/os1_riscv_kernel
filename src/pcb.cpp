#include "../h/pcb.hpp"
#include "../h/syscall_cpp.h"

PCB* PCB::running = nullptr;
PCB* PCB::kernel = nullptr;
void* PCB::ret_val = nullptr;
uint64 PCB::call_code = 0;
void* PCB::arg = nullptr;
PCB::Body PCB::fun_ptr = nullptr;
bool PCB::in_interrupt = false;

PCB::PCB(bool alloc_stack, Body body, void* arg) {
    if (alloc_stack) {
        MemoryAllocator *m = MemoryAllocator::getInstance();
        stack = (uint64*)m->allocate(stack_size * 8);
        my_sp = (uint64)&stack[stack_size];
    }
    else {
        __asm__ volatile("mv %[destination], sp" : [destination] "=r" (PCB::my_sp));
        pcb_started = true; // this is for idle thread
    }
    if (body) {
        my_ra = (uint64)start;
        this->body = body;
        this->body_arg = arg;
    }
}

void PCB::yield(PCB* other) {
    __asm__ volatile("mv %[destination], ra" : [destination] "=r" (running->my_ra));
    saveRegisters();
    __asm__ volatile("mv %[destination], sp" : [destination] "=r" (running->my_sp));

    //running = running->other_pcb;
    running = other;

    __asm__ volatile("mv sp, %[source]" : : [source] "r" (running->my_sp));
    if (running->pcb_started)
        loadRegisters();
    else
        __asm__ volatile("addi sp, sp, -32");
    __asm__ volatile("mv x28, %[source]" : : [source] "r" (running->my_ra));
    __asm__ volatile("sd x28, 0x18(sp)");
    __asm__ volatile("addi s0, sp, 32");
}

void PCB::saveRegisters() {
        __asm__ volatile("addi sp, sp, -256");  //Saving all registers on stack
        __asm__ volatile("sd x3, 0x08(sp)");
        __asm__ volatile("sd x4, 0x10(sp)");
        __asm__ volatile("sd x5, 0x18(sp)");
        __asm__ volatile("sd x6, 0x20(sp)");
        __asm__ volatile("sd x7, 0x28(sp)");
        __asm__ volatile("sd x8, 0x30(sp)");
        __asm__ volatile("sd x9, 0x38(sp)");
        __asm__ volatile("sd x10, 0x40(sp)");
        __asm__ volatile("sd x11, 0x48(sp)");
        __asm__ volatile("sd x12, 0x50(sp)");
        __asm__ volatile("sd x13, 0x58(sp)");
        __asm__ volatile("sd x14, 0x60(sp)");
        __asm__ volatile("sd x15, 0x68(sp)");
        __asm__ volatile("sd x16, 0x70(sp)");
        __asm__ volatile("sd x17, 0x78(sp)");
        __asm__ volatile("sd x18, 0x80(sp)");
        __asm__ volatile("sd x19, 0x88(sp)");
        __asm__ volatile("sd x20, 0x90(sp)");
        __asm__ volatile("sd x21, 0x98(sp)");
        __asm__ volatile("sd x22, 0xa0(sp)");
        __asm__ volatile("sd x23, 0xa8(sp)");
        __asm__ volatile("sd x24, 0xb0(sp)");
        __asm__ volatile("sd x25, 0xb8(sp)");
        __asm__ volatile("sd x26, 0xc0(sp)");
        __asm__ volatile("sd x27, 0xc8(sp)");
        __asm__ volatile("sd x28, 0xd0(sp)");
        __asm__ volatile("sd x29, 0xd8(sp)");
        __asm__ volatile("sd x30, 0xf0(sp)");
        __asm__ volatile("sd x31, 0xf8(sp)");
        __asm__ volatile("ld x28, 0x108(sp)");  //Putting frame pointer value on top of the stack
        __asm__ volatile("addi sp, sp, -16");
        __asm__ volatile("sd x28, 0x08(sp)");
}

void PCB::loadRegisters() {
    __asm__ volatile("ld x28, 0x08(sp)");   //Loading frame pointer value from stack
    __asm__ volatile("addi sp, sp, 16");
    __asm__ volatile("sd x28, 0x108(sp)");  //Saving frame pointer value on what will be top of stack when all register values get popped
    __asm__ volatile("ld x3, 0x08(sp)");
    __asm__ volatile("ld x4, 0x10(sp)");
    __asm__ volatile("ld x5, 0x18(sp)");
    __asm__ volatile("ld x6, 0x20(sp)");
    __asm__ volatile("ld x7, 0x28(sp)");
    __asm__ volatile("ld x8, 0x30(sp)");
    __asm__ volatile("ld x9, 0x38(sp)");
    __asm__ volatile("ld x10, 0x40(sp)");
    __asm__ volatile("ld x11, 0x48(sp)");
    __asm__ volatile("ld x12, 0x50(sp)");
    __asm__ volatile("ld x13, 0x58(sp)");
    __asm__ volatile("ld x14, 0x60(sp)");
    __asm__ volatile("ld x15, 0x68(sp)");
    __asm__ volatile("ld x16, 0x70(sp)");
    __asm__ volatile("ld x17, 0x78(sp)");
    __asm__ volatile("ld x18, 0x80(sp)");
    __asm__ volatile("ld x19, 0x88(sp)");
    __asm__ volatile("ld x20, 0x90(sp)");
    __asm__ volatile("ld x21, 0x98(sp)");
    __asm__ volatile("ld x22, 0xa0(sp)");
    __asm__ volatile("ld x23, 0xa8(sp)");
    __asm__ volatile("ld x24, 0xb0(sp)");
    __asm__ volatile("ld x25, 0xb8(sp)");
    __asm__ volatile("ld x26, 0xc0(sp)");
    __asm__ volatile("ld x27, 0xc8(sp)");
    __asm__ volatile("ld x28, 0xd0(sp)");
    __asm__ volatile("ld x29, 0xd8(sp)");
    __asm__ volatile("ld x30, 0xf0(sp)");
    __asm__ volatile("ld x31, 0xf8(sp)");
    __asm__ volatile("addi sp, sp, 256");
}

void PCB::kernel_body(void* arg) {
    while(true) {
        uint64 a = PCB::call_code;
        __asm__ volatile("mv a0, %[source]" : : [source] "r"(a));
        __asm__ volatile("ecall");
        PCB::yield(Scheduler::get());
    }
}

void PCB::start() {
    running->pcb_started = true;
    if (running->use_body)
        running->body(running->body_arg);
    else
        running->my_thread->run();
    running->pcb_done = true;
    yield(Scheduler::get());
}

bool PCB::isPcbDone() const {
    return pcb_done;
}

void PCB::initKernel() {
    body_arg = nullptr;
    pcb_done = false;
    pcb_started = false;
    stack_size = DEFAULT_STACK_SIZE;

    MemoryAllocator *m = MemoryAllocator::getInstance();
    stack = (uint64*)m->allocate(stack_size * 8);
    my_sp = (uint64)&stack[stack_size];
    my_ra = (uint64)start;
    this->body = &kernel_body;
}

PCB::~PCB() {
    MemoryAllocator* m = MemoryAllocator::getInstance();
    m->deallocate(stack);
}

void PCB::initPCB(bool alloc_stack, PCB::Body body, void *arg) {
    body_arg = nullptr;
    pcb_done = false;
    pcb_started = false;
    stack_size = DEFAULT_STACK_SIZE;

    if (alloc_stack) {
        MemoryAllocator *m = MemoryAllocator::getInstance();
        stack = (uint64*)m->allocate(stack_size * 8);
        my_sp = (uint64)&stack[stack_size];
    }
    else {
        __asm__ volatile("mv %[destination], sp" : [destination] "=r" (PCB::my_sp));
        pcb_started = true; // this is for idle thread
    }
    if (body) {
        my_ra = (uint64)start;
        this->body = body;
        this->body_arg = arg;
    }
}

uint64 *PCB::getStack() const {
    return stack;
}

void PCB::setBody(Body body) {
    PCB::body = body;
}






