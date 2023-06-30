#include "../h/scheduler.hpp"

List<PCB> Scheduler::readyCoroutineQueue;

PCB *Scheduler::get()
{
    return readyCoroutineQueue.removeFirst();
}

void Scheduler::put(PCB *pcb)
{
    readyCoroutineQueue.addLast(pcb);
}

bool Scheduler::isEmpty() {
    if (!readyCoroutineQueue.removeFirst())
    {
        return true;
    }
    return false;
}

void Scheduler::putFirst(PCB *pcb) {
    readyCoroutineQueue.addFirst(pcb);
}

PCB* Scheduler::getLast() {
    return readyCoroutineQueue.removeLast();
}
