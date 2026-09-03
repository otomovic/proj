//
// Created by os on 8/28/26.
//
#include "../h/Semaphore.h"
#include "../h/MemoryAllocator.h"
#include "../h/Scheduler.h"

inline void* operator new(size_t, void* p) noexcept { return p; }

Sem::Sem(unsigned init) : value(init), closed(false), waitHead(nullptr), waitTail(nullptr) {}

void Sem::enqueue(PCB* t, unsigned n) {
    t->waitN = n;
    t->next = nullptr;
    if (waitTail) waitTail->next = t;
    else waitHead = t;
    waitTail = t;
}

int Sem::open(Sem** handle, unsigned init) {
    if (!handle)
        return -1;
    void* mem = MemoryAllocator::mem_alloc(sizeof(Sem));
    if (!mem)
        return -1;
    *handle = new (mem) Sem(init);
    return 0;
}

int Sem::close(Sem* sem) {
    if (!sem)
        return -1;

    sem->closed = true;

    PCB* t = sem->waitHead;
    while (t) {
        PCB* nextT = t->next;
        t->wakeupResult = -1;
        Scheduler::put(t);
        t = nextT;
    }
    sem->waitHead = sem->waitTail = nullptr;

    MemoryAllocator::mem_free(sem);
    return 0;
}

int Sem::wait(Sem* sem) {
    return wait_n(sem, 1);
}

int Sem::signal(Sem* sem) {
    return signal_n(sem, 1);
}

int Sem::wait_n(Sem* sem, unsigned n) {
    if (!sem || sem->closed)
        return -1;

    if (sem->value >= n) {
        sem->value -= n;
        return 0;
    }

    PCB* self = PCB::running;
    sem->enqueue(self, n);
    self->wakeupResult = 0;

    PCB::block();

    return self->wakeupResult;
}

int Sem::signal_n(Sem* sem, unsigned n) {
    if (!sem || sem->closed)
        return -1;

    sem->value += n;

    while (sem->waitHead && sem->waitHead->waitN <= sem->value) {
        PCB* t = sem->waitHead;
        sem->waitHead = t->next;
        if (!sem->waitHead) sem->waitTail = nullptr;

        sem->value -= t->waitN;
        t->wakeupResult = 0;
        Scheduler::put(t);
    }

    return 0;
}