#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include "../lib/hw.h"

class MemoryAllocator {
public:
    struct MemDesc {
        size_t size; //size of allocated memory
        MemDesc* next;
    };
    static MemoryAllocator* getInstance();
    void* allocate(size_t size);
    int deallocate(void* ptr);
private:
    MemoryAllocator();
    static MemoryAllocator* instance;

    MemDesc* allocated_mem_head;
    void* kernel_mem_end;
    void* kernel_stack_pointer;
};

#endif