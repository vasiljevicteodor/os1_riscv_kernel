#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "list.hpp"

class PCB;

class Scheduler
{
private:
    static List<PCB> readyCoroutineQueue;

public:
    static PCB *get();

    static void put(PCB *pcb);

    static void putFirst(PCB* pcb);

    static PCB* getLast();

    static bool isEmpty();

};

#endif
