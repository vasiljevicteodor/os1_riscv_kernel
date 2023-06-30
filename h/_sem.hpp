#ifndef _SEM_HPP
#define _SEM_HPP

#include "../h/scheduler.hpp"
#include "../h/pcb.hpp"

class _sem {
public:
    void wait();
    void signal();

    void initSem(int val);

    void setVal(int val);
    int getVal() const;

protected:
    void block();
    void unblock();
private:
    int val = 1;
    List<PCB> wait_queue;
};

#endif
