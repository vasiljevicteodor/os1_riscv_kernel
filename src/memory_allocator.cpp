#include "../h/memory_allocator.hpp"

MemoryAllocator* MemoryAllocator::instance = nullptr;

MemoryAllocator::MemoryAllocator() : allocated_mem_head(nullptr) {
    kernel_stack_pointer = (void*)((uint64)HEAP_START_ADDR + DEFAULT_STACK_SIZE);
    kernel_mem_end = (char*)kernel_stack_pointer + 1;
}

MemoryAllocator* MemoryAllocator::getInstance() {
    if (!instance) {
        instance = (MemoryAllocator*)((uint64)HEAP_START_ADDR + DEFAULT_STACK_SIZE + 1);
        *instance = MemoryAllocator();
    }
    return instance;
}

void* MemoryAllocator::allocate(size_t size) {
    if (size%16 > 0) {
        size = 16*(size/16 + 1);
    }

    MemDesc* new_mem = nullptr;
    if(!allocated_mem_head) {
        new_mem = (MemDesc*)((uint64)HEAP_END_ADDR -    // izbaceno -1
                                size - sizeof(MemDesc));
        allocated_mem_head = new_mem;
        allocated_mem_head->next = nullptr;
    }
    else {                                  //add before the first element in the list
        if (((uint64)HEAP_END_ADDR - 1 - (uint64)allocated_mem_head -
            allocated_mem_head->size) >
                (size + sizeof(MemDesc))) {
            new_mem = (MemDesc*)((uint64)HEAP_END_ADDR - 1 -
                                    size - sizeof(MemDesc));
            new_mem->next = allocated_mem_head;
            allocated_mem_head = new_mem;
        }
        else {                              //add in the middle of the list
            MemDesc *tmp = allocated_mem_head->next;
            MemDesc *prev = allocated_mem_head;
            while (tmp) {
                if (((uint64) prev - ((uint64) tmp + sizeof(MemDesc) + tmp->size)) >=
                        (size + sizeof(MemDesc))) {
                    new_mem = (MemDesc *) ((uint64) prev -
                                size - sizeof(MemDesc));
                    new_mem->next = prev->next;
                    prev->next = new_mem;
                    break;
                }
                prev = tmp;
                tmp = tmp->next;
            }
            if (!tmp) {                     //add to the end of the list
                if ((uint64)prev - (uint64)kernel_mem_end > (size + sizeof(MemDesc))) {
                    new_mem = (MemDesc *) ((uint64) prev -
                                           size - sizeof(MemDesc));
                    prev->next = new_mem;
                    new_mem->next = nullptr;
                }
            }
        }
    }
    if (new_mem) {
        new_mem->size = /* num_of_blocks */ size;
        return (MemDesc *) new_mem + 1;
    }
    return nullptr;
}

int MemoryAllocator::deallocate(void* ptr) {
    if(!allocated_mem_head) return -1;
    if(!ptr) return 0;
    MemDesc* tmp = allocated_mem_head;
    MemDesc* prev = nullptr;
    while(tmp) {
        if((MemDesc*)ptr == (tmp + 1)) break;
        prev = tmp;
        tmp = tmp->next;
    }
    if(!tmp) return -1;
    if(tmp == allocated_mem_head) {
        if(allocated_mem_head->next) {
            MemDesc* help = allocated_mem_head->next;
            allocated_mem_head->next = nullptr;
            allocated_mem_head = help;
        }
        else
            allocated_mem_head = nullptr;
    }
    else
        prev->next = tmp->next;

    ptr = nullptr;

    return 0;
}


