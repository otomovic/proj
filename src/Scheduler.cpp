//
// Created by os on 8/28/26.
//
#include "../h/Scheduler.h"

PCB* Scheduler::head = nullptr;
PCB* Scheduler::tail = nullptr;

void Scheduler::put(PCB* thread) {
    if (!thread)
        return;
    thread->next = nullptr;
    if (tail) tail->next = thread;
    else head = thread;
    tail = thread;
}

PCB* Scheduler::get() {
    if (!head)
        return nullptr;
    PCB* t = head;
    head = head->next;
    if (!head) tail = nullptr;
    t->next = nullptr;
    return t;
}