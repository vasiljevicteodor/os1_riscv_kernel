#include "../h/abi.hpp"
#include "../h/print.hpp"
#include "../h/memory_allocator.hpp"
#include "../h/pcb.hpp"
#include "../h/_sem.hpp"

void toUserMode() {
    PCB::call_code = 2;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());
}

void toSystemMode() {
    PCB::call_code = 1;
    Scheduler::putFirst(PCB::running);
    Scheduler::putFirst(PCB::kernel);
    PCB::yield(Scheduler::get());
}

void* kAllocate(size_t size) {
    MemoryAllocator* m = MemoryAllocator::getInstance();
    return m->allocate(size);
}

void kFree(void* mem) {
    MemoryAllocator* m = MemoryAllocator::getInstance();
    m->deallocate(mem);
}

void switchContext() {
    Scheduler::put(PCB::running);
    PCB* next = Scheduler::get();
    PCB::yield(next);
}

extern "C" void handleSysCall() {
    switch (Riscv::r_scause()) {
        case Riscv::BitMaskScause::SCAUSE_ECALL_U:
            switch(PCB::call_code) {
                case 1:                                                               //function toSystemMode is called
                    Riscv::ms_sstatus(Riscv::BitMaskSstatus::SSTATUS_SPP);      //setting the value of sstatus registed so that it goes to system mode
                    break;
                case 2:                                                               //function toUserMode is called
                    Riscv::mc_sstatus(Riscv::BitMaskSstatus::SSTATUS_SPP);      //setting the value of sstatus registed so that it goes to user mode
                    break;
                case 3:
                    PCB::ret_val = kAllocate((size_t)PCB::arg);
                    break;
                case 4:
                    kFree(PCB::arg);
                    break;
                case 0x11: {
                    PCB *new_thread = (PCB *) kAllocate(sizeof(PCB));
                    new_thread->initPCB(true, PCB::fun_ptr, PCB::arg);
                    PCB::ret_val = new_thread;
                    break;
                }
                case 0x12: {
                    PCB *to_destroy = Scheduler::getLast();
                    uint64 *pcb_stack = to_destroy->getStack();
                    kFree(pcb_stack);
                    kFree(to_destroy);
                    break;
                }
                case 0x21: {
                    _sem *new_sem = (_sem *) kAllocate(sizeof(_sem));
                    new_sem->initSem((size_t) PCB::arg);
                    PCB::ret_val = new_sem;
                    break;
                }
                case 0x22: {
                    _sem* handle = (_sem*)PCB::arg;
                    while(handle->getVal() < 0)
                        handle->signal();
                    kFree(handle);
                    break;
                }
                case 0x23: {
                    _sem* id = (_sem*)PCB::arg;
                    id->wait();
                    break;
                }
                case 0x24: {
                    _sem* id = (_sem*)PCB::arg;
                    id->signal();
                    break;
                }
                case 0x41:
                    PCB::ret_val = (void *)(size_t)my_console::getc();
                    break;

                case 0x42:
                    my_console::putc((char) (size_t) PCB::arg);
                    break;
            }
            Riscv::mc_sip(Riscv::BitMaskSip::SIP_SSIE);                     //software interrupt handled
            Riscv::w_sepc(Riscv::r_sepc() + 4);                             //setting the value of sepc register to the instruction after ecall
            break;

        case Riscv::BitMaskScause::SCAUSE_ECALL_S:
            switch(PCB::call_code) {
                case 1:                                                      //function toSystemMode is called
                    Riscv::ms_sstatus(Riscv::BitMaskSstatus::SSTATUS_SPP);      //setting the value of sstatus registed so that it goes to system mode
                    break;
                case 2:                                                      //function toUserMode is called
                    Riscv::mc_sstatus(Riscv::BitMaskSstatus::SSTATUS_SPP);      //setting the value of sstatus registed so that it goes to user mode
                    break;
                case 3:
                    PCB::ret_val = kAllocate((size_t)PCB::arg);
                    break;
                case 4:
                    kFree(PCB::arg);
                    break;
                case 0x11: {
                    PCB *new_thread = (PCB *) kAllocate(sizeof(PCB));
                    new_thread->initPCB(true, PCB::fun_ptr, PCB::arg);
                    PCB::ret_val = new_thread;
                    break;
                }
                case 0x12: {
                    PCB *to_destroy = Scheduler::getLast();
                    uint64 *pcb_stack = to_destroy->getStack();
                    kFree(pcb_stack);
                    kFree(to_destroy);
                    break;
                }
                case 0x21: {
                    _sem *new_sem = (_sem *) kAllocate(sizeof(_sem));
                    new_sem->initSem((size_t) PCB::arg);
                    PCB::ret_val = new_sem;
                    break;
                }
                case 0x22: {
                    _sem* handle = (_sem*)PCB::arg;
                    while(handle->getVal() < 0)
                        handle->signal();
                    kFree(handle);
                    break;
                }
                case 0x23: {
                    _sem* id = (_sem*)PCB::arg;
                    id->wait();
                    break;
                }
                case 0x24: {
                    _sem* id = (_sem*)PCB::arg;
                    id->signal();
                    break;
                }
                case 0x41:
                    PCB::ret_val = (void *)(size_t)my_console::getc();
                    break;

                case 0x42:
                    my_console::putc((char) (size_t) PCB::arg);
                    break;
            }

            Riscv::mc_sip(Riscv::BitMaskSip::SIP_SSIE);                     //software interrupt handled
            Riscv::w_sepc(Riscv::r_sepc() + 4);                             //setting the value of sepc register to the instruction after ecall
            break;

        case Riscv::BitMaskScause::SCAUSE_CONSOLE:
            if (plic_claim() == 10)
                plic_complete(10);
            Riscv::mc_sip(Riscv::BitMaskSip::SIP_SEIE);                     //hardware interrupt handled
            break;

        case Riscv::BitMaskScause::SCAUSE_TIMER:
            Riscv::mc_sip(Riscv::BitMaskSip::SIP_SSIE);                     //software interrupt handled
            break;

        default:
            printInteger(Riscv::r_scause());
            myPrintString("\n");
    }
}
