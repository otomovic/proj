//
// Created by os on 8/28/26.
//
#ifndef PROJECT_BASE_V1_1_SCHEDULER_H
#define PROJECT_BASE_V1_1_SCHEDULER_H
#include "PCB.h"

class Scheduler {
public:
    static void put(PCB* thread);
    static PCB* get();
private:
    static PCB* head;
    static PCB* tail;
};
#endif //PROJECT_BASE_V1_1_SCHEDULER_H