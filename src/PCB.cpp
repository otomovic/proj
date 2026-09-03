//
// Created by os on 8/28/26.
//
#include "../h/PCB.h"
#include "../h/MemoryAllocator.h"
#include "../h/Scheduler.h"

inline void* operator new(size_t, void* p) noexcept { return p; }

PCB* PCB::running = nullptr;

// A thread that just exited can't free its own stack/object while
// still executing on them. Stash it here; reclaimed by the next
// thread that passes through switchToNext().
static PCB* zombie = nullptr;

PCB::PCB(Body body_, void* arg_, uint64* stack_, uint64* context_)
    : wakeupResult(0), waitN(0), stack(stack_),
      body(body_), arg(arg_), next(nullptr), finished(false)
{
    context.sp = (uint64)context_;
    context.ra = (uint64)&PCB::threadWrapper;
}

void PCB::threadWrapper() {
    PCB* self = PCB::running;
    if (self && self->body)
        self->body(self->arg);
    PCB::exit();
    for (;;) {}
}

int PCB::create(PCB** handle, Body body, void* arg) {
    if (!handle || !body)
        return -1;

    uint64* stack = (uint64*)MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
    if (!stack)
        return -1;

    // Stack grows down; initial sp is the last valid, 16-byte-aligned
    // address within the allocated region.
    uint64 top = ((uint64)stack + DEFAULT_STACK_SIZE) & ~((uint64)0xF);

    void* mem = MemoryAllocator::mem_alloc(sizeof(PCB));
    if (!mem) {
        MemoryAllocator::mem_free(stack);
        return -1;
    }

    PCB* newThread = new (mem) PCB(body, arg, stack, (uint64*)top);

    *handle = newThread;
    Scheduler::put(newThread);
    return 0;
}

static void switchToNext(bool requeueCurrent) {
    PCB* oldThread = PCB::running;
    PCB* newThread = Scheduler::get();

    if (!newThread) {
        if (requeueCurrent)
            return; // plain yield with nothing else ready — keep running
        // We just gave up the CPU (exited or blocked) and nobody else
        // is ready — regular program end.
        *(volatile uint32*)0x100000 = 0x5555;
        for (;;) {}
    }

    if (oldThread && requeueCurrent)
        Scheduler::put(oldThread);

    PCB::running = newThread;

    if (oldThread)
        PCB::contextSwitch(&oldThread->context, &newThread->context);
    else
        PCB::contextSwitch(&newThread->context, &newThread->context);

    if (zombie) {
        PCB* toFree = zombie;
        zombie = nullptr;
        MemoryAllocator::mem_free(toFree->stack);
        MemoryAllocator::mem_free(toFree);
    }
}

void PCB::dispatch() {
    switchToNext(true);
}

void PCB::block() {
    switchToNext(false);
}

int PCB::exit() {
    if (!running)
        return -1;
    running->finished = true;
    zombie = running;
    switchToNext(false);
    return 0; // unreachable
}