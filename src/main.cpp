#include "../h/print.hpp"
#include "../h/abi.hpp"
#include "../h/memory_allocator.hpp"
#include "../h/pcb.hpp"

void userMain();

int main() {
    uint64 default_stvec = Riscv::r_stvec();
    Riscv::w_stvec((uint64)sysCall);

    MemoryAllocator* m = MemoryAllocator::getInstance();                    //Since kernel thread is used to allocate memory, it needs to be created this way
    PCB::kernel = (PCB*)m->allocate(sizeof(PCB));
    PCB::kernel->initKernel();
    PCB* idle = (PCB*)m->allocate(sizeof(PCB));                        //Same goes for the idle thread
    idle->initPCB(false, nullptr, nullptr);
    PCB::running = idle;

    toUserMode();

    userMain();

    while(true) {
        Scheduler::put(PCB::running);
        PCB* next = Scheduler::get();
        if (next == PCB::running)
            break;
        else
            PCB::yield(next);
    }

    toSystemMode();

    Riscv::w_stvec(default_stvec);
    return 0;
}