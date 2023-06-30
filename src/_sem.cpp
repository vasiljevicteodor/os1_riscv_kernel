#include "../h/_sem.hpp"

void _sem::setVal(int val) {
    this->val = val;
}

void _sem::block() {
    PCB* blocked = Scheduler::get();
    wait_queue.addLast(blocked);
    //PCB* next = Scheduler::get();
    //PCB::yield(next);
}

void _sem::unblock() {
    PCB* next = wait_queue.removeFirst();
    Scheduler::put(next);
}

void _sem::wait() {
    //lock();
    if(--val < 0) block();
    //unlock();
}

void _sem::signal() {
    //lock();
    if(++val<=0) unblock();
    //unlock();
}

int _sem::getVal() const {
    return val;
}

void _sem::initSem(int val) {
    this->val = val;
    this->wait_queue.resetHead();
    this->wait_queue.resetTail();
}


