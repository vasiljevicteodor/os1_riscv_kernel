#include "../h/syscall_cpp.h"
#include "../h/scheduler.hpp"

void* operator new (size_t n) {
    return mem_alloc(n);
}

void operator delete (void* mem) {
    mem_free(mem);
}

Thread::Thread(void (*body)(void *), void *arg) {
    myHandle = new PCB(true, body, arg);
    myHandle->my_thread = this;
}

Thread::Thread() {
    myHandle = new PCB(true, nullptr, nullptr);
    myHandle->my_thread = this;
    myHandle->my_ra = (uint64)PCB::start;
    myHandle->use_body = false;
}

int Thread::start() {
    Scheduler::put(myHandle);
    return 0;
}

Thread::~Thread() {
    delete myHandle;
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t) {             //should be implemented...
    return 0;
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}
